#ifndef ZMQCLIENT_H
#define ZMQCLIENT_H

#include <QObject>
#include <QString>
#include <QScopedPointer>
#include <zmq.hpp>
#include <QDebug>
#include <QThread>

class ZmqClient : public QObject {
    Q_OBJECT

public:
    explicit ZmqClient(QString connect_url, QObject *parent = nullptr);
    ~ZmqClient();

    void ProcessZmqCommand(const QString &command);
signals:
    void messageReceived(const QString &msg);

private:
    QString sendCommand(const QString &command);

    QScopedPointer<zmq::context_t> context;
    QScopedPointer<zmq::socket_t> socket;
};

#endif // ZMQCLIENT_H
