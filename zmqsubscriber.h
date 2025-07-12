#ifndef SUBSCRIBERWORKER_H
#define SUBSCRIBERWORKER_H

#include <QObject>
#include <QString>
#include <zmq.hpp>

class ZmqSubscriber : public QObject {
    Q_OBJECT

public:
    ZmqSubscriber(const QString& topic, const QString& url, zmq::context_t* context, QObject* parent = nullptr);
    ~ZmqSubscriber();

public slots:
    void process(); // Main loop
    void stop() ;

signals:
    void receiveSignal(const QString& topic, const QByteArray& data);
    void finished();

private:
    QString topic;
    QString url;
    zmq::socket_t* socket;
    zmq::context_t* context;
    bool running = true;
};

#endif
