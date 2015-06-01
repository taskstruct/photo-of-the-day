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
