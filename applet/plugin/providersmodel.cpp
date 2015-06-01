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

#include <algorithm>

#include <QtGui/QIcon>

#include <KService/KPluginTrader>

#include "providersmodel.h"

#include <QDebug>

ProvidersModel::ProvidersModel(QObject* parent): QAbstractListModel(parent)
{
    m_providers = KPluginTrader::self()->query( "plasma/dataengine/photooftheday", "PhotoOfTheDay/Plugin", "'' != [X-KDE-PhotoOfTheDayPlugin-Identifier]" );
}

ProvidersModel::~ProvidersModel()
{
}

QVariant ProvidersModel::data(const QModelIndex &index, int role) const
{
    if( !index.isValid() ) {
        return QVariant();
    }
    
    auto p = m_providers.at(index.row());
    
    if( !p.isValid() ) {
        return QVariant();
    }
    
    QVariant v;
    
    switch( role ) {
        case Qt::DisplayRole:
        case RealNameRole: {
            v.setValue<QString>( p.name() );
            break;
        }
        
        case Qt::DecorationRole: {
            v.setValue<QIcon>( QIcon::fromTheme( p.icon() ) );
            break;
        }
        
        case Qt::ToolTipRole: {
            v.setValue<QString>( p.comment() );
            break;
        }
        
        case PluginNameRole: {
            v.setValue<QString>( p.property( QLatin1String( "X-KDE-PhotoOfTheDayPlugin-Identifier" ) ).toString() );
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
    qDebug() << "ProvidersModel::getIndex() for plugin" << pluginName;
    
    if( pluginName.isEmpty() ) {
        return -1;
    }

    auto iter = std::find_if( m_providers.cbegin(), m_providers.cend(), [&pluginName]( const KPluginInfo& p ) {
        return ( p.property( QLatin1String( "X-KDE-PhotoOfTheDayPlugin-Identifier" ) ).toString() == pluginName );
    } );
    
    if( m_providers.cend() != iter ) {
        return iter - m_providers.cbegin();
    } else {
        return -1;
    }
}

QString ProvidersModel::getPluginName(int index)
{
    qDebug() << "ProvidersModel::getPluginName() index " << index;
    
    if( index < 0 ) {
        return QString();
    }
    
    if( index >= m_providers.count() ) {
        return QString();
    }

    const QString name = m_providers.at(index).property( QLatin1String( "X-KDE-PhotoOfTheDayPlugin-Identifier" ) ).toString();
    
    qDebug() << "ProvidersModel::getPluginName() returns " << name;

    return name;
}


#include "providersmodel.moc"
