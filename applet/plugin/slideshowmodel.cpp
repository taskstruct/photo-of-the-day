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
#include <random>

#include <QtCore/QDirIterator>

#include "slideshowmodel.h"

#include <QDebug>

//TODO: Use QStringList QMimeType::suffixes() const
const QStringList FilenameFilters { "*.jpeg", "*.jpg", "*.png", "*.svg", "*.svgz", "*.bmp", "*.tif" };

SlideShowModel::SlideShowModel(QObject *parent) : QAbstractListModel(parent)
{
    connect( this, &SlideShowModel::recursiveChanged, [this] {

        auto iter = m_pathObjects.begin();
        auto end = m_pathObjects.end();

        while( iter != end ) {
            auto po = *iter;
            const int oldFileCount = po->picturesList.size();
            po->picturesList = findPictures( po->path );

            if( po->picturesList.size() != oldFileCount ) {
                const int row = iter - m_pathObjects.begin();
                emit dataChanged( index( row ), index(row), QVector<int>{ FileCountRole } );
            }

            ++iter;
        }
    } );
}

QPixmap SlideShowModel::requestNext()
{
    QString picPath;

    if( m_random ) {
        std::random_device rd;
        std::mt19937 mt( rd() );
        std::uniform_int_distribution<int> dist1( 0, m_pathObjects.size() - 1 );

        const auto picList = m_pathObjects.at( dist1(mt) )->picturesList;

        std::uniform_int_distribution<int> dist2( 0, picList.size() - 1 );

        picPath = picList.at( dist2(mt) );
    }
    else {
        ++m_picIndex;

        if( m_picIndex >= m_pathObjects.at(m_poIndex)->picturesList.size() )
        {
            // if this was the last image from this directory, go to first in next one
            m_picIndex = 0;

            ++m_poIndex;

            // end of directory list is reached
            if( m_poIndex >= m_pathObjects.size() ) {
                m_poIndex = 0;
            }
        }

        picPath = m_pathObjects.at(m_poIndex)->picturesList.at(m_picIndex);
    }

    return QPixmap( picPath );
}

void SlideShowModel::addPath(const QUrl &url)
{
    if( !url.isLocalFile() ) {
        return;
    }

    //TODO: Ckech if it is not subdirectory of already indexed directory and recursive == true

    const QString path = url.toLocalFile();

    qDebug() << "SlideShowModel::addPath() " << path;

    const int pos = m_pathObjects.size();
    beginInsertRows( QModelIndex(), pos, pos );

    addDirHelper( path );

    endInsertRows();

    emit pathsChanged();
}

void SlideShowModel::removePath(int index)
{
    if( ( index < 0 )|| ( index >= m_pathObjects.size() ) ) {
        return;
    }

    beginRemoveRows( QModelIndex(), index, index );

    m_pathObjects.removeAt(index);

    endRemoveRows();

    emit pathsChanged();
}

int SlideShowModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return m_pathObjects.size();
}

QVariant SlideShowModel::data(const QModelIndex &index, int role) const
{
    if( !index.isValid() ) {
        return QVariant();
    }

    if( index.row() >= m_pathObjects.size() ) {
        return QVariant();
    }

    QVariant v;

    if( PathRole == role ) {
        const PathObject* po = m_pathObjects.at( index.row() );
        const QString path = po->path;

        v.setValue<QString>( path );

        return v;
    }

    if( FileCountRole == role ) {
        v.setValue<int>( m_pathObjects.at( index.row() )->picturesList.size() );
        return v;
    }

    return v;
}

const QStringList SlideShowModel::paths() const
{
    QStringList paths;

    for( auto po: m_pathObjects ) {
        paths.append( po->path );
    }

    return paths;
}

void SlideShowModel::setPaths(const QStringList &dirs)
{
    qDebug() << "SlideShowModel::setPaths" << dirs;
    //TODO: Detect only new/removed. Also may be async scan

    if( dirs.size() == m_pathObjects.size() ) {
        if( std::equal( dirs.cbegin(), dirs.cend(), m_pathObjects.cbegin(), []( const QString& dir, const PathObject* po ) { return dir == po->path; } ) ) {
            return;
        }
    }

    beginResetModel();

    // remove all
    qDeleteAll(m_pathObjects);
    m_pathObjects.clear();

    // resert indexes
    m_poIndex = 0;
    m_picIndex = 0;

    for( const QString &d : dirs )
    {
        addDirHelper(d);
    }

    endResetModel();

    emit pathsChanged();
}

void SlideShowModel::setRecursive(bool recursive)
{
    if( m_recursive != recursive ) {
        m_recursive = recursive;
        emit recursiveChanged();
    }
}

void SlideShowModel::setRandom(bool rand)
{
    if( m_random != rand ) {
        m_random = rand;
        emit randomChanged();
    }
}

QHash<int, QByteArray> SlideShowModel::roleNames() const
{
    static const QHash<int, QByteArray> s_roleNames = {
        { PathRole, "pathRole" },
        { FileCountRole, "fileCountRole" }
    };

    return s_roleNames;
}

void SlideShowModel::addDirHelper(const QString &dir)
{
    auto po = new PathObject;
    po->path = dir;
    po->picturesList = findPictures(dir);

    m_pathObjects.append( po );
}

QStringList SlideShowModel::findPictures(const QString &dir)
{
    QDirIterator iter( dir, FilenameFilters, QDir::Files, (m_recursive ? QDirIterator::Subdirectories | QDirIterator::FollowSymlinks : QDirIterator::NoIteratorFlags) );

    QStringList picturesList;

    while( iter.hasNext() ) {
        iter.next();
        picturesList.append( iter.filePath() );
    }

    // QDirIterator does not support sorting, so we have to sort manually
    picturesList.sort();

    return picturesList;
}

#include "slideshowmodel.moc"
