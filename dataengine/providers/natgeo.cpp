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

#include "natgeo.h"

#include <QDebug>

K_EXPORT_PLASMA_PHOTOOFTHEDAYPROVIDER( "photooftheday-provider-natgeo.json", NatGeoProvider )

//===== helper function =====

static QString getTitle( const QString &source )
{
    const QString title = i18n("National Geographic photo of the day");
    
    // first find <div id="caption">
    int divTagStart = source.indexOf("<div id=\"caption\">");
    
    if( -1 == divTagStart ) {
        return title;
    }
    
    // than find it's first child h2 tag
    int h2TagStart = source.indexOf( "<h2>", divTagStart + 18 );
    
    if( -1 == h2TagStart ) {
        return title;
    }
    
    h2TagStart += 4; // add its length
    
    // find h2 end
    int h2TagEnd = source.indexOf( "</h2>", h2TagStart );
    
    if( -1 == h2TagEnd ) {
        return title;
    }
    
    // get content of <h2></h2>
    return source.mid( h2TagStart, h2TagEnd - h2TagStart );
}

//==========
NatGeoProvider::NatGeoProvider(QObject *parent, const QVariantList &args):
ProviderCore(parent, args)
{
    connect( this, &NatGeoProvider::photoDownloaded, this, &NatGeoProvider::onImageDownloaded );
}

void NatGeoProvider::checkForNewPhoto()
{   
    const QUrl pageUrl = QString( QLatin1Literal("http://photography.nationalgeographic.com/photography/photo-of-the-day/") );

    qDebug() << "NatGeo: requesting page";

    m_data[cPageUrlKey] = pageUrl;
    
    connect( KIO::storedGet( pageUrl, KIO::Reload, KIO::HideProgressInfo ), &KJob::result, [this] ( KJob *job )
    {        
        qDebug() << "NatGeo: Page received. Error: " << job->error();

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
void NatGeoProvider::onImageDownloaded()
{
    qDebug() << "NatGeo: Image downloaded";

    saveInCache();

    // Finish
    emit newPhotoAvailable(m_data);
}

void NatGeoProvider::parseWebPage(const QByteArray& source)
{    
    const QString data = QString::fromUtf8( source );

    const QString pattern = QLatin1String("<meta name=\"image\" property=\"og:image\" content=\"(http://.*/)\" />");
    QRegExp regExp;
    regExp.setMinimal(true);
    regExp.setPattern( pattern );
    
    if( regExp.indexIn( data ) == -1 ) {
        emit error( i18n("Web page parsing error") );
        qDebug() << "Web page parsing error 1";
        return;
    }

    const QUrl photoUrl = QUrl( regExp.cap(1) );

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
    
    // get title
    m_data[cTitleKey] = getTitle(data);
        
    //TODO: Description [opt]: <meta name="description" content="Sharks gather to feed in the waters off Mexico’s Guadalupe Island in this Photo of the Day from the National Geographic Photo Contest." />
    
    qDebug() << "NatGeo: Page parsed";

    downloadPhoto( photoUrl );
}


#include "natgeo.moc"
