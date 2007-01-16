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

#include <QFile>
#include <QTextStream>
#include <QSettings>
#include <QEvent>
#include <QList>
#include <QTimer>
#include <QResizeEvent>
#include <QApplication>
#include <QWidget>
#include <QPainter>
#include <QGroupBox>
#include <QPixmap>
#include <QImage>
#include <QDesktopWidget>
#include <QX11Info>
#include <QStylePlugin>
#include <QProgressBar>
#include <QMenu>
#include <QMenuBar>
#include <QStyleOptionProgressBarV2>
#include <QListWidget>
#include <QAbstractButton>
#include <QPushButton>
#include <QScrollBar>
#include <QTabBar>
#include <QTabWidget>
#include <QComboBox>
#include <QCheckBox>
#include <QRadioButton>
#include <QSplitterHandle>
#include <QToolBar>
#include <QFrame>
#include <QLineEdit>
#include <QAbstractScrollArea>
// qt3 support
#include <Q3ScrollView>

// #include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/keysym.h>
#include <X11/extensions/XTest.h>
// #include <fixx11h.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cmath>

#undef DEBUG
#ifdef DEBUG
#define MOUSEDEBUG #
#include <QtDebug>
#include "debug.h"
#define oDEBUG qDebug()
#include <QTime>
#define _PROFILESTART_ QTime timer; int time; timer.start();
#define _PROFILERESTART_ timer.restart();
#define _PROFILESTOP_(_STRING_) time = timer.elapsed(); qDebug("%s: %d",_STRING_,time);
#else
#define oDEBUG //
#undef MOUSEDEBUG
#endif

#include "../imagebase/round-ul.xbm"
#include "../imagebase/round-ur.xbm"
#include "../imagebase/round-ll.xbm"
#include "../imagebase/round-lr.xbm"
#include "oxygen.h"
#include "dynamicbrush.h"

#define _IsNotHtmlWidget(w) qstrcmp( w->name(), "__khtml" )
#define _IsHtmlWidget(w) !qstrcmp( w->name(), "__khtml" )
#define _IsViewportChild(w) w->parent() &&\
( !qstrcmp(w->parent()->name(), "qt_viewport") || \
  !qstrcmp(w->parent()->name(), "qt_clipped_viewport") )

#define _HighContrastColor(c) (qGray(c.rgb()) < 128 ) ? Qt::white : Qt::black


#define _BLOCKEVENTS_(obj) obj->installEventFilter(eventKiller)
#define _UNBLOCKEVENTS_(obj) obj->removeEventFilter(eventKiller)

/** extern C stuff*/
class OxygenStylePlugin : public QStylePlugin
{
public:
   QStringList keys() const {
      return QStringList() << "Oxygen";
   }
   
   QStyle *create(const QString &key) {
      if (key == "oxygen")
         return new Oxygen::OxygenStyle;
      return 0;
   }
};

Q_EXPORT_PLUGIN(OxygenStylePlugin)


using namespace Oxygen;


#include "inlinehelp.cpp"

/** Internal, handles the shared bg pixmaps*/

static Pixmap bgPix = 0;
static Pixmap fgPix = 0;
static QPixmap nullPix;

static QMap<QWidget*, uint> progressbars;

Pixmap shadowPix;
int bgYoffset_;
Config config;

static void loadPixmaps()
{
   QFile file("/tmp/oxygenPixIDs");
   char ignore;
   if (file.open( QIODevice::ReadOnly ))
   {
      QTextStream ts( &file );
      ts >> bgPix >> ignore >> shadowPix >> ignore >> fgPix >> ignore >> bgYoffset_;
      file.close();
//       QSettings settings;
//       settings.beginGroup("/oxygen/Style");
//       config.bgMode = (BGTileMode) settings.value("BackgroundMode", FullPix).toInt();
//       settings.endGroup();
   }
   else
   {
      fgPix = shadowPix = bgPix = 0;
   }
}

/**For painting the shared xpixmap on a qmenu*/

inline void *qt_getClipRects( const QRegion &r, int &num )
{
   return r.clipRectangles( num );
}

extern Drawable qt_x11Handle(const QPaintDevice *pd);
/**==================================================================================================*/

/**Some static variables*/

// static const int windowsItemFrame	= 1; // menu item frame width
// static const int windowsSepHeight	= 2; // separator item height
// static const int windowsItemHMargin	= 3; // menu item hor text margin
// static const int windowsItemVMargin	= 1; // menu item ver text margin
// static const int windowsArrowHMargin	= 6; // arrow horizontal margin
// static const int windowsTabSpacing	= 12; // space between text and tab
// static const int windowsCheckMarkHMargin= 2; // horiz. margins of check mark
// static const int windowsRightBorder	= 12; // right border on windows
// static const int windowsCheckMarkWidth	= 14; // checkmarks width on windows

static QColor originalBgColor_;
static QColor groupShadowColor_;
static EventKiller *eventKiller = 0L;

/**Our frame tileset*/
TileSet::TileSet(const QPixmap &pix, int xOff, int yOff, int width, int height, int top, int bottom, int left, int right, QRegion *topLeft, QRegion *topRight, QRegion *btmLeft, QRegion *btmRight)
{
   size[0] = top; size[1] = left; size[2] = bottom; size[3] = right;
   
   if (topLeft)
      border[TopLeft] = new QRegion(*topLeft);
   else
      border[TopLeft] = 0;
   if (topRight)
      border[TopRight] = new QRegion(*topRight);
   else
      border[TopRight] = 0;
   if (btmLeft)
      border[BtmLeft] = new QRegion(*btmLeft);
   else
      border[BtmLeft] = 0;
   if (btmRight)
      border[BtmRight] = new QRegion(*btmRight);
   else
      border[BtmRight] = 0;
   
   int rOff = pix.width() - xOff - width;
   int bOff = pix.height() - yOff - height;
   int amount = 32/width+1;
   int amount2 = 32/height+1;
   int i;
   
   QPainter p;
   QColor trans(0,0,0,0);
   
   pixmaps[TopLeft] = QPixmap(xOff, yOff);
   pixmaps[TopLeft].fill(trans); p.begin(&pixmaps[TopLeft]);
   p.drawPixmap(0, 0, pix, 0, 0, xOff, yOff);
   p.end();
//    masks[TopLeft] = alphaMask(pixmaps[TopLeft]);
//    masks[TopLeft].setMask(masks[TopLeft]);
   
   pixmaps[TopMid] = QPixmap(amount*width, yOff);
   pixmaps[TopMid].fill(trans); p.begin(&pixmaps[TopMid]);
   for (i = 0; i < amount; i++)
      p.drawPixmap(i*width, 0, pix, xOff, 0, width, yOff);
   p.end();
//    masks[TopMid] = alphaMask(pixmaps[TopMid]);
//    masks[TopMid].setMask(masks[TopMid]);
   
   pixmaps[TopRight] = QPixmap(rOff, yOff);
   pixmaps[TopRight].fill(trans); p.begin(&pixmaps[TopRight]);
   p.drawPixmap(0, 0, pix, xOff+width, 0, rOff, yOff);
   p.end();
//    masks[TopRight] = alphaMask(pixmaps[TopRight]);
//    masks[TopRight].setMask(masks[TopRight]);
   
   //----------------------------------
   pixmaps[MidLeft] = QPixmap(xOff, amount2*height);
   pixmaps[MidLeft].fill(trans); p.begin(&pixmaps[MidLeft]);
   for (i = 0; i < amount2; i++)
      p.drawPixmap(0, i*height, pix, 0, yOff, xOff, height);
   p.end();
//    masks[MidLeft] = alphaMask(pixmaps[MidLeft]);
//    masks[MidLeft].setMask(masks[MidLeft]);
   
      // TODO: this is a bit of waste as we assume a *frame* to be empty inside ;)
   pixmaps[Center] = QPixmap(amount*width, amount2*height);
   pixmaps[Center].fill(trans); p.begin(&pixmaps[Center]);
   for (i = 0; i < amount; i++)
      for (int j = 0; i < amount2; i++)
         p.drawPixmap(i*width, j*height, pix, xOff, yOff, width, height);
   p.end();
//    masks[Center] = alphaMask(pixmaps[Center]);
//    masks[Center].setMask(masks[Center]);
   pixmaps[MidRight] = QPixmap(rOff, amount2*height);
   pixmaps[MidRight].fill(trans); p.begin(&pixmaps[MidRight]);
   for (i = 0; i < amount2; i++)
      p.drawPixmap(0, i*height, pix, xOff+width, yOff, rOff, height);
   p.end();
//    masks[MidRight] = alphaMask(pixmaps[MidRight]);
//    masks[MidRight].setMask(masks[MidRight]);
   
   //----------------------------------
   
   pixmaps[BtmLeft] = QPixmap(xOff, bOff);
   pixmaps[BtmLeft].fill(trans); p.begin(&pixmaps[BtmLeft]);
   p.drawPixmap(0, 0, pix, 0, yOff+height, xOff, bOff);
   p.end();
//    masks[BtmLeft] = alphaMask(pixmaps[BtmLeft]);
//    masks[BtmLeft].setMask(masks[BtmLeft]);
   
   pixmaps[BtmMid] = QPixmap(amount*width, bOff);
   pixmaps[BtmMid].fill(trans); p.begin(&pixmaps[BtmMid]);
   for (i = 0; i < amount; i++)
      p.drawPixmap(i*width, 0, pix, xOff, yOff+height, width, bOff);
   p.end();
//    masks[BtmMid] = alphaMask(pixmaps[BtmMid]);
//    masks[BtmMid].setMask(masks[BtmMid]);
   
   pixmaps[BtmRight] = QPixmap(rOff, bOff);
   pixmaps[BtmRight].fill(trans); p.begin(&pixmaps[BtmRight]);
   p.drawPixmap(0, 0, pix, xOff+width, yOff+height, rOff, bOff);
   p.end();
//    masks[BtmRight] = alphaMask(pixmaps[BtmRight]);
//    masks[BtmRight].setMask(masks[BtmRight]);
}

const QRegion TileSet::innerRegion(const QRect &r) const
{
   QRegion tmpRegion;
   
   QRegion ret(r.x()+size[1], r.y()+size[0], r.width()-(size[1]+size[3]), r.height()-(size[0]+size[2]));

   if (border[TopLeft])
   {
      tmpRegion = *border[TopLeft];
      tmpRegion.translate(r.x(), r.y());
      ret = ret.subtract(tmpRegion);
   }
   if (border[BtmLeft])
   {
      tmpRegion = *border[BtmLeft];
      tmpRegion.translate(r.x(), r.bottom() - tmpRegion.boundingRect().height() + 1);
      ret = ret.subtract(tmpRegion);
   }
   if (border[TopRight])
   {
      tmpRegion = *border[TopRight];
      tmpRegion.translate(r.right() - tmpRegion.boundingRect().width() + 1, r.y());
      ret = ret.subtract(tmpRegion);
   }
   if (border[BtmRight])
   {
      tmpRegion = *border[BtmRight];
      tmpRegion.translate(r.right()-tmpRegion.boundingRect().width() + 1,
                          r.bottom()-tmpRegion.boundingRect().height() + 1);
      ret = ret.subtract(tmpRegion);
   }
   return ret;
}

const QRegion TileSet::outerRegion(const QRect &r) const
{
   QRegion tmpRegion;

   QRegion ret = QRegion(r).subtract(QRect(r.x()+size[1], r.y()+size[0], r.width()-(size[1]+size[3]), r.height()-(size[0]+size[2])));
   
   if (border[TopLeft])
   {
      tmpRegion = *border[TopLeft];
      tmpRegion.translate(r.x(), r.y());
      ret = ret.unite(tmpRegion);
   }
   if (border[BtmLeft])
   {
      tmpRegion = *border[BtmLeft];
      tmpRegion.translate(r.x(), r.bottom() - tmpRegion.boundingRect().height() + 1);
      ret = ret.unite(tmpRegion);
   }
   if (border[TopRight])
   {
      tmpRegion = *border[TopRight];
      tmpRegion.translate(r.right() - tmpRegion.boundingRect().width() + 1, r.y());
      ret = ret.unite(tmpRegion);
   }
   if (border[BtmRight])
   {
      tmpRegion = *border[BtmRight];
      tmpRegion.translate(r.right()-tmpRegion.boundingRect().width() + 1,
                          r.bottom()-tmpRegion.boundingRect().height() + 1);
      ret = ret.unite(tmpRegion);
   }
   return ret;
}

static bool invColorRole(QPalette::ColorRole &from, QPalette::ColorRole &to,
                         QPalette::ColorRole defFrom = QPalette::WindowText, QPalette::ColorRole defTo = QPalette::Window)
{
   switch (from)
   {
   case QPalette::WindowText: //0
      to = QPalette::Window; break;
   case QPalette::Window: //10
      to = QPalette::WindowText; break;
   case QPalette::Base: //9
      to = QPalette::Text; break;
   case QPalette::Text: //6
      to = QPalette::Base; break;
   case QPalette::Button: //1
      to = QPalette::ButtonText; break;
   case QPalette::ButtonText: //8
      to = QPalette::Button; break;
   case QPalette::Highlight: //12
      to = QPalette::HighlightedText; break;
   case QPalette::HighlightedText: //13
      to = QPalette::Highlight; break;
   default:
      from = defFrom;
      to = defTo;
      return false;
   }
   return true;
}

/**THE STYLE ITSELF*/
OxygenStyle::OxygenStyle() : QCommonStyle(), progressShift(0), anmiationUpdate(false), mouseButtonPressed_(false), internalEvent_(false), _bgBrush(0L), popupPix(0L), timer(0L)
{

   //========= READ SETTINGS =====================
   QSettings settings("Oxygen", "Style");
   settings.beginGroup("Style");
   
   config.acceleration = (Acceleration) settings.readNumEntry("Acceleration", None);
   config.gradientIntensity = settings.value("GradientIntensity",70).toInt();
   
   config.inversePopups = settings.value("InversePopups",false).toBool();
   config.inverseButtons = settings.value("InverseButtons",false).toBool();
   config.swapHoveredButtonColors = settings.value("InverseHoveredButtonColor",false).toBool();
   config.menuHighlightBevel = settings.value("MenuHighlightBevel", true).toBool();
   
   
   config.progressColor = (QPalette::ColorRole)settings.value("ProgressColor", QPalette::WindowText).toInt();
   invColorRole(config.progressColor, config.progressTextColor);
   config.tabColor = (QPalette::ColorRole)settings.value("TabColor", QPalette::Window).toInt();
   invColorRole(config.tabColor, config.tabTextColor, QPalette::Window, QPalette::WindowText);
   config.scrollbarFg = (QPalette::ColorRole)settings.value("ScrollbarColor", QPalette::Window).toInt();
   invColorRole(config.scrollbarFg, config.scrollbarBg, QPalette::Window, QPalette::WindowText);
   
   config.roundButtons = settings.value("RoundButtons", false).toBool();
   config.raisedButtons = settings.value("RaisedButtons", true).toBool();
   
   config.tabwidget3D = (Orientation3D)(settings.value("TabWidget3D", 1).toInt());
   
   config.bgMode = (BGMode) settings.readNumEntry("BackgroundMode", FullPix);
   settings.endGroup();

   
   //=======================================
   
//======= INIT PIXMAPS ====================
   
   _scanlines[0] = _scanlines[1] = _scanlines[2] = 0L;
   
   frames[Sunken] = TileSet(QPixmap(":/frame-sunken"),5,5,1,1, 2,2,2,2);
   frames[Relief] = TileSet(QPixmap(":/frame-relief"),4,5,1,1, 2,2,2,2);
   frames[Raised] = TileSet(QPixmap(":/frame-raised"),4,6,1,1, 2,2,2,2);
   
   
   QRegion *ul = new QRegion(QBitmap::fromData(QSize(round_ul_width, round_ul_height), round_ul_bits));
   QRegion *ur = new QRegion(QBitmap::fromData(QSize(round_ur_width, round_ur_height), round_ur_bits));
   QRegion *ll = new QRegion(QBitmap::fromData(QSize(round_ll_width, round_ll_height), round_ll_bits));
   QRegion *lr = new QRegion(QBitmap::fromData(QSize(round_lr_width, round_lr_height), round_lr_bits));
   
   
//    QRegion *ll2 = new QRegion(QBitmap::fromData(QSize(round_relief_ll_width, round_relief_ll_height), round_relief_ll_bits, QImage::Format_Mono));
//    QRegion *lr2 = new QRegion(QBitmap::fromData(QSize(round_relief_lr_width, round_relief_lr_height), round_relief_lr_bits, QImage::Format_Mono));
   
   round_frames[Sunken] = TileSet(QPixmap(":/round-sunken"),8,8,11,8, 2,2,2,2, ul,ur,ll,lr);
   round_frames[Relief] = TileSet(QPixmap(":/round-relief"),8,10,11,6, 2,2,2,2, ul,ur,ll,lr);
   round_frames[Raised] = TileSet(QPixmap(":/round-raised"),8,8,11,8, 1,2,2,2, ul,ur,ll,lr);
   
   delete ul; delete ur; delete ll; delete lr; //delete ll2; delete lr2;
   ul = ur = ll = lr = 0L;//ll2 = lr2 = 0L;


   //=======================================
   
   //====== TOOLTIP ======================
//    tooltipPalette = qApp->palette();
//    tooltipPalette.setBrush( QPalette::Background, QColor( 255, 255, 220 ) );
//    tooltipPalette.setBrush( QPalette::Foreground, Qt::black );
   //=======================================
   
   
//    if (KApplication::kApplication())
//       connect(KApplication::kApplication(), SIGNAL(kipcMessage(int, int)), this, SLOT (handleIPC(int, int)));
   
#if 0
   // TEST!!!
   if (KApplication::kApplication())
   {
      _PROFILESTART_
      QByteArray data, replyData;
      QCString replyType;
      QDataStream arg(data, IO_WriteOnly);
      arg << "";
      if (!KApplication::kApplication()->dcopClient()->call("kwin", "OxygenInterface", "pixmap()", data, replyType, replyData))
         qDebug("there was some error using DCOP.");
      else
      {
         QDataStream reply(replyData, IO_ReadOnly);
         if (replyType == "QPixmap")
         {
            QPixmap result;
            reply >> result;
         }
         else
            qDebug("pixmap() returned an unexpected type of reply!");
      }
      _PROFILESTOP_("dcop pixmap")
   }
   //===========
#endif

   // start being animated
   timer = new QTimer( this );
   timer->start(50);
   connect(timer, SIGNAL(timeout()), this, SLOT(updateProgressbars()));
}

OxygenStyle::~OxygenStyle()
{
   for (int i = 0; i < 2; i++)
      for (int j = 0; j < 2; j++)
      {
         gradientCache[i][j].clear();
         glossCache[i][j].clear();
      }
   glowCache.clear();
   roundGlowCache.clear();
//    if (timer)
//    {
//       timer->disconnect();
//       timer->stop();
//       delete timer;
//    }
   progressbars.clear();

//    bfi.clear();
//    fadeColorMap.clear();

   bgPix = 0L;
   shadowPix = 0L;
   fgPix = 0L;
}

/**Slot to animate the progressbars*/
void OxygenStyle::updateProgressbars()
{
#if 1
   //Update the registered progressbars.
   QMap<QWidget*, uint>::iterator iter;
   QPainter p;
   QProgressBar *pb;
   QStyleOptionProgressBarV2 *opt = 0L;
   anmiationUpdate = true;
   for (iter = progressbars.begin(); iter != progressbars.end(); iter++)
   {
      if ( !qobject_cast<QProgressBar*>(iter.key()) )
         continue;
      pb = (QProgressBar*)(iter.key());
      if (pb->paintingActive() || !pb->isVisible() || !(pb->value() > pb->minimum()) || !(pb->value() < pb->maximum()))
         continue;
#ifdef Q_WS_X11
      if (!opt)
         opt = new QStyleOptionProgressBarV2();
      opt->palette = pb->palette();
      opt->minimum = pb->minimum();
      opt->maximum = pb->maximum();
      opt->progress = pb->value();
      opt->textAlignment = pb->alignment();
      opt->textVisible = pb->isTextVisible();
      opt->text = pb->text();
      opt->rect = pb->rect();
      opt->state = 0;
      if (pb->isEnabled()) opt->state |= State_Enabled;
      if (pb->hasFocus()) opt->state |= State_HasFocus;
      if (pb->orientation() == Qt::Horizontal) opt->state |= State_Horizontal;
      opt->bottomToTop = pb->textDirection() == QProgressBar::BottomToTop;
      opt->invertedAppearance = pb->invertedAppearance();
      opt->orientation = pb->orientation();
      opt->direction = pb->layoutDirection();
      opt->rect = visualRect(pb->layoutDirection(), pb->rect(), subElementRect( SE_ProgressBarContents, opt, pb ));
#endif
      // we want custom sized rings, depending on the height of the progress
      if (pb->orientation() == Qt::Horizontal)
         progressShift = ++iter.value() % (pb->width()<<1);
      else
         progressShift = ++iter.value() % ((opt->rect.height()<<1)-1);
      // i don't want to paint the whole progressbar, as only the progress and maybe the percentage needs an update - saves cpu especially on lack of HW accelerated XRender alpha blending
      // TODO: this is X11 only, find a better way (maybe just repaint the whole widget)
#ifndef Q_WS_X11
#warning not X11, animated progress may be slow
      pb->repaint();
#else
      pb->setAttribute ( Qt::WA_PaintOutsidePaintEvent );
      p.begin(pb);
      drawControl( CE_ProgressBarContents, opt, &p, pb );
      opt->rect = visualRect(pb->layoutDirection(), pb->rect(), subElementRect( SE_ProgressBarLabel, opt, pb ));
      drawControl( CE_ProgressBarLabel, opt, &p, pb );
      p.end();
      pb->setAttribute ( Qt::WA_PaintOutsidePaintEvent, false );
#endif
   }
   if (opt)
      delete opt;
   anmiationUpdate = false;
#endif
}

void OxygenStyle::progressbarDestroyed(QObject* obj)
{
   progressbars.remove(static_cast<QWidget*>(obj));
}

/**handles updates to the bg pixmap*/
void OxygenStyle::handleIPC(int id, int data)
{
#if 0
   if (id != 32 || data != 151616) //TODO: use custom ID as soon as kapp supports random values on this
      return;
   loadPixmaps();
   if (qApp->desktop())
      bgYoffset_ = qApp->desktop()->height()*bgYoffset_/738;
   popupPix = config.inversePopups ? fgPix : bgPix;
   // tell the deco that this it a oxygen styled qt window
   if (!bgPix)
   {
      foreach (QWidget *w, QApplication::topLevelWidgets())
      {
         if (!(qobject_cast<QMenuBar*>(w) ||
           qobject_cast<QMenu*>(w) ||
            (w->windowType() == Qt::Desktop) ||
           w->inherits("QTipLabel") ||
           qobject_cast<QAbstractScrollArea*>(w)))
           w->setPalette(QPalette()); //TODO: make sure this is a bgPix'd widget!
      }
   }
   else
   {
      foreach (QWidget *w, QApplication::topLevelWidgets())
      {
         if (!(qobject_cast<QMenuBar*>(w) ||
               qobject_cast<QMenu*>(w) ||
               (w->windowType() == Qt::Desktop) ||
               w->inherits("QTipLabel") ||
               qobject_cast<QAbstractScrollArea*>(w)))
         {
            // faking resize to make sure the window's bg is updated
            QResizeEvent* rev = new QResizeEvent(w->size(), QSize(0,0));
            backgroundHandler_->eventFilter( w, rev );
         }
      }
   }
#endif
}

/** for creating the highlight glow pixmps*/
QPixmap *OxygenStyle::tint(const QImage &img, const QColor& oc) const
{
   QImage *dest = new QImage( img.width(), img.height(), QImage::Format_ARGB32/*_Premultiplied*/ );
//    dest->fill(0);
   unsigned int *data = ( unsigned int * ) img.bits();
   unsigned int *destData = ( unsigned int* ) dest->bits();
   int total = img.width() * img.height();
   int red, green, blue;
   int current;
   oc.getHsv(&red,&green,&blue); // not really ;) green is saturation
   current = green/60;
   green = CLAMP(green*(1+current)/current,0,255);
   QColor c; c.setHsv(red,green,blue);
   
   int srcR = c.red() - 128;
   int srcG = c.green() - 128;
   int srcB = c.blue() - 128;
   int destR, destG, destB;

   // grain/merge from the gimp. TODO: use mmx/sse here
   for ( current = 0 ; current < total ; ++current )
   {
      red = srcR + qRed( data[ current ] );
      green = srcG + qGreen( data[ current ] );
      blue = srcB + qBlue( data[ current ] );
      destR = CLAMP(red, 0, 255);
      destG = CLAMP(green, 0, 255);
      destB = CLAMP(blue, 0, 255);
      destData[ current ] = qRgba( destR, destG, destB, qAlpha(data[ current ]) );
   }
   QPixmap *pix = new QPixmap(QPixmap::fromImage(*dest, 0));
   delete dest;
   return pix;
}

const TileSet &OxygenStyle::glow(const QColor & c, bool round) const
{
   TileCache *cache = const_cast<TileCache*>(round ? &roundGlowCache : &glowCache);
   TileCache::const_iterator it = cache->find(c.rgb());
   if (it != cache->end())
      return it.value();
   
   // need to create a new one
   QPixmap *pix;
   TileSet frame;
   
   if (round)
   {
      pix = tint(QImage(":/round-glow"), c);
      frame = TileSet(*pix,10,10,7,6, 2,2,2,2);
   }
   else
   {
      pix = tint(QImage(":/glow"), c);
      frame = TileSet(*pix,15,12,6,4, 4,4,4,4);
   }

   delete pix;
   if (cache->size() == cache->capacity())
      cache->clear();
   it = cache->insert(c.rgb(), frame);
   return it.value();
}
/**=========================*/

/** render the everywhere used frames*/
void OxygenStyle::renderFrame( QPainter *p, const QRect &r, Orientation3D o3D, PosFlags pf, const QWidget *widget, bool highlighted, bool round) const
{
#define HIGHCOLOR palette().color(QPalette::Active, QPalette::Highlight)
   //TODO:pass option->palette
   const TileSet *frame = highlighted ?
      &glow(widget ? widget->HIGHCOLOR : qApp->HIGHCOLOR, round) :
      round ? &round_frames[o3D] : &frames[o3D];
#undef HIGHCOLOR
   
   int rOff = 0; int xOff; int yOff; int width; int height;

   r.getRect(&xOff, &yOff, &width, &height);
   int tlh = frame->height(TileSet::TopLeft),
      blh = frame->height(TileSet::BtmLeft),
      trh = frame->height(TileSet::TopRight),
      brh = frame->height(TileSet::BtmLeft),
      tlw = frame->width(TileSet::TopLeft),
      blw = frame->width(TileSet::BtmLeft),
      trw = frame->width(TileSet::TopRight),
      brw = frame->width(TileSet::BtmRight);
   
   if (pf & Left)
   {
      width -= frame->width(TileSet::TopLeft);
      xOff += frame->width(TileSet::TopLeft);
      if (pf & (Top | Bottom) && tlh + blh > r.height()) // vertical edge overlap
      {
         tlh = tlh*r.height()/(tlh+blh);
         blh = r.height() - tlh;
      }
   }
   if (pf & Right)
   {
      width -= frame->width(TileSet::TopRight);
      if (pf & (Top | Bottom) && trh + brh > r.height()) // vertical edge overlap
      {
         trh = trh*r.height()/(trh+brh);
         brh = r.height() - trh;
      }
   }
   
   if (pf & Top)
   {
      if (pf & (Left | Right) && tlw + trw > r.width()) // horizontal edge overlap
      {
         tlw = tlw*r.width()/(tlw+trw);
         trw = r.width() - tlw;
      }
      rOff = r.right()-trw+1;
      yOff += tlh;
      height -= tlh;
      if (pf & Left) // upper left
         p->drawPixmap(r.x(),r.y(),frame->pixmap(TileSet::TopLeft), 0, 0, tlw, tlh);
      if (pf & Right) // upper right
         p->drawPixmap(rOff, r.y(), frame->pixmap(TileSet::TopRight), frame->width(TileSet::TopRight)-trw, 0, trw, trh);
      
      // upper line
      if (width > 0)
         p->drawTiledPixmap(xOff, r.y(), width, frame->height(TileSet::TopMid), frame->pixmap(TileSet::TopMid));
   }
   if (pf & Bottom)
   {
      if (pf & (Left | Right) && blw + brw > r.width()) // horizontal edge overlap
      {
         blw = blw*r.width()/(blw+brw);
         brw = r.width() - blw;
      }
      rOff = r.right()-brw+1;
      int bOff = r.bottom()-blh+1;
      height -= blh;
      if (pf & Left) // lower left
         p->drawPixmap(r.x(), bOff, frame->pixmap(TileSet::BtmLeft), 0, frame->height(TileSet::BtmLeft)-blh, blw, blh);
      if (pf & Right) // lower right
         p->drawPixmap(rOff, bOff, frame->pixmap(TileSet::BtmRight), frame->width(TileSet::BtmRight)-brw, frame->height(TileSet::BtmRight)-brh, brw, brh);
      
      // lower line
      if (width > 0)
         p->drawTiledPixmap(xOff, bOff, width, frame->height(TileSet::BtmMid), frame->pixmap(TileSet::BtmMid));
   }
   
   if (height > 0)
   {
      if (pf & Left) // left line
         p->drawTiledPixmap(r.x(), yOff, frame->width(TileSet::MidLeft), height, frame->pixmap(TileSet::MidLeft));
      rOff = r.right()-frame->width(TileSet::MidRight)+1;
      if (pf & Right) // right line
         p->drawTiledPixmap(rOff, yOff, frame->width(TileSet::MidRight), height, frame->pixmap(TileSet::MidRight));
   }

}

const QPixmap OxygenStyle::gradient(const QColor &oc, const int size, Qt::Orientation o, Orientation3D o3D) const
{
   QColor c = oc;
   PixmapCache *cache = &(const_cast<OxygenStyle*>( this )->gradientCache[o-1][o3D?1:0]);
   int v = colorValue(c);
   if (v < 80) // very dark colors won't make nice buttons ;)
   {
      int h,s;
      c.getHsv(&h,&s,&v);
      c.setHsv(h,s,80);
   }
   /*
   Ok, we want to cache the gradients, but unfortunately we have no idea about what kind of gradients will be demanded in the future
   Therefore creating a 2 component map (uint for color and uint for size) would be some overhead and cause a lot of unused space in the dictionaries
   So we store all the gradients by a uint index
   Therefore we substitute the alpha component (byte << 24) of the demanded color with the demanded size
   As this would limit the size to 255/256 pixels we'll be a bit sloppy, depending on the users resolution (e.g. use 0 to store a gradient with 2px, usefull for demand of 1px or 2px) and shift the index (e.g. gradients from 0 to 6 px size will hardly be needed - maybe add statistics on this)
   So the handled size is actually demandedSize + (demandedSize % sizeSloppyness), beeing at least demanded size and the next sloppy size above at max
   */
   int sizeSloppyness = 1;
   uint magicNumber = 0;
   PixmapCache::const_iterator it;
   if (size < 105884) // this is where our dictionary reaches - should be enough for the moment ;)
   {
      int frameBase = 0;
      int frameSize = 20;

      while ((frameBase += frameSize) < size)
      {
         sizeSloppyness++;
         frameSize += 20;
      }

      frameBase -=frameSize;
      frameSize -= 20;
      // The mapping is done by the "magicNumber"
//       magicNumber =  (((frameSize + (size - frameBase)/sizeSloppyness) & 0xff) << 24) | (c.rgb() & 0xffffff);
      magicNumber =  (((frameSize + (size - frameBase)/sizeSloppyness) & 0xff) << 21) | 
         (((c.red() >> 1) & 0x7f) << 14) |
         (((c.green() >> 1) & 0x7f) << 7 ) |
         ((c.blue() >> 1) & 0x7f);
      
      it = cache->find(magicNumber);
      if (it != cache->end())
         return it.value();
   }
   else
   {
      qWarning("gradient with more than 105883 steps requested, returning NULL pixmap");
      return nullPix;
   }
   /*
   adepted from kpixmapeffect.cpp - WARNING! this is specialised. only use for single column/row pixmaps. pixmaps with depth 8 will become plain (for the moment...)
   also horizontal gradients will become bilinear!
   */
   QPixmap pix;
   if (o == Qt::Horizontal)
      pix = QPixmap(size + (size % sizeSloppyness), 32);
   else
      pix = QPixmap(32, size + (size % sizeSloppyness));
   
   if (pix.isNull())
   {
      qWarning("NULL Pixmap requested, size was %d",size);
      return nullPix;
   }
   
   QPainter p(&pix);
   
   QColor ca;
   QColor cb;
      
   if (o3D == Raised)
   {
      ca = c.light(100+config.gradientIntensity*60/100);
      cb = c.dark(100+config.gradientIntensity*20/100);
   }
   else
   {
      ca = c.dark(100+config.gradientIntensity*20/100);
      cb = c.light(100+config.gradientIntensity*60/100);
   }

   int rDiff, gDiff, bDiff;
   int rca, gca, bca;
   
   register int x, y;

   rDiff = (cb.red())   - (rca = ca.red());
   gDiff = (cb.green()) - (gca = ca.green());
   bDiff = (cb.blue())  - (bca = ca.blue());
   
   register int rl = rca << 16;
   register int gl = gca << 16;
   register int bl = bca << 16;
   
   int rcdelta = ((1<<16) / (o == Qt::Vertical ? pix.height() : pix.width())) * rDiff;
   int gcdelta = ((1<<16) / (o == Qt::Vertical ? pix.height() : pix.width())) * gDiff;
   int bcdelta = ((1<<16) / (o == Qt::Vertical ? pix.height() : pix.width())) * bDiff;
   
   // these for-loops could be merged, but the if's in the inner loop
   // would make it slow
   if (o == Qt::Vertical)
      for ( y = 0; y < pix.height(); y++ )
      {
         rl += rcdelta;
         gl += gcdelta;
         bl += bcdelta;
         
         p.setPen(QColor(rl>>16, gl>>16, bl>>16));
         p.drawLine(0, y, 31 , y);
      }
   else
   {
      for( x = 0; x < pix.width(); x++)
      {
         rl += rcdelta;
         gl += gcdelta;
         bl += bcdelta;
         
         p.setPen(QColor(rl>>16, gl>>16, bl>>16));
         p.drawLine(x, 0, x, 31);
      }
   }
   
//    if (size < 105883) // this is where our dictionary reaches - should be enough for the moment ;)
   {
      // cache for later ;)
      if (cache->size() == cache->capacity())
         // we're full, wipe for this time
         cache->clear();
      it = cache->insert(magicNumber, pix);
      return it.value();
   }
//    return pix;
}

const QPixmap OxygenStyle::gloss(const QColor &oc, const int size, Qt::Orientation o, Orientation3D o3D) const
{
   // SEE gradient(.)
   PixmapCache *cache = &(const_cast<OxygenStyle*>( this )->glossCache[o-1][o3D?1:0]);
   int sizeSloppyness = 1;
   uint magicNumber = 0;
   PixmapCache::const_iterator it;
   if (size < 105884) // this is where our dictionary reaches - should be enough for the moment ;)
   {
      int frameBase = 0;
      int frameSize = 20;
      while ((frameBase += frameSize) < size)
      {
         sizeSloppyness++;
         frameSize += 20;
      }
      frameBase -=frameSize;
      frameSize -= 20;
      // The mapping is done by the "magicNumber"
//       magicNumber =  (((frameSize + (size - frameBase)/sizeSloppyness) & 0xff) << 24) | (oc.rgb() & 0xffffff);
      magicNumber =  (((frameSize + (size - frameBase)/sizeSloppyness) & 0xff) << 21) | 
         (((oc.red() >> 1) & 0x7f) << 14) |
         (((oc.green() >> 1) & 0x7f) << 7 ) |
         ((oc.blue() >> 1) & 0x7f);
      it = cache->find(magicNumber);
      if (it != cache->end())
         return it.value();
   }
   else
   {
      qWarning("gradient with more than 105883 steps requested, returning NULL pixmap");
      return nullPix;
   }
   
   QPixmap pix;
   if (o == Qt::Horizontal)
      pix = QPixmap(size + (size % sizeSloppyness), 32);
   else
      pix = QPixmap(32, size + (size % sizeSloppyness));
   
   if (pix.isNull())
   {
      qWarning("NULL Pixmap requested, size was %d",size);
      return nullPix;
   }
   
   // calculate the determining colors
   QColor d,dd,b,bb;
   int h,s,v, ch,cs,cv, delta, add;
   oc.getHsv(&h,&s,&v);
   if (v < 85) v = 85;
   b.setHsv(h,s,v); d = b;
   if (o3D == Sunken) dd = b;
   
   add = ((180-qGray(b.rgb()))>>1);
   if (add < 0) add = -add/2;
   
   if (o3D == Raised)
   {
      cv = v+27+add;
      if (cv > 255)
      {
         delta = cv-255; cv = 255;
         cs = s - delta; if (cs < 0) cs = 0;
         ch = h - delta/6; if (ch < 0) ch = 360+ch;
      }
      else
      {
         ch = h; cs = s;
      }
      bb.setHsv(ch,cs,cv);
   }
   
   cv = v - 14-add; if (cv < 0) cv = 0;
   cs = s*13/7; if (cs > 255) cs = 255;
   if (o3D == Raised)
      dd.setHsv(h,cs,cv);
   else
      bb.setHsv(h,cs,cv);
   
   // real painting action
   QPainter p(&pix);
   
   int rDiff = b.red() - bb.red();
   int gDiff = b.green() - bb.green();
   int bDiff = b.blue() - bb.blue();
   
   register int rl = (bb.red() << 16);
   register int gl = (bb.green() << 16);
   register int bl = (bb.blue() << 16);
   
   int rcdelta; int gcdelta; int bcdelta;
   
   if (o == Qt::Vertical)
   {
      register int y;
      h = (1<<17) / (pix.height()+1);
      rcdelta = h * rDiff;
      gcdelta = h * gDiff;
      bcdelta = h * bDiff;
      
      for ( y = 0; y < (pix.height()>>1); y++ )
      {
         rl += rcdelta; gl += gcdelta; bl += bcdelta;
         p.setPen(QColor(rl>>16, gl>>16, bl>>16));
         p.drawLine(0, y, 31 , y);
      }
      
      
      rDiff = d.red() - dd.red();
      gDiff = d.green() - dd.green();
      bDiff = d.blue() - dd.blue();
      
      rl = (dd.red() << 16);
      gl = (dd.green() << 16);
      bl = (dd.blue() << 16);
      
      rcdelta = h * rDiff;
      gcdelta = h * gDiff;
      bcdelta = h * bDiff;
      
      for ( ; y < pix.height(); y++ )
      {
         rl += rcdelta; gl += gcdelta; bl += bcdelta;
         p.setPen(QColor(rl>>16, gl>>16, bl>>16));
         p.drawLine(0, y, 31 , y);
      }
   }
   else
   {
      register int x;
      if (o3D == Raised)
      {
         h = (1<<16) / ((pix.width()+1)*8/18);
         s = (pix.width()*8/18);
      }
      else
      {
         h = (1<<17) / (pix.width()+1);
         s = pix.width()>>1;
      }
      rcdelta = h * rDiff;
      gcdelta = h * gDiff;
      bcdelta = h * bDiff;
      
      for ( x = 0; x < s; x++ )
      {
         rl += rcdelta; gl += gcdelta; bl += bcdelta;
         p.setPen(QColor(rl>>16, gl>>16, bl>>16));
         p.drawLine(x, 0, x , 31);
      }
      
      rDiff = d.red() - dd.red();
      gDiff = d.green() - dd.green();
      bDiff = d.blue() - dd.blue();
      
      rl = (dd.red() << 16);
      gl = (dd.green() << 16);
      bl = (dd.blue() << 16);
      
      if (o3D == Raised)
         h = (1<<16) / ((pix.width()+1)*10/18);
      rcdelta = h * rDiff;
      gcdelta = h * gDiff;
      bcdelta = h * bDiff;
      
      for ( ; x < pix.width(); x++ )
      {
         rl += rcdelta; gl += gcdelta; bl += bcdelta;
         p.setPen(QColor(rl>>16, gl>>16, bl>>16));
         p.drawLine(x, 0, x , 31);
      }
   }

   // store the result
   // 1768 is where our dictionary reaches - should be enough for the moment ;)
//    if (size < 105883)
   {

      // cache for later ;)
      if (cache->size() == cache->capacity())
      // we're full, wipe for this time
         cache->clear();
      it = cache->insert(magicNumber, pix);

      return it.value();
   }
//    return pix;
}
/**======================================*/

/**QStyle reimplementation ========================================= */

void OxygenStyle::polish ( QApplication * app )
{
   if (timer && !timer->isActive())
      timer->start(50);
   loadPixmaps();
   if (app->desktop())
      bgYoffset_ = app->desktop()->height()*bgYoffset_/738;
   popupPix = config.inversePopups ? fgPix : bgPix;
   QPalette pal = app->palette();
   polish(pal);
   app->setPalette(pal);
   app->installEventFilter(this);
}

#define _SHIFTCOLOR_(clr) clr = QColor(CLAMP(clr.red()-10,0,255),CLAMP(clr.green()-10,0,255),CLAMP(clr.blue()-10,0,255))

void OxygenStyle::polish( QPalette &pal )
{
   switch (config.bgMode)
   {
   case FullPix:
   case BrushedMetal:
   {
      int h,s,v;
      // create use a nice background
      QColor c = pal.color(QPalette::Active, QPalette::Background);
      c.getHsv(&h,&s,&v);
      if (v < 70) // very dark colors won't make nice backgrounds ;)
         c.setHsv(h,s,70);
      originalBgColor_ = c;
      if (config.acceleration == None)
         _SHIFTCOLOR_(c);
      pal.setColor( QPalette::Window, c );
      
      if (!_bgBrush)
      {
         if (config.bgMode == FullPix)
         {
            if (config.acceleration > None)
               _bgBrush = new DynamicBrush((DynamicBrush::Mode)config.acceleration, this);
            else
               _bgBrush = new DynamicBrush(bgPix, shadowPix, bgYoffset_, this);
         }
         else
            _bgBrush = new DynamicBrush(QImage(":/leftCenter"), QImage(":/leftTile"), this);
      }
      break;
   }
   case Scanlines:
   {
      QColor c = pal.color(QPalette::Active, QPalette::Background);
      if (!_scanlines[0])
      {
         _scanlines[0] = new QPixmap(64, 64);
         _scanlines[0]->fill( c.light(110).rgb() );
         QPainter p( _scanlines[0] );
         p.setPen( c );
         int i;
         for ( i = 1; i < 64; i += 4 )
         {
            p.drawLine( 0, i, 63, i );
            p.drawLine( 0, i+2, 63, i+2 );
         }
         p.setPen( c.dark( 108 ) );
         for ( i = 2; i < 63; i += 4 )
            p.drawLine( 0, i, 63, i );
         p.end();
      }
      QBrush brush( c, *_scanlines[0] );
      pal.setBrush( QColorGroup::Background, brush );
      break;
   }
   case Plain:
   default:
      break;
   }

   int highlightGray = qGray(pal.color(QPalette::Active, QPalette::Highlight).rgb());
   pal.setColor(QPalette::Disabled, QPalette::Highlight, QColor(highlightGray,highlightGray,highlightGray));
   
   //inactive palette
   pal.setColor(QPalette::Inactive, QPalette::WindowText, midColor(pal.color(QPalette::Active, QPalette::Window),pal.color(QPalette::Active, QPalette::WindowText)));
   pal.setColor(QPalette::Inactive, QPalette::Text, midColor(pal.color(QPalette::Active, QPalette::Base),pal.color(QPalette::Active, QPalette::Text),1,3));
   pal.setColor(QPalette::Inactive, QPalette::Highlight, midColor(pal.color(QPalette::Active, QPalette::Highlight),pal.color(QPalette::Active, QPalette::HighlightedText),3,1));
   pal.setColor(QPalette::Inactive, QPalette::AlternateBase, midColor(pal.color(QPalette::Active, QPalette::AlternateBase),pal.color(QPalette::Active, QPalette::Base),3,1));
   
//    loadPixmaps();
//    if (qApp->desktop())
//       bgYoffset_ = qApp->desktop()->height()*bgYoffset_/738;
   
   popupPix = config.inversePopups ? fgPix : bgPix;
}

void OxygenStyle::polish( QWidget * widget)
{
   // installs dynamic brush to all widgets, taking care of a correct bg pixmap size
   //TODO maybe we can exclude some more widgets here... (currently only popup menus)
   if (!(qobject_cast<QMenu*>(widget) ||
         widget->inherits("QAlphaWidget") ||
         widget->inherits("QComboBoxPrivateContainer")
        ))
      widget->installEventFilter(_bgBrush);

#ifdef MOUSEDEBUG
   widget->installEventFilter(this);
#endif
   
   if (qobject_cast<QPushButton *>(widget)
#ifndef QT_NO_COMBOBOX
       || qobject_cast<QComboBox *>(widget)
#endif
#ifndef QT_NO_SPINBOX
       || qobject_cast<QAbstractSpinBox *>(widget)
#endif
       || qobject_cast<QCheckBox *>(widget)
       || qobject_cast<QScrollBar *>(widget)
       || widget->inherits("QHeaderView")
       || qobject_cast<QRadioButton *>(widget)
#ifndef QT_NO_SPLITTER
       || qobject_cast<QSplitterHandle *>(widget)
#endif
#ifndef QT_NO_TABBAR
       || qobject_cast<QTabBar *>(widget)
#endif
       || widget->inherits("QWorkspaceTitleBar")
       || widget->inherits("QToolButton")
       || widget->inherits("QDockWidget")
       || widget->inherits("QToolBar")
       || widget->inherits("QToolBarHandle")
       || widget->inherits("QDockSeparator")
       || widget->inherits("QToolBoxButton")
       || widget->inherits("QAbstractSlider")
       || widget->inherits("QDockWidgetSeparator")
       || widget->inherits("Q3DockWindowResizeHandle")
      )
         widget->setAttribute(Qt::WA_Hover);
   
   if (qobject_cast<QProgressBar*>(widget))
   {
      widget->installEventFilter(this);
      connect(widget, SIGNAL(destroyed(QObject*)), this, SLOT(progressbarDestroyed(QObject*)));
   }
   
   if (qobject_cast<QAbstractScrollArea*>(widget) || qobject_cast<Q3ScrollView*>(widget) ||
       widget->inherits("QWorkspaceTitleBar"))
      widget->installEventFilter(this);
   
   if (widget->inherits("QWorkspace"))
      connect(this, SIGNAL(MDIPopup(QPoint)), widget, SLOT(_q_popupOperationMenu(QPoint)));

   
   if (false // to simplify the #ifdefs
#ifndef QT_NO_MENUBAR
       || qobject_cast<QMenuBar *>(widget)
#endif
#ifdef QT3_SUPPORT
       || widget->inherits("Q3ToolBar")
#endif
#ifndef QT_NO_TOOLBAR
       || qobject_cast<QToolBar *>(widget)
       || (widget && qobject_cast<QToolBar *>(widget->parent()))
#endif
      )
   {
      widget->setBackgroundRole(QPalette::Window);
      if (config.bgMode == Scanlines)
      {
         widget->setAutoFillBackground ( true );
         QPalette pal = widget->palette();
         QColor c = pal.color(QPalette::Active, QPalette::Window);
         if (!_scanlines[1])
         {
            _scanlines[1] = new QPixmap(64, 64);
            _scanlines[1]->fill( c.light(110).rgb() );
            QPainter p(_scanlines[1]);
            p.setPen( c.light( 106 ) );
            for ( int i = 1; i < 64; i += 4 )
            {
               p.drawLine( 0, i, 63, i );
               p.drawLine( 0, i+2, 63, i+2 );
            }
            p.setPen( c.dark( 108 ) );
            for ( int i = 2; i < 63; i += 4 )
               p.drawLine( 0, i, 63, i );
         }
         QBrush brush( c, *_scanlines[1] );
         pal.setBrush( QColorGroup::Window, brush );
         widget->setPalette(pal);
      }
   }
   
   if (qobject_cast<QGroupBox *>(widget) && config.bgMode == Scanlines && !_scanlines[2])
   {
      _scanlines[2] = new QPixmap(64, 64);
      QPalette pal = widget->palette();
      QColor c = pal.color(QPalette::Active, QPalette::Window).dark(105);
      _scanlines[2]->fill( c.rgb() );
      QPainter p(_scanlines[2]);
      p.setPen( c.dark(103) );
      int i;
      for ( i = 1; i < 64; i += 4 )
      {
         p.drawLine( 0, i, 63, i );
         p.drawLine( 0, i+2, 63, i+2 );
      }
      p.setPen( c.dark( 108 ) );
      for ( i = 2; i < 63; i += 4 )
         p.drawLine( 0, i, 63, i );
      p.end();
   }
   
   if (qobject_cast<QMenu *>(widget))
   {
      widget->setBackgroundRole ( QPalette::WindowText );
      widget->setForegroundRole ( QPalette::Window );
      QFont tmpFont = widget->font();
      tmpFont.setBold(true);
      widget->setFont(tmpFont);
   }
}

bool OxygenStyle::eventFilter( QObject *object, QEvent *ev )
{
   switch (ev->type())
   {
   case QEvent::MouseButtonPress:
      {
         QMouseEvent *mev = (QMouseEvent*)ev;
#ifdef MOUSEDEBUG
         qDebug() << object;
#endif
         if (( mev->button() == Qt::LeftButton) && object->inherits("QWorkspaceTitleBar"))
         {
            //TODO this is a hack to get the popupmenu to the right side. bug TT to query the position with a SH
            QWidget *widget = (QWidget*)object;
            // check for menu button
            QWidget *MDI = qobject_cast<QWidget*>(widget->parent()); if (!MDI) return false; //this is elsewhat...
            /// this does not work as TT keeps the flag in a private to the titlebar (for no reason?)
//             if (!(widget->windowFlags() & Qt::WindowSystemMenuHint)) return false;
            // check if we clicked it..
            if (mev->x() < widget->width()-widget->height()-2) return false;
            // find popup
            MDI = qobject_cast<QWidget*>(MDI->parent()); if (!MDI) return false; //this is elsewhat...
            MDI = MDI->findChild<QMenu *>("qt_internal_mdi_popup");
            if (!MDI)
            {
               qWarning("MDI popup not found, unable to calc menu position");
               return false;
            }
            // calc menu position
            emit MDIPopup(widget->mapToGlobal( QPoint(widget->width() - MDI->sizeHint().width(), widget->height())));
            return true;
         }
         return false;
      }
   case QEvent::Show:
   {
      if (qobject_cast<QProgressBar*>(object) && ((QWidget*)object)->isEnabled())
      {
         progressbars[(QWidget*)object] = 0;
         return false;
      }
   }
#define HANDLE_SCROLL_AREA_EVENT \
         if (area->horizontalScrollBar()->isVisible())\
            area->horizontalScrollBar()->repaint();\
         if (area->verticalScrollBar()->isVisible())\
            area->verticalScrollBar()->repaint();\

   case QEvent::Enter:
      if (QAbstractScrollArea* area = qobject_cast<QAbstractScrollArea*>(object))
      {
         if (!area->isEnabled()) return false;
         HANDLE_SCROLL_AREA_EVENT
         return false;
      }
      else if (Q3ScrollView* area = qobject_cast<Q3ScrollView*>(object))
      {
         if (!area->isEnabled()) return false;
         HANDLE_SCROLL_AREA_EVENT
         return false;
      }
      return false;
   case QEvent::Leave:
      if (QAbstractScrollArea* area = qobject_cast<QAbstractScrollArea*>(object))
      {
         HANDLE_SCROLL_AREA_EVENT return false;
      }
      else if (Q3ScrollView* area = qobject_cast<Q3ScrollView*>(object))
      {
         HANDLE_SCROLL_AREA_EVENT return false;
      }
      return false;
   case QEvent::EnabledChange:
      if (qobject_cast<QProgressBar*>(object))
      {
         if (((QWidget*)object)->isEnabled())
            progressbars[(QWidget*)object] = 0;
         else
            progressbars.remove((QWidget*)object);
         return false;
      }
      return false;
   default:
      return false;
   }
}


void OxygenStyle::unPolish( QApplication *app )
{
   if (timer)
      timer->stop();
//    inExitPolish = true;
//    QPalette pal( app->palette() );
//    // rQWidgeteset bg Tileset
//    for (int i = QPalette::Disabled; i < QPalette::NColorGroups; i++)
//    {
//       if ( !pal.brush( (QPalette::ColorGroup)i, QPalette::Background ).texture().isNull() )
//          pal.setBrush( (QPalette::ColorGroup)i, QPalette::Background, originalBgColor_ );
//       if ( !pal.brush( (QPalette::ColorGroup)i, QPalette::Button ).texture().isNull() )
//          pal.setBrush( (QPalette::ColorGroup)i, QPalette::Button, originalBgColor_ );
//    }
//    app->setPalette( pal );
//    inExitPolish = false;
}

void OxygenStyle::unPolish( QWidget *widget )
{
   if (qobject_cast<QProgressBar*>(widget))
   {
      widget->removeEventFilter(this);
      disconnect(widget, SIGNAL(destroyed(QObject*)), this, SLOT(progressbarDestroyed(QObject*)));
   }
   if (qobject_cast<QAbstractScrollArea*>(widget) || qobject_cast<Q3ScrollView*>(widget))
      widget->removeEventFilter(this);
   if (_bgBrush)
      widget->removeEventFilter(this);
//    w->removeEventFilter(this);
//    if (w->isTopLevel() || qobject_cast<QGroupBox*>(w) || w->inherits("KActiveLabel"))
//       w->setPalette(QPalette());
}

QPalette OxygenStyle::standardPalette () const
{
   return QPalette ( Qt::black, QColor(245,245,245), Qt::white, QColor(200,200,200), QColor(220,220,220), Qt::black, Qt::black, Qt::white, QColor(245,245,245) );
}

/** eventcontrol slots*/

void OxygenStyle::fakeMouse()
{
   if (mouseButtonPressed_) // delayed mousepress for move event
   {
      QCursor::setPos ( cursorPos_ );
      XTestFakeButtonEvent(QX11Info::display(),1, false, 0);
      XTestFakeKeyEvent(QX11Info::display(),XKeysymToKeycode(QX11Info::display(), XK_Alt_L), true, 0);
      XTestFakeButtonEvent(QX11Info::display(),1, true, 0);
      XTestFakeKeyEvent(QX11Info::display(),XKeysymToKeycode(QX11Info::display(), XK_Alt_L), false, 0);
      XFlush(QX11Info::display());
   }
}

void OxygenStyle::fadeIn(QPushButton *button)
{
   ButtonFades::iterator it = bfi.find(button->winId());
   if (it == bfi.end())
      it = bfi.insert(button->winId(), ButtonFadeInfo());
   it.value().fadeIn = true;
   if (it.value().timerId == 0)
   {
      it.value().index = 0;
      it.value().timerId = button->startTimer(50);
   }
}

void OxygenStyle::fadeOut(QPushButton *button)
{
   ButtonFades::iterator it = bfi.find(button->winId());
   if (it == bfi.end())
      it = bfi.insert(button->winId(), ButtonFadeInfo());
   it.value().fadeIn = false;
   if (it.value().timerId == 0)
   {
      it.value().index = 9;
      it.value().timerId = button->startTimer(50);
   }
}

QColor OxygenStyle::mapFadeColor(const QColor &color, int index) const
{
   OxygenStyle *ptr = const_cast<OxygenStyle*>( this );
   FadeColors::iterator it = ptr->fadeColorMap.find(color.rgb());
   if (it != fadeColorMap.end())
      return QColor(it.value()[index]);
   // no color map found, create one and return the queried value
   QRgb *rgb = new QRgb[10];
   int iRed = qApp->palette().color(QPalette::Active, QPalette::Button).red();
   int iGreen = qApp->palette().color(QPalette::Active, QPalette::Button).green();
   int iBlue = qApp->palette().color(QPalette::Active, QPalette::Button).blue();
   int cRed = color.red();
   int cGreen = color.green();
   int cBlue = color.blue();
   for (int i = 1; i < 11; i++)
   {
      rgb[i-1] = qRgb( iRed + (cRed-iRed)*(i)/11, iGreen + (cGreen-iGreen)*(i)/11, iBlue + (cBlue-iBlue)*(i)/11);
   }
   ptr->fadeColorMap.insert(color.rgb(), rgb);
   return QColor(rgb[index]);
}
