#ifndef MANIFESTMANAGER_H
#define MANIFESTMANAGER_H

#include <QObject>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QString>
#include <QFile>
#include <QDebug>

/* Definition of ECUID of ECUs */
#define     ECUID_CENTRAL       "00"
#define     ECUID_FRONT         "01"
#define     ECUID_REAR          "02"
#define     ECUID_AFS           "11"
#define     ECUID_MC            "12"

class   ManifestManager : public QObject
{
    Q_OBJECT
public:
    explicit ManifestManager(QObject *parent = nullptr);
    ManifestManager(QString manifestfile) ;
    ~ManifestManager() ;
    void readManifest(QJsonArray& manifest) ;
    int writeManifest(QJsonArray& updatedManifest) ;

    bool updateLocalManifest(const QString ecuid, const QString matchkey,const QJsonValue &value) ;
    bool updateLocalManifest(const QJsonObject& obj) ;

    void changeManifestState(const QString& currentstate, const QString& newstate) ;
    void searchforEcusbyState(QJsonArray& foundecus,const QString& state);
    /* Container Manifest Functions */
    void updateContainersManifest(const QJsonObject & obj) ;
    bool getECUsfromManifestbyState(QJsonArray& returned_arr,const QString& state) ;
signals:
private :
    QString manifestpath ;
    QJsonArray manifestarr ;
    bool initialized = false ;

};


#endif // MANIFESTMANAGER_H
