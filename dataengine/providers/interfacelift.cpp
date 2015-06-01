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

#include "interfacelift.h"

#include <QDebug>

K_EXPORT_PLASMA__PHOTOOFTHEDAYPROVIDER_WITH_JSON( interfacelift , InterfaceliftProvider, "photooftheday-provider-interfacelift.json" )

static const QLatin1Literal cInterfaceliftUrl = QLatin1Literal("https://interfacelift.com/");

//==========
InterfaceliftProvider::InterfaceliftProvider(QObject *parent, const QVariantList &args):
ProviderCore(parent, args)
{
    connect( this, &InterfaceliftProvider::photoDownloaded, this, &InterfaceliftProvider::onImageDownloaded );
}

void InterfaceliftProvider::checkForNewPhoto()
{   
    const QUrl pageUrl = QString( cInterfaceliftUrl );

    qDebug() << "IFL: requesting page";
    
    connect( KIO::storedGet( pageUrl, KIO::Reload, KIO::HideProgressInfo ), &KJob::result, [this] ( KJob *job )
    {        
        qDebug() << "IFL: Page received. Error: " << job->error();

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
void InterfaceliftProvider::onImageDownloaded()
{
    qDebug() << "IFL: Image downloaded";

    saveInCache();

    // Finish
    emit newPhotoAvailable(m_data);
}

void InterfaceliftProvider::parseWebPage(const QByteArray& source)
{    
    const QString data = QString::fromUtf8( source );

    QString pattern = QLatin1String("<img id=\"wall_img\" src=\"(/wallpaper/previews/.*)\" />");
    QRegExp regExp;
    regExp.setMinimal(true);
    regExp.setPattern( pattern );
    
    if( regExp.indexIn( data ) == -1 ) {
        emit error( i18n("Web page parsing error") );
        qDebug() << "Web page parsing error 1";
        return;
    }

    const QString photoUrlString = cInterfaceliftUrl + regExp.cap(1);
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
    
    // get page url
    pattern = QLatin1String("<a class=\"label\" href=\"(/wallpaper/details/.*)\">");
    regExp.setPattern(pattern);

    if( regExp.indexIn(data) != -1 )
    {
        m_data[cPageUrlKey] = QUrl( cInterfaceliftUrl + regExp.cap(1) );
    }
    else
    {
        m_data[cPageUrlKey] = QUrl( cInterfaceliftUrl );
    }

    // get title
    pattern = QLatin1String("<h1>(.*) &rarr;</h1>");
    regExp.setPattern( pattern );

    if( regExp.indexIn(data) != -1 )
    {
        m_data[cTitleKey] = regExp.cap(1);
    }
    else
    {
        m_data[cTitleKey] = QString( QLatin1String("Interfacelift wallpaper") );
    }
    
    qDebug() << "IFL: Page parsed";

    downloadPhoto( photoUrl );
}


#include "interfacelift.moc"
