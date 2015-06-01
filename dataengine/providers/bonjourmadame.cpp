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

#include "bonjourmadame.h"

#include <QDebug>

K_EXPORT_PLASMA__PHOTOOFTHEDAYPROVIDER_WITH_JSON( bonjourmadame, BonjourMadameProvider, "photooftheday-provider-bonjourmadame.json" )

static const QUrl cBonjourMadameUrl = QUrl(QLatin1String("http://www.bonjourmadame.fr/"));

//==========
BonjourMadameProvider::BonjourMadameProvider(QObject *parent, const QVariantList &args):
ProviderCore(parent, args)
{
    connect( this, &BonjourMadameProvider::photoDownloaded, this, &BonjourMadameProvider::onImageDownloaded );
}

void BonjourMadameProvider::checkForNewPhoto()
{   
    qDebug() << "BM: requesting page";
    
    connect( KIO::storedGet( cBonjourMadameUrl, KIO::Reload, KIO::HideProgressInfo ), &KJob::result, [this] ( KJob *job )
    {        
        qDebug() << "BM: Page received. Error: " << job->error();

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
void BonjourMadameProvider::onImageDownloaded()
{
    qDebug() << "BM: Image downloaded";

    saveInCache();

    // Finish
    emit newPhotoAvailable(m_data);
}

void BonjourMadameProvider::parseWebPage(const QByteArray& source)
{    
    const QString data = QString::fromUtf8( source );


    const QByteArray marker("<div class=\"photo post\">");

    int startIndex = data.indexOf( marker );

    if( -1 == startIndex ) {
        emit error( i18n("Web page parsing error") );
        qDebug() << "Web page parsing error 1";
        return;
    }

    startIndex += marker.size();

    const int endIndex = source.indexOf("</div>", startIndex);

    const QString divTag = source.mid( startIndex, endIndex - startIndex );

    qDebug() << "dirTag: " << divTag;

    QString pattern = QLatin1String("<img src=\"(.*)\" alt=\"(.*)\"/>");
    QRegExp regExp;
    regExp.setMinimal(true);
    regExp.setPattern( pattern );
    
    if( regExp.indexIn( divTag ) == -1 ) {
        emit error( i18n("Web page parsing error") );
        qDebug() << "Web page parsing error 2";
        return;
    }

    const QUrl photoUrl = QUrl( regExp.cap(1) );

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

    // get photo page URL
    pattern = QLatin1String("<a href=\"(http://www.bonjourmadame.fr/image/[0-9]{12})\">");
    regExp.setPattern( pattern );

    if( regExp.indexIn( divTag ) != -1 )
    {
        const QUrl pageUrl( regExp.cap(1) );

        if( !regExp.isValid() ) {
             m_data[cPageUrlKey] = cBonjourMadameUrl;
        }

        m_data[cPageUrlKey] = pageUrl;
    }
    else {
        m_data[cPageUrlKey] = cBonjourMadameUrl;
    }
    
    qDebug() << "BM: Page parsed";

    downloadPhoto( photoUrl );
}


#include "bonjourmadame.moc"
