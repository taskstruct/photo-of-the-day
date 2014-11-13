#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QStandardPaths>

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlRecord>

#include "cachemanager.h"

CacheManager* CacheManager::create(const QString& pid, const QString& createStatement)
{
    CacheManager* cm = new CacheManager(pid);
    
    return cm;
}

CacheManager::CacheManager( const QString &pid ): m_provider(pid)
{   
    QString dbLocation = QStandardPaths::locate(QStandardPaths::DataLocation, QString(), QStandardPaths::LocateDirectory);
    dbLocation += QLatin1String("/photooftheday/") + m_provider + ".db";
    
    QSqlDatabase db = QSqlDatabase::addDatabase( QLatin1String("QSQLITE"), m_provider );
    db.setDatabaseName(dbLocation);
    
    db.open();
}

CacheManager::~CacheManager()
{}

bool CacheManager::isValid() const
{
    return true;
}

int CacheManager::insert(const QSqlQuery& q)
{
    QSqlDatabase db = QSqlDatabase::database(m_provider);
    
    if( !db.isValid() || !db.isOpen() ) {
        return -1;
    }
    
    db.exec();
    
    return 1;
}

QSqlRecord CacheManager::get()
{
    return QSqlRecord();
}

bool CacheManager::save(const QString& imageName, const QByteArray& data, QString& cacheName)
{
    QString dataLocation = QStandardPaths::locate(QStandardPaths::DataLocation, QString(), QStandardPaths::LocateDirectory);
    
    dataLocation += QLatin1String("/photooftheday/") + m_provider + '/';
    
    cacheName.clear();
    
    QDir dataDir;
    
    if( !dataDir.exists(dataLocation) )
    {
        if( !dataDir.mkpath(dataLocation) ) {
            return false;
        }
    }
        
    cacheName = dataLocation + imageName;
    QFile imageFile( cacheName );
    
    if( imageFile.exists() ) {
        return false;
    }
    
    if( !imageFile.open( QIODevice::WriteOnly ) ) {
        return false;
    }
    
    quint64 fileSize = imageFile.write( data );
    
    return ( fileSize == data.size() );
}


#include "cachemanager.moc"
