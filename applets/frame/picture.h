/***************************************************************************
 *   Copyright  2008 by Anne-Marie Mahfouf <annma@kde.org>                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA .        *
 ***************************************************************************/

#ifndef PICTURE_H
#define PICTURE_H

#include <QPixmap>
#include <QImage>

class KUrl;

/**
 * @brief Picture choice
 * @author Anne-Marie Mahfouf <mahfouf@kde.org>
 *
 * This class handles the choice of the picture and 
 * makes it ready for the Frame class to paint this picture.
 */

class Picture 
{
    public:
	Picture();
	~Picture();
	/** 
	* Set Default picture with written message @p message if no picture or folder was choosen 
	* by the user
	**/
	QImage defaultPicture(const QString &message);
	/** 
	* Set picture from location @p currentUrl 
	**/
	QImage setPicture( KUrl currentUrl);
	/**
	* Find all the pictures in each of the dirs that are listed in @p slideShowPaths
	**/
	QStringList findSlideShowPics(const QStringList &slideShowPaths);

    private:
        QImage getPicture(){return m_picture;};
	QImage m_picture;
	/// Maximum Picture Dimension
        static const int m_maxDimension=800;

};


#endif

