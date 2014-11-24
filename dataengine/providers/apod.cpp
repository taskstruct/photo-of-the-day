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

#include "apod.h"

K_EXPORT_PLASMA_PHOTOOFTHEDAYPROVIDER( "photooftheday-provider-apod.json", ApodProvider )

//===== helper function =====

//==========
ApodProvider::ApodProvider(QObject *parent, const QVariantList &args):
ProviderCore(parent, args)
{
    // create cache if does not exists
}

//====== DataFetcher =====
DataFetcher::DataFetcher( QObject *parent): QObject(parent)
{
}

void DataFetcher::GetWebPage( const QUrl & url)
{
    //=================================================================================
    m_data.canonicalUrl = QLatin1String("http://apod.nasa.gov/apod/astropix.html");
    m_data.cacheUrl = QLatin1String("/home/nikolay/jiraf.jpg");
    m_data.title = QLatin1String("Apod");
    m_data.photoUrl = QLatin1String("/home/nikolay/jiraf.jpg");
    m_data.prevDayUrl = QString();
    
    emit finished(0);
    return;
    
    //=================================================================================
    
    
    
//     m_data.canonicalUrl = url.toString();
//     
//     connect( KIO::storedGet( url, KIO::Reload, KIO::HideProgressInfo ), &KJob::result, [this] ( KJob *job )
//     {        
//         if( 0 != job->error() )
//         {
//             m_error = job->errorString();
//             emit finished(1);
//         }
//         else
//         {
//             KIO::StoredTransferJob *stJob = qobject_cast<KIO::StoredTransferJob*>(job);
//             
//             Q_ASSERT(stJob != nullptr);
//             
//             if( parseWebPage( stJob->data() ) )
//             {
//                 GetImage();
//             }
//             else
//             {
//                 m_error = i18n("Web page parsing error");
//             }
//         }
//         
//         job->deleteLater(); //NOTE: "The default for any KJob is to automatically delete itself."
//     } );
}

void DataFetcher::GetImage()
{
//     connect( KIO::storedGet( m_data.photoUrl, KIO::Reload, KIO::HideProgressInfo ), &KJob::result, [this] ( KJob *job )
//     {
//         if( 0 != job->error() )
//         {
//             m_error = job->errorString();
//             emit finished(2);
//         }
//         else
//         {
//             // save
//             // TODO: insert in db if cache enabled will be done by plugin
//             auto *stJob = qobject_cast<KIO::StoredTransferJob*>(job);
//             Q_ASSERT(stJob != nullptr);
//             
//             auto provider = qobject_cast<ApodProvider*>( parent() );
//             
//             if( provider->SaveImage( stJob->data(), stJob->url().fileName(), QLatin1String("natgeo") ) )
//             {
//             }
//             else
//             {
//                 //TODO: When we don't use cache or save failed pass image URL to applet
//                 m_data.cacheUrl = m_data.photoUrl;
//             }
//             
//             emit finished(0);
//         }
//     });
}

bool DataFetcher::parseWebPage(const QByteArray& source)
{
//     // get canonical URL
//     QString pattern( QLatin1Literal( "<link rel=\"canonical\" href=\"(http://photography.nationalgeographic.com/photography/photo-of-the-day/.*/)\" />" ) );
//     QRegExp regExp( pattern );
//     regExp.setMinimal( true );
//  
//     if( regExp.indexIn( source ) == -1 ) {
//         return false;
//     }
//     
//     m_data.canonicalUrl = regExp.cap(1);
//     
//     
//     // get image URL. I use URL for Twitter. There is another URL for Google+ and "real" URL in <div class="primary_photo">. All generate the same image
//     pattern = QLatin1String("<meta name=\"twitter:image\" content=\"(//images.nationalgeographic.com/wpf/media-live/photos/.*)\"/>");
//     regExp.setPattern( pattern );
//     
//     if( regExp.indexIn( source ) == -1 ) {
//         return false;
//     }
//     
//     m_data.photoUrl = QLatin1Literal("http:") + regExp.cap(1);
//     
//     
//     // get previous day URL
//     pattern = QLatin1String("<p class=\"prev first\"><a href=\"(/photography/photo-of-the-day/.*/)\">&laquo; Previous</a></p>");
//     regExp.setPattern( pattern );
//     
//     if( regExp.indexIn( source ) != -1 ) {
//         m_data.prevDayUrl = QLatin1Literal("http://photography.nationalgeographic.com") + regExp.cap(1);
//     }
//     
//     // get title
//     m_data.title = getTitle(source);
//     
//         
//     //TODO: Description [opt]: <meta name="description" content="Sharks gather to feed in the waters off Mexico’s Guadalupe Island in this Photo of the Day from the National Geographic Photo Contest." />
//     
    return true;   
}




#include "apod.moc"
