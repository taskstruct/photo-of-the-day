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

#include "imagemodel.h"

ImageModel::ImageModel(QObject *parent) : QObject(parent)
{

}

void ImageModel::setRecentImages(const QStringList &list)
{
    if( list == m_history ) {
        return;
    }

    //todo: remove deleted images
    m_history = list;

    emit recentImagesChanged();
}

bool ImageModel::addImage(const QString &path)
{
    //TODO: Remove "file://

    if( m_history.contains( path ) ) {
        return false;
    }

    if( m_history.size() == cHistorySize ) {
        m_history.removeFirst();
    }

    m_history.append( path );

    emit recentImagesChanged();
    return true;
}

void ImageModel::removeImage(int i)
{
    if( ( i < 0 ) || ( i >= m_history.size() ) ) {
        return;
    }

    m_history.removeAt(i);

    emit recentImagesChanged();
}

#include "imagemodel.moc"

