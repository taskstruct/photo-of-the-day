#include <QtCore/QDir>
#include <QtCore/QHash>
#include <QtCore/QStandardPaths>

#include <QDebug>

#include <KService/KPluginTrader>

#include "providercore.h"
#include "potddatacontainer.h"
#include "photooftheday.h"

K_EXPORT_PLASMA_DATAENGINE_WITH_JSON(photooftheday, PhotoOfTheDay, "plasma-dataengine-photooftheday.json")

static constexpr QLatin1String cProvidersSourceName("Providers");

static constexpr QLatin1String cProviderDirectory("plasma/dataengine/photooftheday");
static constexpr QLatin1String cServiceType("PhotoOfTheDay/Plugin");

inline const QString providerFromSource(const QString &source) {
    return source.split(':').at(0);
}

PhotoOfTheDay::PhotoOfTheDay(QObject* parent, const QVariantList& args): Plasma::DataEngine(parent, args)
{    
    // set up Providers DataSource
    auto providersList = KPluginTrader::self()->query( cProviderDirectory, cServiceType, "'' != [X-KDE-PhotoOfTheDayPlugin-Identifier]" );
    
    for( const auto &provider: providersList )
    {
        const QString id = provider.property( QLatin1String( "X-KDE-PhotoOfTheDayPlugin-Identifier" ) ).toString();

        setData( cProvidersSourceName, id, provider.name() );
    }
    
    if( providersList.isEmpty() ) {
        setData( cProvidersSourceName, Plasma::DataEngine::Data() );
    }
    
    //TODO: Listen for changes with QFileSystemWatcher

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

    auto providersList = KPluginTrader::self()->query( cProviderDirectory, cServiceType, constraint );

    if( providersList.size() != 1 )
    {
        // there should be only one provider with that name
        return false;
    }

    KPluginLoader loader( providersList.at(0).libraryPath() );

    const QVariantList argsWithMetaData = QVariantList() << loader.metaData().toVariantMap();

    KPluginFactory* factory = loader.factory();
    ProviderCore* provider = nullptr;

    if( factory ) {
        provider = factory->create<ProviderCore>(this, argsWithMetaData);
    }

    if( !provider ) {
        qDebug() << "Unable to create provider for source '" << source << "'";
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
