/*
 *   Copyright 2007 Tobias Koenig <tokoe@kde.org>
 *   Copyright 2008 Anne-Marie Mahfouf <annma@kde.org>
 *   Copyright 2013 Aaron Seigo <aseigo@kde.org>
 *
 *   This program is free software; you can redistribute it and/or modify  
 *   it under the terms of the GNU General Public License as published by  
 *   the Free Software Foundation; either version 2 of the License, or     
 *   (at your option) any later version.      
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef NATGEOPROVIDER_H
#define NATGEOPROVIDER_H

#include "potdprovider.h"

/**
 * This class provides the image for APOD 
 * "Astronomy Picture Of the Day"
 * located at http://antwrp.gsfc.nasa.gov/apod.
 * Direct link to the picture of the day page is 
 * http://antwrp.gsfc.nasa.gov/apod/apYYMMDD.html 
 * where YY is the year last 2 digits,
 * MM is the month and DD the day, in 2 digits.
 */
class NatGeoProvider : public PotdProvider
{
    Q_OBJECT

    public:
        /**
         * Creates a new APOD provider.
         *
         * @param date The date for which the image shall be fetched.
         * @param parent The parent object.
         */
        NatGeoProvider( QObject *parent, const QVariantList &args );

        /**
         * Destroys the APOD provider.
         */
        ~NatGeoProvider() override;

        /**
         * Returns the requested image.
         *
         * Note: This method returns only a valid image after the
         *       finished() signal has been emitted.
         */
        QImage image() const override;

    private:
      class Private;
      Private* const d;
};

#endif
