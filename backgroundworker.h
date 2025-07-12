#ifndef BACKGROUNDWORKER_H
#define BACKGROUNDWORKER_H

#include <QObject>
#include <QTimer>
#include <QThread>
#include <QDebug>
class BackgroundWorker : public QObject
{
    Q_OBJECT
public:
    explicit BackgroundWorker(QObject *parent = nullptr);
    void setText(const QString &newText);

public slots:
    void startWorker();

private slots:
        void printText();
signals:
    void updateText(QString text) ;
private :
    QString m_text ;
};

#endif // BACKGROUNDWORKER_H
