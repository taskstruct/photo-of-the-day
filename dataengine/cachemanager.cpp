#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QStandardPaths>

#include "cachemanager.h"

CacheManager* CacheManager::create(const QString& pid)
{
    CacheManager* cm = new CacheManager(pid);
    
    return cm;
}

CacheManager::CacheManager( const QString &pid ): m_provider(pid)
{   
    m_path = QStandardPaths::locate(QStandardPaths::DataLocation, QString(), QStandardPaths::LocateDirectory) + QLatin1String("/photooftheday/");
}

CacheManager::~CacheManager()
{}

bool CacheManager::save(const QString& imageName, const QByteArray& data, QString& cacheName)
{
    QString dataLocation = m_path + m_provider + '/';
    
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
