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

#ifndef POTDPROVIDER_H
#define POTDPROVIDER_H

#include <QtCore/QObject>

#include <kpluginfactory.h>
#include <plasma/dataengine.h>

#include "photooftheday_export.h"

namespace Plasma {
    class DataEngine;
}

class PotdDataContainer;

class PHOTOOFTHEDAY_EXPORT ProviderCore : public QObject
{
    Q_OBJECT

public:
    static const QString cPhotoKey; // Local path to cached photo (QPixmap)
    static const QString cPhotoUrlKey; // Photo URL on web site (QUrl)
    static const QString cPageUrlKey;// URL to photo page (QUrl)
    static const QString cTitleKey;// (QString)
    static const QString cErrorKey;// (QString)

    //TODO: check if parent can be default param
    ProviderCore(QObject* parent = 0, const QVariantList& args = QVariantList() );
    
    virtual void checkForNewPhoto() = 0;

    ///@brief Restores data from cache if time since last update is smaller than update interval
    ///@param interval Update interval in milliseconds
    const Plasma::DataEngine::Data restore(quint32 interval);
    
Q_SIGNALS:
    void newPhotoAvailable( const Plasma::DataEngine::Data data );
    void error( const QString error );

    void photoDownloaded();

protected:
    void downloadPhoto(const QUrl &url);
    void savePhoto(const QPixmap &pixmap);
    void saveInCache();

    Plasma::DataEngine::Data m_data;
};


#define K_EXPORT_PLASMA_PHOTOOFTHEDAYPROVIDER(libname, classname) \
K_PLUGIN_FACTORY(factory, registerPlugin<classname>();) \
K_EXPORT_PLUGIN_VERSION(1.0)

#define K_EXPORT_PLASMA__PHOTOOFTHEDAYPROVIDER_WITH_JSON(libname, classname, jsonFile) \
    K_PLUGIN_FACTORY_WITH_JSON(factory, jsonFile, registerPlugin<classname>();) \
    K_EXPORT_PLUGIN_VERSION(PLASMA_VERSION)

#endif // POTDPROVIDER_H
