#ifndef CacheManager_H
#define CacheManager_H

class ProviderCore;

class CacheManager
{
public:
    explicit CacheManager(const ProviderCore* provder);
    ~CacheManager();

    void init(const ProviderCore* provder);

    void saveData( const Plasma::DataEngine::Data &data );
    void restoreData( Plasma::DataEngine::Data &data );

private:
};

#endif // CacheManager_H
