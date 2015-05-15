/*
 * Copyright (c) 2014, Nikolay Nikolov <nikolay.stef.nikolov@gmail.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *     * Neither the name of the <organization> nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY Nikolay Nikolov <nikolay.stef.nikolov@gmail.com> ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL Nikolay Nikolov <nikolay.stef.nikolov@gmail.com> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <QtCore/QSettings>
#include <QtCore/QStandardPaths>

#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QUrl>

#include <QtGui/QPixmap>

#include <KI18n/KLocalizedString>

#include <KIO/StoredTransferJob>

#include "providercore.h"

#include <QDebug>

const QString ProviderCore::cPhotoKey = QString("Photo");
const QString ProviderCore::cPhotoUrlKey = QLatin1Literal("PhotoUrl");
const QString ProviderCore::cPageUrlKey = QLatin1Literal("PageUrl");
const QString ProviderCore::cTitleKey = QLatin1Literal("Title");
const QString ProviderCore::cErrorKey = QLatin1Literal("Error");

ProviderCore::ProviderCore(QObject *parent, const QVariantList &args):
    QObject(parent)
{
}

const Plasma::DataEngine::Data ProviderCore::restore( quint32 interval )
{
    const QChar dirSep = QDir::separator();

    auto dirPath = QStandardPaths::writableLocation(QStandardPaths::GenericCacheLocation) + dirSep + "photo-of-the-day" + dirSep;

    auto cacheFileName = dirPath + objectName() + ".cache";

    if( !QFile::exists( cacheFileName ) ) {
        // first use of this provider
        checkForNewPhoto();
        return Plasma::DataEngine::Data();
    }

    QSettings cache( cacheFileName, QSettings::IniFormat, this );

    QDateTime lastUpdate = cache.value( "LastUpdate", QDateTime() ).toDateTime();

    if( !lastUpdate.isValid() ) {
        checkForNewPhoto();
        return Plasma::DataEngine::Data();
    }

    const auto elapsedTime = lastUpdate.msecsTo( QDateTime::currentDateTime() );

    if( ( elapsedTime < 0 ) || ( elapsedTime >= interval ) ) {
        qDebug() << "LastUpdate = " << lastUpdate << " now = " << QDateTime::currentDateTime();
        qDebug() << "elapsedTime = " << elapsedTime << "  interval = " << interval;
        checkForNewPhoto();
        return Plasma::DataEngine::Data();
    }

    auto photoFileName = dirPath + objectName() + ".jpg";

    if( !QFile::exists( photoFileName ) ) {
        checkForNewPhoto();
        return Plasma::DataEngine::Data();
    }

    QPixmap photo;

    if( !photo.load( photoFileName ) ) {
        checkForNewPhoto();
        return Plasma::DataEngine::Data();
    }

    qDebug() << "Restored";

    m_data[cPhotoKey] = photo;
    m_data[cPhotoUrlKey] = cache.value(cPhotoUrlKey).toUrl();
    m_data[cPageUrlKey] = cache.value(cPageUrlKey).toUrl();
    m_data[cTitleKey] = cache.value(cTitleKey).toString();

    return Plasma::DataEngine::Data(m_data);
}

void ProviderCore::downloadPhoto(const QUrl &url)
{
    auto job = KIO::storedGet( url, KIO::Reload, KIO::HideProgressInfo);

    connect( job, &KIO::StoredTransferJob::result, [this] ( KJob *job ) {
        if( job->error() == 0 )
        {
            auto storedJob = qobject_cast<KIO::StoredTransferJob*>(job);

            const QString type = storedJob->mimetype().split('/').at(1);
            const QByteArray format = type.toLatin1();

            QPixmap pixmap;
            pixmap.loadFromData( storedJob->data(), format.data() );

            this->m_data[cPhotoKey] = pixmap;
            this->savePhoto( pixmap );

            emit this->photoDownloaded();
        }
        else {
            emit this->error( i18n("Error while downloading photo: %1", job->errorString() ) );
        }
    } );
}

void ProviderCore::savePhoto(const QPixmap &pixmap)
{
    const QChar dirSep = QDir::separator();
    auto dirPath = QStandardPaths::writableLocation(QStandardPaths::GenericCacheLocation) + dirSep + "photo-of-the-day" + dirSep;
    auto photoFileName = dirPath + objectName() + ".jpg";
    // always save in jpg. Most(if not all) of web sites use this format. Also we don't need transparency or animated gifs
    qDebug() << "Saving photo in " << photoFileName;
    pixmap.save( photoFileName, "JPG" );
}

void ProviderCore::saveInCache()
{
    const QChar dirSep = QDir::separator();

    auto dirPath = QStandardPaths::writableLocation(QStandardPaths::GenericCacheLocation) + dirSep + "photo-of-the-day" + dirSep;

    auto cacheFileName = dirPath + objectName() + ".cache";
    qDebug() << "Saving cache in " << cacheFileName << " at " << QDateTime::currentDateTime();
    QSettings cache( cacheFileName, QSettings::IniFormat, this );

    cache.setValue( cPhotoUrlKey, m_data[cPhotoUrlKey].toUrl() );
    cache.setValue( cPageUrlKey, m_data[cPageUrlKey].toUrl() );
    cache.setValue( cTitleKey, m_data[cTitleKey].toString() );

    cache.setValue( "LastUpdate", QDateTime::currentDateTime() );
}


#include "providercore.moc"
