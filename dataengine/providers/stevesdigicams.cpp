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

#include "stevesdigicams.h"

#include <QDebug>

K_EXPORT_PLASMA__PHOTOOFTHEDAYPROVIDER_WITH_JSON( stevesdigicams, StevesDigicamsProvider, "photooftheday-provider-stevesdigicams.json" )

//==========
StevesDigicamsProvider::StevesDigicamsProvider(QObject *parent, const QVariantList &args):
ProviderCore(parent, args)
{
    connect( this, &StevesDigicamsProvider::photoDownloaded, this, &StevesDigicamsProvider::onImageDownloaded );
}

void StevesDigicamsProvider::checkForNewPhoto()
{   
    const QUrl pageUrl = QString( QLatin1Literal("http://www.steves-digicams.com/photo-of-the-day/") );

    m_data[cPageUrlKey] = pageUrl;

    qDebug() << "SD: requesting page";
    
    connect( KIO::storedGet( pageUrl, KIO::Reload, KIO::HideProgressInfo ), &KJob::result, [this] ( KJob *job )
    {        
        qDebug() << "SD: Page received. Error: " << job->error();

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
void StevesDigicamsProvider::onImageDownloaded()
{
    qDebug() << "SD: Image downloaded";

    saveInCache();

    // Finish
    emit newPhotoAvailable(m_data);
}

void StevesDigicamsProvider::parseWebPage(const QByteArray& source)
{    
    const QString data = QString::fromUtf8( source );


    const QByteArray marker("<div id=\"picture-wrapper\" class=\"master\">");

    int startIndex = data.indexOf( marker );

    if( -1 == startIndex ) {
        emit error( i18n("Web page parsing error") );
        qDebug() << "Web page parsing error 1";
        return;
    }

    startIndex += marker.size();

    const int endIndex = data.indexOf('>', startIndex) + 1;

    const QString imageTag = data.mid( startIndex, endIndex - startIndex );

    QString pattern = QLatin1String("<img src=\"(/photo-of-the-day/pub/.*)\" alt=\"(.*)\" />");
    QRegExp regExp;
    regExp.setMinimal(true);
    regExp.setPattern( pattern );
    
    if( regExp.indexIn( imageTag ) == -1 ) {
        emit error( i18n("Web page parsing error") );
        qDebug() << "Web page parsing error 2";
        return;
    }

    const QUrl photoUrl = QUrl( QLatin1String("http://www.steves-digicams.com") + regExp.cap(1) );

    if( !photoUrl.isValid() ) {
        emit error( i18n("Web page parsing error") );
        qDebug() << "Web page parsing error 3";
        return;
    }

    if( m_data[cPhotoUrlKey].toUrl() == photoUrl ) {
        qDebug() << "we have latest photo. Stop";
        // we have latest photo. Stop
        return;
    }

    m_data[cPhotoUrlKey] = photoUrl;

    // get title
    m_data[cTitleKey] = regExp.cap(2);
    
    qDebug() << "SD: Page parsed";

    downloadPhoto( photoUrl );
}


#include "stevesdigicams.moc"
