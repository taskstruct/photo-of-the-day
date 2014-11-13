#include "photoofthedayplugin.h"
#include "providersmodel.h"

#include <QtQml>

void PhotoOfTheDayPlugin::registerTypes(const char *uri)
{
    Q_ASSERT(uri == QLatin1String("org.task_struct.private.photooftheday"));
    
    qmlRegisterType<ProvidersModel>(uri, 1, 0, "ProvidersModel");
}