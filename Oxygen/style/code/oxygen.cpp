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

/**================== Qt4 includes ======================*/
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
/**============= Qt3 support includes ======================*/
#include <Q3ScrollView>
/**========================================================*/

/**============= System includes ==========================*/
// #include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/keysym.h>
#include <X11/extensions/XTest.h>
#include <X11/extensions/Xrender.h>
// #include <fixx11h.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cmath>
/**========================================================*/

/**============= DEBUG includes ==========================*/
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
/**========================================================*/

/**============= Oxygen includes ==========================*/
#include "oxygen.h"
#include "dynamicbrush.h"
/**=========================================================*/


/**============= Convenience Defs ==========================*/
#define _IsNotHtmlWidget(w) qstrcmp( w->name(), "__khtml" )
#define _IsHtmlWidget(w) !qstrcmp( w->name(), "__khtml" )
#define _IsViewportChild(w) w->parent() &&\
( !qstrcmp(w->parent()->name(), "qt_viewport") || \
  !qstrcmp(w->parent()->name(), "qt_clipped_viewport") )

#define _HighContrastColor(c) (qGray(c.rgb()) < 128 ) ? Qt::white : Qt::black

#define _BLOCKEVENTS_(obj) obj->installEventFilter(eventKiller)
#define _UNBLOCKEVENTS_(obj) obj->removeEventFilter(eventKiller)

#define _IsTabStack(w) !qstrcmp( w->name(), "qt_tabwidget_stackedwidget" )
/**=========================================================*/


/**============= extern C stuff ==========================*/
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
/**=========================================================*/
using namespace Oxygen;

/** static config object */
Config config;
Dpi dpi;

/** The event killer*/
bool EventKiller::eventFilter( QObject *, QEvent *)
{
   return true;
}
static EventKiller *eventKiller = new EventKiller();

/** to be able to send nullpix refs */
static QPixmap nullPix;

/** Get some excluded code */
#include "inlinehelp.cpp"
#include "animation.cpp"
#include "gradients.cpp"

/** Internal, handles the shared bg pixmaps*/
static Pixmap bgPix = 0;
static Pixmap fgPix = 0;
Pixmap shadowPix;
int bgYoffset_;

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
/**=====================================================================*/

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

void OxygenStyle::readSettings()
{
   QSettings settings("Oxygen", "Style");
   settings.beginGroup("Style");
   
   config.scale = settings.value("Scale", 1.0).toDouble();
   config.checkType = settings.value("CheckType", 0).toInt();
   
   config.acceleration = (Acceleration) settings.value("Acceleration", XRender).toInt();
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
   
   config.tabwidget3D = (Orientation3D)(settings.value("TabWidget3D", 1).toInt());
   
   config.bgMode = (BGMode) settings.value("BackgroundMode", FullPix).toInt();
   
   config.tabTransition = (TabTransition) settings.value("TabTransition", ScanlineBlend).toInt();
   config.HAL9000 = settings.value("HAL9000", false).toBool();
   settings.endGroup();
}

#define SCALE(_N_) lround(_N_*config.scale)

void OxygenStyle::generatePixmaps()
{
   frames.rect[Sunken] = Tile::Set(QPixmap(":/frame-sunken"),5,5,1,1);
   frames.rect[Relief] = Tile::Set(QPixmap(":/frame-relief"),4,5,1,1);
   frames.rect[Raised] = Tile::Set(QPixmap(":/frame-raised"),4,6,1,1);
   
   masks.rect[Sunken] = Tile::Mask(nullPix,0,0,0,0, 2,2,-2,-2);
   masks.rect[Relief] = Tile::Mask(nullPix,0,0,0,0, 2,2,-2,-2);
   masks.rect[Raised] = Tile::Mask(nullPix,0,0,0,0, 2,2,-2,-2);

   frames.round[Sunken] = Tile::Set(QPixmap(":/round-sunken"),8,8,11,8);
   frames.round[Relief] = Tile::Set(QPixmap(":/round-relief"),8,10,11,6);
   frames.round[Raised] = Tile::Set(QPixmap(":/round-raised"),8,8,11,8);
   
   masks.round[Sunken] = Tile::Mask(QPixmap(":/mask-round-sunken"),8,8,11,8,1,1,-2,-2);
   masks.round[Relief] = Tile::Mask(QPixmap(":/mask-round-relief"),8,10,11,6,1,1,-1,-3);
   masks.round[Raised] = Tile::Mask(QPixmap(":/mask-round-raised"),8,8,11,8,2,1,-2,-2);
   
   QPixmap tmp; QPainter p;
   
   // PUSHBUTTON =====================================
   // shadow
   int $1 = dpi.$1, $2 = dpi.$2, $2_2 = lround($2/2.0);
   int $9 = dpi.$9, $9_2 = ($9-1)/2;
   tmp = QPixmap($9,$9);
   for (int i = 0; i < 2; ++i)
   {
      tmp.fill(Qt::transparent);
      p.begin(&tmp);
      p.setPen(Qt::NoPen);
      p.setRenderHint(QPainter::Antialiasing);
      p.setBrush(QColor(0,0,0,(i+1)*16));
      p.drawRoundRect(0,0,$9,$9,60,60);
      p.drawRoundRect($2_2,$2_2,$9-$2,$9-$2,75,75);
      p.end();
      shadows.button[i] = Tile::Set(tmp,$9_2,$9_2,$9-2*$9_2,$9-2*$9_2);
   }
   
   // -> sunken
   QImage tmpImg($9,$9, QImage::Format_ARGB32);
   tmpImg.fill(Qt::transparent);

   p.begin(&tmpImg);
   p.setPen(Qt::NoPen);
   p.setRenderHint(QPainter::Antialiasing);
   p.setBrush(QColor(0,0,0,85)); p.drawRoundRect(0,0,$9,$9,80,80);
   p.setCompositionMode( QPainter::CompositionMode_DestinationOut );
   p.setBrush(QColor(0,0,0,120)); p.drawRoundRect(0,$1,$9,dpi.$8,75,75);
   p.setBrush(QColor(0,0,0,140)); p.drawRoundRect(0,$2,$9,dpi.$7,80,80);
   p.setBrush(QColor(0,0,0,160)); p.drawRoundRect($1,dpi.$3,dpi.$7,dpi.$6,85,85);
   p.setBrush(QColor(0,0,0,180)); p.drawRoundRect($2,dpi.$4,dpi.$3,dpi.$5,90,90);
   p.end();

   shadows.sunken = Tile::Set(QPixmap::fromImage(tmpImg),$9_2,$9_2,$9-2*$9_2,$9-2*$9_2);
   
   
   // outlines
   tmp = QPixmap($9,$9);
   for (int i = 0; i < 2; ++i)
   {
      tmp.fill(Qt::transparent);
      p.begin(&tmp);
      p.setRenderHint(QPainter::Antialiasing);
      p.setPen(QColor(255,255,255,100+i*60));
      p.setBrush(Qt::NoBrush);
      p.drawRoundRect(0,0,$9,2*$9,75,38);
      p.end();
      frames.button[i] = Tile::Set(tmp,$9_2,$9_2,$9-2*$9_2,$9-2*$9_2);
   }
//    frames.button[0] = Tile::Nuno(100);
//    frames.button[1] = Tile::Nuno(160);
   
   // mask
   tmp = QPixmap($9,$9);
   tmp.fill(Qt::transparent);
   p.begin(&tmp);
   p.setPen(Qt::NoPen);
   p.setRenderHint(QPainter::Antialiasing);
   p.setBrush(QColor(0,0,0,255));
   p.drawRoundRect(0,0,$9,$9,60,60);
   p.end();
   masks.button = Tile::Mask(tmp,$9_2,$9_2,$9-2*$9_2,$9-2*$9_2,0,0,0,0,60,60);
   
   // toplight
   int $49 = SCALE(49);
   int $49_2 = ($49-1)/2;
   tmp = QPixmap($49,$49);
   tmp.fill(Qt::transparent);
   QRadialGradient rg( tmp.rect().center(), $49_2 );
   rg.setColorAt ( 0, QColor(255,255,255,160) );
   rg.setColorAt ( 1, QColor(255,255,255,0) );
   p.begin(&tmp);
   p.fillRect(0,0,$49,$49,rg);
   p.end();
   tmp = tmp.scaled( $49, dpi.$5, Qt::IgnoreAspectRatio, Qt::SmoothTransformation );
   tmp = tmp.copy(0,$2,$49,dpi.$3);
   lights.top = Tile::Line(tmp,Qt::Horizontal,$49_2,-$49_2);
   
   // ================================================================
   
   // RADIOUTTON =====================================
   int rw = dpi.ExclusiveIndicator;
   int rh = dpi.ExclusiveIndicator;
   // shadow
   for (int i = 0; i < 2; ++i)
   {
      shadows.radio[i] = QPixmap(rw, rh);
      shadows.radio[i].fill(Qt::transparent);
      p.begin(&shadows.radio[i]);
      p.setPen(Qt::NoPen);
      p.setRenderHint(QPainter::Antialiasing);
      p.setBrush(QColor(0,0,0,(i+1)*9));
      p.drawEllipse(0,0,rw,rh);
      p.setBrush(QColor(0,0,0,(i+1)*20));
      p.drawEllipse($2_2,$2_2,rw-$2,rh-$2);
      p.end();
   }
   
   // mask
   rw -= dpi.$4; rh -= dpi.$4;
   masks.radio = QPixmap(rw, rh);
   masks.radio.fill(Qt::transparent);
   p.begin(&masks.radio);
   p.setPen(Qt::NoPen);
   p.setRenderHint(QPainter::Antialiasing);
   p.setBrush(QColor(0,0,0,255));
   p.drawEllipse(0,0,rw,rh);
   p.end();
   
   // mask fill
   rw -= dpi.$4; rh -= dpi.$4;
   masks.radioIndicator = QPixmap(rw, rh);
   masks.radioIndicator.fill(Qt::transparent);
   p.begin(&masks.radioIndicator);
   p.setPen(Qt::NoPen);
   p.setRenderHint(QPainter::Antialiasing);
   p.setBrush(QColor(0,0,0,255));
   p.drawEllipse(0,0,rw,rh);
   p.end();
   
   // ================================================================
   
   // TABBAR =====================================
   
   // mask
   int $17 = SCALE(17);
   tmp = QPixmap($17,$17);
   tmp.fill(Qt::transparent);
   p.begin(&tmp);
   p.setPen(Qt::NoPen);
   p.setRenderHint(QPainter::Antialiasing);
   p.setBrush(QColor(0,0,0,255));
   p.drawRoundRect(0,0,$17,$17,60,60);
   p.end();
   int $17_2 = ($17-1)/2;
   masks.tab = Tile::Mask(tmp,$17_2,$17_2,$17-2*$17_2,$17-2*$17_2,0,0,0,0,60,60);
   
   // shadow
   tmpImg = QImage($17,$17, QImage::Format_ARGB32);
   tmpImg.fill(Qt::transparent);
   p.begin(&tmpImg);
   p.setPen(Qt::NoPen);
   p.setRenderHint(QPainter::Antialiasing);
   p.setBrush(QColor(0,0,0,11)); p.drawRoundRect(0,0,$17,$17,90,90);
   p.setBrush(QColor(0,0,0,13)); p.drawRoundRect($1,$1,$17-$2,$17-$2,93,93);
   p.setBrush(QColor(0,0,0,15)); p.drawRoundRect($2,$2,$17-dpi.$4,$17-dpi.$4,96,96);
   p.setBrush(QColor(0,0,0,18)); p.drawRoundRect(dpi.$3,dpi.$3,$17-dpi.$6,$17-dpi.$6,99,99);
   p.setCompositionMode( QPainter::CompositionMode_DestinationOut );
   p.setBrush(QColor(0,0,0,255)); p.drawRoundRect($2,$1,$17-dpi.$4,$17-dpi.$5,99,99);
   p.setCompositionMode( QPainter::CompositionMode_SourceOver );
   p.setPen(QColor(255,255,255,170)); p.setBrush(Qt::NoBrush);
   p.drawRoundRect($2,$1,$17-dpi.$4,$17-dpi.$5,99,99);
   p.end();
   shadows.tab = Tile::Set(QPixmap::fromImage(tmpImg),$17_2,$17_2,$17-2*$17_2,$17-2*$17_2);
   
   // GROUPBOX =====================================
   
   // shadow
   tmpImg = QImage($49,$49, QImage::Format_ARGB32);
   tmpImg.fill(Qt::transparent);
   p.begin(&tmpImg);
   p.setPen(Qt::NoPen);
   p.setRenderHint(QPainter::Antialiasing);
   p.setBrush(QColor(0,0,0,11)); p.drawRoundRect(0,0,$49,2*$49,45,22);
   p.setBrush(QColor(0,0,0,13)); p.drawRoundRect($1,$1,$49-$2,2*$49,46,23);
   p.setBrush(QColor(0,0,0,15)); p.drawRoundRect($2,$2,$49-dpi.$4,2*$49,47,23);
//    p.setBrush(QColor(0,0,0,18)); p.drawRoundRect(dpi.$3,dpi.$3,$49-dpi.$6,2*$49,48,24);
   p.setCompositionMode( QPainter::CompositionMode_DestinationOut );
   p.setBrush(QColor(0,0,0,255)); p.drawRoundRect(dpi.$3,dpi.$2,$49-dpi.$6,2*$49,48,24);
   p.setCompositionMode( QPainter::CompositionMode_SourceOver );
   p.setPen(QColor(255,255,255,170)); p.setBrush(Qt::NoBrush);
   p.drawRoundRect(dpi.$3,dpi.$3,$49-dpi.$6,2*$49,48,24);
   p.setCompositionMode( QPainter::CompositionMode_DestinationOut );
   p.setBrush(QColor(0,0,0,6));
   int $32 = dpi.$32;
   for (int i = 1; i < $32; ++i)
      p.drawRect(0,$49-i,$49,i);
   p.end();
   int $12 = dpi.$12;
   shadows.group = Tile::Set(QPixmap::fromImage(tmpImg),$12,$12,$49-2*$12,$1);
   
   // mask
   int $25 = SCALE(25);
   tmp = QPixmap($25,$25);
   tmp.fill(Qt::transparent);
   p.begin(&tmp);
   p.setPen(Qt::NoPen);
   p.setRenderHint(QPainter::Antialiasing);
   p.setBrush(QColor(0,0,0,255));
   p.drawRoundRect(0,0,$25,$25,88,88);
   p.end();
   $12 = ($25-1)/2;
   masks.group = Tile::Mask(tmp,$12,$12,$25-2*$12,$25-2*$12,0,0,0,0,88,88);
   
   // shadow line
   tmp = QPixmap($49,$2);
   tmp.fill(Qt::transparent);
   p.begin(&tmp);
   QLinearGradient lg(0,0,$49,0);
   QGradientStops stops;
   stops << QGradientStop( 0, QColor(255,255,255,0) )
      << QGradientStop( 0.5, QColor(255,255,255,80) )
      << QGradientStop( 1, QColor(255,255,255,0) );
   lg.setStops(stops);
   p.fillRect(0,0,$49,$1,lg);
   stops.clear();
   stops << QGradientStop( 0, QColor(0,0,0,0) )
      << QGradientStop( 0.5, QColor(0,0,0,80) )
      << QGradientStop( 1, QColor(0,0,0,0) );
   lg.setStops(stops);
   p.fillRect(0,$1,$49,$2-$1,lg);
   p.end();
   shadows.line = Tile::Line(tmp,Qt::Horizontal,$49_2,-$49_2);
   
   // ================================================================
}

void OxygenStyle::initMetrics()
{
   dpi.$1 = SCALE(1); dpi.$2 = SCALE(2);
   dpi.$3 = SCALE(3); dpi.$4 = SCALE(4);
   dpi.$5 = SCALE(5); dpi.$6 = SCALE(6);
   dpi.$7 = SCALE(7); dpi.$8 = SCALE(8);
   dpi.$9 = SCALE(9); dpi.$10 =SCALE(10);
   
   dpi.$12 = SCALE(12); dpi.$16 = SCALE(16);
   dpi.$18 = SCALE(18); dpi.$32 = SCALE(32);
   dpi.$80 = SCALE(80);
   
   dpi.ScrollBarExtent = SCALE(19);
   dpi.ScrollBarSliderMin = SCALE(40);
   dpi.SliderThickness = SCALE(24);
   dpi.SliderControl = SCALE(17);
   dpi.Indicator = SCALE(20);
   dpi.ExclusiveIndicator = SCALE(17);
}

#undef SCALE

/**THE STYLE ITSELF*/
OxygenStyle::OxygenStyle() : QCommonStyle(), progressShift(0), anmiationUpdate(false), mouseButtonPressed_(false), internalEvent_(false), _bgBrush(0L), popupPix(0L), timer(0L)
{
   _scanlines[0] = _scanlines[1] = _scanlines[2] = 0L;
   readSettings();
   initMetrics();
   generatePixmaps();
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
//    timer->start(50);
   connect(timer, SIGNAL(timeout()), this, SLOT(updateProgressbars()));
   connect(timer, SIGNAL(timeout()), this, SLOT(updateTabAnimation()));
}

OxygenStyle::~OxygenStyle()
{
   for (int i = 0; i < 2; i++)
      for (int j = 0; j < NumGrads; j++)
            gradients[i][j].clear();
   glowCache.clear();
   _btnAmbient.clear();
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

/**handles updates to the bg pixmap*/
void OxygenStyle::handleIPC(int /*id*/, int /*data*/)
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

const Tile::Set &OxygenStyle::glow(const QColor & c, bool round) const
{
   TileCache *cache = const_cast<TileCache*>(round ? &roundGlowCache : &glowCache);
   TileCache::const_iterator it = cache->find(c.rgb());
   if (it != cache->end())
      return it.value();
   
   // need to create a new one
   QPixmap *pix;
   Tile::Set frame;
   
   if (round)
   {
      pix = tint(QImage(":/round-glow"), c);
      frame = Tile::Set(*pix,10,10,7,6);
   }
   else
   {
      pix = tint(QImage(":/glow"), c);
      frame = Tile::Set(*pix,15,12,6,4);
   }

   delete pix;
   if (cache->size() == cache->capacity())
      cache->clear();
   it = cache->insert(c.rgb(), frame);
   return it.value();
}
/**=========================*/

/** render the everywhere used frames*/
//TODO: GET RID OF THIS!, frames now render themselves...
void OxygenStyle::renderFrame( QPainter *p, const QRect &r, Orientation3D o3D, Tile::PosFlags pf, const QWidget *widget, bool highlighted, bool round) const
{
#define HIGHCOLOR palette().color(QPalette::Active, QPalette::Highlight)
   //TODO:pass option->palette
   const Tile::Set *frame = highlighted ?
      &glow(widget ? widget->HIGHCOLOR : qApp->HIGHCOLOR, round) :
      round ? &frames.round[o3D] : &frames.rect[o3D];
#undef HIGHCOLOR
   frame->render(r, p, pf);
}

void OxygenStyle::fillWithMask(QPainter *painter, const QPoint &xy, const QPixmap &pix, const QPixmap &mask, QPoint offset) const
{
   QPixmap qPix(mask.size());
   QPainter p(&qPix);
   p.drawTiledPixmap(mask.rect(),pix,offset);
   p.end();
   qPix = OXRender::applyAlpha(qPix, mask);
   painter->drawPixmap(xy, qPix);
}

void OxygenStyle::fillWithMask(QPainter *painter, const QRect &rect, const QBrush &brush, const Tile::Mask *mask, Tile::PosFlags pf, bool justClip, QPoint offset) const
{
   bool pixmode = !brush.texture().isNull();
   if (!mask)
   {
      if (pixmode)
         painter->drawTiledPixmap(rect, brush.texture(), offset);
      else
         painter->fillRect(rect, brush.color());
      return;
   }
   if (justClip)
   {
      painter->save();
      painter->setClipRegion(mask->clipRegion(rect, pf));
      if (pixmode)
         painter->drawTiledPixmap(rect, brush.texture(), offset);
      else
         painter->fillRect(rect, brush.color());
      painter->restore();
      return;
   }
   // first the inner region
   //NOTE: using full alphablend can become enourmously slow due to VRAM size - even on HW that has Render acceleration!
   painter->save();
   painter->setClipRegion(mask->clipRegion(rect, pf), Qt::IntersectClip);
   if (pixmode)
      painter->drawTiledPixmap(rect, brush.texture(), offset);
   else
      painter->fillRect(rect, brush.color());
   painter->restore();
   if (!mask->hasCorners()) return;
   QPixmap corner, fill; QPainter p;
#define MAKE_CORNER(_CORNER_,_OFF_)\
   corner = mask->corner(_CORNER_);\
   fill = QPixmap(corner.size());\
   w = rect.width()/2;\
   w = qMin(corner.width(), ((_CORNER_) & Tile::Top) ? w : rect.width()-w);\
   h = rect.height()/2;\
   h = qMin(corner.height(), ((_CORNER_) & Tile::Top) ? h : rect.height()-h);\
   p.begin(&fill);\
   if (pixmode)\
      p.drawTiledPixmap(fill.rect(),brush.texture(),_OFF_+offset);\
   else\
      fill.fill(brush.color());\
   p.end();\
   corner = OXRender::applyAlpha(fill, corner)
   // the corners ========
   QPoint off(0,0); int w,h;
   // top/left
   if (Tile::matches(Tile::Top | Tile::Left, pf))
   {
      MAKE_CORNER(Tile::Top | Tile::Left, off);
      painter->drawPixmap(rect.topLeft(), corner, QRect(0,0,w,h));
   }
   // top/right
   off.setX(rect.width()-fill.width());
   if (Tile::matches(Tile::Top | Tile::Right, pf))
   {
      MAKE_CORNER(Tile::Top | Tile::Right, off);
      painter->drawPixmap(rect.right()-w+1, rect.top(), corner,
                          corner.width()-w,0, w,h );
   }
   // bottom/right
   off.setY(rect.height()-fill.height());
   if (Tile::matches(Tile::Bottom | Tile::Right, pf))
   {
      MAKE_CORNER(Tile::Bottom | Tile::Right, off);
      painter->drawPixmap(rect.right()-w+1, rect.bottom()-h+1, corner,
                          corner.width()-w,corner.height()-h, w,h );
   }
   // bottom/left
   off.setX(0);
   if (Tile::matches(Tile::Bottom | Tile::Left, pf))
   {
      MAKE_CORNER(Tile::Bottom | Tile::Left, off);
      painter->drawPixmap(rect.x(), rect.bottom()-h+1, corner,
                          0,corner.height()-h, w,h);
   }
   return;
#undef MAKE_CORNER
}

/**======================================*/

/**QStyle reimplementation ========================================= */

void OxygenStyle::polish ( QApplication * app )
{
//    if (timer && !timer->isActive())
//       timer->start(50);
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
   case VerticalGradient:
   case HorizontalGradient:
   {
      int h,s,v;
      // create use a nice background
      QColor c = pal.color(QPalette::Active, QPalette::Background);
      c.getHsv(&h,&s,&v);
      if (v < 70) // very dark colors won't make nice backgrounds ;)
         c.setHsv(h,s,70);
      originalBgColor_ = c;
      _SHIFTCOLOR_(c);
      pal.setColor( QPalette::Window, c );
      if (!_bgBrush)
      {
         if (config.bgMode == VerticalGradient)
            _bgBrush = new DynamicBrush(DynamicBrush::VerticalGradient, this);
         else
            _bgBrush = new DynamicBrush(DynamicBrush::HorizontalGradient, this);
      }
      break;
   }
   case Scanlines:
   {
      QColor c = pal.color(QPalette::Active, QPalette::Background);
      if (!_scanlines[0])
      {
#if 1 // bricks
         _scanlines[0] = new QPixmap(64, 64);
         QPainter p(_scanlines[0]);
         p.setPen(Qt::NoPen);
         QColor ch = c.light(102);
         p.setBrush(ch);
         p.drawRect(0,0,16,16);
         p.drawRect(48,0,16,16);
         p.drawRect(0,16,32,16);
         p.drawRect(0,32,16,16);
         p.drawRect(48,32,16,16);
         p.drawRect(0,48,32,16);
         ch = c.dark(102);
         p.setBrush(ch);
         p.drawRect(16,0,32,16);
         p.drawRect(0,16,16,16);
         p.drawRect(32,16,32,16);
         p.drawRect(16,32,32,16);
         p.drawRect(32,48,32,16);
         p.end();
#elif 0 //checkboard
         _scanlines[0] = new QPixmap(64, 64);
         QPainter p(_scanlines[0]);
         p.setPen(Qt::NoPen);
         QColor ch = c.light(102);
         p.setBrush(ch);
         p.drawRect(0,0,32,32);
         p.drawRect(32,32,32,32);
         ch = c.dark(102);
         p.setBrush(ch);
         p.drawRect(32,0,32,32);
         p.drawRect(0,32,32,32);
         p.end();
#elif 0 //scanlines
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
#endif
      }
      QBrush brush( c, *_scanlines[0] );
      pal.setBrush( QPalette::Background, brush );
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
   if (_bgBrush && !(qobject_cast<QMenu*>(widget) ||
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
      if (!timer->isActive()) timer->start(50);
      connect(widget, SIGNAL(destroyed(QObject*)), this, SLOT(progressbarDestroyed(QObject*)));
   }
   
   if (qobject_cast<QTabWidget*>(widget))
   {
      connect((QTabWidget*)widget, SIGNAL(currentChanged(int)), this, SLOT(tabChanged(int)));
      connect(widget, SIGNAL(destroyed(QObject*)), this, SLOT(tabDestroyed(QObject*)));
   }
   
//    if (widget->inherits("QStackedWidget") && _IsTabStack(widget))
//       widget->installEventFilter(this);
   
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
         pal.setBrush( QPalette::Window, brush );
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
//    case QEvent::Paint:
//       {
//          QWidget *sw = qobject_cast<QWidget*>(object);
//          if (!(sw && sw->inherits("QStackedWidget") && _IsTabStack(sw)))
//             return false;
//          QMap<QWidget*, TabAnimInfo>::const_iterator i = tabwidgets.find(sw->parentWidget());
//          if (i == tabwidgets.end() || !i.value().animStep)
//             return false;
//          QPainter p (sw);
//          p.drawPixmap(0,0, i.value().tabPix[2]);
//          p.end();
//          return true;
//       }
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
      if (qobject_cast<QTabWidget*>(object))
      {
         tabwidgets[(QTabWidget*)object] =
            new TabAnimInfo((QTabWidget*)object, ((QTabWidget*)object)->currentIndex());
         return false;
      }
      return false;
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

void OxygenStyle::unPolish( QApplication */*app */)
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
// cause of cmake
#include "oxygen.moc"

