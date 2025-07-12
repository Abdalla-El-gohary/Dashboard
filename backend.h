#ifndef BACKEND_H
#define BACKEND_H

#include <QObject>
#include <QString>
#include <QDebug>
#include "updater.h"
#include <QThread>
#include "zmqclient.h"
#include <QImage>
#include <QUrl>
#include <QTemporaryFile>
#include <QBuffer>
#include <QDir>
#include <QStandardPaths>
#include "commmanager.h"




class Backend : public QObject {
    Q_OBJECT
    Q_DISABLE_COPY(Backend);
    Q_PROPERTY(QString paragraphText READ paragraphText WRITE setParagraphText NOTIFY paragraphTextChanged)
    Q_PROPERTY(bool rebootState READ rebootState WRITE setRebootState NOTIFY rebootStateChanged)
    Q_PROPERTY(bool updateState READ updateState WRITE setUpdateState NOTIFY updateStateChanged)
    Q_PROPERTY(bool searchState READ searchState WRITE setSearchState NOTIFY searchStateChanged)
    Q_PROPERTY(int bootwidth READ bootwidth WRITE setBootwidth NOTIFY bootwidthChanged)
    Q_PROPERTY(QUrl frameUrl READ frameUrl NOTIFY frameUpdated)
public:
    explicit Backend(const QString &remote_repo,const QString& remote_registry, const QString &fota_utils_path,QObject *parent = nullptr) ;
    ~Backend() ;

    QString paragraphText() const ;
    void setParagraphText(const QString &text);

    Q_INVOKABLE void fetchUpdates();
    Q_INVOKABLE void installUpdate();
    //Q_INVOKABLE void publisher(QString butt);
    Q_INVOKABLE void sendprofileDate(QString name,QString password);
    Q_INVOKABLE void reboot();
    /* camera view */
    Q_INVOKABLE void openCamera();
    Q_INVOKABLE bool captureFront();
    Q_INVOKABLE bool captureLeft();
    Q_INVOKABLE bool captureRight();
    Q_INVOKABLE void cameraStop();
    Q_INVOKABLE void captureCanceled();
    Q_INVOKABLE QString readFile(const QString &filePath);
    Q_INVOKABLE bool writeFile(const QString &filePath, const QString &data);

    Q_INVOKABLE void userSelected(const QString &name) ;
    bool rebootState() const;
    void setRebootState(const bool &state);

    bool updateState() const;
    void setUpdateState(const bool &state);QUrl frameUrl() const;




    Q_INVOKABLE void feature_state_changed(const QString &name, bool state);
 private slots :
    /* Updater Slots */
    void onTextUpdated(QString text) ;
    bool searchState();
    void setSearchState(const bool &state);
    int bootwidth();
    void setBootwidth(const int width);

    /* Comm Slots */
    void onreplyReceived(const QString &msg) ;
    void oncaptureReceived(const QImage &image) ;
    void onUpdateProcessCompleted();
    void onUpdateProcessFailed();
public slots:
    void ondescriptionReady(QString description) ;
    void onImagesTakenSuccess();
    void updateFrame(const QImage &image);  // slot connected to CameraThread


signals:
    void captureFrontResult(bool result) ;
    void captureLeftResult(bool result) ;
    void captureRightResult(bool result) ;
    void paragraphTextChanged();
    void rebootStateChanged();
    void updateStateChanged();
    void searchStateChanged();
    void bootwidthChanged();
    /* frames */
    void frameUpdated();
    void capturingProfileImagesdone() ;
    /* user profile */
    void currentUserChanged(const QString &name);
    void userAuthorized(const QString& name);
private:
    void initializeReplyHandlers() ;
    QString m_paragraphText;
    bool rebootButton = false;
    bool updateButton = false;
    bool searchButton = true;

    int Bootwidth = 100; //should be from 0 to 400
    /* updater private members */
    Updater* updaterManagerPtr = nullptr ;
    QThread* thread = nullptr ;
    void stopWorkerThread();
    void startWorkerThread(const QString& text) ;
    ZmqClient* m_zmqclientPtr = nullptr;
    QThread*  m_clientthreadPtr = nullptr ;
    QString current_command ;
    QUrl m_frameUrl;
    CommManager* m_communicationmanager  = nullptr;
    /* flags */
    bool captured_front = false ;
    bool captured_right = false ;
    bool captured_left = false ;
    bool got_nack = false ;
    /* Comm Messages Handlers essential variables */

    using MessageHandler = std::function<void()>;
    QHash<QString, MessageHandler> ackHandlers;
    QHash<QString, MessageHandler> nackHandlers;
};

#endif
