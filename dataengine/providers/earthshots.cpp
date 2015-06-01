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

#include <KI18n/KLocalizedString>

#include <KCoreAddons/KJob>
#include <KIO/Job>
#include <KIO/StoredTransferJob>

#include "earthshots.h"

#include <QDebug>

K_EXPORT_PLASMA__PHOTOOFTHEDAYPROVIDER_WITH_JSON( earthshots, EarthshotsProvider, "photooftheday-provider-earthshots.json" )

//==========
EarthshotsProvider::EarthshotsProvider(QObject *parent, const QVariantList &args):
ProviderCore(parent, args)
{
    connect( this, &EarthshotsProvider::photoDownloaded, this, &EarthshotsProvider::onImageDownloaded );
}

void EarthshotsProvider::checkForNewPhoto()
{   
    const QUrl pageUrl = QString( QLatin1Literal("http://www.earthshots.org/") );

    qDebug() << "ES: requesting page";
    
    connect( KIO::storedGet( pageUrl, KIO::Reload, KIO::HideProgressInfo ), &KJob::result, [this] ( KJob *job )
    {        
        qDebug() << "ES: Page received. Error: " << job->error();

        if( 0 != job->error() )
        {
            emit error(job->errorString());
        }
        else
        {
            auto *storedJob = qobject_cast<KIO::StoredTransferJob*>(job);
            
            Q_ASSERT(storedJob != nullptr);
            
            this->parseWebPage( storedJob->data() );
        }
        
        job->deleteLater(); //NOTE: "The default for any KJob is to automatically delete itself."
    } );
}

//TOOD: This function can be moved to ProviderCore and used immediately, instead of ImageDownloaded signal
void EarthshotsProvider::onImageDownloaded()
{
    qDebug() << "ES: Image downloaded";

    saveInCache();

    // Finish
    emit newPhotoAvailable(m_data);
}

void EarthshotsProvider::parseWebPage(const QByteArray& source)
{    
    const QString data = QString::fromUtf8( source );

    QString pattern = QLatin1String("<link rel=\"image_src\" href=\"(http://img.earthshots.org/.*)\" />");
    QRegExp regExp;
    regExp.setMinimal(true);
    regExp.setPattern( pattern );
    
    if( regExp.indexIn( data ) == -1 ) {
        emit error( i18n("Web page parsing error") );
        qDebug() << "Web page parsing error 1";
        return;
    }

    const QString photoUrlString = regExp.cap(1);
    const QUrl photoUrl = QUrl( photoUrlString );

    if( !photoUrl.isValid() ) {
        emit error( i18n("Web page parsing error") );
        qDebug() << "Web page parsing error 2";
        return;
    }

    if( m_data[cPhotoUrlKey].toUrl() == photoUrl ) {
        qDebug() << "we have latest photo. Stop";
        // we have latest photo. Stop
        return;
    }

    m_data[cPhotoUrlKey] = photoUrl;
    

    // get page link from <a> tag and photo title from img alt attribute
    pattern = QString( QLatin1String("<a href=\"(http://www.earthshots.org/.*/)\"><img src=\"%1\" alt=\"(.*)\" /></a>")).arg(photoUrlString);
    regExp.setPattern( pattern );

    if( regExp.indexIn(data) != -1 )
    {
        m_data[cTitleKey] = regExp.cap(2);

        const QUrl pageUrl( regExp.cap(1) );

        if(pageUrl.isValid()) {
            m_data[cPageUrlKey] = pageUrl;
        }
        else {
            m_data[cPageUrlKey] = QUrl( QLatin1String("http://www.earthshots.org/") );
        }
    }
    else
    {
        m_data[cTitleKey] = QString( QLatin1String("Earth Shots") );
        m_data[cPageUrlKey] = QUrl( QLatin1String("http://www.earthshots.org/") );
    }
    
    qDebug() << "ES: Page parsed";

    downloadPhoto( photoUrl );
}


#include "earthshots.moc"
