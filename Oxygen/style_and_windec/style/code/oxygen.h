/***************************************************************************
 *   Copyright (C) 2006 by Thomas Lübking                                  *
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

#ifndef OXYGEN_STYLE_H
#define OXYGEN_STYLE_H

class QAbstractButton;
class QHeaderView;
class QMenuBar;
class QPushButton;
class QScrollBar;
class QTabBar;
class DynamicBrush;

#include <QHash>
#include <QMap>
#include <QCommonStyle>
#include <QBitmap>
#include <QRegion>
#include <X11/Xlib.h>
#include <fixx11h.h>

namespace Oxygen {

enum BGMode { Plain = 0, Scanlines, BrushedMetal, FullPix };
enum Acceleration { None = 0, XRender, OpenGL };

class TileSet
{
public:
   enum Tile {TopLeft = 0, TopRight, BtmLeft, BtmRight, TopMid, MidLeft, MidRight, BtmMid, Center };
   TileSet(const QPixmap &pix, int xOff, int yOff, int width, int height,
           int top, int bottom, int left, int right, QRegion *topLeft = 0, QRegion *topRight = 0, QRegion *btmLeft = 0, QRegion *btmRight = 0);
   TileSet(){for (int i=0; i < 4; i++) border[i] = 0L;}
   ~TileSet()
   {
//       for (int i=0; i < 4; i++)
//          if (border[i]) delete border[i];
   }
   inline const QPixmap &pixmap( Tile pos ) const
   {
      return ( pixmaps[ ( int ) pos ] );
   }
   inline const QBitmap &mask( Tile pos ) const
   {
      return ( masks[ ( int ) pos ] );
   }
   inline int width(Tile pos) const
   {
      return pixmaps[pos].width();
   }
   inline int height(Tile pos) const
   {
      return pixmaps[pos].height();
   }
   inline QRegion *corner(Tile Pos) const
   {
      if (Pos < TopMid)
         return border[Pos];
      else
         return 0L;
   }
   const QRegion outerRegion(const QRect &r) const;
   const QRegion innerRegion(const QRect &r) const;
private:
   QPixmap pixmaps[ 9 ];
   QBitmap masks[9];
   QRegion *border[4];
   int size[4];
};



class EventKiller : public QObject 
{
   Q_OBJECT
public:
   bool eventFilter( QObject *, QEvent *)
   {
      return true;
   }
};

class OxygenStyle;

class ButtonFadeInfo 
{
public:
   ButtonFadeInfo()
   {
   timerId=0; index=0; fadeIn=TRUE;
   }
public:
   int timerId;
   int index;
   bool fadeIn;
};

enum Orientation3D {Sunken = 0, Relief, Raised};

typedef struct Config
{
   bool raisedButtons; // whether to use soft gradients instead of glosses
   bool roundButtons; // whether pushbuttons have round edges like toolbarbuttons
   bool menuHighlightBevel; //whether the higlight indicator is a gloss/gradient or plain
   bool inversePopups; // whether the popups are foreground or background colored
   bool inverseButtons;
   bool swapHoveredButtonColors;
   QPalette::ColorRole scrollbarBg;
   QPalette::ColorRole scrollbarFg;
   QPalette::ColorRole progressColor; // color mode of the progress
   QPalette::ColorRole progressTextColor; // --""-- percentage (autocalculated)
   QPalette::ColorRole tabColor;
   QPalette::ColorRole tabTextColor;
   BGMode bgMode;
   Acceleration acceleration;
   Oxygen::Orientation3D tabwidget3D;
   int gradientIntensity;
   int fullPixMode;
} Config;

class OxygenStyle : public QCommonStyle
{
   Q_OBJECT
public:
   enum Position {Top = 0x1, Left=0x2, Bottom=0x4, Right=0x8, Full=0xf, Center=0x10};
   typedef uint PosFlags;
   enum WidgetState{Basic = 0, Hovered, Focused, Active};
   
   OxygenStyle();
   virtual ~OxygenStyle();

   //inheritance from QStyle
   virtual void drawComplexControl ( ComplexControl control, const QStyleOptionComplex * option, QPainter * painter, const QWidget * widget = 0 ) const;
  
   virtual void drawControl ( ControlElement element, const QStyleOption * option, QPainter * painter, const QWidget * widget = 0 ) const;
   
   /**what do they do?
   virtual void drawItemPixmap ( QPainter * painter, const QRect & rect, int alignment, const QPixmap & pixmap ) const;
   virtual void drawItemText ( QPainter * painter, const QRect & rect, int alignment, const QPalette & pal, bool enabled, const QString & text, QPalette::ColorRole textRole = QPalette::NoRole ) const;
   */
   
   virtual void drawPrimitive ( PrimitiveElement elem, const QStyleOption * option, QPainter * painter, const QWidget * widget = 0 ) const;
   
   virtual QPixmap standardPixmap ( StandardPixmap standardPixmap, const QStyleOption * option = 0, const QWidget * widget = 0 ) const;
   
   /**what do they do?
   virtual QPixmap generatedIconPixmap ( QIcon::Mode iconMode, const QPixmap & pixmap, const QStyleOption * option ) const;
   virtual SubControl hitTestComplexControl ( ComplexControl control, const QStyleOptionComplex * option, const QPoint & pos, const QWidget * widget = 0 ) const;
   virtual QRect itemPixmapRect ( const QRect & rect, int alignment, const QPixmap & pixmap ) const;
   virtual QRect itemTextRect ( const QFontMetrics & metrics, const QRect & rect, int alignment, bool enabled, const QString & text ) const;
   */
   
   virtual int pixelMetric ( PixelMetric metric, const QStyleOption * option = 0, const QWidget * widget = 0 ) const;
   
   virtual void polish( QWidget *w );
   virtual void polish( QApplication * );
   virtual void polish( QPalette &pal );
   
   virtual QSize sizeFromContents ( ContentsType type, const QStyleOption * option, const QSize & contentsSize, const QWidget * widget = 0 ) const;
      
   virtual int styleHint ( StyleHint hint, const QStyleOption * option = 0, const QWidget * widget = 0, QStyleHintReturn * returnData = 0 ) const;
   
   virtual QRect subControlRect ( ComplexControl control, const QStyleOptionComplex * option, SubControl subControl, const QWidget * widget = 0 ) const;
   
   virtual QRect subElementRect ( SubElement element, const QStyleOption * option, const QWidget * widget = 0 ) const;
   
   virtual QPalette standardPalette () const;
   
   virtual void unPolish( QWidget *w );
   virtual void unPolish( QApplication *a );
   
   // from QObject
   bool eventFilter( QObject *object, QEvent *event );
   
signals:
   void MDIPopup(QPoint);
   
private slots:
   void fakeMouse();
   void handleIPC(int, int);
   void progressbarDestroyed(QObject*);
   void updateProgressbars();

private:
   OxygenStyle( const OxygenStyle & );
   OxygenStyle& operator=( const OxygenStyle & );
   void renderFrame( QPainter *p, const QRect &r, Orientation3D o3D, PosFlags pf, const QWidget *widget = 0L, bool highlighted = false, bool round = false) const;
   const QPixmap colorRun(const QColor &c, const int size, Qt::Orientation o, Orientation3D o3D, bool smooth = false) const;
   const QPixmap gradient(const QColor &c, const int size, Qt::Orientation o, Orientation3D o3D) const;
   const QPixmap gloss(const QColor &c, const int size, Qt::Orientation o, Orientation3D o3D) const;
   QColor mapFadeColor(const QColor &color, int index) const;
   void fadeIn(QPushButton *button);
   void fadeOut(QPushButton *button);
   QPixmap *tint(const QImage &img, const QColor& c) const;
   const TileSet &glow(const QColor & c, bool round = false) const;
   
private:
   // pixmaps
   TileSet frames[3];
   TileSet round_frames[3];
   QPixmap *_scanlines[3];
   
   // cache
   typedef QHash<uint, QPixmap> PixmapCache;
   PixmapCache gradientCache[2][2];
   PixmapCache glossCache[2][2];
   
   typedef QHash<uint, TileSet> TileCache;
   TileCache glowCache;
   TileCache roundGlowCache;

   
   //anmiated progressbars
   QMap<QWidget*, uint> progressbars;
   uint progressShift;
   bool anmiationUpdate;

   // button fading
   
   typedef QMap<uint, ButtonFadeInfo> ButtonFades;
   ButtonFades bfi;
   typedef QMap<QRgb, QRgb*> FadeColors;
   FadeColors fadeColorMap;

   // indices of current itmes
   // just benders - TODO: see if we can limit their number
   
//    QPalette polishedPalette, tooltipPalette;
//    uint qtrcModificationTime;
//    uint oxygenrcModificationTime;
//    bool initialPaletteLoaded, inExitPolish;
   
   // toolbar title functionality
   QPoint cursorPos_;
   bool mouseButtonPressed_;
   bool internalEvent_;
   DynamicBrush *_bgBrush;
   Pixmap popupPix;
   QTimer* timer;
};

} // namespace Oxygen
#endif //OXYGEN_STYLE_H
