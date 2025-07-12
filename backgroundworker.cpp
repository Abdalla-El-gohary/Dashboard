#include "backgroundworker.h"

BackgroundWorker::BackgroundWorker(QObject *parent)
    : QObject{parent}
{
    setText(" ");
}

void BackgroundWorker::startWorker() {
    QTimer* timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &BackgroundWorker::printText);
    timer->start(500); /* every 500 ms */
}

void BackgroundWorker::printText()
{
        static int dotCount = 0;
        QString message = m_text + QString(".").repeated(dotCount + 1);
        emit updateText(message);

        dotCount = (dotCount + 1) % 3; // ...
}

void BackgroundWorker::setText(const QString &newText)
{
    m_text = newText;
}
