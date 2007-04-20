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

#ifndef DYNAMICBRUSH_H
#define DYNAMICBRUSH_H

class QTimer;
class QImage;
class QRect;
class QGLWidget;

#include <QMap>
#include <QObject>
#include <QSize>
#include <QRect>
#include <QPixmap>
#include <X11/Xlib.h>
#include <fixx11h.h>

typedef QMap<QWidget*, QPixmap> BgPixCache;

class DynamicBrush : public QObject
{
   Q_OBJECT
public:
   enum Mode {Tiled = 0, QtGradient, XRender, OpenGL,
         VGradient1, HGradient1, VGradient2, HGradient2, Glass};
   DynamicBrush(Mode mode, QObject *parent = 0);
   DynamicBrush(Pixmap pixmap = -1, Pixmap shadow = -1, int bgYoffset = 0, QObject *parent = 0);
   ~DynamicBrush();
//    QPixmap shadow(const QRect &rect);
   void setMode(Mode);
   void setXPixmap(Pixmap pixmap = -1, Pixmap shadow = -1);
   virtual bool eventFilter ( QObject * watched, QEvent * event );
private:
   BgPixCache::iterator checkCache(bool &found);
   void updateBrushTiled();
   void updateBrushGL();
   void updateBrushRender();
   void updateBrushGradient1();
   void updateBrushGradient2();
   void updateBrushQt();
   void updateBrushGlass();
   void initGL();
   void generateTiles(Mode mode);
   QPixmap glPixmap(const QRect &rect, int darkness = 0);
   QSize _size;
   QWidget *_topLevelWidget;
   Pixmap _pixmap, _shadow;
   int _bgYoffset;
   Mode _mode;
   QTimer *_timerBgWipe;
   QPixmap _tile[2][2];
   QColor _bgC[2];
   bool _isActiveWindow;
   QPixmap _glShadow;
   QRect _lastShadowRect;
   QGLWidget *_glContext;
private slots:
   void wipeBackground();
};

#endif //DYNAMICBRUSH_H
