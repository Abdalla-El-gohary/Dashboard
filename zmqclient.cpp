#include "zmqclient.h"

ZmqClient::ZmqClient(QString connect_url, QObject *parent)
    : QObject(parent) {
    context.reset(new zmq::context_t(1));
    socket.reset(new zmq::socket_t(*context, ZMQ_REQ));
    socket->connect(connect_url.toStdString());
}

ZmqClient::~ZmqClient() {
    qInfo() << "Zmq Client Destructed";
}

QString ZmqClient::sendCommand(const QString &command) {
    if (!socket || socket->handle() == nullptr) {
        qInfo() << "No socket for communication.";
        return "error";
    }

    zmq::message_t request(command.toStdString().c_str(), command.toStdString().length());
    socket->send(request, zmq::send_flags::none);
    qDebug() << "Message sent:" << command;
    qInfo() << "Running ZmqClient on thread:" << QThread::currentThread();

    zmq::message_t reply;
    socket->recv(reply, zmq::recv_flags::none);
    return QString::fromStdString(reply.to_string());
}


void ZmqClient::ProcessZmqCommand(const QString &command) {

    QString response = sendCommand(command);
    emit messageReceived(response);
}
