/***************************************************************************
 *   Copyright (C) 2006-2007 by Thomas Lübking                             *
 *   thomas.luebking@web.de                                                *
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
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef OXRENDER_H
#define OXRENDER_H

#include <QPixmap>
#include <QVector>
#include <X11/extensions/Xrender.h>

typedef QVector<double> PointArray;
typedef QVector<QColor> ColorArray;

class OXRender
{
// public:
//    OXRender(){}
public:
   static bool blend(const QPixmap &upper, const QPixmap &lower, double opacity = 0.5);
   static void setColor(XRenderColor &xc, double r, double g, double b, double a = 1);
   static void setColor(XRenderColor &xc, QColor qc);
   static QPixmap applyAlpha(QPixmap &toThisPix, QPixmap &fromThisPix);
   static void setGradient(XLinearGradient &lg, QPoint p1, QPoint p2);
   static void setGradient(XLinearGradient &lg, XFixed x1, XFixed y1, XFixed x2, XFixed y2);
   static Picture gradient(const QPoint start, const QPoint stop, const ColorArray &colors, const PointArray &stops = PointArray());
};

#define Q2XRenderColor(_XRC_, _QC_) XRenderColor _XRC_; OXRender::setColor(_XRC_, _QC_)

#endif //OXRENDER_H
