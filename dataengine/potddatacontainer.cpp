/*
 * Copyright (c) 2014, Nikolay Nikolov <nikolay.stef.nikolov@gmail.com>
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
 * THIS SOFTWARE IS PROVIDED BY Nikolay Nikolov <nikolay.stef.nikolov@gmail.com> ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL Nikolay Nikolov <nikolay.stef.nikolov@gmail.com> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include "photooftheday.h"
#include "providercore.h"
#include "potddatacontainer.h"

#include <QDebug>

PotdDataContainer::PotdDataContainer(ProviderCore* provider, QObject* parent): Plasma::DataContainer(parent),
m_provider(provider)
{
    Q_ASSERT(m_provider == nullptr);

    qDebug() << "Adding container " << objectName();

    m_provider->registerContainer();
   
    connect( this, &Plasma::DataContainer::updateRequested, [](Plasma::DataContainer * source) {
        // request update from provider. This signal is emitted every time the polling interval expires.
        auto dc = qobject_cast<PotdDataContainer*>(source);
        if(dc) {
            dc->m_provider->checkForNewPhoto(dc);
        }
    } );

    connect( provider, &ProviderCore::newPhotoAvailable, [this](const Plasma::DataEngine::Data data) {

        auto iter = data.cbegin();
        while( iter != data.cend() ) {
            this->setData( iter.key(), iter.value() );
            ++iter;
        }

        this->setData( cErrorKey, QString() );
        this->triggerUpdate();
    } );

    connect( provider, &ProviderCore::error, [this](const QString error) {
        this->setData( cErrorKey, error );
        this->triggerUpdate();
    } );
    
    m_provider->checkForNewPhoto(this);
}

PotdDataContainer::~PotdDataContainer()
{
    m_provider->unregisterContainer();
}

