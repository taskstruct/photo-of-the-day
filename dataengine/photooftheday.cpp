#include <QtCore/QHash>

#include <QDebug>

#include <KServiceTypeTrader>
#include <KSycoca>

#include "providercore.h"
#include "potddatacontainer.h"
#include "photooftheday.h"

K_EXPORT_PLASMA_DATAENGINE_WITH_JSON(photooftheday, PhotoOfTheDay, "plasma-dataengine-photooftheday.json")

PhotoOfTheDay::PhotoOfTheDay(QObject* parent, const QVariantList& args): Plasma::DataEngine(parent, args)
{    
    // set up Providers DataSource
    KService::List services = KServiceTypeTrader::self()->query(QLatin1String( "PhotoOfTheDay/Plugin" ));
    
    for( const KService::Ptr &service: services )
    {
        const QString provider = service->property(QLatin1String( "X-KDE-PhotoOfTheDayPlugin-Identifier" ), QVariant::String).toString();
        m_providers.insert(provider, service);
        setData( QLatin1String( "Providers"), provider, service->name() );
    }
    
    if( services.isEmpty() ) {
        setData( QLatin1String( "Providers" ), Plasma::DataEngine::Data() );
    }
    
    //TODO: Listen for changes with void KSycoca::databaseChanged (   const QStringList &     changedResources    )   
}

bool PhotoOfTheDay::sourceRequestEvent(const QString& identifier)
{   
    auto provider = providerForSource(identifier);

    if( provider == Q_NULLPTR ) {
        return false;
    }
    
    auto dc = new PotdDataContainer( provider, this);
    dc->setObjectName(identifier);
    dc->setStorageEnabled(true);
    
    addSource( dc );
    
    return true;
}

Plasma::Service* PhotoOfTheDay::serviceForSource(const QString& source)
{
    if( m_instances.contains(source) )
    {
    }
    else
    {
    }
    
    return Plasma::DataEngine::serviceForSource(source);
}

ProviderCore* PhotoOfTheDay::providerForSource(const QString& source)
{
    ProviderCore *instance = Q_NULLPTR;
    
    const QString provider = source.split(':').at(0);
    
    if( provider.isEmpty() ) {
        qDebug() << "Invalid source name: " << source;
        return Q_NULLPTR;
    }
    
    if( m_instances.contains(provider) )
    {
        instance = m_instances.value(provider);
        instance->addref();
    }
    else if( m_providers.contains(provider) )
    {
        // create instance
        
        qDebug() << "Creating instance of " << provider << " for source: " << source;
        
        QString error;
        
        instance = m_providers[provider]->createInstance<ProviderCore>( this, QVariantList(), &error );
        
        if( nullptr == instance ) {
            qDebug() << "Unable to create instance of " << m_providers[provider]->library() << " because " << error;
            return Q_NULLPTR;
        }
        
        m_instances.insert( provider, instance );
    }
    
    return instance;
}

void PhotoOfTheDay::unregisterProvider(const QString& source)
{
    const QString provider = source.split(':').at(0);
    
    auto pr = m_instances.value(provider);
    pr->deref();
    
    if(pr->refcount() == 0) {
        m_instances.remove(provider);
    }
}


#include "photooftheday.moc"