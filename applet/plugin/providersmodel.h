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

#ifndef PROVIDERSMODEL_H
#define PROVIDERSMODEL_H

#include <QtCore/QAbstractListModel>

#include <KService/KPluginInfo>

class ProvidersModel : public QAbstractListModel
{
    Q_OBJECT
//     Q_PROPERTY(QStringList selectedTimeZones WRITE setSelectedTimeZones MEMBER m_selectedTimeZones NOTIFY selectedTimeZonesChanged)
    
public:
    explicit ProvidersModel(QObject *parent = 0);
    ~ProvidersModel();
    
    enum Roles {
        RealNameRole = Qt::UserRole,
        PluginNameRole
    };
    
    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    
    Q_INVOKABLE int getIndex(const QString &pluginName);
    Q_INVOKABLE QString getPluginName(int index);
    
protected:
    QHash<int, QByteArray> roleNames() const;
    
private:
    
    KPluginInfo::List m_providers;
};

#endif // PROVIDERSMODEL_H
