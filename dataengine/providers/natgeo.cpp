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

#include "potddatacontainer.h"
#include "natgeo.h"

K_EXPORT_PLASMA_PHOTOOFTHEDAYPROVIDER( "photooftheday-provider-natgeo.json", NatGeoProvider )

//===== helper function =====

static QString getTitle( const QByteArray &source )
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
    CacheManager *m_cache = CacheManager::create( QLatin1String("natgeo"), QLatin1String("CREATE TABLE...") ); //TODO
}

void NatGeoProvider::requestPhoto(const QString &source, int offset)
{   
    // calculate download url
    QString newUrl;
    
    if( 0 == offset  )
    {
        // download todays photo
        newUrl = QLatin1String("http://photography.nationalgeographic.com/photography/photo-of-the-day/");
        
        //
    }
    else
    {
        //TODO: In 2.0
        //TODO: check in cache "SELECT * FROM natgeo WHERE canonicalUrl = " 
    }
    
    //TODO: check if fetch is running
    for( auto df: m_fetchers )
    {
        if( newUrl == df->m_data->canonicalUrl )
        {
            df->m_associatedSources.append(source);
            return;
        }
    }
    
    // we need new fetcher for this URL
    DataFetcher *newDF = new DataFetcher(this);
    m_fetchers.append(newDF);
        
    connect( newDF, &DataFetcher::finished, [this](int result) {
        auto df = qobject_cast<DataFetcher*>( sender() );
        Q_ASSERT( df != Q_NULLPTR );
        
        if( 0 == result )
        {    
            for( const QString& s: df->m_associatedSources )
            {
                Plasma::DataEngine::Data d;
                
                d.insert( cPhotoKey, df->m_data->cacheUrl );
                d.insert( cPageUrlKey, df->m_data->canonicalUrl );
                d.insert( cPrevPageUrlKey, df->m_data->prevDayUrl );
                d.insert( cTitleKey, df->m_data->title );
                
                emit photoReady(s, d);
            }
        }
        else {
            //TODO: Handle error
        }
        
        m_fetchers.removeOne(df);
        df->deleteLater();
        
    } );
    
    newDF->GetWebPage( newUrl );
}

void NatGeoProvider::checkForNewPhoto(PotdDataContainer* dataContainer)
{
    if( m_checker ) {
        // check is running. just register and wait to finish
        m_checker->registerDataContainer(dataContainer);
        return;
    }
    
    m_checker = new DataFetcher(this);
    m_checker->registerDataContainer(dataContainer);
    m_checker->GetWebPage( QUrl( QLatin1Literal("http://photography.nationalgeographic.com/photography/photo-of-the-day/") ) );
    
//     connect( m_checker.data(), &DataFetcher::finished, [this](int result) {
//         // delete object
//         m_checker->deleteLater();
//     } );
}


//====== DataFetcher =====
DataFetcher::DataFetcher( QObject *parent): QObject(parent),
m_data( new Data )
{
}

void DataFetcher::GetWebPage( const QUrl & url)
{
    //=================================================================================
    m_data->canonicalUrl = QLatin1String("http://photography.nationalgeographic.com/photography/photo-of-the-day/");
    m_data->cacheUrl = QLatin1String("/home/nikolay/Siberian-Tiger-Image.jpg");
    m_data->title = QLatin1String("Siberian tiger");
    m_data->photoUrl = QLatin1String("/home/nikolay/Siberian-Tiger-Image.jpg");
    m_data->prevDayUrl = QString();
    
    emit finished(0);
    return;
    
    //=================================================================================
    
    
    
    m_data->canonicalUrl = url.toString();
    
    connect( KIO::storedGet( url, KIO::Reload, KIO::HideProgressInfo ), &KJob::result, [this] ( KJob *job )
    {        
        if( 0 != job->error() )
        {
            m_error = job->errorString();
            emit finished(1);
        }
        else
        {
            KIO::StoredTransferJob *stJob = qobject_cast<KIO::StoredTransferJob*>(job);
            
            Q_ASSERT(stJob != nullptr);
            
            if( parseWebPage( stJob->data() ) )
            {
                //TODO: Store in cache
                GetImage();
            }
            else
            {
                m_error = i18n("Web page parsing error");
            }
        }
        
        job->deleteLater(); //NOTE: "The default for any KJob is to automatically delete itself."
    } );
}

void DataFetcher::GetImage()
{
    connect( KIO::storedGet( m_data->photoUrl, KIO::Reload, KIO::HideProgressInfo ), &KJob::result, [this] ( KJob *job )
    {
        if( 0 != job->error() )
        {
            m_error = job->errorString();
            emit finished(2);
        }
        else
        {
            // save
            // TODO: insert in db if cache enabled will be done by plugin
            auto *stJob = qobject_cast<KIO::StoredTransferJob*>(job);
            Q_ASSERT(stJob != nullptr);
            
            auto provider = qobject_cast<NatGeoProvider*>( parent() );
            
            if( provider->cacheManager()->save( stJob->url().fileName(), stJob->data(), m_data->cacheUrl ) )
            {
            }
            else
            {
                //TODO: When we don't use cache or save failed pass image URL to applet
                m_data->cacheUrl = m_data->photoUrl;
            }
            
            emit finished(0);
        }
    });
}

bool DataFetcher::parseWebPage(const QByteArray& source)
{
    // get canonical URL
    QString pattern( QLatin1Literal( "<link rel=\"canonical\" href=\"(http://photography.nationalgeographic.com/photography/photo-of-the-day/.*/)\" />" ) );
    QRegExp regExp( pattern );
    regExp.setMinimal( true );
 
    if( regExp.indexIn( source ) == -1 ) {
        return false;
    }
    
    m_data->canonicalUrl = regExp.cap(1);
    
    
    // get image URL. I use URL for Twitter. There is another URL for Google+ and "real" URL in <div class="primary_photo">. All generate the same image
    pattern = QLatin1String("<meta name=\"twitter:image\" content=\"(//images.nationalgeographic.com/wpf/media-live/photos/.*)\"/>");
    regExp.setPattern( pattern );
    
    if( regExp.indexIn( source ) == -1 ) {
        return false;
    }
    
    m_data->photoUrl = QLatin1Literal("http:") + regExp.cap(1);
    
    
    // get previous day URL
    pattern = QLatin1String("<p class=\"prev first\"><a href=\"(/photography/photo-of-the-day/.*/)\">&laquo; Previous</a></p>");
    regExp.setPattern( pattern );
    
    if( regExp.indexIn( source ) != -1 ) {
        m_data->prevDayUrl = QLatin1Literal("http://photography.nationalgeographic.com") + regExp.cap(1);
    }
    
    // get title
    m_data->title = getTitle(source);
    
        
    //TODO: Description [opt]: <meta name="description" content="Sharks gather to feed in the waters off Mexico’s Guadalupe Island in this Photo of the Day from the National Geographic Photo Contest." />
    
    return true;   
}

void DataFetcher::onFiniched(int result)
{
    if( result ) {
        //TODO: set result to all registered data containers
        return;
    }
    
    for( PotdDataContainer* dc: m_associatedDataContainers ) {
        
        // save data in case 
        dc->setDataStruct( m_data );
        
        dc->setData( cPhotoKey, m_data->cacheUrl );
        dc->setData( cPageUrlKey, m_data->canonicalUrl );
        dc->setData( cPrevPageUrlKey, m_data->prevDayUrl );
        dc->setData( cTitleKey, m_data->title );
    }
}



#include "natgeo.moc"
