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

#include <QtCore/QSettings>

#include <KI18n/KLocalizedString>

#include <KCoreAddons/KJob>
#include <KIO/Job>
#include <KIO/StoredTransferJob>

#include "apod.h"

#include <QDebug>

K_EXPORT_PLASMA__PHOTOOFTHEDAYPROVIDER_WITH_JSON( apod, ApodProvider, "photooftheday-provider-apod.json" )

//===== helper function =====
static QString getTitle( const QString& data )
{
    QString title = QLatin1String("Astronomy Picture of the Day");

    const QString pattern = QLatin1String("<center>\n<b>(.*?)</b>");
    QRegExp regExp;
    regExp.setMinimal(true);
    regExp.setPattern( pattern );

    if( regExp.indexIn( data ) == -1 ) {
        title = regExp.cap(1);
    }

    return title;
}

//==========
ApodProvider::ApodProvider(QObject *parent, const QVariantList &args):
ProviderCore(parent, args)
{
    connect( this, &ApodProvider::photoDownloaded, this, &ApodProvider::onImageDownloaded );
}

void ApodProvider::checkForNewPhoto()
{
    const QUrl pageUrl = QString( QLatin1Literal("http://apod.nasa.gov/apod/astropix.html") );

    qDebug() << "Apotd: requesting page";

    m_data[cPageUrlKey] = pageUrl;

    connect( KIO::storedGet( pageUrl, KIO::Reload, KIO::HideProgressInfo ), &KJob::result, [this] ( KJob *job )
    {
        qDebug() << "Apotd: Page received. Error: " << job->error();

        if( 0 != job->error() )
        {
            emit error(job->errorString());
        }
        else
        {
            auto *storedJob = qobject_cast<KIO::StoredTransferJob*>(job);

            Q_ASSERT(storedJob != nullptr);

            qDebug() << "Parse page";

            this->parseWebPage( storedJob->data() );
        }

        job->deleteLater(); //NOTE: "The default for any KJob is to automatically delete itself."
    } );

}

//TOOD: This function can be moved to ProviderCore and used immediately, instead of ImageDownloaded signal
void ApodProvider::onImageDownloaded()
{
    saveInCache();

    // Finish
    emit newPhotoAvailable(m_data);
}

void ApodProvider::parseWebPage(const QByteArray& source)
{
    const QString data = QString::fromUtf8( source );

    const QString pattern( QLatin1String( "<a href=\"(image/.*)\"" ) );
    QRegExp exp( pattern );
    exp.setMinimal( true );

    if ( exp.indexIn( data ) == -1 ) {
        emit error( i18n("Web page parsing error") );
        qDebug() << "Web page parsing error 1";
        return;
    }

    const QString sub = exp.cap(1);
    QUrl photoUrl( QString( QLatin1String( "http://antwrp.gsfc.nasa.gov/apod/%1" ) ).arg( sub ) );

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

    // get title
    m_data[cTitleKey] = getTitle(data);

    downloadPhoto(photoUrl);
}





#include "apod.moc"
