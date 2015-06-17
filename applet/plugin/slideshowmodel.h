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

#ifndef SLIDESHOWMODEL_H
#define SLIDESHOWMODEL_H

#include <QAbstractListModel>

#include <QtCore/QList>
#include <QtCore/QUrl>

#include <QtGui/QPixmap>

struct PathObject {
    QString path;
    QStringList picturesList;
};

class SlideShowModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(bool recursive READ recursive WRITE setRecursive NOTIFY recursiveChanged)
    Q_PROPERTY(bool random READ random WRITE setRandom NOTIFY randomChanged)
    Q_PROPERTY(QStringList paths READ paths WRITE setPaths NOTIFY pathsChanged)
    Q_PROPERTY(QPixmap image READ image NOTIFY imageChanged)

public:
    enum Roles {
        PathRole = Qt::UserRole,
        FileCountRole
    };

    explicit SlideShowModel(QObject *parent = 0);

    Q_INVOKABLE QPixmap requestNext();
    Q_INVOKABLE void addPath(const QUrl& url );
    Q_INVOKABLE void removePath( int index );

    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;

    bool recursive() const { return m_recursive; }
    bool random() const { return m_random; }
    const QStringList paths() const;
    const QPixmap image() const { return QPixmap(); }

    void setRecursive( bool recursive );
    void setRandom( bool rand );
    void setPaths( const QStringList& dirs );

signals:
    void pathsChanged();
    void recursiveChanged();
    void randomChanged();
    void imageChanged();

protected:
    QHash<int, QByteArray> roleNames() const;

private:
    void addDirHelper( const QString &dir );
    QStringList findPictures( const QString &dir );

    QList<PathObject*> m_pathObjects;
    bool m_recursive = false;
    bool m_random = false;
    int m_poIndex = 0;
    int m_picIndex = 0;
};

#endif // SLIDESHOWMODEL_H
