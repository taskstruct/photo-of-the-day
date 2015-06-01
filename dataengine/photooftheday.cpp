/*
 * Copyright 2015  Nikolay S Nikolov <nikolay.stef.nikolov@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

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
        qDebug() << "Source removed: " << source;
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
    qDebug() << "PhotoOfTheDay::sourceRequestEvent for source: " << source;

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

    /*TODO: How to get update interval */
    // Restore only if cache is no more than 1h old
    setData( source, provider->restore( 60*60*1000 ) );

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
    qDebug() << "PhotoOfTheDay::updateSourceEvent: " << source;
    if( m_instances.contains( source ) ) {
        m_instances.value(source)->checkForNewPhoto();
    }

    return false;
}



#include "photooftheday.moc"
