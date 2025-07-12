#include "networkmanager.h"

NetworkManager::NetworkManager(QObject *parent)
    : QObject{parent}
{

}

NetworkManager::NetworkManager(const QString &remote_repo, const QString &remote_registry,QObject* parent): QObject{parent}
{
    m_remoteRepoUrl = remote_repo ;
    m_remoteRegistryUrl = remote_registry ;
    connect(&networkmanager,&QNetworkAccessManager::authenticationRequired,this,&NetworkManager::authenticationRequired);
    connect(&networkmanager,&QNetworkAccessManager::preSharedKeyAuthenticationRequired,this,&NetworkManager::preSharedKeyAuthenticationRequired);
    connect(&networkmanager,&QNetworkAccessManager::proxyAuthenticationRequired,this,&NetworkManager::proxyAuthenticationRequired);
    connect(&networkmanager,&QNetworkAccessManager::encrypted,this,&NetworkManager::encrypted);
    connect(&networkmanager,&QNetworkAccessManager::finished,this,&NetworkManager::finished);
}

NetworkManager::~NetworkManager()
{
    qInfo() << "Network Manager Destructed " ;
}

void NetworkManager::getBinUpdate(const QJsonObject &ecu_info){
    QString ecuid =  ecu_info.value("ecuid").toString() ;
    QString ecu_name =  ecu_info.value("name").toString() ;
    QString apiUrl = QString(m_remoteRepoUrl+"ecu_%1_%2/%2.bin").arg(ecuid,ecu_name);
    get(apiUrl);
}

void NetworkManager::getRemoteManifest()
{
/* Get Remote Manifest */
get(m_remoteRepoUrl + QString("/manifest.json")) ;
}

void NetworkManager::getContainersMetadata()
{
    /* get the containers name */
    fetchContainersNames() ;
}

void NetworkManager::get(const QString& location)
{
    qInfo() << "Debug: Getting from:" << location ;
    QNetworkReply* reply = nullptr;
    if (location.startsWith(m_remoteRepoUrl)){
        qInfo() << "Debug: Gett Req to Github Repo ... " ;
        QNetworkRequest request((QUrl(location)));
        request.setRawHeader("Authorization", "Bearer  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");
        request.setHeader(QNetworkRequest::UserAgentHeader, "QtApp");

        reply = networkmanager.get(request);

    }else if (location.startsWith(m_remoteRegistryUrl)){
    /* Registry Fetching (Containers Names or Containers Digest) */
        qInfo() << "Debug: Get Req to GHCR ... " ;
        QNetworkRequest request((QUrl(location)));
        request.setRawHeader("Authorization", "Bearer  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");
        request.setRawHeader("Accept", "application/vnd.github+json");

        reply = networkmanager.get(request);
    }
    else{
        reply = networkmanager.get(QNetworkRequest(QUrl(location)));
    }
    connect(reply,&QNetworkReply::readyRead,this,&NetworkManager::readyRead);

}

void NetworkManager::post(const QString& location, QByteArray data)
{
    qInfo() << "Debug: Posting to: " << location ;
    QNetworkRequest request = QNetworkRequest(QUrl(location));
    request.setHeader(QNetworkRequest::ContentTypeHeader,"test/plain");
    QNetworkReply* reply = networkmanager.post(request,data);
    connect(reply,&QNetworkReply::readyRead,this,&NetworkManager::readyRead);

}

void NetworkManager::readyRead()
{
    qInfo() << "Debug: Ready Read Reply: "  ;
    QNetworkReply* reply = static_cast<QNetworkReply*> (sender()) ;

    if (reply){
        QString url = reply->url().toString() ;
        if (url.startsWith(m_remoteRepoUrl)){
            if (reply->error() == QNetworkReply::NoError){
                if (url == (m_remoteRepoUrl+"/manifest.json")){

                    QJsonArray* ecuListPtr = getManifestfromApiContent(reply) ;
                    if (ecuListPtr != nullptr){
                        //qInfo() << *ecuListPtr ;
                        emit  remoteManifestFetched(*ecuListPtr) ;
                        delete ecuListPtr ;
                    }
                }else if (url.endsWith(".bin")){
                    QStringList parsing_url = url.split("/") ;
                    if (parsing_url.count() != 0){
                        QString file_name = parsing_url.at(parsing_url.count()-1) ;
                        QByteArray downloaded_file ;
                        getBinfromApiContent(reply,downloaded_file);
                        emit binFileDownloaded(downloaded_file,file_name);
                    }
                    /* fetch binary file */
                }
            }else{
                qInfo() << "Error in Reply:" << reply->errorString() ;
            }
        }else if (reply->url().toString()== (m_remoteRegistryUrl + "?package_type=container")){
            if (reply->error() == QNetworkReply::NoError){
            qInfo() << "Getting Containers Names" ;
            parseContainersNamesfromApi(reply) ;
            }else{
               qInfo() << "Error in Reply:" << reply->errorString() ;
            }
        }else if (reply->url().toString().contains("versions")){
            if (reply->error() == QNetworkReply::NoError){
                qInfo() << "Getting Containers Digests" ;
                parseContainersDigestsfromApi(reply) ;
            }else{
                qInfo() << "Error in Reply:" << reply->errorString() ;
            }

        }else{
            qInfo() << reply->readAll() ;
        }
    }else{

        qInfo() << "No Reply ... " ;
    }
}


void NetworkManager::authenticationRequired(QNetworkReply *reply, QAuthenticator *authenticator)
{
    qInfo() << "Need Authentication !!" ;
    Q_UNUSED(reply) ;
    Q_UNUSED(authenticator);
}

void NetworkManager::encrypted(QNetworkReply *reply)
{
    Q_UNUSED(reply) ;
}

void NetworkManager::finished(QNetworkReply *reply)
{
    qInfo() << "Finished .. " ;
    if (reply->url().toString()== (m_remoteRegistryUrl + "?package_type=container")){
        reply->deleteLater() ;
        /* end fetching the names of containers */
        fetchContainersDigests() ;
    }else if (reply->url().toString().contains("versions")){
            digest_counter++  ;
            if (digest_counter == m_containers_metadata.count()){
                digest_counter = 0 ;
                emit containersMetadataFetched(m_containers_metadata);
            }
    }else{
        reply->deleteLater() ;
    }
}

void NetworkManager::preSharedKeyAuthenticationRequired(QNetworkReply *reply, QSslPreSharedKeyAuthenticator *authenticator)
{
    Q_UNUSED(reply) ;
    Q_UNUSED(authenticator);
}

void NetworkManager::proxyAuthenticationRequired(const QNetworkProxy &proxy, QAuthenticator *authenticator)
{
    Q_UNUSED(proxy) ;
    Q_UNUSED(authenticator);
}

void NetworkManager::sslErrors(QNetworkReply *reply, const QList<QSslError> &errors)
{
    Q_UNUSED(reply) ;
    Q_UNUSED(errors);
}

void NetworkManager::parseContainersNamesfromApi(QNetworkReply *reply)
{
    QByteArray responseData = reply->readAll();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);

    if (!jsonDoc.isArray()) {
        qDebug() << "Invalid JSON structure!";
        reply->deleteLater();
    }

    QJsonArray containersarr = jsonDoc.array();
    QJsonObject iter_obj  ;
    for (int i = 0 ; i< containersarr.count() ; i++){
        QJsonObject obj = containersarr.at(i).toObject();
        QString name = obj["name"].toString();
        iter_obj["name"] = name ;
        m_containers_metadata.append(iter_obj) ;
        qInfo() << name ;
    }
}

void NetworkManager::parseContainersDigestsfromApi(QNetworkReply *reply)
{
    QStringList url_list = reply->url().toString().split("/");
    /* Container Name is the */
    QString container_name = url_list.at(7);
    qInfo() << "container_name:" <<container_name ;
    QByteArray responseData = reply->readAll();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);

    if (!jsonDoc.isArray()) {
        qDebug() << "Invalid JSON structure!";
        qInfo() << "in" << reply->url().toString();
        reply->deleteLater();
    }
    else {
        QJsonObject container_metadata = jsonDoc.array().at(0).toObject();
        QJsonObject iter_object ;
        for (int i = 0 ; i< m_containers_metadata.count();i++){
            iter_object = m_containers_metadata.at(i).toObject() ;
            if (iter_object.contains("name") && (iter_object.value("name").toString() == container_name)){
                iter_object["digest"] = container_metadata["name"].toString() ;
                iter_object["tags"] = container_metadata["metadata"].toObject()["container"].toObject()["tags"].toArray();
                m_containers_metadata[i] = iter_object ;
            }
        }
    }
}

void NetworkManager::getBinfromApiContent(QNetworkReply *reply,QByteArray& bin_file_ret)
{
    QByteArray responseData = reply->readAll();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);

    if (!jsonDoc.isObject()) {
        qDebug() << "Invalid JSON structure!";
        reply->deleteLater();
    }

    QJsonObject rootObj = jsonDoc.object();

    /* extract content field */
    QString encodedContent = rootObj.value("content").toString();
    QByteArray decodedContent = QByteArray::fromBase64(encodedContent.toUtf8());

    bin_file_ret = decodedContent ;
}

QJsonArray* NetworkManager::getManifestfromApiContent(QNetworkReply *reply)
{
    QByteArray responseData = reply->readAll();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);

    if (!jsonDoc.isObject()) {
        qDebug() << "Invalid JSON structure!";
        reply->deleteLater();
        return nullptr ;
    }

    QJsonObject rootObj = jsonDoc.object();

    /* extract content field */
    QString encodedContent = rootObj.value("content").toString();
    QByteArray decodedContent = QByteArray::fromBase64(encodedContent.toUtf8());
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(decodedContent, &parseError);

    if (parseError.error == QJsonParseError::NoError && doc.isArray()) {
        qInfo() << "No Errors in remote manifest" ;
        QJsonArray* ecuListPtr = new QJsonArray(doc.array());
        qInfo() << *ecuListPtr;
        return ecuListPtr ;
    }else{

        qInfo() << "Error in Json Format"  ;
        qInfo() << "Reply:" << decodedContent;
        return nullptr ;
    }
}

void NetworkManager::fetchContainersDigests()
{
    for (int i = 0 ; i < m_containers_metadata.count() ;i++){
        QString container_name = m_containers_metadata.at(i)["name"].toString() ;
        qInfo() <<"getting digest of :"<<container_name ;
        get(m_remoteRegistryUrl+(QString("/container/%1/versions").arg(container_name))) ;
    }
}

void NetworkManager::fetchContainersNames()
{
    /* get request to fetch containers names */
    get(m_remoteRegistryUrl+"?package_type=container") ;
}
