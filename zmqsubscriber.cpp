#include "zmqsubscriber.h"
#include <QDebug>
#include <QThread>

ZmqSubscriber::ZmqSubscriber(const QString& topic, const QString& url, zmq::context_t* context, QObject* parent)
    : QObject(parent), topic(topic), url(url), context(context) {
    socket = new zmq::socket_t(*context, zmq::socket_type::sub);
    socket->connect(url.toStdString());
    socket->set(zmq::sockopt::subscribe, topic.toStdString());
}

ZmqSubscriber::~ZmqSubscriber() {
    running = false;
    socket->close();
    delete socket;
}

void ZmqSubscriber::process() {
    QString topicStr ;
    while (running) {
        zmq::message_t topic_msg;
        zmq::message_t data_msg;

        if (socket->recv(topic_msg, zmq::recv_flags::dontwait) && socket->recv(data_msg,zmq::recv_flags::dontwait)) {
            topicStr = QString::fromStdString(topic_msg.to_string());

            QByteArray data(reinterpret_cast<const char*>(data_msg.data()), static_cast<int>(data_msg.size()));
            emit receiveSignal(topicStr, data);
        }
    }
    qInfo() << "Subscriber on topic" << topicStr << "Finished" ;
    emit finished();
}

void ZmqSubscriber::stop()
{
        running = false;
}
