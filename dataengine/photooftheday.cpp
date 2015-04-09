#include <QtCore/QHash>

#include <QDebug>

#include <KServiceTypeTrader>
#include <KSycoca>

#include "providercore.h"
#include "potddatacontainer.h"
#include "photooftheday.h"

K_EXPORT_PLASMA_DATAENGINE_WITH_JSON(photooftheday, PhotoOfTheDay, "plasma-dataengine-photooftheday.json");

static constexpr QLatin1String _providersSourceName("Providers");

inline const QString providerFromSource(const QString &source) {
    return source.split(':').at(0);
}

PhotoOfTheDay::PhotoOfTheDay(QObject* parent, const QVariantList& args): Plasma::DataEngine(parent, args)
{    
    // set up Providers DataSource
    KService::List services = KServiceTypeTrader::self()->query(QLatin1String( "PhotoOfTheDay/Plugin" ));
    
    for( const KService::Ptr &service: services )
    {
        const QString provider = service->property(QLatin1String( "X-KDE-PhotoOfTheDayPlugin-Identifier" ), QVariant::String).toString();
        m_availableProviders.insert(provider, service);
        setData( _providersSourceName, provider, service->name() );
    }
    
    if( services.isEmpty() ) {
        setData( _providersSourceName, Plasma::DataEngine::Data() );
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
    
    if( provider.isEmpty() )
    {
        qDebug() << "Invalid source name: " << source;
        return instance;
    }
    
    if( m_instances.contains(provider) )
    {
        instance = m_instances.value(provider);
        return instance;
    }

    if( m_availableProviders.contains(provider) )
    {
        // create instance
        
        qDebug() << "Creating instance of " << provider << " for source: " << source;
        
        QString error;
        
        instance = m_availableProviders[provider]->createInstance<ProviderCore>( this, QVariantList(), &error );
        
        if( nullptr == instance ) {
            qDebug() << "Unable to create instance of " << m_availableProviders[provider]->library() << " because " << error;
            return Q_NULLPTR;
        }
        
        m_instances.insert( provider, instance );

        connect( instance, &ProviderCore::unused, [this]() {
            auto provider = sender();

            auto iter = this->m_instances.begin();
            auto last = this->m_instances.end();

            while( iter != last ) {
                if( iter.value() == provider ) {
                    iter.value()->deleteLater();
                    this->m_instances.erase(iter);
                    break;
                }

                ++iter;
            }

        } );
    }
    
    return instance;
}


#include "photooftheday.moc"
