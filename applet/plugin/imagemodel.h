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

#ifndef IMAGEMODEL_H
#define IMAGEMODEL_H

#include <QtCore/QObject>
#include <QtCore/QStringList>

class ImageModel : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QStringList recentImages READ recentImages WRITE setRecentImages NOTIFY recentImagesChanged)

public:
    explicit ImageModel(QObject *parent = 0);

    const QStringList recentImages() const { return m_history; }
    void setRecentImages( const QStringList& list );

    Q_INVOKABLE bool addImage( const QString &path );
    Q_INVOKABLE void removeImage( int i );

signals:
    void recentImagesChanged();

private:
    static const int cHistorySize = 20;

    QStringList m_history;
};

#endif // IMAGEMODEL_H