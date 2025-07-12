
#include "backend.h"
#include "backgroundworker.h"

Backend::Backend(const QString &remote_repo, const QString &remote_registry, const QString &fota_utils_path, QObject *parent) : QObject(parent), m_paragraphText("press search for new update")
{
    updaterManagerPtr = new Updater(remote_repo,remote_registry,fota_utils_path) ;
    connect(updaterManagerPtr,&Updater::descriptionReady,this,&Backend::ondescriptionReady) ;
    connect(updaterManagerPtr,&Updater::allContainersUpdated,this,&Backend::onUpdateProcessCompleted) ;
    connect(updaterManagerPtr,&Updater::containersUpdateFailed,this,&Backend::onUpdateProcessFailed);
    m_communicationmanager = new CommManager(this) ;
    connect(m_communicationmanager,&CommManager::recieved_reply,this,&Backend::onreplyReceived);
    initializeReplyHandlers();

}

Backend::~Backend()
{
    if (updaterManagerPtr != nullptr){
        delete updaterManagerPtr ;
        updaterManagerPtr = nullptr ;
    }
    if (m_clientthreadPtr!=nullptr){
        m_clientthreadPtr->deleteLater();
        m_clientthreadPtr = nullptr ;
    }
    if (m_communicationmanager != nullptr){
        delete m_communicationmanager ;
        m_communicationmanager = nullptr ;
    }
}

void Backend::setParagraphText(const QString &text) {
    if (m_paragraphText != text) {
        m_paragraphText = text;
        emit paragraphTextChanged();
    }

}


QString Backend::paragraphText () const
{
    return m_paragraphText;
}

void Backend::fetchUpdates()
{
    startWorkerThread("Fetching");
    setSearchState(false) ;
    updaterManagerPtr->fetchRemoteManifest() ;
}

void Backend::installUpdate()
{

    startWorkerThread("Updating") ;

    qDebug() << "Downloading Updates ..." ;
    QJsonArray scheduled_updates ;
    updaterManagerPtr->scheduleCurrentUpdates(scheduled_updates) ;

    if (scheduled_updates.isEmpty()){
        qInfo() << "No Scheduled Updates" ;
    }else{

        updaterManagerPtr->getUpdatesfromGithub(scheduled_updates);
    }

}


bool Backend::rebootState() const
{
    return rebootButton;
}


void Backend::setRebootState(const bool &state)
{
    if(rebootButton != state)
    {
        rebootButton = state;
        emit rebootStateChanged();
    }

}



    bool Backend::updateState() const
    {
        return updateButton;
    }
    void Backend::setUpdateState(const bool &state)
    {
        if(state != updateButton)
        {
            updateButton = state ;
            emit updateStateChanged();
        }
    }

    bool Backend::searchState()
    {
        return searchButton;
    }
    void Backend::setSearchState(const bool &state)
    {
        if(state != searchButton)
        {
            searchButton = state ;
            emit searchStateChanged();
        }
    }

    int Backend::bootwidth()
    {
        return Bootwidth;
    }
    void Backend::setBootwidth(const int width)
    {
        if(width != Bootwidth)
        {
            Bootwidth = width ;
            emit bootwidthChanged();
        }
    }

    void Backend::onreplyReceived(const QString &msg)
    {
        qDebug() << "in receive slot";

        if (msg == current_command + "_ack") {
            qDebug() << "Rec: 'ACK', current-command:" << current_command;
            if (ackHandlers.contains(current_command)) {
                ackHandlers[current_command]();
            } else {
                qWarning() << "Unhandled ACK for command:" << current_command;
            }
        }
        else if (msg == current_command + "_nack") {
            qDebug() << "Rec: 'NACK', current-command:" << current_command;
            if (nackHandlers.contains(current_command)) {
                nackHandlers[current_command]();
            } else {
                qWarning() << "Unhandled NACK for command:" << current_command;
            }
        }
        else {
            qDebug() << "Rec: 'Invalid message'" << current_command << "msg:" << msg;
        }
    }

void Backend::oncaptureReceived(const QImage &image)
{
    updateFrame(image);
}

void Backend::onUpdateProcessCompleted()
{
    stopWorkerThread();
    setParagraphText("Updated Successfuly");
    setRebootState(true);
    setUpdateState(false) ;
}
void Backend::onUpdateProcessFailed(){

    stopWorkerThread();
    setParagraphText("Update Failed: Try Again");
}

    void Backend::onImagesTakenSuccess()
    {
     qDebug() << "Received Close signal" ;
    }


void Backend::ondescriptionReady(QString description)
{

    stopWorkerThread();
    setParagraphText(description) ;
    qDebug() << description ;
    if (description != "No Update Found.\n"){
        setSearchState(false) ;
        setUpdateState(true) ;
    }else{
        setSearchState(true) ;
    }

}

void Backend::stopWorkerThread(){
    if (thread && thread->isRunning()){
        thread->quit() ;
        thread->wait() ;
        thread = nullptr ;
    }
}

void Backend::startWorkerThread(const QString &text)
{
    /* Set Update Description */
    if (thread && thread->isRunning()) {
        thread->quit();
        thread->wait();
        delete thread;
        thread = nullptr;
    }
    thread = new QThread;
    BackgroundWorker* worker = new BackgroundWorker();
    worker->setText(text);
    QObject::connect(worker, &BackgroundWorker::updateText, this, &Backend::onTextUpdated);
    worker->moveToThread(thread);
    QObject::connect(thread, &QThread::started, worker, &BackgroundWorker::startWorker);
    QObject::connect(this, &Backend::destroyed, thread, &QThread::quit);
    QObject::connect(thread, &QThread::finished, worker, &QObject::deleteLater);
    QObject::connect(thread, &QThread::finished, thread, &QObject::deleteLater);

    thread->start();
}
void Backend::onTextUpdated(QString text)
{
    setParagraphText(text) ;
}





 QString Backend::readFile(const QString &filePath) {
    QFile file("../../users/users.json");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        //qWarning() << "Cannot open file for reading:"        return "";
    }
    QTextStream in(&file);
    QString content = in.readAll();
    file.close();
    return content;
}

bool Backend::writeFile(const QString &filePath, const QString &data) {
    QFile file("../../users/users.json");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Cannot open file for writing:" << filePath;
        return false;
    }
    QTextStream out(&file);
    out << data;
    file.close();
    return true;
}






void Backend::sendprofileDate(QString name,QString password)
{
    // m_client.send("username:"+name+"pass:"+password) ;
    QJsonObject user_metadata ;
    user_metadata["username"] = name ;
    user_metadata["password"] = password ;
    QJsonDocument usermeta_doc(user_metadata);
    QString metadata = usermeta_doc.toJson(QJsonDocument::Compact);
    qInfo() << metadata ;
    current_command = "user_metadata" ;
    m_communicationmanager->SendCaptureRequest(metadata);

}

    void Backend::reboot()
    {
        /* start sending reboot signals to all ecus then reboot myself */
        updaterManagerPtr->preRebootSequence() ;
    }

    void Backend::openCamera()
    {
        qDebug() << "*** Opened Camera *** " ;
        qDebug() << "Running Backend on" << QThread::currentThread() ;
        current_command = "start_add_profile" ;
        m_communicationmanager->StartAddProfileClient();
        m_communicationmanager->sendRequest(current_command);


    }


void Backend::cameraStop()
{
   /* */
    qInfo() << "camera stop" ;
}


void Backend::captureCanceled()
{

    current_command = "cancel_capture" ;
    m_communicationmanager->SendCaptureRequest(current_command);

}

void Backend::userSelected(const QString &name)
{
    qInfo() << "User:" << name ;
    current_command = "start_login" ;
    m_communicationmanager->sendRequest("start_login") ;
    m_communicationmanager->addSubscriber("capture",ADD_PROFILE_SUB_URL);
    emit currentUserChanged(name);
    /* add subscriber to publishes of hatem */

   //emit userAuthorized(name);
}


QUrl Backend:: frameUrl() const {
    return m_frameUrl;
}

void Backend::feature_state_changed(const QString &name, bool state)
{
    qDebug() << "Feature" << name << "is now" << (state ? "ON" : "OFF");

    if (name == "Voice Command"){
        current_command = "start_voice_command" ;
        m_communicationmanager->sendRequest(current_command);
    }
}


void Backend::updateFrame(const QImage &image) {
    QString path = QStandardPaths::writableLocation(QStandardPaths::TempLocation)
    + "/" + QUuid::createUuid().toString() + ".jpg";
    image.save(path, "JPG");
    m_frameUrl = QUrl::fromLocalFile(path);
    emit frameUpdated();
}


bool Backend::captureFront()
{
    //consider if it called more than one for one user if he press try agian handle it
    current_command = "capture_front" ;
    m_communicationmanager->SendCaptureRequest("capture_front") ;
    return true  ;
}
bool Backend::captureLeft()
{   current_command = "capture_left" ;
     m_communicationmanager->SendCaptureRequest("capture_left") ;
    return true  ;
}
bool Backend::captureRight()
{   current_command = "capture_right" ;
    m_communicationmanager->SendCaptureRequest("capture_right") ;
    return true ;
}

void Backend::initializeReplyHandlers() {
    // ACK Handlers
    ackHandlers["start_add_profile"] = [this]() {
        m_communicationmanager->addSubscriber(ADD_PROFILE_TOPIC, ADD_PROFILE_SUB_URL);
        connect(m_communicationmanager, &CommManager::captureReceived, this, &Backend::oncaptureReceived);
        qInfo() << "Start Taking Frames";
    };
    ackHandlers["capture_front"] = [this]() {
        captured_front = true;
        emit captureFrontResult(captured_front);
    };
    ackHandlers["capture_left"] = [this]() {
        captured_left = true;
        emit captureLeftResult(captured_left);
    };
    ackHandlers["capture_right"] = [this]() {
        captured_right = true;
        emit captureRightResult(captured_right);
        m_communicationmanager->removeSubscriber(ADD_PROFILE_TOPIC);
    };
    ackHandlers["cancel_capture"] = [this]() {
        qInfo() << "Capture Cancelled Successfully" ;
    };

    ackHandlers["user_metadata"] = [this]() {
        qInfo() << "Received: user_metadata ack";
        current_command = "store_to_db" ;
        m_communicationmanager->SendCaptureRequest(current_command);
    };
    ackHandlers["store_to_db"] = [this]() {
        qInfo() << "User Stored to Database Successfully";
    };

    ackHandlers["start_voice_command"] = [this]() {
        m_communicationmanager->addSubscriber("VOICE_COMMAND_URL", "URL");
    };

    // NACK Handlers
    nackHandlers["capture_front"] = [this]() {
        emit captureFrontResult(captured_front);
    };
    nackHandlers["capture_left"] = [this]() {
        emit captureLeftResult(captured_left);
    };
    nackHandlers["capture_right"] = [this]() {
        emit captureRightResult(captured_right);
    };
    nackHandlers["store_to_db"] = [this]() {
        qInfo() << "User Can't be stored in Database";
    };
    nackHandlers["cancel_capture"] = [this]() {
        qInfo() << "Capture Cancelled Failed" ;
    };

}
