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

#ifndef APODPROVIDER_H
#define APODPROVIDER_H

#include "providercore.h"

class KJob;

struct Data {
    QString photoUrl;
    QString cacheUrl;
    QString canonicalUrl;
    QString prevDayUrl;
    
    QString title;
    QString description;
    
    int id;
};

class DataFetcher: public QObject
{
    Q_OBJECT
public:
    explicit DataFetcher( QObject *parent = 0 );
    ~DataFetcher() {};
    
    void GetWebPage( const QUrl & url);
    void GetImage();
    
Q_SIGNALS:
    void finished( int result );
    
private:
    bool parseWebPage( const QByteArray & source );
    
    QString m_error = QString();
        
    Data m_data;
    
    friend class ApodProvider;
};

class ApodProvider : public ProviderCore
{
    Q_OBJECT

public:
    ApodProvider(QObject* parent = 0, const QVariantList& args = QVariantList());
    
    void requestPhoto(const QString& source, int offset = 0);
    
Q_SIGNALS:
    void error(const QString& reason);
    
private:
    DataFetcher *m_fetcher = nullptr;
};

#endif // APODPROVIDER_H
