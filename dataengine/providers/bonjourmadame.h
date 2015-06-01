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

#ifndef BONJOURMADAMEPROVIDER_H
#define BONJOURMADAMEPROVIDER_H

#include "providercore.h"

class BonjourMadameProvider : public ProviderCore
{
    Q_OBJECT

public:
    explicit BonjourMadameProvider(QObject* parent = 0, const QVariantList& args = QVariantList());
    ~BonjourMadameProvider() = default;
    
    void checkForNewPhoto();

public Q_SLOTS:
    void onImageDownloaded();
    
private:
    void parseWebPage( const QByteArray & source );
};

#endif // BONJOURMADAMEPROVIDER_H
