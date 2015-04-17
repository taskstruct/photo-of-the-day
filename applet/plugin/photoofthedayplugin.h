#ifndef PHOTOOFTHEDAYPLUGIN_H
#define PHOTOOFTHEDAYPLUGIN_H

#include <QtQml/QQmlExtensionPlugin>

class QQmlEngine;

class PhotoOfTheDayPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")
    
public:
    void registerTypes(const char *uri);
    void initializeEngine(QQmlEngine * engine, const char * uri);
};

#endif // PHOTOOFTHEDAYPLUGIN_H
