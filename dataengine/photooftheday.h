#ifndef PHOTOOFTHEDAY_H
#define PHOTOOFTHEDAY_H

#include <Plasma/DataEngine>

class ProviderCore;

class PhotoOfTheDay: public Plasma::DataEngine
{
    Q_OBJECT
    
public:
    PhotoOfTheDay(QObject* parent, const QVariantList& args);
        
protected:
    bool sourceRequestEvent( const QString &identifier );
    Q_INVOKABLE Plasma::Service *serviceForSource(const QString &source);
    
private:
    QHash<QString, KService::Ptr> m_providers;
    
    ///@brief 
    QHash<QString, ProviderCore*> m_instances;
};

#endif // PHOTOOFTHEDAY_H