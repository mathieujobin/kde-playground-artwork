/* Plastik widget style for KDE 4
   Copyright (C) 2005 Sandro Giessl <sandro@giessl.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Steet, Fifth Floor,
   Boston, MA 02110-1301, USA.
 */

#ifndef __PLASTIK_H
#define __PLASTIK_H

#include <kstyle.h>

class Plastik2Style : public KStyle
{
//     Q_OBJECT
// 
public:
    Plastik2Style();
    virtual ~Plastik2Style();

    virtual void drawKStylePrimitive(WidgetType widgetType, int primitive,
                                     const QStyleOption* opt,
                                     QRect r, QPalette pal, State flags,
                                     QPainter* p,
                                     const QWidget* widget = 0,
                                     Option* kOpt = 0) const;
};

#endif // __PLASTIK_H
