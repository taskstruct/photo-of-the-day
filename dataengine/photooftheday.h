#ifndef PHOTOOFTHEDAY_H
#define PHOTOOFTHEDAY_H

#include <Plasma/DataEngine>

class ProviderCore;

class PhotoOfTheDay: public Plasma::DataEngine
{
    Q_OBJECT
    
public:
    PhotoOfTheDay(QObject* parent, const QVariantList& args);
        
protected:
    bool sourceRequestEvent(const QString &source );
    
private:
    
    ///@brief 
    QHash<QString, ProviderCore*> m_instances;
};

#endif // PHOTOOFTHEDAY_H
