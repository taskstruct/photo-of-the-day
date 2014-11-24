#ifndef CacheManager_H
#define CacheManager_H

#include "photooftheday_export.h"

class PHOTOOFTHEDAY_EXPORT CacheManager
{
public:
    static CacheManager* create( const QString& pid );
    
    ~CacheManager();
    
    // image cache
    bool save(const QString& imageName, const QByteArray& data, QString& cacheName );

private:
    CacheManager(const QString& pid);
    
    QString m_provider;
    QString m_path;
};

#endif // CacheManager_H
