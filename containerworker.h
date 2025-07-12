#ifndef CONTAINERWORKER_H
#define CONTAINERWORKER_H

#include <QObject>
#include <QProcess>

class ContainerWorker : public QObject {
    Q_OBJECT

public:
    explicit ContainerWorker(QObject *parent = nullptr);
    void startPull(const QString &imageName);
    void startSave(const QString &imageName, const QString &outputPath);

signals:
    void pullFinished(QString imageName, bool success);
    void saveFinished(QString imageName, bool success);
    void progress(QString imageName, QString stage, int attempt);

private:
    QProcess *process;
    int retryCount = 0;
    const int maxRetries = 5;
    QString currentImage;
    QString savePath;
    bool isPull = true;

private slots:
    void handleFinished(int exitCode, QProcess::ExitStatus);
    void retry();
};
#endif // CONTAINERWORKER_H
