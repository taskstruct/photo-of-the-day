/*
 * Copyright (c) 2014, Nikolay S Nikolov <nikolay.stef.nikolov@gmail.com>
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
 * THIS SOFTWARE IS PROVIDED BY Nikolay S Nikolov <nikolay.stef.nikolov@gmail.com> ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL Nikolay S Nikolov <nikolay.stef.nikolov@gmail.com> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
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
        m_data[cTitleKey] = regExp.cap(1);

        const QUrl pageUrl( regExp.cap(2) );

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
