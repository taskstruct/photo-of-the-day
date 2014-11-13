#include <QtCore/QHash>

#include <QDebug>

#include <KServiceTypeTrader>
#include <KSycoca>

#include "providercore.h"
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
}

bool PhotoOfTheDay::sourceRequestEvent(const QString& identifier)
{
    const QStringList idSplit = identifier.split(':', QString::SkipEmptyParts);
    
    if( ( idSplit.count() != 2 ) || idSplit[0].isEmpty() || idSplit[1].isEmpty() ) {
        qDebug() << "Invalid source name: " << identifier;
        return false;
    }
    
    const QString provider = idSplit[0];
    const QString appletId = idSplit[1];
    
    qDebug() << "identifier = " << identifier;
    
    
    // create instance
    if( m_providers.contains(provider) )
    {
        qDebug() << "Creating instance of " << provider << " for source: " << identifier;
        
        QString error;
        ProviderCore* instance = m_providers[provider]->createInstance<ProviderCore>( this, QVariantList(), &error );
        
        if( nullptr == instance ) {
            qDebug() << "Unable to create instance of " << m_providers[provider]->library() << " because " << error;
            return false;
        }
        
        instance->setObjectName( identifier );
        
        m_instances.insert( identifier, instance );
        
        connect( instance, &ProviderCore::photoReady, this, [this](const QString&source, const Plasma::DataEngine::Data& data) { 
                   setData( source, data );
        } );
        
        connect( this, &PhotoOfTheDay::sourceRemoved, this, [this](const QString& source)
        {               
            if( m_instances.contains( source ) )
            {
                qDebug()<< "SOURCE REM " << source;
                m_instances[source]->deleteLater();
                m_instances.remove(source);
            }
        } );
        
        instance->requestPhoto(identifier);
        
        // create empty source until HTTP requests finished
        setData( identifier, DataEngine::Data() );
        
        return true;
    }
    else
    {
        qDebug() << provider << "not found";
    }
    
    return false;
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


#include "photooftheday.moc"