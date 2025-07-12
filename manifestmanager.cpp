#include "manifestmanager.h"

ManifestManager::ManifestManager(QObject *parent)
    : QObject{parent}
{}

ManifestManager::ManifestManager(QString manifestpath)
{
    this->manifestpath = manifestpath ;
    readManifest(manifestarr) ;
    /* enable reading from cache */
    this->initialized = true  ;
}

ManifestManager::~ManifestManager()
{
    /* Destructor */
    qInfo() << "Manifest Manager: Destructed" ;
}

bool ManifestManager::updateLocalManifest(const QString ecuid, const QString matchkey, const QJsonValue &value)
{
    QJsonObject currentobj ;
    bool modified = false ;
    for (int i = 0 ; i < manifestarr.count() ; i++){
        currentobj = manifestarr[i].toObject() ;
        if (currentobj["ecuid"] == ecuid){
            if (currentobj.contains(matchkey)  && currentobj[matchkey] != value){
                currentobj[matchkey] = value ;
                modified = true ;
                manifestarr[i] = currentobj ;
                if (writeManifest(manifestarr)) {
                    qDebug() << "updated successfuly ... " ;
                }
            }
        }

    }
    return modified ;
}

bool ManifestManager::updateLocalManifest(const QJsonObject &obj)
{   QJsonObject currentobj ;
    bool modified = false ;
    int index = -1 ;
    if (obj.contains("ecuid")){

        for (int i = 0 ; i < manifestarr.count() ; i++){
            currentobj = manifestarr[i].toObject() ;
            if (currentobj["ecuid"] == obj["ecuid"]){
                index = i  ;
                /* loop on the current keys in argumented object */
                foreach (const QString &key,obj.keys()){
                    /* make sure that current ecu has the key and need to be changed */
                    if (obj.contains(key) && currentobj.contains(key) && currentobj[key] != obj[key]) {
                        /* update current object */
                        currentobj[key] = obj[key];
                        modified = true;
                    }
                }
                if (modified){
                    /* if desired object modified get out from the loop */
                    break ;
                }
            }
        }
        if (index == -1 ){
            qCritical() << "Error: ecuid not found" ;
            return false ;
        }

        if (modified){
            manifestarr[index] = currentobj ;
            /* flush new manifest data */
            if (writeManifest(manifestarr)) {
                qDebug() << "updated successfuly ... " ;
            }
        }
    }else{
        qCritical() << "Invalid object format: has no ecuid key" ;
    }
    return modified ;
}

void ManifestManager::readManifest(QJsonArray& manifestarr){
    if (initialized){
        /* Cached Version */
        manifestarr = this->manifestarr ;
    }else{
        QFile file(manifestpath);
        if (file.open(QIODevice::ReadOnly)){
            /* read all data in the file */
            QByteArray bytes = file.readAll()  ;
            /* close file after read */
            file.close() ;
            /* error check in json syntax */
            QJsonParseError JsonError;
            QJsonDocument manifestdoc  = QJsonDocument::fromJson(bytes,&JsonError) ;
            if (JsonError.error != QJsonParseError::NoError){
                qCritical() << "Error in Manifest Json File (JSonError): " << JsonError.errorString();
                exit(-1) ;

            }else{
                qCritical() << "No Error in Manifest Json File" ;
                if (manifestdoc.isArray()){
                    qInfo() << "Manifest File Ready to read" ;
                    manifestarr = manifestdoc.array() ;
                }else{
                    qCritical() << "Manifest File is not in the right format" ;
                    exit(-1) ;
                }
            }



        }else{
            qCritical() << manifestpath ;
            qCritical() << "Error opening the Manifest: " << file.errorString() ;
            exit(-1) ;
        }
    }
}

void ManifestManager::changeManifestState(const QString &currentstate, const QString &newstate)
{   bool modified  = false  ;
    QJsonObject iter_object ;
    for (int i = 0 ; i < manifestarr.count() ; i++){
        iter_object = manifestarr[i].toObject() ;
        if (iter_object.contains("state") && iter_object.value("state").toString() == currentstate){
            iter_object["state"] = newstate ;
            manifestarr[i] = iter_object ;
            modified = true ;
        }
    }
    if (modified){
        /* flush new manifest data */
        if (writeManifest(manifestarr)) {
            qDebug() << "Local Manifest updated successfuly ... " ;
        }
    }
}

void ManifestManager::searchforEcusbyState(QJsonArray& foundecus, const QString &state)
{
    QJsonObject iter_object ;
    for (int i = 0 ; i < manifestarr.count() ; i++){
        iter_object = manifestarr[i].toObject() ;
        if (iter_object.contains("state") && iter_object.value("state").toString() == state){
            foundecus.append(iter_object) ;
        }
    }
}

void ManifestManager::updateContainersManifest(const QJsonObject &obj)
{   QJsonObject iter_obj ;
    bool found = false  ;
    for (int i = 0 ; i< manifestarr.count() ;i++){
        iter_obj = manifestarr.at(i).toObject() ;
        if ((obj.contains("name")) &&
           (obj.value("name").toString() == iter_obj.value("name").toString())){
            manifestarr[i] = obj ;
            found = true ;
        }
    }

    if (found == false){
        /* if not found in containers manifest */
        manifestarr.append(obj) ;
    }

    if (writeManifest(manifestarr) == 0 ){
        qInfo() << "Containers Manifest Updated" ;
    }
}

bool ManifestManager::getECUsfromManifestbyState(QJsonArray &returned_arr, const QString &state)
{   QJsonObject iter_obj ;
    bool found = false ;
    for (int i = 0; i < manifestarr.count() ;i++){
        iter_obj = manifestarr.at(i).toObject() ;
        if (iter_obj.contains("state") && iter_obj.value("state") == state){
            returned_arr.append(iter_obj) ;
            found = true ;
        }
    }
    return found ;
}


int ManifestManager::writeManifest(QJsonArray& updatedManifest)
{
    QFile manifestfile(manifestpath) ;
    if (manifestfile.open(QIODevice::WriteOnly)){
        QJsonDocument manifestdoc(updatedManifest) ;
        manifestfile.resize(0) ;
        if (manifestfile.write(manifestdoc.toJson())) {
            qInfo() << "manifest written successfully";
        }else{
            qCritical() << "Nothing written to manifest file" ;
            return -2 ;
        }
        manifestfile.close() ;
    }else{
        qCritical() << "Can't Open Manifest File" ;
        return -1 ;
    }
    return 0  ;
}
