#include "containerworker.h"


ContainerWorker::ContainerWorker(QObject *parent)
{

}

void ContainerWorker::startPull(const QString &imageName) {
    currentImage = imageName;
    isPull = true;
    retryCount = 0;
    process = new QProcess();
    connect(process, &QProcess::finished, this, &ContainerWorker::handleFinished);
    connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            process, &QObject::deleteLater);
    process->start("docker", QStringList() << "pull" << imageName);
    emit progress(imageName, "pulling", retryCount + 1);
}

void ContainerWorker::startSave(const QString &imageName, const QString &outputPath) {
    currentImage = imageName;
    savePath = outputPath;
    isPull = false;
    retryCount = 0;
    process = new QProcess();
    connect(process, &QProcess::finished, this, &ContainerWorker::handleFinished);
    connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            process, &QObject::deleteLater);
    process->start("docker", QStringList() << "save" << "-o" << outputPath << imageName);
    emit progress(imageName, "saving", retryCount + 1);
}

void ContainerWorker::handleFinished(int exitCode, QProcess::ExitStatus) {
    if (exitCode == 0) {
        if (isPull)
            emit pullFinished(currentImage, true);
        else
            emit saveFinished(currentImage, true);
    } else if (++retryCount < maxRetries) {
        emit progress(currentImage, isPull ? "pulling" : "saving", retryCount + 1);
        retry(); /* Retry again */
    } else {

        if (isPull)
            emit pullFinished(currentImage, false);
        else
            emit saveFinished(currentImage, false);
    }
}

void ContainerWorker::retry() {
    process->deleteLater();
    process = new QProcess();
    connect(process, &QProcess::finished, this, &ContainerWorker::handleFinished);
    connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            process, &QObject::deleteLater);
    if (isPull)
        process->start("docker", QStringList() << "pull" << currentImage);
    else
        process->start("docker", QStringList() << "save" << "-o" << savePath << currentImage);
}
