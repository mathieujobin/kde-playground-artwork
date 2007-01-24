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

#include <QX11Info>
#include <X11/Xlib.h>
#include <fixx11h.h>
#include "oxrender.h"

static Window root = RootWindow (QX11Info::display(), DefaultScreen (QX11Info::display()));

static Picture createFill(Display *dpy, const XRenderColor *c)
{
   XRenderPictureAttributes pa;
   Pixmap pixmap = XCreatePixmap (dpy, root, 1, 1, 32);
   if (!pixmap)
      return X::None;
   pa.repeat = True;
   Picture fill = XRenderCreatePicture (dpy, pixmap, XRenderFindStandardFormat (dpy, PictStandardARGB32), CPRepeat, &pa);
   if (!fill)
   {
      XFreePixmap (dpy, pixmap);
      return X::None;
   }
   XRenderFillRectangle (dpy, PictOpSrc, fill, c, 0, 0, 1, 1);
   XFreePixmap (dpy, pixmap);
   return fill;
}

bool OXRender::blend(const QPixmap &upper, const QPixmap &lower, double opacity)
{
   Display *dpy = QX11Info::display();
   XRenderColor c = {0,0,0, (short uint)(opacity * 0xffff) };
   Picture alpha = createFill (dpy, &c);
   if (alpha == X::None)
      return false;
   XRenderComposite (dpy, PictOpOver, upper.x11PictureHandle(), alpha, lower.x11PictureHandle(), 0, 0, 0, 0, 0, 0, upper.width(), upper.height());
   XRenderFreePicture (dpy, alpha);
   return true;
}

QPixmap OXRender::applyAlpha(QPixmap &toThisPix, QPixmap &fromThisPix)
{
   QPixmap pix(toThisPix.size());
   pix.fill(Qt::transparent);
   XRenderComposite (QX11Info::display(), PictOpOver, pix.x11PictureHandle(),
                     fromThisPix.x11PictureHandle(), toThisPix.x11PictureHandle(),
                     0, 0, 0, 0, 0, 0, pix.width(), pix.height());
   return pix;
}

void OXRender::setColor(XRenderColor &xc, double r, double g, double b, double a)
{
   xc.red = ((short uint)(r*0xffff)) & 0xffff;
   xc.green = ((short uint)(g*0xffff)) & 0xffff;
   xc.blue = ((short uint)(b*0xffff)) & 0xffff;
   xc.alpha = ((short uint)(a*0xffff)) & 0xffff;
}

void OXRender::setColor(XRenderColor &xc, QColor qc)
{
   xc.red = qc.red()*0x101; xc.green = qc.green()*0x101;
   xc.blue = qc.blue()*0x101; xc.alpha = qc.alpha()*0x101;
}

void OXRender::setGradient(XLinearGradient &lg, QPoint p1, QPoint p2)
{
   lg.p1.x = p1.x(); lg.p1.y = p1.y();
   lg.p2.x = p2.x(); lg.p2.y = p2.y();
}

void OXRender::setGradient(XLinearGradient &lg, XFixed x1, XFixed y1, XFixed x2, XFixed y2)
{
   lg.p1.x = x1; lg.p1.y = y1;
   lg.p2.x = x2; lg.p2.y = y2;
}
