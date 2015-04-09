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

PotdDataContainer::PotdDataContainer(ProviderCore* provider, QObject* parent): DataContainer(parent),
m_provider(provider)
{
    Q_ASSERT(m_provider == nullptr);

    m_provider->registerContainer();
   
    connect( this, &Plasma::DataContainer::updateRequested, [this](DataContainer *  source) {
        // request update from provider. This signal is emitted every time the polling interval expires.
        this->m_provider->checkForNewPhoto(this);
    } );
    
    m_provider->checkForNewPhoto(this);
}

PotdDataContainer::~PotdDataContainer()
{
    m_provider->unregisterContainer();
}

