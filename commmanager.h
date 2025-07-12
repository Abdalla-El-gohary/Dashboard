#ifndef COMMMANAGER_H
#define COMMMANAGER_H

#include <QObject>
#include "zmqclient.h"
#include "zmqsubscriber.h"
#include <QtConcurrent>
#include <QFuture>
#include <QImage>
#include "commconfig.h"
#include "QDebug"

class CommManager : public QObject
{
    Q_OBJECT
public:
    explicit CommManager(QObject *parent = nullptr);
    ~CommManager();
    void sendRequest(const QString& message) ;
    void addSubscriber(const QString& topic, const QString& url);
    void removeSubscriber(const QString& topic) ;
    void StartAddProfileClient() ;
    void CloseAddProfileClient() ;
    void SendCaptureRequest(const QString& message);
signals:
    /* managment signals */
    void stopSubscriberRequested(const QString& topic);

    /* upper emmited signals  */
    void captureReceived(const QImage& image) ;
    void recieved_reply(const QString& message) ;

public slots:
    void onResponseReceived(const QString msg) ;
    void onMessageReceived(const QString& topic, const QByteArray& data);

private :
    zmq::context_t context;
    QList<QThread*> threads;
    ZmqClient* m_zmqclientPtr ;
    ZmqClient* m_captureClient;
    QMap<QString, QPair<QThread*, ZmqSubscriber*>> subscribers;
};

#endif // COMMMANAGER_H
