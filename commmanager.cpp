#include "commmanager.h"

CommManager::CommManager(QObject* parent)
    : QObject(parent), context(1) {
    m_zmqclientPtr = new ZmqClient(SERVICE_REQUEST_URL);
    connect(m_zmqclientPtr,&ZmqClient::messageReceived,this,&CommManager::onResponseReceived) ;

}

CommManager::~CommManager() {
    for (int i = 0; i < threads.size(); ++i) {
        threads[i]->quit();
        threads[i]->wait();
        delete threads[i];
    }
    threads.clear();
    if (m_zmqclientPtr) {
        delete m_zmqclientPtr;
        m_zmqclientPtr = nullptr;
    }

}

void CommManager::sendRequest(const QString &message)
{
    QFuture<void> future = QtConcurrent::run([this,message]{
        /* Send Message Request */
        m_zmqclientPtr->ProcessZmqCommand(message) ;
    }) ;
}

void CommManager::addSubscriber(const QString& topic, const QString& url) {
    ZmqSubscriber* subscriber = new ZmqSubscriber(topic, url, &context);
    QThread* thread = new QThread();

    subscriber->moveToThread(thread);

    connect(thread, &QThread::started, subscriber, &ZmqSubscriber::process);
    connect(subscriber, &ZmqSubscriber::receiveSignal, this, &CommManager::onMessageReceived);
    connect(subscriber, &ZmqSubscriber::finished, thread, &QThread::quit);
    connect(thread, &QThread::finished, subscriber, &ZmqSubscriber::deleteLater);
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);
    connect(this, &CommManager::stopSubscriberRequested, subscriber, [subscriber, topic](const QString& t) {
        qInfo() << "stopping" << subscriber << "on topic:" << topic ;
        if (t == topic) subscriber->stop();
    });

    subscribers[topic] = qMakePair(thread, subscriber);
    thread->start();
}

void CommManager::removeSubscriber(const QString& topic){
    if (subscribers.contains(topic)) {

        emit stopSubscriberRequested(topic);  // or emit stopWorkerRequested(topic);

        subscribers.remove(topic);

        qInfo() << "Debug: Stopping Subscriber on topic" << topic ;
    }
}

void CommManager::StartAddProfileClient()
{
    m_captureClient = new ZmqClient(CAPTURE_URL);
    connect(m_captureClient, &ZmqClient::messageReceived, this,&CommManager::onResponseReceived) ;

}

void CommManager::SendCaptureRequest(const QString &message)
{
    qInfo() << "Sending: "<< "Capture Client" << message  ;
    QFuture<void> future = QtConcurrent::run([this,message]{
        /* Send Message Request */
         m_captureClient->ProcessZmqCommand(message);
    }) ;
}

void CommManager::onResponseReceived(const QString msg)
{
    emit recieved_reply(msg) ;
}

void CommManager::onMessageReceived(const QString& topic, const QByteArray& data) {

    if (topic == ADD_PROFILE_TOPIC) {
        QByteArray decodedData = QByteArray::fromBase64(data);
        QImage image;

        // If base64 decoding produced valid data, use it; otherwise, try raw data
        if (!decodedData.isEmpty()) {
            image = QImage::fromData(decodedData);
        } else {
            image = QImage::fromData(data); // Fallback to raw JPEG bytes
        }

        if (!image.isNull()) {
            emit captureReceived(image);
        } else {
            qWarning() << "Failed to parse image data (tried base64 and raw)";
        }
    }
    else if (topic == "sensor_data") {
        qInfo() << "sensor data" ;
    }
    else if (topic == "") {

    }

}
