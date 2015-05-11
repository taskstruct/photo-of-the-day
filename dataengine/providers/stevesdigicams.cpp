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

#include "stevesdigicams.h"

#include <QDebug>

K_EXPORT_PLASMA_PHOTOOFTHEDAYPROVIDER( "photooftheday-provider-stevesdigicams.json", StevesDigicamsProvider )

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