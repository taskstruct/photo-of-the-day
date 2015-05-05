#include <QtCore/QDir>
#include <QtCore/QHash>
#include <QtCore/QStandardPaths>

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
        setData( _providersSourceName, provider, service->name() );
    }
    
    if( services.isEmpty() ) {
        setData( _providersSourceName, Plasma::DataEngine::Data() );
    }
    
    //TODO: Listen for changes with void KSycoca::databaseChanged (   const QStringList &     changedResources    )   

    // remove unused providers
    connect( this, &PhotoOfTheDay::sourceRemoved, [this](const QString &source) {
        if( this->m_instances.contains(source) ) {
            auto provider = this->m_instances.value(source);
            this->m_instances.remove(source);
            provider->deleteLater();
        }
    } );

    // create our cache directory
    QString genericLocation = QStandardPaths::writableLocation(QStandardPaths::GenericCacheLocation);
    QDir(genericLocation).mkdir(QLatin1String("photo-of-the-day"));
}

bool PhotoOfTheDay::sourceRequestEvent(const QString& source)
{   
    QString constraint = QString(QLatin1String("[X-KDE-PhotoOfTheDayPlugin-Identifier] == '%1'")).arg(source);

    KService::List services = KServiceTypeTrader::self()->query(QLatin1String( "PhotoOfTheDay/Plugin" ), constraint );

    if( services.size() != 1 )
    {
        // there should be only one provider with that name
        return false;
    }

    QString error;

    auto provider = services.at(0)->createInstance<ProviderCore>( this, QVariantList(), &error );

    if(!provider) {
        qDebug() << "Unable to create instance of " << services.at(0)->library() << " because " << error;
        return false;
    }

    m_instances.insert( source, provider );

    provider->setObjectName(source);
    provider->restore( 10000 /*TODO: How to get interval??? */ );

    setData( source, DataEngine::Data() );

    connect( provider, &ProviderCore::newPhotoAvailable, [this, provider]( const Plasma::DataEngine::Data data ) {
        this->setData( provider->objectName(), data );
    } );

    connect( provider, &ProviderCore::error, [this, provider](const QString error) {
        this->setData( provider->objectName(), ProviderCore::cErrorKey, error );
    } );

    return true;
}

bool PhotoOfTheDay::updateSourceEvent(const QString &source)
{
    if( m_instances.contains( source ) ) {
        m_instances.value(source)->checkForNewPhoto();
    }

    return false;
}



#include "photooftheday.moc"
