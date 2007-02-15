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

#ifndef OXYGEN_STYLE_H
#define OXYGEN_STYLE_H

class QAbstractButton;
class QHeaderView;
class QMenuBar;
class QPushButton;
class QScrollBar;
class QTabBar;
class DynamicBrush;
class QPaintEvent;

#include <QHash>
#include <QMap>
#include <QCommonStyle>
#include <QBitmap>
#include <QRegion>
#include <X11/Xlib.h>
#include <fixx11h.h>
#include "tileset.h"

namespace Oxygen {

enum BGMode { Plain = 0, Scanlines, BrushedMetal, FullPix, VerticalGradient, HorizontalGradient };
enum Acceleration { None = 0, XRender, OpenGL };
enum TabTransition {Jump = 0, CrossFade, ScanlineBlend, SlideIn, SlideOut, RollIn, RollOut, OpenVertically, CloseVertically, OpenHorizontally, CloseHorizontally };

class EventKiller : public QObject 
{
   Q_OBJECT
public:
   bool eventFilter( QObject *object, QEvent *event);
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

class TabAnimInfo : public QObject
{
   Q_OBJECT
public:
   TabAnimInfo(QObject *parent = 0, int currentTab = -1) :
      QObject(parent), lastTab(currentTab), animStep(0){}
protected:
   bool eventFilter( QObject* object, QEvent* event );
public:
   QList < QWidget* > autofillingWidgets;
   int lastTab, animStep;
   QPixmap tabPix[3];
};

enum Orientation3D {Sunken = 0, Relief, Raised};
// typedef Tile::PosFlags PosFlags;
// enum Position
// {
//    Top = 0x1, Left=0x2, Bottom=0x4, Right=0x8,
//    Ring=0xf, Center=0x10, Full=0x1f
// };

typedef struct
{
   int $1, $2, $3, $4, $5, $6, $7, $8, $9, $10;
   int $12, $16, $18, $80;
   int ScrollBarExtent;
   int ScrollBarSliderMin;
   int SliderThickness;
   int SliderControl;
   int Indicator;
   int ExclusiveIndicator;
} Dpi;

typedef struct Config
{
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
   TabTransition tabTransition;
   Oxygen::Orientation3D tabwidget3D;
   int gradientIntensity;
   int fullPixMode;
   bool HAL9000;
   double scale;
   int checkType;
} Config;

class OxygenStyle : public QCommonStyle
{
   Q_OBJECT
public:
   enum WidgetState{Basic = 0, Hovered, Focused, Active};
   enum GradientType{GradSimple,GradSunken,GradGloss,GradButton,GradButtonHover,GradButtonDisabled,GradGroup,NumGrads};
   
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
   /** animation */
   void progressbarDestroyed(QObject*);
   void updateProgressbars();
   void tabChanged(int index);
   void updateTabAnimation();
   void tabDestroyed(QObject* obj);

private:
   OxygenStyle( const OxygenStyle & );
   OxygenStyle& operator=( const OxygenStyle & );
   void renderFrame( QPainter *p, const QRect &r, Orientation3D o3D, Tile::PosFlags pf, const QWidget *widget = 0L, bool highlighted = false, bool round = false) const;
   const QPixmap &gradient(const QColor &c, int size, Qt::Orientation o, GradientType type = GradSimple) const;
   const QPixmap &btnAmbient(int height) const;
   void fillWithMask(QPainter *painter, const QRect &rect, const QBrush &brush, const Tile::Mask *mask, Tile::PosFlags pf = Tile::Full, bool justClip = false, QPoint offset = QPoint()) const;
   void fillWithMask(QPainter *painter, const QPoint &xy, const QPixmap &pix, const QPixmap &mask, QPoint offset = QPoint()) const;
   QColor mapFadeColor(const QColor &color, int index) const;
   void fadeIn(QPushButton *button);
   void fadeOut(QPushButton *button);
   QPixmap *tint(const QImage &img, const QColor& c) const;
   const Tile::Set &glow(const QColor & c, bool round = false) const;
   void readSettings();
   void generatePixmaps();
   void initMetrics();
   
private:
   typedef QHash<uint, QPixmap> PixmapCache;
   typedef QHash<uint, Tile::Set> TileCache;
   struct
   {
      Tile::Mask rect[3];
      Tile::Mask round[3];
      Tile::Mask button;
      Tile::Mask tab;
      QPixmap radio, radioIndicator;
   } masks;
   struct
   {
      Tile::Set button, tab, sunken;
      QPixmap radio[2];
   } shadows;
   struct
   {
      Tile::Set rect[3];
      Tile::Set round[3];
      Tile::Nuno button[2];
   } frames;
   struct
   {
      Tile::Line top;
   } lights;
   
   // pixmaps
   QPixmap *_scanlines[3];
   // cache
   PixmapCache gradients[2][NumGrads];
   PixmapCache _btnAmbient;
   TileCache glowCache;
   TileCache roundGlowCache;

   
   //anmiated progressbars
   uint progressShift;
   bool anmiationUpdate;

   // button fading
   typedef QMap<uint, ButtonFadeInfo> ButtonFades;
   ButtonFades bfi;
   typedef QMap<QRgb, QRgb*> FadeColors;
   FadeColors fadeColorMap;

   
//    QPalette tooltipPalette;
   
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
