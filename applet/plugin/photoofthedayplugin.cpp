#include "photoofthedayplugin.h"
#include "providersmodel.h"
#include "shadowimageprovider.h"

#include <QtQml>

void PhotoOfTheDayPlugin::registerTypes(const char *uri)
{
    Q_ASSERT(uri == QLatin1String("org.task_struct.private.photooftheday"));
    
    qmlRegisterType<ProvidersModel>(uri, 1, 0, "ProvidersModel");
}

void PhotoOfTheDayPlugin::initializeEngine(QQmlEngine *engine, const char *uri)
{
    Q_ASSERT(uri == QLatin1String("org.task_struct.private.photooftheday"));

    engine->addImageProvider(QLatin1String("shadow"), new ShadowImageProvider);
}
