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

#include "photoofthedayplugin.h"
#include "providersmodel.h"
#include "slideshowmodel.h"
#include "imagemodel.h"
#include "shadowimageprovider.h"

#include <QtQml>

class QPixmapLoader : public QObject
{
    Q_OBJECT
public:
    explicit QPixmapLoader(QObject *parent = 0): QObject(parent)
    {
    }

    ~QPixmapLoader() = default;

    Q_INVOKABLE QPixmap load( const QString& path ) { return QPixmap( path ); }
};

static QObject *photooftheday_qpixmaploader_provider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    QPixmapLoader *example = new QPixmapLoader();
    return example;
}

void PhotoOfTheDayPlugin::registerTypes(const char *uri)
{
    Q_ASSERT(uri == QLatin1String("org.task_struct.private.photooftheday"));
    
    qmlRegisterType<ProvidersModel>(uri, 1, 0, "ProvidersModel");
    qmlRegisterType<SlideShowModel>(uri, 1, 0, "SlideShowModel");
    qmlRegisterType<ImageModel>(uri, 1, 0, "ImageModel");

    qmlRegisterSingletonType<QPixmapLoader>(uri, 1, 0, "PixmapLoader", photooftheday_qpixmaploader_provider );
}

void PhotoOfTheDayPlugin::initializeEngine(QQmlEngine *engine, const char *uri)
{
    Q_ASSERT(uri == QLatin1String("org.task_struct.private.photooftheday"));

    engine->addImageProvider(QLatin1String("shadow"), new ShadowImageProvider);
}

#include "photoofthedayplugin.moc"
