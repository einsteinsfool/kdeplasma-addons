/***************************************************************************
 *   Copyright (C) 2009 by Björn Ruberg <bjoern@ruberg-wegener.de>    *
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


#include "FuncKey.h"

#include <kpushbutton.h>
#include <QPainter>
#include <plasma/theme.h>
#include "Helpers.h"

FuncKey::FuncKey(QPoint relativePosition, QSize relativeSize, unsigned int keycode, QString label):
        AlphaNumKey(relativePosition, relativeSize, keycode), is_pressed(false), toggler(false) {
    setLabel(label);
}

void FuncKey::paint(QPainter *painter){
    AlphaNumKey::paint(painter);
}

void FuncKey::paintArrow(QPainter *painter){

    painter->setBrush(Plasma::Theme::defaultTheme()->color(Plasma::Theme::ButtonTextColor));
	painter->drawLine(-1, 0 , 3, 0);

	const QPointF points[3] = {
	     QPointF(-3, 0),
	     QPointF(-1, 1),
	     QPointF(-1, -1),
	 };

	painter->drawConvexPolygon(points, 3);
}

void FuncKey::released() {
	if(!toggler){
		BoardKey::released();
	}
}

void FuncKey::setKey(unsigned int code, bool sendUp, const QString text) {
        //setKeycode(code, sendUp);
        //setText(text);
}

void FuncKey::sendKeycodeToggled() {
    if( is_pressed ) {
		sendKeycodeRelease();
		toggleOff();
	}
	else {
		sendKeycodePress();
		toggleOn();
	}
}

void FuncKey::toggleOn(){
    is_pressed = true;
}

void FuncKey::toggleOff(){
    is_pressed = false;
}

void FuncKey::toggle(bool toggle){
    is_pressed = toggle;
}

bool FuncKey::toggled(){
    return is_pressed;
}
