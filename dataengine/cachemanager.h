#ifndef CacheManager_H
#define CacheManager_H

#include "photooftheday_export.h"

class QSqlQuery;
class QSqlRecord;

class PHOTOOFTHEDAY_EXPORT CacheManager
{
public:
    static CacheManager* create( const QString& pid, const QString& createStatement );
    
    ~CacheManager();
    
    bool isValid() const;

    // SQL cache
    int insert( const QSqlQuery& q );
    QSqlRecord get();
    
    // image cache
    bool save(const QString& imageName, const QByteArray& data, QString& cacheName );

private:
    CacheManager(const QString& pid);
    
    QString m_provider;
};

#endif // CacheManager_H
