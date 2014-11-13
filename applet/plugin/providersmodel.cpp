#include <QtGui/QIcon>

#include <KServiceTypeTrader>
#include <KSycoca>

#include "providersmodel.h"

#include <QDebug>

ProvidersModel::ProvidersModel(QObject* parent): QAbstractListModel(parent)
{
    m_providers = KServiceTypeTrader::self()->query(QLatin1String( "PhotoOfTheDay/Plugin" ));
}

ProvidersModel::~ProvidersModel()
{
}

QVariant ProvidersModel::data(const QModelIndex &index, int role) const
{
    if( !index.isValid() ) {
        return QVariant();
    }
    
    KService::Ptr s = m_providers.at(index.row());
    
    if( !s->isValid() ) {
        return QVariant();
    }
    
    QVariant v;
    
    switch( role ) {
        case Qt::DisplayRole:
        case RealNameRole: {
            v.setValue<QString>( s->name() );
            break;
        }
        
        case Qt::DecorationRole: {
            v.setValue<QIcon>( QIcon::fromTheme( s->icon() ) );
            break;
        }
        
        case Qt::ToolTipRole: {
            v.setValue<QString>( s->comment() );
            break;
        }
        
        case PluginNameRole: {
            v.setValue<QString>( s->property(QLatin1String( "X-KDE-PhotoOfTheDayPlugin-Identifier" ), QVariant::String).toString() );
        }
        
        default: break;
    }
    
    return v;
}

QHash< int, QByteArray > ProvidersModel::roleNames() const
{
    static const QHash<int, QByteArray> s_roleNames({
        {RealNameRole, "RealNameRole"},
        {PluginNameRole, "PluginNameRole"}
    });
    
    return s_roleNames;
}

int ProvidersModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    
    return m_providers.count();
}

int ProvidersModel::getIndex(const QString& pluginName)
{
    qDebug() << "getIndex for '" << pluginName << '\'';
    
    if( pluginName.isEmpty() ) {
        return -1;
    }
    
    const int length = m_providers.size();
    int index = -1;
    
    for( int i = 0; i < length; ++i )
    {
        const KService::Ptr s = m_providers.at(i);
        
        if( s->property(QLatin1String( "X-KDE-PhotoOfTheDayPlugin-Identifier" )).toString() == pluginName ) {
            index = i;
        }
    }
    
    return index;
}

QString ProvidersModel::getPluginName(int index)
{
    qDebug() << "getPluginName for " << index;
    
    if( index < 0 ) {
        return QString();
    }
    
    if( index >= m_providers.count() ) {
        return QString();
    }
    
    return m_providers.at(index)->property(QLatin1String( "X-KDE-PhotoOfTheDayPlugin-Identifier" )).toString();
}


#include "providersmodel.moc"
