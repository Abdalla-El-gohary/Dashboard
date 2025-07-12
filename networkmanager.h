#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QObject>
#include <QUrl>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QAuthenticator>
#include <QNetworkProxy>
#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
class NetworkManager : public QObject
{
    Q_OBJECT
public:
    explicit NetworkManager(QObject *parent = nullptr);
    NetworkManager(const QString &remote_repo, const QString &remote_registry,QObject* parent = nullptr) ;
    ~NetworkManager() ;
    void getBinUpdate(const QJsonObject& ecu_info) ;
public slots:
    void getRemoteManifest() ;
    void getContainersMetadata() ;
    void get(const QString& location) ;
    void post(const QString& location, QByteArray data) ;

signals :
    void remoteManifestFetched(QJsonArray remoteManifest) ;
    void containersMetadataFetched(QJsonArray containersmetadata) ;
    void binFileDownloaded(QByteArray file_bytes,QString file_name);

private slots:
    void readyRead() ;
    void authenticationRequired(QNetworkReply *reply, QAuthenticator *authenticator);
    void encrypted(QNetworkReply *reply) ;
    void finished(QNetworkReply *reply);
    void preSharedKeyAuthenticationRequired(QNetworkReply *reply, QSslPreSharedKeyAuthenticator *authenticator);
    void proxyAuthenticationRequired(const QNetworkProxy &proxy, QAuthenticator *authenticator);
    void sslErrors(QNetworkReply *reply, const QList<QSslError> &errors);

signals:


private :
    QNetworkAccessManager networkmanager ;
    QString m_remoteRepoUrl ;
    QString m_remoteRegistryUrl ;
    QJsonArray m_containers_metadata ;
    QJsonArray* getManifestfromApiContent(QNetworkReply *reply) ;
    void getBinfromApiContent(QNetworkReply *reply,QByteArray& bin_file_ret);
    void parseContainersNamesfromApi(QNetworkReply* reply) ;
    void parseContainersDigestsfromApi(QNetworkReply* reply) ;
    void fetchContainersDigests() ;
    void fetchContainersNames() ;
    int digest_counter = 0 ;
};

#endif // NETWORKMANAGER_H
