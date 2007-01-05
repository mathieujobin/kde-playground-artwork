//////////////////////////////////////////////////////////////////////////////
// oxygenbutton.cpp
// -------------------
// Oxygen window decoration for KDE. Buttons.
// -------------------
// Copyright (c) 2006, 2007 Riccardo Iaconelli <ruphy@fsfe.org>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//////////////////////////////////////////////////////////////////////////////
#include <QPainterPath>
#include <QPainter>
#include <QBitmap>

#include <kdecoration.h>


#include "oxygenclient.h"
#include "oxygenbutton.h"
#include "oxygen.h"
#include "definitions.cpp"

namespace Oxygen
{

// class OxygenClient;
/*
extern int BUTTONSIZE;
extern int DECOSIZE;*/

// static const int BUTTONSIZE      = 18;
// static const int DECOSIZE        = 8;
//////////////////////////////////////////////////////////////////////////////
// OxygenButton Class                                                      //
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// OxygenButton()
// ---------------
// Constructor

OxygenButton::OxygenButton(OxygenClient *parent,
                             const QString& tip, ButtonType type,
                             const unsigned char *bitmap)
    : QAbstractButton(parent->widget()), client_(parent), type_(type),
      deco_(0), lastmouse_(0)
{
    //setBackgroundMode(Qt::NoBackground); PORT to qt4
    setFixedSize(BUTTONSIZE, BUTTONSIZE);
    //setCursor(Qt::arrowCursor); PORT to qt4
    if (bitmap) setBitmap(bitmap);
    setToolTip(tip);
}

OxygenButton::~OxygenButton()
{
    if (deco_) delete deco_;
}

//////////////////////////////////////////////////////////////////////////////
// setBitmap()
// -----------
// Set the button decoration

void OxygenButton::setBitmap(const unsigned char *bitmap)
{
    if (!bitmap) return; // no bitmap, probably the menu button

    if (deco_) delete deco_;
    deco_ = new QBitmap(DECOSIZE, DECOSIZE);
    //PORT to qt4 set actual pixels   , bitmap, true);
    deco_->setMask(*deco_);
    repaint();
}

//////////////////////////////////////////////////////////////////////////////
// sizeHint()
// ----------
// Return size hint

QSize OxygenButton::sizeHint() const
{
    return QSize(BUTTONSIZE, BUTTONSIZE);
}

//////////////////////////////////////////////////////////////////////////////
// enterEvent()
// ------------
// Mouse has entered the button

void OxygenButton::enterEvent(QEvent *e)
{
    // if we wanted to do mouseovers, we would keep track of it here
    QAbstractButton::enterEvent(e);
}

//////////////////////////////////////////////////////////////////////////////
// leaveEvent()
// ------------
// Mouse has left the button

void OxygenButton::leaveEvent(QEvent *e)
{
    // if we wanted to do mouseovers, we would keep track of it here
    QAbstractButton::leaveEvent(e);
}

//////////////////////////////////////////////////////////////////////////////
// drawButton()
// ------------
// Draw the button

void OxygenButton::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    QRadialGradient grad1(QPointF(5.0, 5.0),5.0);
    grad1.setColorAt(0.9, QColor(0,0,0,100));
    grad1.setColorAt(1.0, QColor(0,0,0,0));
    QRadialGradient grad2(QPointF(5.0, 5.0),5.0);
    grad2.setColorAt(0.0, QColor(235,235,235));
    grad2.setColorAt(1.0, QColor(220,220,220));
    QRadialGradient grad3(QPointF(5.0, 3.75), 3.5,QPointF(5.0, 2.5));
    grad3.setColorAt(0, QColor(255,255,255,50));
    grad3.setColorAt(1, QColor(255,255,255,0));
    QRadialGradient grad4(QPointF(5.0, 3.3), 3.5, QPointF(5.0, 2.1));
    grad4.setColorAt(0, QColor(255,255,255,50));
    grad4.setColorAt(1, QColor(255,255,255,0));
    QBrush brush1(grad1);
    QBrush brush2(grad2);
    QBrush brush3(grad3);
    QBrush brush4(grad4);

    painter.scale(1.6, 1.6);
    painter.setRenderHint(QPainter::Antialiasing,true);

    QPainterPath path1;
    path1.addEllipse(0.0, 0.0, 10.0, 10.0);
    painter.fillPath(path1, brush1);

    QPainterPath path2;
    path2.addEllipse(0.5, 0.5, 9.0, 9.0);
    painter.fillPath(path2, brush2);

    QPainterPath path3;
    path3.addEllipse(1.5, 0.5, 7.0, 6.0);
    painter.fillPath(path3, brush3);

    QPainterPath path4;
    path4.addEllipse(1.5, 0.5, 7.0, 6.0);
    painter.fillPath(path4, brush4);
//     QPainter painter(this);
// 
//     if (!OxygenFactory::initialized()) return;
// 
//     QPalette palette;
//     int dx, dy;
// 
//     // paint a plain box with border
//     palette = KDecoration::options()->palette(KDecoration::ColorFrame, client_->isActive());
//     painter.fillRect(rect(), palette.button());
//     
//     if (type_ == ButtonMenu) {
//         // we paint the mini icon (which is 16 pixels high)
//         dx = (width() - 16) / 2;
//         dy = (height() - 16) / 2;
// //PORT qt4        painter.drawPixmap(dx, dy, client_->icon().pixmap(QIcon::Small, QIcon::Normal));
//     } else {
// //       painter->fillRect(rect(), palette.button());
//        int x,y,w,h;
//        rect().getRect(&x, &y, &w, &h);
//        painter.setBrush(palette.dark());
//        painter.drawEllipse(x, y, w, h);
// 
//        if (deco_) {
//            // otherwise we paint the deco
//            dx = (width() - DECOSIZE) / 2;
//            dy = (height() - DECOSIZE) / 2;
//            painter.setBrush(palette.dark());
//            painter.drawPixmap(dx, dy, *deco_);
//        }
//     }
}








} //namespace Oxygen
