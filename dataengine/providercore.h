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

#ifndef POTDPROVIDER_H
#define POTDPROVIDER_H

#include <QtCore/QObject>

#include <kpluginfactory.h>
#include <plasma/dataengine.h>

#include "photooftheday_export.h"

namespace Plasma {
    class DataEngine;
}

const QLatin1Literal cPhotoKey("Photo");
const QLatin1Literal cPageUrlKey("PageUrl");
const QLatin1Literal cPrevPageUrlKey("PrevPageUrl");
const QLatin1Literal cTitleKey("Title");

class PHOTOOFTHEDAY_EXPORT ProviderCore : public QObject
{
    Q_OBJECT

public:
    //TODO: heck if parent can be default param
    ProviderCore(QObject* parent = 0, const QVariantList& args = QVariantList() );
    
    virtual void requestPhoto(const QString& source, int offset = 0) = 0;
        
Q_SIGNALS:
    void photoReady(const QString& source, const Plasma::DataEngine::Data& data);

private:
    
    inline Plasma::DataEngine* dataEngine() { return qobject_cast<Plasma::DataEngine*>( parent() ); }
};


#define K_EXPORT_PLASMA_PHOTOOFTHEDAYPROVIDER(libname, classname) \
K_PLUGIN_FACTORY(factory, registerPlugin<classname>();) \
K_EXPORT_PLUGIN_VERSION(1.0)

#endif // POTDPROVIDER_H
