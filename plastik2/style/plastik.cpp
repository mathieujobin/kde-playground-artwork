/* Plastik2 widget style for KDE 3
   Copyright (C) 2003-2005 Sandro Giessl <sandro@giessl.com>

   based on the KDE style "dotNET":

   Copyright (C) 2001-2002, Chris Lee <clee@kde.org>
                            Carsten Pfeiffer <pfeiffer@kde.org>
                            Karol Szwed <gallium@kde.org>
   Drawing routines completely reimplemented from KDE3 HighColor, which was
   originally based on some stuff from the KDE2 HighColor.

   based on drawing routines of the style "Keramik":

   Copyright (c) 2002 Malte Starostik <malte@kde.org>
             (c) 2002,2003 Maksim Orlovich <mo002j@mail.rochester.edu>
   based on the KDE3 HighColor Style
   Copyright (C) 2001-2002 Karol Szwed      <gallium@kde.org>
             (C) 2001-2002 Fredrik Höglund  <fredrik@kde.org>
   Drawing routines adapted from the KDE2 HCStyle,
   Copyright (C) 2000 Daniel M. Duley       <mosfet@kde.org>
             (C) 2000 Dirk Mueller          <mueller@kde.org>
             (C) 2001 Martijn Klingens      <klingens@kde.org>
   Progressbar code based on KStyle,
   Copyright (C) 2001-2002 Karol Szwed <gallium@kde.org>

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

#include <qimage.h>
#include <qstylefactory.h>
#include <qpointarray.h>
#include <qpainter.h>
#include <qtabbar.h>
#include <qprogressbar.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qcleanuphandler.h>
#include <qheader.h>
#include <qlineedit.h>
#include <qlistbox.h>
#include <qlistview.h>
#include <qscrollbar.h>
#include <qstyleplugin.h>
#include <qpushbutton.h>
#include <qtabwidget.h>
#include <qtimer.h>
#include <qtoolbutton.h>
#include <qtoolbar.h>
#include <qmenubar.h>
#include <qpopupmenu.h>
#include <qdrawutil.h>
#include <qapplication.h>
#include <qvariant.h>
#include <qradiobutton.h>
#include <qregion.h>
#include <qslider.h>
#include <qsettings.h>
#include <kpixmap.h>

#include "plastik.h"
#include "plastik.moc"

// some bit maps...
#define checkmark_width 9
#define checkmark_height 9
static unsigned char checkmark_bits[] = {
    0x00, 0x00, 0x80, 0x01, 0xc0, 0x01, 0xe3, 0x00, 0x77, 0x00, 0x3e, 0x00,
    0x1c, 0x00, 0x08, 0x00, 0x00, 0x00 };
#define tristatemark_width 9
#define tristatemark_height 9
static unsigned char tristatemark_bits[] = {
    0x00, 0x00, 0xff, 0x01, 0x00, 0x00, 0xff, 0x01, 0x00, 0x00, 0xff, 0x01,
    0x00, 0x00, 0xff, 0x01, 0x00, 0x00 };
#define radiomark_width 9
#define radiomark_height 9
static unsigned char radiomark_bits[] = {
    0x00, 0x00, 0x38, 0x00, 0x7c, 0x00, 0xfe, 0x00, 0xfe, 0x00, 0xfe, 0x00,
    0x7c, 0x00, 0x38, 0x00, 0x00, 0x00 };
#define radio_bg_width 15
#define radio_bg_height 15
static unsigned char radio_bg_bits[] = {
    0x00, 0x00, 0x00, 0x00, 0xe0, 0x03, 0xf0, 0x07, 0xf8, 0x0f, 0xfc, 0x1f,
    0xfc, 0x1f, 0xfc, 0x1f, 0xfc, 0x1f, 0xfc, 0x1f, 0xf8, 0x0f, 0xf0, 0x07,
    0xe0, 0x03, 0x00, 0x00, 0x00, 0x00 };
#define radio_contour_width 15
#define radio_contour_height 15
static unsigned char radio_contour_bits[] = {
    0x00, 0x00, 0xe0, 0x03, 0x10, 0x04, 0x08, 0x08, 0x04, 0x10, 0x02, 0x20,
    0x02, 0x20, 0x02, 0x20, 0x02, 0x20, 0x02, 0x20, 0x04, 0x10, 0x08, 0x08,
    0x10, 0x04, 0xe0, 0x03, 0x00, 0x00 };
#define radio_mask_width 15
#define radio_mask_height 15
static unsigned char radio_mask_bits[] = {
    0xe0, 0x03, 0xf0, 0x07, 0xf8, 0x0f, 0xfc, 0x1f, 0xfe, 0x3f, 0xff, 0x7f,
    0xff, 0x7f, 0xff, 0x7f, 0xff, 0x7f, 0xff, 0x7f, 0xfe, 0x3f, 0xfc, 0x1f,
    0xf8, 0x0f, 0xf0, 0x07, 0xe0, 0x03 };
#define radio_shade_bottom_width 15
#define radio_shade_bottom_height 15
static unsigned char radio_shade_bottom_bits[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40,
    0x00, 0x40, 0x00, 0x40, 0x00, 0x40, 0x00, 0x40, 0x02, 0x20, 0x04, 0x10,
    0x08, 0x08, 0x10, 0x04, 0xe0, 0x03 };
#define radio_shade_top_width 15
#define radio_shade_top_height 15
static unsigned char radio_shade_top_bits[] = {
    0xe0, 0x03, 0x10, 0x04, 0x08, 0x08, 0x04, 0x10, 0x02, 0x20, 0x01, 0x00,
    0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };


static const int cbButtonWidth = 15;
static const int cbFrameWidth = 2;
// popupmenu item constants...
static const int itemHMargin = 6;
static const int itemFrame = 2;
static const int arrowHMargin = 6;
static const int rightBorder = 12;

// -- Style Plugin Interface -------------------------
class Plastik2StylePlugin : public QStylePlugin
{
    public:
        Plastik2StylePlugin() {}
        ~Plastik2StylePlugin() {}

        QStringList keys() const {
            return QStringList() << "Plastik2";
        }

        QStyle* create( const QString& key ) {
            if (key.lower() == "plastik2")
                return new Plastik2Style;
            return 0;
        }
};

KDE_Q_EXPORT_PLUGIN( Plastik2StylePlugin )
// -- end --

Plastik2Style::Plastik2Style() : KStyle( AllowMenuTransparency, ThreeButtonScrollBar),
    kickerMode(false),
    kornMode(false),
    flatMode(false),
    defaultMode(false)
{
    hoverWidget = 0;

    horizontalDots = 0;
    verticalDots = 0;

    horizontalLine = 0;
    verticalLine = 0;

    QSettings settings;
    _contrast = settings.readNumEntry("/Qt/KDE/contrast", 6);
    settings.beginGroup("/plastik2style/Settings");
    _animateProgressBar = settings.readBoolEntry("/animateProgressBar", true);
    _drawToolBarSeparator = settings.readBoolEntry("/drawToolBarSeparator", true);
    _drawToolBarItemSeparator = settings.readBoolEntry("/drawToolBarItemSeparator", true);
    _drawFocusRect = settings.readBoolEntry("/drawFocusRect", true);
    _drawTriangularExpander = settings.readBoolEntry("/drawTriangularExpander", false);
    _inputFocusHighlight = settings.readBoolEntry("/inputFocusHighlight", true);
    _customOverHighlightColor = settings.readBoolEntry("/customOverHighlightColor", false);
    _overHighlightColor.setNamedColor( settings.readEntry("/overHighlightColor", "black") );
    _customFocusHighlightColor = settings.readBoolEntry("/customFocusHighlightColor", false);
    _focusHighlightColor.setNamedColor( settings.readEntry("/focusHighlightColor", "black") );
    _customCheckMarkColor = settings.readBoolEntry("/customCheckMarkColor", false);
    _checkMarkColor.setNamedColor( settings.readEntry("/checkMarkColor", "black") );
    settings.endGroup();

    // setup pixmap cache...
    pixmapCache = new QIntCache<CacheEntry>(150000, 499);
    pixmapCache->setAutoDelete(true);

    if ( _animateProgressBar )
    {
        animationTimer = new QTimer( this );
        connect( animationTimer, SIGNAL(timeout()), this, SLOT(updateProgressPos()) );
    }
}


void Plastik2Style::updateProgressPos()
{
    QProgressBar* pb;
    //Update the registered progressbars.
    QMap<QWidget*, int>::iterator iter;
    bool visible = false;
    for (iter = progAnimWidgets.begin(); iter != progAnimWidgets.end(); iter++)
    {   
        if ( !::qt_cast<QProgressBar*>(iter.key()) )
            continue;
        
        pb = dynamic_cast<QProgressBar*>(iter.key());
        if ( iter.key() -> isEnabled() && 
             pb -> progress() != pb->totalSteps() )
        {
            // update animation Offset of the current Widget
            iter.data() = (iter.data() + 1) % (2*pixelMetric(PM_ProgressBarChunkWidth, pb) );
            iter.key()->update();
        }
        if (iter.key()->isVisible())
            visible = true;
    }
    if (!visible)
        animationTimer->stop();
}


Plastik2Style::~Plastik2Style()
{
    delete pixmapCache;
    delete horizontalDots;
    delete verticalDots;
    delete horizontalLine;
    delete verticalLine;
}

void Plastik2Style::polish(QApplication* app)
{
    if (!qstrcmp(app->argv()[0], "kicker"))
        kickerMode = true;
    else if (!qstrcmp(app->argv()[0], "korn"))
        kornMode = true;
}

void Plastik2Style::polish(QWidget* widget)
{
    if( !strcmp(widget->name(), "__khtml") ) { // is it a khtml widget...?
        khtmlWidgets[widget] = true;
        connect(widget, SIGNAL(destroyed(QObject*)), this, SLOT(khtmlWidgetDestroyed(QObject*)));
    }

    // use qt_cast where possible to check if the widget inheits one of the classes. might improve
    // performance compared to QObject::inherits()
    if ( ::qt_cast<QPushButton*>(widget) || ::qt_cast<QComboBox*>(widget) ||
            ::qt_cast<QSpinWidget*>(widget) || ::qt_cast<QSlider*>(widget) ||
            ::qt_cast<QCheckBox*>(widget) || ::qt_cast<QRadioButton*>(widget) ||
            ::qt_cast<QToolButton*>(widget) || widget->inherits("QSplitterHandle") )
    {
//         widget->setBackgroundMode(PaletteBackground);
        widget->installEventFilter(this);
    } else if (::qt_cast<QLineEdit*>(widget)) {
        widget->installEventFilter(this);
    } else if (::qt_cast<QTabBar*>(widget)) {
        widget->setMouseTracking(true);
        widget->installEventFilter(this);
    } else if (::qt_cast<QPopupMenu*>(widget)) {
        widget->setBackgroundMode( NoBackground );
    } else if ( !qstrcmp(widget->name(), "kde toolbar widget") ) {
        widget->installEventFilter(this);
    }

    if( _animateProgressBar && ::qt_cast<QProgressBar*>(widget) )
    {
        widget->installEventFilter(this);
        progAnimWidgets[widget] = 0;
        connect(widget, SIGNAL(destroyed(QObject*)), this, SLOT(progressBarDestroyed(QObject*)));
        if (!animationTimer->isActive())
            animationTimer->start( 50, false );
    }

    KStyle::polish(widget);
}

void Plastik2Style::unPolish(QWidget* widget)
{
    if( !strcmp(widget->name(), "__khtml") ) { // is it a khtml widget...?
        khtmlWidgets.remove(widget);
    }

    // use qt_cast to check if the widget inheits one of the classes.
    if ( ::qt_cast<QPushButton*>(widget) || ::qt_cast<QComboBox*>(widget) ||
            ::qt_cast<QSpinWidget*>(widget) || ::qt_cast<QSlider*>(widget) ||
            ::qt_cast<QCheckBox*>(widget) || ::qt_cast<QRadioButton*>(widget) ||
            ::qt_cast<QToolButton*>(widget) || ::qt_cast<QLineEdit*>(widget) ||
            widget->inherits("QSplitterHandle") )
    {
        widget->removeEventFilter(this);
    }
    else if (::qt_cast<QTabBar*>(widget)) {
        widget->setMouseTracking(false);
        widget->removeEventFilter(this);
    } else if (::qt_cast<QPopupMenu*>(widget)) {
        widget->setBackgroundMode( PaletteBackground );
    } else if ( !qstrcmp(widget->name(), "kde toolbar widget") ) {
        widget->removeEventFilter(this);
    }

    if ( ::qt_cast<QProgressBar*>(widget) )
    {
        progAnimWidgets.remove(widget);
    }

    KStyle::unPolish(widget);
}

void Plastik2Style::khtmlWidgetDestroyed(QObject* obj)
{
    khtmlWidgets.remove(static_cast<QWidget*>(obj));
}

void Plastik2Style::progressBarDestroyed(QObject* obj)
{
    progAnimWidgets.remove(static_cast<QWidget*>(obj));
}

void Plastik2Style::renderShade(QPainter *p, const QRect &r, const QColor &background, const uint flags, const QColor &hl) const
{
    int r_w = r.width();
    int r_h = r.height();
    int r_x, r_y, r_x2, r_y2;
    r.coords(&r_x, &r_y, &r_x2, &r_y2);

    if((r.width() <= 0)||(r.height() <= 0))
        return;

    const bool drawLeft = flags&Draw_Left;
    const bool drawRight = flags&Draw_Right;
    const bool drawTop = flags&Draw_Top;
    const bool drawBottom = flags&Draw_Bottom;
    const bool roundUpperLeft = flags&Round_UpperLeft;
    const bool roundUpperRight = flags&Round_UpperRight;
    const bool roundBottomLeft = flags&Round_BottomLeft;
    const bool roundBottomRight = flags&Round_BottomRight;
    const bool fillEdges = flags&Fill_Edges;
    const bool sunken = flags&Is_Sunken;
    bool highlight = flags&Is_HighlightShade;
    if (highlight) {
        highlight = hl != Qt::color0;
    }

    if (flags&Draw_AlphaBlend) {
        QPixmap *light;
        QPixmap *dark;
        if (highlight) {
            light = dark = getAlphaPixel(hl, 80);
        } else {
            light = getAlphaPixel(Qt::white, 25);
            dark = getAlphaPixel(Qt::black, 20); // a bit less alpha as dark blended is much more visible than white
        }

        if (!light || !dark)
            return;

        QPixmap *pm = sunken ? dark : light;

        // top side, top edges
        if (drawTop) {
            int x1, x2; // x coordinates of the line

            if (drawLeft) {
                if (roundUpperLeft) {
                    p->drawPixmap(r_x+2, r_y+1, *pm); // edge
                    p->drawPixmap(r_x+1, r_y+2, *pm);
                    x1 = r_x+3;
                } else {
                    p->drawPixmap(r_x+1, r_y+1, *pm); // edge
                    x1 = r_x+2;
                }
            } else {
                x1 = r_x;
            }

            if (drawRight) {
                if (roundUpperRight) {
                    p->drawPixmap(r_x2-2, r_y+1, *pm); // edge
                    p->drawPixmap(r_x2-1, r_y+2, *pm);
                    x2 = r_x2-3;
                } else {
                    p->drawPixmap(r_x2-1, r_y+1, *pm); // edge
                    x2 = r_x2-2;
                }
            } else {
                x2 = r_x2;
            }

            p->drawTiledPixmap(x1, r_y, x2-x1+1, 1, *pm); // top line
        }

        // left side
        if (drawLeft) {
            int y1, y2; // y coordinates of the line

            if (drawTop) {
                if (roundUpperLeft) {
                    y1 = r_y+3;
                } else {
                    y1 = r_y+2;
                }
            } else {
                y1 = r_y;
            }

            if (drawBottom) {
                if (roundBottomLeft) {
                    y2 = r_y2-3;
                } else {
                    y2 = r_y2-2;
                }
            } else {
                y2 = r_y2;
            }

            p->drawTiledPixmap(r_x, y1, 1, y2-y1+1, *pm); // left line
        }


        pm = sunken ? light : dark;

        // bottom side, bottom edges
        if (drawBottom) {
            int x1, x2; // x coordinates of the line

            if (drawLeft) {
                if (roundBottomLeft) {
                    p->drawPixmap(r_x+2, r_y2-1, *pm); // edge
                    p->drawPixmap(r_x+1, r_y2-2, *pm);
                    x1 = r_x+3;
                } else {
                    p->drawPixmap(r_x+1, r_y2-1, *pm); // edge
                    x1 = r_x+2;
                }
            } else {
                x1 = r_x;
            }

            if (drawRight) {
                if (roundBottomRight) {
                    p->drawPixmap(r_x2-2, r_y2-1, *pm); // edge
                    p->drawPixmap(r_x2-1, r_y2-2, *pm);
                    x2 = r_x2-3;
                } else {
                    p->drawPixmap(r_x2-1, r_y2-1, *pm); // edge
                    x2 = r_x2-2;
                }
            } else {
                x2 = r_x2;
            }

            p->drawTiledPixmap(x1, r_y2, x2-x1+1, 1, *pm); // bottom line
        }

        // right side
        if (drawRight) {
            int y1, y2; // y coordinates of the line

            if (drawTop) {
                if (roundUpperRight) {
                    y1 = r_y+3;
                } else {
                    y1 = r_y+2;
                }
            } else {
                y1 = r_y;
            }

            if (drawBottom) {
                if (roundBottomRight) {
                    y2 = r_y2-3;
                } else {
                    y2 = r_y2-2;
                }
            } else {
                y2 = r_y2;
            }

            p->drawTiledPixmap(r_x2, y1, 1, y2-y1+1, *pm); // right line
        }

    } else {

        if (highlight) {
            p->setPen (alphaBlendColors(sunken ? background.dark(105+_contrast) : background.light(105+_contrast), hl, 150 ) );
        } else {
            p->setPen (sunken ? background.dark(105+_contrast) : background.light(105+_contrast) );
        }

        // top side, top edges
        if (drawTop) {
            int x1, x2; // x coordinates of the line

            if (drawLeft) {
                if (roundUpperLeft) {
                    p->drawPoint(r_x+2, r_y+1); // edge
                    p->drawPoint(r_x+1, r_y+2);
                    x1 = r_x+3;
                } else {
                    p->drawPoint(r_x+1, r_y+1); // edge
                    x1 = r_x+2;
                }
            } else {
                x1 = r_x;
            }

            if (drawRight) {
                if (roundUpperRight) {
                    p->drawPoint(r_x2-2, r_y+1); // edge
                    p->drawPoint(r_x2-1, r_y+2);
                    x2 = r_x2-3;
                } else {
                    p->drawPoint(r_x2-1, r_y+1); // edge
                    x2 = r_x2-2;
                }
            } else {
                x2 = r_x2;
            }

            p->drawLine(x1, r_y, x2, r_y);
        }

        // left side
        if (drawLeft) {
            int y1, y2; // y coordinates of the line

            if (drawTop) {
                if (roundUpperLeft) {
                    y1 = r_y+3;
                } else {
                    y1 = r_y+2;
                }
            } else {
                y1 = r_y;
            }

            if (drawBottom) {
                if (roundBottomLeft) {
                    y2 = r_y2-3;
                } else {
                    y2 = r_y2-2;
                }
            } else {
                y2 = r_y2;
            }

            p->drawLine(r_x, y1, r_x, y2);
        }


        if (highlight) {
            p->setPen (alphaBlendColors(sunken ? background.light(105+_contrast) : background.dark(105+_contrast), hl, 150 ) );
        } else {
            p->setPen (sunken ? background.light(105+_contrast) : background.dark(105+_contrast) );
        }

        // bottom side, bottom edges
        if (drawBottom) {
            int x1, x2; // x coordinates of the line

            if (drawLeft) {
                if (roundBottomLeft) {
                    p->drawPoint(r_x+2, r_y2-1); // edge
                    p->drawPoint(r_x+1, r_y2-2);
                    x1 = r_x+3;
                } else {
                    p->drawPoint(r_x+1, r_y2-1); // edge
                    x1 = r_x+2;
                }
            } else {
                x1 = r_x;
            }

            if (drawRight) {
                if (roundBottomRight) {
                    p->drawPoint(r_x2-2, r_y2-1); // edge
                    p->drawPoint(r_x2-1, r_y2-2);
                    x2 = r_x2-3;
                } else {
                    p->drawPoint(r_x2-1, r_y2-1); // edge
                    x2 = r_x2-2;
                }
            } else {
                x2 = r_x2;
            }

            p->drawLine(x1, r_y2, x2, r_y2);
        }

        // right side
        if (drawRight) {
            int y1, y2; // y coordinates of the line

            if (drawTop) {
                if (roundUpperRight) {
                    y1 = r_y+3;
                } else {
                    y1 = r_y+2;
                }
            } else {
                y1 = r_y;
            }

            if (drawBottom) {
                if (roundBottomRight) {
                    y2 = r_y2-3;
                } else {
                    y2 = r_y2-2;
                }
            } else {
                y2 = r_y2;
            }

            p->drawLine(r_x2, y1, r_x2, y2);
        }


        // fill all the edges if requested...
        if (fillEdges) {
            p->setPen(background);

            if (drawLeft && drawTop) {
                if (roundUpperLeft) {
                    p->drawLine(r_x, r_y, r_x+2, r_y);
                    p->drawLine(r_x, r_y+1, r_x+1, r_y+1);
                    p->drawPoint(r_x, r_y+2);
                } else {
                    p->drawLine(r_x, r_y, r_x+1, r_y);
                    p->drawPoint(r_x, r_y+1);
                }
            }

            if (drawRight && drawTop) {
                if (roundUpperRight) {
                    p->drawLine(r_x2-2, r_y, r_x2, r_y);
                    p->drawLine(r_x2-1, r_y+1, r_x2, r_y+1);
                    p->drawPoint(r_x2, r_y+2);
                } else {
                    p->drawLine(r_x2-1, r_y, r_x2, r_y);
                    p->drawPoint(r_x2, r_y+1);
                }
            }

            if (drawLeft && drawBottom) {
                if (roundBottomLeft) {
                    p->drawLine(r_x, r_y2, r_x+2, r_y2);
                    p->drawLine(r_x, r_y2-1, r_x+1, r_y2-1);
                    p->drawPoint(r_x, r_y2-2);
                } else {
                    p->drawLine(r_x, r_y2, r_x+1, r_y2);
                    p->drawPoint(r_x, r_y2-1);
                }
            }

            if (drawRight && drawBottom) {
                if (roundBottomRight) {
                    p->drawLine(r_x2-2, r_y2, r_x2, r_y2);
                    p->drawLine(r_x2-1, r_y2-1, r_x2, r_y2-1);
                    p->drawPoint(r_x2, r_y2-2);
                } else {
                    p->drawLine(r_x2-1, r_y2, r_x2, r_y2);
                    p->drawPoint(r_x2, r_y2-1);
                }
            }

        }

    }
}

void Plastik2Style::renderContour(QPainter *p, const QRect &r, const QColor &background, const QColor &contour, const uint flags) const
{
    int r_w = r.width();
    int r_h = r.height();
    int r_x, r_y, r_x2, r_y2;
    r.coords(&r_x, &r_y, &r_x2, &r_y2);

    if((r_w <= 0)||(r_h <= 0))
        return;

    const bool drawLeft = flags&Draw_Left;
    const bool drawRight = flags&Draw_Right;
    const bool drawTop = flags&Draw_Top;
    const bool drawBottom = flags&Draw_Bottom;
    const bool roundUpperLeft = flags&Round_UpperLeft;
    const bool roundUpperRight = flags&Round_UpperRight;
    const bool roundBottomLeft = flags&Round_BottomLeft;
    const bool roundBottomRight = flags&Round_BottomRight;
    const bool disabled = flags&Is_Disabled;
    const bool alphaBlend = flags&Draw_AlphaBlend;

    QColor color;
    if (disabled) {
        color = background.dark(150);
    } else {
        color = contour;
    }

    if (flags&Is_Highlight) {
        color = color.light(140);
    }

    p->setPen(color);

    // edges + left and right sides
    if (drawLeft) {
        int y1, y2; // y coordinates of the line

        if (drawTop) {
            if (roundUpperLeft) {
                p->drawPoint(r_x+1, r_y+1); // edge
                y1 = r_y+2;
            } else {
                y1 = r_y+1;
            }
        } else {
            y1 = r_y;
        }

        if (drawBottom) {
            if (roundBottomLeft) {
                p->drawPoint(r_x+1, r_y2-1); // edge
                y2 = r_y2-2;
            } else {
                y2 = r_y2-1;
            }
        } else {
            y2 = r_y2;
        }

        p->drawLine(r_x, y1, r_x, y2);
    }

    if (drawRight) {
        int y1, y2; // y coordinates of the line

        if (drawTop) {
            if (roundUpperRight) {
                p->drawPoint(r_x2-1, r_y+1); // edge
                y1 = r_y+2;
            } else {
                y1 = r_y+1;
            }
        } else {
            y1 = r_y;
        }

        if (drawBottom) {
            if (roundBottomRight) {
                p->drawPoint(r_x2-1, r_y2-1); // edge
                y2 = r_y2-2;
            } else {
                y2 = r_y2-1;
            }
        } else {
            y2 = r_y2;
        }

        p->drawLine(r_x2, y1, r_x2, y2);
    }

    // top + bottom sides
    if (drawTop) {
        int x1, x2; // x coordinates of the line

        if (drawLeft) {
            if (roundUpperLeft) {
                x1 = r_x+2;
            } else {
                x1 = r_x+1;
            }
        } else {
            x1 = r_x;
        }

        if (drawRight) {
            if (roundUpperRight) {
                x2 = r_x2-2;
            } else {
                x2 = r_x2-1;
            }
        } else {
            x2 = r_x2;
        }

        p->drawLine(x1, r_y, x2, r_y);
    }

    if (drawBottom) {
        int x1, x2; // x coordinates of the line

        if (drawLeft) {
            if (roundBottomLeft) {
                x1 = r_x+2;
            } else {
                x1 = r_x+1;
            }
        } else {
            x1 = r_x;
        }

        if (drawRight) {
            if (roundBottomRight) {
                x2 = r_x2-2;
            } else {
                x2 = r_x2-1;
            }
        } else {
            x2 = r_x2;
        }

        p->drawLine(x1, r_y2, x2, r_y2);
    }
}

void Plastik2Style::renderSurface(QPainter *p, const QRect &r, const QColor &background, const QColor &button,
                                   const QColor &highlight, int intensity, const uint flags) const
{
    int r_w = r.width();
    int r_h = r.height();
    int r_x, r_y, r_x2, r_y2;
    r.coords(&r_x, &r_y, &r_x2, &r_y2);

    if((r_w <= 0)||(r_h <= 0))
        return;

    const bool disabled = flags&Is_Disabled;

    const bool drawLeft = flags&Draw_Left;
    const bool drawRight = flags&Draw_Right;
    const bool drawTop = flags&Draw_Top;
    const bool drawBottom = flags&Draw_Bottom;
    const bool roundUpperLeft = flags&Round_UpperLeft;
    const bool roundUpperRight = flags&Round_UpperRight;
    const bool roundBottomLeft = flags&Round_BottomLeft;
    const bool roundBottomRight = flags&Round_BottomRight;
    const bool sunken = flags&Is_Sunken;
    const bool horizontal = flags&Is_Horizontal;

    QColor color = disabled?background:button;

    if (flags&Is_Highlight) {
        color = color.light(120+intensity*2);
    }

    if (disabled)
        intensity = intensity/2;

// button area...
    int width = r_w;
    int height = r_h;
    if (drawLeft) width--;
    if (drawRight) width--;
    if (drawTop) height--;
    if (drawBottom) height--;
    renderGradient(p, QRect(drawLeft?r_x+1:r_x, drawTop?r_y+1:r_y, width, height),
                   sunken?color.dark(100+intensity):color.light(100+intensity),
                   sunken?color.light(100+intensity):color.dark(100+intensity),
                   horizontal);

// sides
    p->setPen(sunken?color.dark(110+intensity*2):color.light(110+intensity*2) );

    if (drawLeft) {
        p->drawLine(r_x, (roundUpperLeft&&drawTop)?r_y+1:r_y,
                    r_x, (roundBottomLeft&&drawBottom)?r_y2-1:r_y2);
    }
    if (drawRight) {
        p->drawLine(r_x2, (roundUpperRight&&drawTop)?r_y+1:r_y,
                    r_x2, (roundBottomRight&&drawBottom)?r_y2-1:r_y2);
    }
    if (drawTop) {
        p->drawLine((roundUpperLeft&&drawLeft)?r_x+1:r_x, r_y,
                        (roundUpperRight&&drawRight)?r_x2-1:r_x2, r_y);
    }
    if (drawBottom) {
        p->drawLine((roundBottomLeft&&drawLeft)?r_x+1:r_x, r_y2,
                        (roundBottomRight&&drawRight)?r_x2-1:r_x2, r_y2 );
    }

}

void Plastik2Style::renderMask(QPainter *p,
                              const QRect &r,
                              const QColor &color,
                              const uint flags) const
{
    // TODO: renderMask() needs an update.

    if((r.width() <= 0)||(r.height() <= 0))
        return;

    const bool roundUpperLeft = flags&Round_UpperLeft;
    const bool roundUpperRight = flags&Round_UpperRight;
    const bool roundBottomLeft = flags&Round_BottomLeft;
    const bool roundBottomRight = flags&Round_BottomRight;


    p->fillRect (QRect(r.x()+1, r.y()+1, r.width()-2, r.height()-2) , color);

    p->setPen(color);
    // sides
    p->drawLine(roundUpperLeft?r.x()+1:r.x(), r.y(),
                roundUpperRight?r.right()-1:r.right(), r.y() );
    p->drawLine(roundBottomLeft?r.x()+1:r.x(), r.bottom(),
                roundBottomRight?r.right()-1:r.right(), r.bottom() );
    p->drawLine(r.x(), roundUpperLeft?r.y()+1:r.y(),
                r.x(), roundBottomLeft?r.bottom()-1:r.bottom() );
    p->drawLine(r.right(), roundUpperLeft?r.y()+1:r.y(),
                r.right(), roundBottomLeft?r.bottom()-1:r.bottom() );
}

inline QPixmap *Plastik2Style::getAlphaPixel(const QColor &color, int alpha) const
{
    QRgb rgb = color.rgb();
    // generate a quite unique key -- use the unused width field to store the alpha value.
    CacheEntry search(cAlphaDot, alpha, 0, rgb);
    int key = search.key();

    CacheEntry *cacheEntry;
    if( (cacheEntry = pixmapCache->find(key)) ) {
        if( search == *cacheEntry ) { // match! we can draw now...
            if(cacheEntry->pixmap)
                return cacheEntry->pixmap;
        } else { //Remove old entry in case of a conflict!
            pixmapCache->remove( key );
        }
    }


    QImage aImg(1,1,32); // 1x1
    aImg.setAlphaBuffer(true);
    aImg.setPixel(0,0,qRgba(qRed(rgb),qGreen(rgb),qBlue(rgb),alpha));
    QPixmap *result = new QPixmap(aImg);

    // add to the cache...
    CacheEntry *toAdd = new CacheEntry(search);
    toAdd->pixmap = result;
    bool insertOk = pixmapCache->insert( key, toAdd, result->depth()/8);
    if(!insertOk) {
        delete result;
        return 0;
    } else {
        return result;
    }
}

void Plastik2Style::renderButton(QPainter *p,
                               const QRect &r,
                               const QColorGroup &g,
                               bool sunken,
                               bool mouseOver,
                               bool horizontal,
                               bool enabled,
                               bool khtmlMode) const
{
    int r_w = r.width();
    int r_h = r.height();
    int r_x, r_y, r_x2, r_y2;
    r.coords(&r_x, &r_y, &r_x2, &r_y2);

    // small fix for the kicker buttons...
    if(kickerMode) enabled = true;

    const QPen oldPen( p->pen() );

    uint contourFlags = Draw_Left|Draw_Right|Draw_Top|Draw_Bottom;
    if(!enabled) contourFlags|=Is_Disabled;
    if(khtmlMode) contourFlags|=Draw_AlphaBlend;

    uint surfaceFlags = Draw_Left|Draw_Right|Draw_Top|Draw_Bottom;
    if(horizontal) surfaceFlags|=Is_Horizontal;
    if(!enabled) surfaceFlags|=Is_Disabled;
    else {
        if(sunken) surfaceFlags|=Is_Sunken;
        else {
            if(mouseOver) {
                surfaceFlags|=Is_Highlight;
                contourFlags|=Is_Highlight;
            }
        }
    }

    if (flatMode) {
        renderContour(p, r, g.background(), g.button().dark(105+_contrast*3),
                      contourFlags);
        renderSurface(p, SHRINK_RECT(r, 1)/*QRect(r.left()+1, r.top()+1, r.width()-2, r.height()-2)*/,
                      g.background(), g.button(), getColor(g,MouseOverHighlight), _contrast/2, surfaceFlags);

        flatMode = false;
    } else {
        contourFlags |= Round_UpperLeft|Round_UpperRight|Round_BottomLeft|Round_BottomRight;
        surfaceFlags |= Round_UpperLeft|Round_UpperRight|Round_BottomLeft|Round_BottomRight;

        if (defaultMode) {
            contourFlags |= Is_Default;
            surfaceFlags |= Is_Default;
        }

        renderShade(p, r, g.background(), contourFlags|Is_Sunken|Fill_Edges);
        renderContour(p, SHRINK_RECT(r, 1),
                       g.background(), getColor(g,defaultMode?DefaultButtonContour:ButtonContour), contourFlags);
        renderSurface(p, SHRINK_RECT(r, 2),
                       g.background(), g.button(), getColor(g,MouseOverHighlight), _contrast, surfaceFlags);

        if (defaultMode)
            defaultMode = false;
    }

    p->setPen(oldPen);
}

void Plastik2Style::renderDot(QPainter *p, int x, int y, const QColor &c, const bool sunken) const
{
    p->setPen (c.dark(120+_contrast*6) );
    p->drawPoint(x, y);

    p->setPen (sunken ? c.dark(105+_contrast) : c.light(105+_contrast) );
    p->drawLine(x-1, y-1, x, y-1);
    p->drawPoint(x-1, y);

    p->setPen (sunken ? c.light(105+_contrast) : c.dark(105+_contrast) );
    p->drawLine(x, y+1, x+1, y+1);
    p->drawPoint(x+1, y);
}

void Plastik2Style::renderGradient(QPainter *painter,
                                  const QRect &rect,
                                  const QColor &c1,
                                  const QColor &c2,
                                  bool horizontal) const
{
    if((rect.width() <= 0)||(rect.height() <= 0))
        return;

    // generate a quite unique key for this surface.
    CacheEntry search(cGradientTile,
                      horizontal ? 0 : rect.width(),
                      horizontal ? rect.height() : 0,
                      c1.rgb(), c2.rgb(), horizontal );
    int key = search.key();

    CacheEntry *cacheEntry;
    if( (cacheEntry = pixmapCache->find(key)) ) {
        if( search == *cacheEntry ) { // match! we can draw now...
            if(cacheEntry->pixmap) {
                painter->drawTiledPixmap(rect, *(cacheEntry->pixmap) );
            }
            return;
        } else {
            // Remove old entry in case of a conflict!
            // This shouldn't happen very often, see comment in CacheEntry.
            pixmapCache->remove(key);
        }
    }

    // there wasn't anything matching in the cache, create the pixmap now...
    QPixmap *result = new QPixmap(horizontal ? 10 : rect.width(),
                                  horizontal ? rect.height() : 10);
    QPainter p(result);

    int r_w = result->rect().width();
    int r_h = result->rect().height();
    int r_x, r_y, r_x2, r_y2;
    result->rect().coords(&r_x, &r_y, &r_x2, &r_y2);

    int rDiff, gDiff, bDiff;
    int rc, gc, bc;

    register int x, y;

    rDiff = ( c2.red())   - (rc = c1.red());
    gDiff = ( c2.green()) - (gc = c1.green());
    bDiff = ( c2.blue())  - (bc = c1.blue());

    register int rl = rc << 16;
    register int gl = gc << 16;
    register int bl = bc << 16;

    int rdelta = ((1<<16) / (horizontal ? r_h : r_w)) * rDiff;
    int gdelta = ((1<<16) / (horizontal ? r_h : r_w)) * gDiff;
    int bdelta = ((1<<16) / (horizontal ? r_h : r_w)) * bDiff;

    // these for-loops could be merged, but the if's in the inner loop
    // would make it slow
    if(horizontal) {
        for ( y = 0; y < r_h; y++ ) {
            rl += rdelta;
            gl += gdelta;
            bl += bdelta;

            p.setPen(QColor(rl>>16, gl>>16, bl>>16));
            p.drawLine(r_x, r_y+y, r_x2, r_y+y);
        }
    } else {
        for( x = 0; x < r_w; x++) {
            rl += rdelta;
            gl += gdelta;
            bl += bdelta;

            p.setPen(QColor(rl>>16, gl>>16, bl>>16));
            p.drawLine(r_x+x, r_y, r_x+x, r_y2);
        }
    }

    p.end();

    // draw the result...
    painter->drawTiledPixmap(rect, *result);

    // insert into cache using the previously created key.
    CacheEntry *toAdd = new CacheEntry(search);
    toAdd->pixmap = result;
    bool insertOk = pixmapCache->insert( key, toAdd, result->width()*result->height()*result->depth()/8 );

    if(!insertOk)
        delete result;
}

void Plastik2Style::renderPanel(QPainter *p,
                              const QRect &r,
                              const QColorGroup &g,
                              const bool pseudo3d,
                              const bool sunken) const
{
    int x, x2, y, y2, w, h;
    r.rect(&x,&y,&w,&h);
    r.coords(&x, &y, &x2, &y2);

    if (kickerMode &&
            p->device() && p->device()->devType() == QInternal::Widget &&
            QCString(static_cast<QWidget*>(p->device())->className()) == "FittsLawFrame") {
        // TODO: have a look at this case
    //  Stolen wholesale from Keramik. I don't like it, but oh well.
        if (sunken) {
            const QCOORD corners[] = { x2, y, x2, y2, x, y2, x, y };
            p->setPen(g.background().dark());
            p->drawConvexPolygon(QPointArray(4, corners));
            p->setPen(g.background().light());
            p->drawPolyline(QPointArray(4, corners), 0, 3);
        } else {
            const QCOORD corners[] = { x, y2, x, y, x2, y, x2, y2 };
            p->setPen(g.background().dark());
            p->drawPolygon(QPointArray(4, corners));
            p->setPen(g.background().light());
            p->drawPolyline(QPointArray(4, corners), 0, 3);
        }
    } else {
        if (pseudo3d) {
            uint shadeFlags = Draw_Left|Draw_Right|Draw_Top|Draw_Bottom;
            if (sunken)
                shadeFlags |= Is_Sunken;
            renderShade (p, r, g.background(), shadeFlags);
        }

        p->setPen( getColor(g, PanelContour) );
        p->drawLine(r.left()+2, r.top()+1, r.right()-2, r.top()+1);
        p->drawLine(r.left()+1, r.top()+2, r.left()+1, r.bottom()-2);
        p->drawLine(r.left()+2, r.bottom()-1, r.right()-2, r.bottom()-1);
        p->drawLine(r.right()-1, r.top()+2, r.right()-1, r.bottom()-2);
    }
}

void Plastik2Style::renderMenuBlendPixmap( KPixmap &pix, const QColorGroup &cg,
    const QPopupMenu* /* popup */ ) const
{
    pix.fill( cg.background().light(105) );
}

void Plastik2Style::drawKStylePrimitive(KStylePrimitive kpe,
                                      QPainter *p,
                                      const QWidget* widget,
                                      const QRect &r,
                                      const QColorGroup &cg,
                                      SFlags flags,
                                      const QStyleOption& opt) const
{
    // some "global" vars...
    const bool enabled = (flags & Style_Enabled);

    switch( kpe ) {
        case KPE_SliderGroove:
        case KPE_SliderHandle: {
            const int thickness = pixelMetric(PM_SliderThickness, widget)-2;
            const int length = pixelMetric(PM_SliderLength, widget);
            const int xcenter = (r.left()+r.right()) / 2;
            const int ycenter = (r.top()+r.bottom()) / 2;

            const QSlider* slider = (const QSlider*)widget;
            bool horizontal = slider->orientation() == Horizontal;

            QRect sr = r;

            if (kpe == KPE_SliderGroove) {
                QRect groove;
                if (horizontal) {
                    sr.setRect(r.x()+1, ycenter - thickness/2, r.width()-2, thickness);
                } else {
                    sr.setRect(xcenter - thickness/2, r.y()+1, thickness, r.height()-2 );
                }

                renderShade(p, sr, cg.background(), Draw_Left|Draw_Right|Draw_Top|Draw_Bottom|Is_Sunken);
            } else {
            // KPE_SliderHandle
                const QSlider* slider = (const QSlider*)widget;
                bool horizontal = slider->orientation() == Horizontal;

                const bool pressed = (flags&Style_Active);
                const WidgetState s = enabled?(pressed?IsPressed:IsEnabled):IsDisabled;
                const QColor contour = getColor(cg,DragButtonContour,s),
                             surface = getColor(cg,DragButtonSurface,s);

                if (horizontal) {
                    sr.setRect(xcenter - length/2, ycenter - thickness/2, length, thickness);
                } else {
                    sr.setRect(xcenter - thickness/2, ycenter - length/2, thickness, length );
                }

                int cx = sr.x() + sr.width()/2;
                int cy = sr.y() + sr.height()/2;

                if (horizontal) {
                    renderContour(p, SHRINK_RECT(sr, 1),
                                   cg.background(), contour, Draw_Left|Draw_Right|Draw_Top|Draw_Bottom);
                    renderSurface(p, SHRINK_RECT(sr, 2),
                                  cg.background(), surface, getColor(cg,MouseOverHighlight), _contrast,
                                  Draw_Left|Draw_Right|Draw_Top|Draw_Bottom|Is_Horizontal);


                    renderDot(p, cx, cy-2, surface, true );
                    renderDot(p, cx, cy+2, surface, true );

                } else {
                    renderContour(p, SHRINK_RECT(sr, 1),
                                   cg.background(), contour, Draw_Left|Draw_Right|Draw_Top|Draw_Bottom);
                    renderSurface(p, SHRINK_RECT(sr, 2),
                                  cg.background(), surface, getColor(cg,MouseOverHighlight), _contrast,
                                  Draw_Left|Draw_Right|Draw_Top|Draw_Bottom);

                    renderDot(p, cx-2, cy, surface, true );
                    renderDot(p, cx+2, cy, surface, true );
                }
            }

            break;
        }

        case KPE_ListViewExpander: {
            int radius = (r.width() - 4) / 2;
            int centerx = r.x() + r.width()/2;
            int centery = r.y() + r.height()/2;

            renderContour(p, r, cg.base(), cg.dark(), Draw_Left|Draw_Right|Draw_Top|Draw_Bottom|Round_UpperLeft|Round_UpperRight|Round_BottomLeft|Round_BottomRight );

            p->setPen( cg.text() );
            if(!_drawTriangularExpander)
            {
                // plus or minus
                p->drawLine( centerx - radius, centery, centerx + radius, centery );
                if ( flags & Style_On ) // Collapsed = On
                    p->drawLine( centerx, centery - radius, centerx, centery + radius );
            } else if(_drawTriangularExpander) {
              if( flags & Style_On )
                  drawPrimitive(PE_ArrowRight, p, MODIFY_RECT(r, +1, +1, 0, 0), cg,ButtonContour, flags);
              if( flags & Style_Off )
                  drawPrimitive(PE_ArrowDown, p, MODIFY_RECT(r, +1, +1, 0, 0), cg,ButtonContour, flags);
            }

            break;
        }

    // copied and slightly modified from KStyle.
    case KPE_ListViewBranch: {
        // Typical Windows style listview branch element (dotted line).

        // Create the dotline pixmaps if not already created
        if ( !verticalLine )
        {
            // make 128*1 and 1*128 bitmaps that can be used for
            // drawing the right sort of lines.
            verticalLine   = new QBitmap( 1, 129, true );
            horizontalLine = new QBitmap( 128, 1, true );
            QPointArray a( 64 );
            QPainter p2;
            p2.begin( verticalLine );

            int i;
            for( i=0; i < 64; i++ )
                a.setPoint( i, 0, i*2+1 );
            p2.setPen( color1 );
            p2.drawPoints( a );
            p2.end();
            QApplication::flushX();
            verticalLine->setMask( *verticalLine );

            p2.begin( horizontalLine );
            for( i=0; i < 64; i++ )
                a.setPoint( i, i*2+1, 0 );
            p2.setPen( color1 );
            p2.drawPoints( a );
            p2.end();
            QApplication::flushX();
            horizontalLine->setMask( *horizontalLine );
        }

        p->setPen( cg.mid() );

        if (flags & Style_Horizontal)
        {
            int point = r.x();
            int other = r.y();
            int end = r.x()+r.width();
            int thickness = r.height();

            while( point < end )
            {
                int i = 128;
                if ( i+point > end )
                    i = end-point;
                p->drawPixmap( point, other, *horizontalLine, 0, 0, i, thickness );
                point += i;
            }

        } else {
            int point = r.y();
            int other = r.x();
            int end = r.y()+r.height();
            int thickness = r.width();
            int pixmapoffset = (flags & Style_NoChange) ? 0 : 1;	// ### Hackish

            while( point < end )
            {
                int i = 128;
                if ( i+point > end )
                    i = end-point;
                p->drawPixmap( other, point, *verticalLine, 0, pixmapoffset, thickness, i );
                point += i;
            }
        }

        break;
    }

        default:
            KStyle::drawKStylePrimitive(kpe, p, widget, r, cg, flags, opt);
    }
}


void Plastik2Style::drawPrimitive(PrimitiveElement pe,
                                QPainter *p,
                                const QRect &r,
                                const QColorGroup &cg,
                                SFlags flags,
                                const QStyleOption &opt ) const
{
    bool down   = flags & Style_Down;
    bool on     = flags & Style_On;
    bool sunken = flags & Style_Sunken;
    bool horiz  = flags & Style_Horizontal;
    const bool enabled = flags & Style_Enabled;
    const bool mouseOver = flags & Style_MouseOver;

    bool hasFocus = flags & Style_HasFocus;

    int r_w = r.width();
    int r_h = r.height();
    int r_x, r_y, r_x2, r_y2;
    r.coords(&r_x, &r_y, &r_x2, &r_y2);

    switch(pe) {

        case PE_FocusRect: {
            if(_drawFocusRect)
                p->drawWinFocusRect( r );
            break;
        }

        case PE_HeaderSection: {
            // the taskbar buttons seems to be painted with PE_HeaderSection but I
            // want them look like normal buttons - so fall through in kickerMode
            if(!kickerMode) {
                // render header...
//                 if(kickerMode) enabled = true;
                const bool horizontal = true;
                const bool sunken = (on||down); // overwrite sunken...

                // detect if this is the right most header item
                bool rightAlignedSection = false;
                QHeader *header = dynamic_cast<QHeader*>(p->device() );
                if (header) {
                    bool resizeFull = false;
                    QListView *lv = dynamic_cast<QListView*>(header->parent() );
                    if (lv)
                        resizeFull = lv->resizeMode() == QListView::AllColumns ||
                                lv->resizeMode() == QListView::LastColumn;
                    if (resizeFull)
                        rightAlignedSection = header->mapToIndex(header->sectionAt(r_x) )==(header->count()-1);
                }

                uint contourFlags = Draw_Bottom;
                if(!enabled) contourFlags|=Is_Disabled;
                if (!rightAlignedSection)
                    contourFlags |= Draw_Right;

                uint surfaceFlags = Draw_Left|Draw_Right|Draw_Top|Draw_Bottom;
                if(horizontal) surfaceFlags|=Is_Horizontal;
                if(!enabled) surfaceFlags|=Is_Disabled;
                else {
                    if(sunken) surfaceFlags|=Is_Sunken;
                    else {
                        if(mouseOver) {
                            surfaceFlags|=Is_Highlight;
                            if(horizontal) {
                                surfaceFlags|=Highlight_Top;
                                surfaceFlags|=Highlight_Bottom;
                            } else {
                                surfaceFlags|=Highlight_Left;
                                surfaceFlags|=Highlight_Right;
                            }
                        }
                    }
                }

                // if it's the right most header item, don't draw the right contour.
                renderContour(p, r,
                              cg.background(), getColor(cg,ButtonContour), contourFlags);
                renderSurface(p, MODIFY_RECT(r, 0, 0, (rightAlignedSection?0:-1), -1),
                              cg.background(), cg.button(), getColor(cg,MouseOverHighlight), _contrast, surfaceFlags);

                break;
            }
        } // fall through in kickerMode intended
        case PE_ButtonBevel:
        case PE_ButtonTool:
        case PE_ButtonDropDown:
        case PE_ButtonCommand: {
            bool khtmlMode = khtmlWidgets.contains(opt.widget());
            renderButton(p, r, cg, (on||down), mouseOver, true, enabled, khtmlMode );
            break;
        }

        case PE_ButtonDefault: {
            // this is not used by normal pushbuttons, but in case some app or so relies on PE_ButtonDefault...
            uint contourFlags = Draw_Left|Draw_Right|Draw_Top|Draw_Bottom|
                    Round_UpperLeft|Round_UpperRight|Round_BottomLeft|Round_BottomRight;
            if(!enabled) contourFlags|=Is_Disabled;
            renderContour(p, r, cg.background(), getColor(cg, ButtonContour), contourFlags);
            break;
        }

        case PE_SpinWidgetPlus:
        case PE_SpinWidgetMinus: {
            p->setPen( cg.buttonText() );

            int l = QMIN( r_w-2, r_h-2 );
            // make the length even so that we get a nice symmetric plus...
            if(l%2 != 0)
                --l;
            QPoint c = r.center();

            p->drawLine( c.x()-l/2, c.y(), c.x()+l/2, c.y() );
            if ( pe == PE_SpinWidgetPlus ) {
                p->drawLine( c.x(), c.y()-l/2, c.x(), c.y()+l/2 );
            }
            break;
        }

        case PE_ScrollBarSlider: {
            const WidgetState s = enabled?(down?IsPressed:IsEnabled):IsDisabled;
            const QColor surface = getColor(cg, DragButtonSurface, s);

            QRect sr; // the rect where the actual scrollbar is painted.
            p->setPen(cg.background() );
            if(horiz) {
                // TODO: check reverse layout.
                sr = QRect(r_x, r_y, r_w, r_h-1);
                p->drawLine(r_x, r_y2, r_x2, r_y2); // fill the empty area...
            } else {
                sr = QRect(r_x, r_y, r_w-1, r_h);
                p->drawLine(r_x2, r_y, r_x2, r_y2); // fill the empty area...
            }

            uint contourFlags = Draw_Left|Draw_Right|Draw_Top|Draw_Bottom;
            if(!enabled) contourFlags|=Is_Disabled;

            renderContour(p, sr, cg.background(), getColor(cg, DragButtonContour, s), contourFlags);

            uint surfaceFlags = Draw_Left|Draw_Right|Draw_Top|Draw_Bottom;
            if(horiz) surfaceFlags|=Is_Horizontal;
            if(!enabled) surfaceFlags|=Is_Disabled;
            if(r.height() >= 4) {
                renderSurface(p, SHRINK_RECT(sr, 1),
                               cg.background(), surface, cg.background(), _contrast, surfaceFlags);
                // fill the remaining edges
                if (horiz) {
                    p->setPen(getColor(cg,ShadeDark) );
                    p->drawPoint(sr.x(), sr.y() );
                    p->drawPoint(sr.right(), sr.y() );
                    p->setPen(getColor(cg,ShadeLight) );
                    p->drawPoint(sr.x(), sr.bottom() );
                    p->drawPoint(sr.right(), sr.bottom() );
                } else {
                    p->setPen(getColor(cg,ShadeDark) );
                    p->drawPoint(sr.x(), sr.y() );
                    p->drawPoint(sr.x(), sr.bottom() );
                    p->setPen(getColor(cg,ShadeLight) );
                    p->drawPoint(sr.right(), sr.y() );
                    p->drawPoint(sr.right(), sr.bottom() );
                }
            }

            const int d = 5;
            int n = ((horiz?sr.width():sr.height() )-8)/d;
            if(n>5) n=5;
            if(!horiz) {
                for(int j = 0; j < n; j++) {
                    int yPos = sr.center().y()-(n*d)/2+d*j+2;
                    int cx = sr.center().x();
                    renderDot(p, 5, yPos, surface, true );
                    renderDot(p, 5+5, yPos, surface, true );
                }
            } else {
                for(int j = 0; j < n; j++) {
                    int xPos = sr.center().x()-(n*d)/2+d*j+2;
                    renderDot(p, xPos, 5, surface, true );
                    renderDot(p, xPos, 5+5, surface, true );
                }
            }

            break;
        }

        // ScrollBar groove
        case PE_ScrollBarAddPage:
        case PE_ScrollBarSubPage: {
            QRect sr;
            if(horiz) {
                sr = MODIFY_RECT(r, 0, +1, 0, -3);

                p->setPen(cg.background() );
                p->drawLine(r_x, r_y2, r_x2, r_y2); // fill the empty area...
                p->setPen(getColor(cg,ShadeDark) );
                p->drawLine(r_x, r_y, r_x2, r_y); // dark shade
                p->setPen(getColor(cg,ShadeLight) );
                p->drawLine(r_x, r_y2-1, r_x2, r_y2-1); // light shade
            } else {
                sr = MODIFY_RECT(r, +1, 0, -3, 0);

                p->setPen(cg.background() );
                p->drawLine(r_x2, r_y, r_x2, r_y2); // fill the empty area...
                p->setPen(getColor(cg,ShadeDark) );
                p->drawLine(r_x, r_y, r_x, r_y2); // dark shade
                p->setPen(getColor(cg,ShadeLight) );
                p->drawLine(r_x2-1, r_y, r_x2-1, r_y2); // li
            }

            QColor color;
            if (on||down) {
                color = cg.background().dark(100+2*_contrast);
            } else {
                color = cg.background();
            }

            renderGradient(p, sr, color.dark(100+_contrast/2), color.light(100+_contrast/2), flags & Style_Horizontal);

            break;
        }

    // SCROLLBAR BUTTONS
    // -----------------
        case PE_ScrollBarAddLine:
        case PE_ScrollBarSubLine: {
            uint contourFlags = Draw_Left|Draw_Right|Draw_Top|Draw_Bottom;
            uint surfaceFlags = Draw_Left|Draw_Right|Draw_Top|Draw_Bottom;
            if(down) surfaceFlags|=Is_Sunken;
            if(!enabled) {
                contourFlags|=Is_Disabled;
                surfaceFlags|=Is_Disabled;
            }

            QRect sr; // the rect where the actual scrollbar is painted.

            // fake part of the scrollBar groove: "empty" 1px wide area between button surface and panel contour
            p->setPen(cg.background() ); // for the empty area...
            if(horiz) {
                contourFlags |= Is_Horizontal;
                surfaceFlags |= Is_Horizontal;
            }

            if (pe == PE_ScrollBarAddLine) {
                // fake part of the scrollBar groove: "empty" 1px wide area between button surface and panel contour
                p->setPen(cg.background() );
                if(horiz) {
                    sr = QRect(r_x, r_y, r_w-1, r_h-1);
                    p->drawLine(r_x2, r_y, r_x2, r_y2);
                    p->drawLine(r_x, r_y2, r_x2-1, r_y2);
                } else {
                    sr = QRect(r_x, r_y, r_w-1, r_h-1);
                    p->drawLine(r_x2, r_y, r_x2, r_y2);
                    p->drawLine(r_x, r_y2, r_x2-1, r_y2);
                }
            } else {
                // fake part of the scrollBar groove: "empty" 1px wide area between button surface and panel contour
                p->setPen(cg.background() );
                if(horiz) {
                    // TODO: change subRect etc. to make the second subLine-button a bit shorter.
                    sr = QRect(r_x+1, r_y, r_w-1, r_h-1);
                    p->drawLine(r_x, r_y2, r_x2, r_y2);
                    p->drawLine(r_x, r_y, r_x, r_y2-1);
                } else {
                    sr = QRect(r_x, r_y+1, r_w-1, r_h-1);
                    p->drawLine(r_x2, r_y, r_x2, r_y2);
                    p->drawLine(r_x, r_y, r_x2-1, r_y);
                }
            }

            // fake part of the scrollBar groove: edges not covered by renderContour
            if (horiz) {
                p->setPen(getColor(cg,ShadeDark) );
                p->drawPoint(sr.x(), sr.y() );
                p->drawPoint(sr.right(), sr.y() );
                p->setPen(getColor(cg,ShadeLight) );
                p->drawPoint(sr.x(), sr.bottom() );
                p->drawPoint(sr.right(), sr.bottom() );
            } else {
                p->setPen(getColor(cg,ShadeDark) );
                p->drawPoint(sr.x(), sr.y() );
                p->drawPoint(sr.x(), sr.bottom() );
                p->setPen(getColor(cg,ShadeLight) );
                p->drawPoint(sr.right(), sr.y() );
                p->drawPoint(sr.right(), sr.bottom() );
            }

            renderContour(p, sr, cg.background(), getColor(cg, ButtonContour), contourFlags);
            renderSurface(p, SHRINK_RECT(sr, 1),
                           cg.background(), cg.button(), getColor(cg,MouseOverHighlight), _contrast,
                           surfaceFlags);

            // arrows
            if (pe == PE_ScrollBarAddLine) {
                p->setPen(cg.foreground());
                drawPrimitive((horiz ? PE_ArrowRight : PE_ArrowDown), p, r, cg, flags);
            } else {
                p->setPen(cg.foreground());
                drawPrimitive((horiz ? PE_ArrowLeft : PE_ArrowUp), p, r, cg, flags);
            }

            break;
        }

        // CheckBox mark
        case PE_Indicator: {
            uint contourFlags = Draw_Left|Draw_Right|Draw_Top|Draw_Bottom|Is_Sunken|Fill_Edges;
            if(!enabled) {
                contourFlags |= Is_Disabled;
            }
            renderShade(p, r, cg.background(), contourFlags);
            renderContour(p, SHRINK_RECT(r, 1), cg.background(), getColor(cg, ButtonContour), contourFlags);

            // surface
            QColor contentColor = enabled?cg.base():cg.background();
            if (mouseOver) {
                contentColor = contentColor.dark(105+_contrast*2);
            }
            p->fillRect(SHRINK_RECT(r, 2), contentColor );

            // check mark
            drawPrimitive(PE_CheckMark, p, r, cg, flags);

            break;
        }

        case PE_IndicatorMask: {
            p->fillRect (r, color1);
            break;
        }


        // RadioButton mark
        case PE_ExclusiveIndicator: {
            QColor contentColor = enabled?cg.base():cg.background();
            if (mouseOver) {
                contentColor = contentColor.dark(105+_contrast*2);
            }

            // surface
            DRAW_BITMAP( p, r_x, r_y, contentColor, radio_bg )
            // contour
            DRAW_BITMAP( p, r_x, r_y, getColor(cg, ButtonContour, enabled), radio_contour )
            // shading
            DRAW_BITMAP( p, r_x, r_y, cg.background().dark(105+_contrast), radio_shade_top )
            DRAW_BITMAP( p, r_x, r_y, cg.background().light(105+_contrast), radio_shade_bottom )

            break;
        }

        case PE_ExclusiveIndicatorMask: {
            p->fillRect(r, color0);
            DRAW_BITMAP( p, r_x, r_y, Qt::color1, radio_mask )

            break;
        }


        case PE_Splitter: {
            // highlight on mouse over
            QColor color = (hoverWidget == p->device())?cg.background().light(100+_contrast):cg.background();
            p->fillRect(r, color);
            if (r_w > r_h) {
                if (r_h > 4) {
                    int ycenter = r_h/2;
                    for(int k = 2*r_w/10; k < 8*r_w/10; k+=5) {
                        renderDot(p, k, ycenter, color, true);
                    }
                }
            } else {
                if (r_w > 4) {
                    int xcenter = r_w/2;
                    for(int k = 2*r_h/10; k < 8*r_h/10; k+=5) {
                        renderDot(p, xcenter, k, color, true);
                    }
                }
            }

                break;
        }

        case PE_PanelGroupBox:
        case PE_GroupBoxFrame: {
            if ( opt.isDefault() || opt.lineWidth() <= 0 )
                break;
            renderPanel(p, r, cg, false);

            break;
        }

        case PE_WindowFrame:
        case PE_Panel: {
            if ( opt.isDefault() || opt.lineWidth() <= 0 )
                break;
            renderPanel(p, r, cg, true, sunken);
            break;
        }

        case PE_PanelLineEdit: {
            bool isReadOnly = false;
            bool isEnabled = true;
            // panel is highlighted by default if it has focus, but if we have access to the
            // widget itself we can try to avoid highlighting in case it's readOnly or disabled.
            if (p->device() && dynamic_cast<QLineEdit*>(p->device()))
            {
                QLineEdit* lineEdit = dynamic_cast<QLineEdit*>(p->device());
                isReadOnly = lineEdit->isReadOnly();
                isEnabled = lineEdit->isEnabled();
            }

            const bool isFocussed = _inputFocusHighlight && hasFocus && !isReadOnly && isEnabled;

            uint contourFlags = Draw_Left|Draw_Right|Draw_Top|Draw_Bottom|
                    Is_Sunken|Fill_Edges;
            if (isFocussed)
                contourFlags |= Is_HighlightShade;

            // HACK!!
            //
            // In order to draw nice edges in khtml, we need to paint alpha-blended.
            // On the other hand, we can't paint alpha-blended in normal widgets.
            //
            // In this place there is no reliable way to detect if we are in khtml; the
            // only thing we know is that khtml buffers its widgets into a pixmap. So
            // when the paint device is a QPixmap, chances are high that we are in khtml.
            // It's possible that this breaks other things, so let's see how it works...
            if (p->device() && dynamic_cast<QPixmap*>(p->device() ) ) {
                contourFlags += Draw_AlphaBlend;
            }

            renderShade(p, r, cg.background(), contourFlags, getColor(cg,FocusHighlight,enabled) );

            if ( _inputFocusHighlight && hasFocus && !isReadOnly && isEnabled)
            {
                renderContour(p, SHRINK_RECT(r, 1),
                               cg.background(), getColor(cg,FocusHighlight,enabled), contourFlags);
            }
            else
            {
                renderContour(p, SHRINK_RECT(r, 1),
                               cg.background(), getColor(cg, ButtonContour, enabled), contourFlags);
            }

            break;
        }

        case PE_StatusBarSection: {
            renderContour(p, r, cg.background(), cg.background().dark(140),
                          Draw_Left|Draw_Right|Draw_Top|Draw_Bottom);
            break;
        }

        case PE_TabBarBase: // Still not sure what this one does
        case PE_PanelTabWidget: {
            renderPanel(p, r, cg, true, sunken);
            break;
        }

        case PE_PanelPopup: {
            renderContour(p, r, cg.background(), cg.background().dark(200),
                          Draw_Left|Draw_Right|Draw_Top|Draw_Bottom);
            break;
        }


        // Menu- / ToolBar panel
        case PE_PanelMenuBar:
        case PE_PanelDockWindow: {
            // fix for toolbar lag (from Mosfet Liquid) 
            QWidget* w = dynamic_cast<QWidget*>(p->device());
            if(w && w->backgroundMode() == PaletteButton) 
                w->setBackgroundMode(PaletteBackground);
            p->fillRect(r, cg.brush(QColorGroup::Background));

            if ( _drawToolBarSeparator ) {
                if ( r.width() > r.height() ) {
                    p->setPen( getColor(cg, PanelLight) );
                    p->drawLine( r.left(), r.top(), r.right(), r.top() );
                    p->setPen( getColor(cg, PanelDark) );
                    p->drawLine( r.left(), r.bottom(), r.right(), r.bottom() );
                }
                else {
                    p->setPen( getColor(cg, PanelLight) );
                    p->drawLine( r.left(), r.top(), r.left(), r.bottom() );
                    p->setPen( getColor(cg, PanelDark) );
                    p->drawLine( r.right(), r.top(), r.right(), r.bottom() );
                }
            }

            break;
        }


        // ToolBar/DockWindow handle
        case PE_DockWindowResizeHandle: {
            renderButton(p, r, cg);
            break;
        }

        case PE_DockWindowHandle: {

            int counter = 1;

            if(horiz) {
                int center = r.left()+r.width()/2;
                for(int j = r.top()+2; j <= r.bottom()-3; j+=5) {
                    renderDot(p, center-1, j, cg.background(), true);
                    counter++;
                }
            } else {
                int center = r.top()+r.height()/2;
                for(int j = r.left()+2; j <= r.right()-3; j+=5) {
                    renderDot(p, j, center-1, cg.background(), true);
                    counter++;
                }
            }

            break;
        }

        // ToolBar Separator
        case PE_DockWindowSeparator: { 
            p->fillRect(r, cg.background());

            if(_drawToolBarItemSeparator) {
                if(horiz) {
                    int center = r.left()+r.width()/2;
                    p->setPen( getColor(cg, PanelDark) );
                    p->drawLine( center-1, r.top()+3, center-1, r.bottom()-3 );
                    p->setPen( getColor(cg, PanelLight) );
                    p->drawLine( center, r.top()+3, center, r.bottom()-3 );
                } else {
                    int center = r.top()+r.height()/2;
                    p->setPen( getColor(cg, PanelDark) );
                    p->drawLine( r.x()+3, center-1, r.right()-3, center-1 );
                    p->setPen( getColor(cg, PanelLight) );
                    p->drawLine( r.x()+3, center, r.right()-3, center );
                }
            }
            break;
        }

        case PE_CheckMark: {
            const QColor contentColor = enabled?cg.base():cg.background();
            QColor checkmarkColor = enabled?getColor(cg,CheckMark):cg.background();
            if(flags & Style_Down) {
                checkmarkColor = alphaBlendColors(contentColor, checkmarkColor, 150);
            }

            int x = r.center().x() - 4, y = r.center().y() - 4;
            if( flags & Style_On ) {
                DRAW_BITMAP( p, x, y, checkmarkColor, checkmark )
            } else if ( flags & Style_Off ) {
                // empty
            } else { // tristate
                DRAW_BITMAP( p, x, y, checkmarkColor, tristatemark )
            }

            break;
        }

        case PE_SpinWidgetUp:
        case PE_SpinWidgetDown:
        case PE_HeaderArrow:
        case PE_ArrowUp:
        case PE_ArrowDown:
        case PE_ArrowLeft:
        case PE_ArrowRight: {
            QPointArray a;

            switch (pe) {
                case PE_SpinWidgetUp:
                case PE_ArrowUp: {
                    a.setPoints(9, u_arrow);
                    break;
                }
                case PE_SpinWidgetDown:
                case PE_ArrowDown: {
                    a.setPoints(9, d_arrow);
                    break;
                }
                case PE_ArrowLeft: {
                    a.setPoints(9, l_arrow);
                    break;
                }
                case PE_ArrowRight: {
                    a.setPoints(9, r_arrow);
                    break;
                }
                default: {
                    if (flags & Style_Up) {
                        a.setPoints(9, u_arrow);
                    } else {
                        a.setPoints(9, d_arrow);
                    }
                }
            }

            const QWMatrix oldMatrix( p->worldMatrix() );

            if (flags & Style_Down) {
                p->translate(pixelMetric(PM_ButtonShiftHorizontal),
                                pixelMetric(PM_ButtonShiftVertical));
            }

            a.translate((r.x()+r.width()/2), (r.y()+r.height()/2));
            // extra-pixel-shift, correcting some visual tics...
            switch(pe) {
                case PE_ArrowLeft:
                case PE_ArrowRight:
                    a.translate(0, -1);
                    break;
                case PE_SpinWidgetUp:
                case PE_SpinWidgetDown:
                    a.translate(+1, 0);
                    break;
                default:
                    a.translate(0, 0);
            }

            if (p->pen() == QPen::NoPen) {
                if (flags & Style_Enabled) {
                    p->setPen(cg.buttonText());
                } else {
                    p->setPen(cg.highlightedText());
                }
            }
            p->drawLineSegments(a, 0);

            p->setWorldMatrix( oldMatrix );

            break;
        }

        default: {
            return KStyle::drawPrimitive(pe, p, r, cg, flags, opt);
        }
    }
}


void Plastik2Style::drawControl(ControlElement element,
                              QPainter *p,
                              const QWidget *widget,
                              const QRect &r,
                              const QColorGroup &cg,
                              SFlags flags,
                              const QStyleOption& opt) const
{
    const bool reverseLayout = QApplication::reverseLayout();

    const bool enabled = (flags & Style_Enabled);

    switch (element) {


        case CE_ProgressBarGroove: {
            uint contourFlags = Is_Sunken|Draw_Left|Draw_Right|Draw_Top|Draw_Bottom;
            renderShade(p, r, cg.background(), contourFlags );
            renderContour(p, SHRINK_RECT(r, 1),
                           cg.background(), getColor(cg, ButtonContour, enabled), contourFlags );
            break;
        }

        case CE_ProgressBarContents: {
            const QProgressBar *pb = dynamic_cast<const QProgressBar*>(widget);
            int steps = pb->totalSteps();

            const QColor bg = enabled?cg.base():cg.background(); // background
            const QColor fg = enabled?cg.highlight():cg.background().dark(110); // foreground

            if( steps == 0 ) { // Busy indicator

                p->fillRect(r, fg);

                p->setPen(bg);

                int count = pb->progress();
                int h = r.height();
                for (int i = r.x(); i <= r.right()+h; ++i) {
                    int prog = count % h;
                    int x = i-h/2;
                    if (x >= r.x() && x <= r.right() )
                        p->drawPoint(x, r.bottom()-prog);

                    ++count;
                }
                count = pb->progress();
                for (int i = r.x(); i <= r.right(); ++i) {
                    int prog = count % h;
                    int x = i;
                    if (x >= r.x() && x <= r.right() )
                        p->drawPoint(x, r.bottom()-prog);

                    ++count;
                }

            } else {
                double percent = static_cast<double>(pb->progress()) / static_cast<double>(steps);

                int w = static_cast<int>(r.width() * percent);
                // renderContour/renderSurface handle small sizes not very well, so set a minimal
                // progressbar width...
                if(w<4) w = 4;
                int w2 = r.width()-(r.width()-w);

                QRect Rempty(reverseLayout?r.left():r.left()+w-1, r.top(), r.width()-w+1, r.height() );
                QRect Rsurface(reverseLayout?r.right()-w2+1:r.left(), r.top(), w2, r.height() );

                p->fillRect(Rempty, bg);

                QRegion mask(Rsurface);
                p->setClipRegion(mask);

                const int cw = pixelMetric(PM_ProgressBarChunkWidth, pb);

//                 int staticShift = 0;
                int animShift = 0;
                if (!_animateProgressBar) {
//                     staticShift = (reverseLayout ? Rsurface.left() : Rsurface.right()) % 40 - 40;
                } else {
                    // find the animation Offset for the current Widget
                    QWidget* nonConstWidget = const_cast<QWidget*>(widget);
                    QMapConstIterator<QWidget*, int> iter = progAnimWidgets.find(nonConstWidget);
                    if (iter != progAnimWidgets.end())
                        animShift = iter.data();
                }

                int x = Rsurface.x() - 2*cw;
                int count = 0;

                QColor c1 = fg;
                QColor c2 = fg.light(100+_contrast);

                while (x < (Rsurface.right()+2*cw) ) {
                    p->fillRect(QRect(reverseLayout?x-animShift:x+animShift, Rsurface.y(), cw, Rsurface.height() ),
                                (count%2==0)?c1:c2);
                    x += cw;
                    count++;
                }

                p->setClipping(false);

            }

            break;
        }


        case CE_RadioButton: {
            drawPrimitive(PE_ExclusiveIndicator, p, r, cg, flags);

            const QColor contentColor = enabled?cg.base():cg.background();
            QColor checkmarkColor = enabled?getColor(cg,CheckMark):cg.background();
            if(flags & Style_Down) {
                checkmarkColor = alphaBlendColors(contentColor, checkmarkColor, 150);
            }

            if (flags & Style_On || flags & Style_Down) {
                int x = r.center().x() - 4, y = r.center().y() - 4;
                DRAW_BITMAP( p, x, y, checkmarkColor, radiomark )
            }

            break;
        }


        case CE_TabBarTab: {
            const QTabBar * tb = (const QTabBar *) widget;
            bool cornerWidget = false;
            if( ::qt_cast<QTabWidget*>(tb->parent()) ) {
                const QTabWidget *tw = (const QTabWidget*)tb->parent();
                // is there a corner widget in the (top) left edge?
                QWidget *cw = tw->cornerWidget(Qt::TopLeft);
                if(cw)
                    cornerWidget = true;
            }
            QTabBar::Shape tbs = tb->shape();
            bool selected = false;
            if (flags & Style_Selected) selected = true;
            TabPosition pos;
            if (tb->count() == 1) {
                pos = Single;
            } else if ((tb->indexOf(opt.tab()->identifier()) == 0)) {
                pos = First;
            } else if (tb->indexOf(opt.tab()->identifier()) == tb->count() - 1) {
                pos = Last;
            } else {
                pos = Middle;
            }


            const bool triangular = (tbs==QTabBar::TriangularAbove) || (tbs==QTabBar::TriangularBelow);
            const bool bottom = (tbs==QTabBar::RoundedBelow) || (tbs==QTabBar::TriangularBelow);
            const bool mouseOver = flags&Style_MouseOver;

            const bool isFirst = (pos == First) || (pos == Single);
            const bool isLast = (pos == Last);
//             const bool isSingle = (pos == Single);

            // TODO: draw shading...

            const int bm = 1; // bottom margin

            if (selected) {
            // is selected
                const bool leftAligned = (isFirst&&!reverseLayout&&!cornerWidget); // aligned to the panel
                const bool rightAligned = (isFirst&&reverseLayout&&!cornerWidget);

                // the top part of the tab which is nearly the same for all positions
                QRect Rc; // contour
                if (!bottom) {
                    if (leftAligned) {
                        Rc = QRect(r.x()+1, r.y(), r.width()-1, r.height()-bm);
                    } else if (rightAligned) {
                        Rc = QRect(r.x(), r.y(), r.width()-1, r.height()-bm);
                    } else {
                        Rc = QRect(r.x(), r.y(), r.width(), r.height()-bm);
                    }
                } else {
                    if (leftAligned) {
                        Rc = QRect(r.x()+1, r.y()+bm, r.width()-1, r.height()-bm);
                    } else if (rightAligned) {
                        Rc = QRect(r.x(), r.y()+bm, r.width()-1, r.height()-bm);
                    } else {
                        Rc = QRect(r.x(), r.y()+bm, r.width(), r.height()-bm);
                    }
                }
                const QRect Rs(Rc.x()+1, bottom?Rc.y():Rc.y()+1, Rc.width()-2, Rc.height()-1); // the resulting surface

                uint contourFlags = Draw_Left|Draw_Right;
                if(!bottom) {
                    contourFlags |= Draw_Top;
                    if (!triangular)
                        contourFlags |= Round_UpperLeft|Round_UpperRight;
                } else {
                    contourFlags |= Draw_Bottom;
                    if (!triangular)
                        contourFlags |= Round_BottomLeft|Round_BottomRight;
                }
                renderContour(p, Rc,
                              cg.background(), getColor(cg,PanelContour),
                              contourFlags);

            // some "position specific" paintings...
                // left/right aligned connection from the panel border to the tab. :)
                if(leftAligned) {
                    p->setPen(getColor(cg,PanelContour) );
                    p->drawPoint(Rc.x(), bottom?Rc.y()-1:Rc.bottom()+1 );
                } else if(rightAligned) {
                    p->setPen(getColor(cg,PanelContour) );
                    p->drawPoint(Rc.right(), bottom?Rc.y()-1:Rc.bottom()+1 );
                }
                // rounded connections to the panel...
                // left
                if( cornerWidget || reverseLayout || (!isFirst&&!reverseLayout) ) {
                    p->setPen(cg.background() );
                    p->drawPoint(Rc.x(), bottom?Rc.y()-1:Rc.bottom()+1);
                }
                // right
                if( cornerWidget || !reverseLayout || (!isFirst&&reverseLayout) ) {
                    p->setPen(cg.background() );
                    p->drawPoint(Rc.right(), bottom?Rc.y()-1:Rc.bottom()+1);
                }

            } else {
            // inactive tabs
                const bool left = (isFirst&&!reverseLayout) || (isLast&&reverseLayout);
                const bool right = (isFirst&&reverseLayout) || (isLast&&!reverseLayout);

                const int tm = triangular?0:2; // top margin

                QRect Rc; // contour rect
                if (isFirst && !reverseLayout && !cornerWidget) {
                    Rc = QRect(r.x()+2, bottom?r.y()+bm:r.y()+tm, r.width()-2, r.height()-bm-tm ); // left indent by 1 px
                } else if (isFirst && reverseLayout && !cornerWidget) {
                    Rc = QRect(r.x()+1, bottom?r.y()+bm:r.y()+tm, r.width()-3, r.height()-bm-tm ); // right indent by 1 px
                } else {
                    Rc = QRect(r.x()+1, bottom?r.y()+bm:r.y()+tm, r.width()-1, r.height()-bm-tm );
                }
                QRect Rs; // surface rect
                if (left) {
                    Rs = QRect(Rc.x()+1, bottom?Rc.y():Rc.y()+1, Rc.width()-2, Rc.height()-1); // draw left contour, make left surface 1 px shorter...
                } else {
                    Rs = QRect(Rc.x(), bottom?Rc.y():Rc.y()+1, Rc.width()-1, Rc.height()-1);
                }

                uint contourFlags;
                uint surfaceFlags = Is_Horizontal|Draw_Left|Draw_Top|Draw_Bottom|Draw_Right;

                if(!bottom) {
                    if (left) {
                        contourFlags = Draw_Left|Draw_Right|Draw_Top|Round_UpperLeft;
                        surfaceFlags |= Round_UpperLeft;
                    } else if ( right ) {
                        contourFlags = Draw_Right|Draw_Top|Round_UpperRight;
                        surfaceFlags |= Round_UpperRight;
                    } else {
                        contourFlags = Draw_Right|Draw_Top;
                    }
                } else {
                    if (left) {
                        contourFlags = Draw_Left|Draw_Right|Draw_Bottom|Round_BottomLeft;
                        surfaceFlags |= Round_BottomLeft;
                    } else if (right) {
                        contourFlags = Draw_Right|Draw_Bottom|Round_BottomRight;
                        surfaceFlags |= Round_BottomRight;
                    } else {
                        contourFlags = Draw_Right|Draw_Bottom;
                    }
                }

                if(mouseOver) {
                    surfaceFlags |= Is_Highlight;
                }

                renderContour(p, Rc,
                              cg.background(), getColor(cg, ButtonContour),
                              contourFlags);

                renderSurface(p, Rs,
                              cg.background(), cg.button(), getColor(cg,MouseOverHighlight), _contrast,
                              surfaceFlags);

            // some "position specific" paintings...
                // fake parts of the panel border
                int px1, px2, py;
                if (isFirst && !reverseLayout) {
                    if (cornerWidget) {
                        px1 = r.x();
                    } else {
                        // left aligned to the panel
                        px1 = r.x()+2;
                    }
                } else {
                    px1 = r.x();
                }
                if (isLast && !reverseLayout) {
                    px2 = r.right();
                } else {
                    if (isFirst && reverseLayout) {
                        if (cornerWidget) {
                            px2 = r.right();
                        } else {
                            // right aligned to the panel
                            px2 = r.right()-2;
                        }
                    } else {
                        px2 = r.right()-1;
                    }
                }
                py = bottom ? r.y() : r.bottom();

                p->setPen(getColor(cg,PanelContour) );
                p->drawLine( px1, py, px2, py );

            }


            break;
        }

        case CE_PushButton: {
            QPushButton *button = (QPushButton *)widget;

            if (button->isFlat() )
                flatMode = true;

            if (button->isDefault() )
                defaultMode = true;

            if (widget == hoverWidget)
                flags |= Style_MouseOver;

            drawPrimitive(PE_ButtonBevel, p,
                    r,
                    cg, flags, QStyleOption(button) );

            break;
        }

        case CE_PushButtonLabel:
        {
            int x, y, w, h;
            r.rect( &x, &y, &w, &h );

            const QPushButton* button = static_cast<const QPushButton *>( widget );
            bool active = button->isOn() || button->isDown();
            bool cornArrow = false;

            // Shift button contents if pushed.
            if ( active )
            {
                x += pixelMetric(PM_ButtonShiftHorizontal, widget);
                y += pixelMetric(PM_ButtonShiftVertical, widget);
                flags |= Style_Sunken;
            }

            // Does the button have a popup menu?
            if ( button->isMenuButton() )
            {
                int dx = pixelMetric( PM_MenuButtonIndicator, widget );
                if ( button->iconSet() && !button->iconSet()->isNull()  &&
                    (dx + button->iconSet()->pixmap (QIconSet::Small, QIconSet::Normal, QIconSet::Off ).width()) >= w )
                {
                    cornArrow = true; //To little room. Draw the arrow in the corner, don't adjust the widget
                }
                else
                {
                    drawPrimitive( PE_ArrowDown, p, visualRect( QRect(x + w - dx - 8, y + 2, dx, h - 4), r ),
                                cg, flags, opt );
                    w -= dx;
                }
            }

            // Draw the icon if there is one
            if ( button->iconSet() && !button->iconSet()->isNull() )
            {
                QIconSet::Mode  mode  = QIconSet::Disabled;
                QIconSet::State state = QIconSet::Off;

                if (button->isEnabled())
                    mode = button->hasFocus() ? QIconSet::Active : QIconSet::Normal;
                if (button->isToggleButton() && button->isOn())
                    state = QIconSet::On;

                QPixmap pixmap = button->iconSet()->pixmap( QIconSet::Small, mode, state );

                if (button->text().isEmpty() && !button->pixmap())
                    p->drawPixmap( x + w/2 - pixmap.width()/2, y + h / 2 - pixmap.height() / 2,
                                    pixmap );
                else
                    p->drawPixmap( x + 4, y + h / 2 - pixmap.height() / 2, pixmap );

                if (cornArrow) //Draw over the icon
                    drawPrimitive( PE_ArrowDown, p, visualRect( QRect(x + w - 6, x + h - 6, 7, 7), r ),
                                cg, flags, opt );


                int  pw = pixmap.width();
                x += pw + 4;
                w -= pw + 4;
            }

            // Make the label indicate if the button is a default button or not
            drawItem( p, QRect(x, y, w, h), AlignCenter|ShowPrefix, button->colorGroup(),
                        button->isEnabled(), button->pixmap(), button->text(), -1,
                        &button->colorGroup().buttonText() );


            if ( flags & Style_HasFocus )
                drawPrimitive( PE_FocusRect, p,
                                visualRect( subRect( SR_PushButtonFocusRect, widget ), widget ),
                                cg, flags );
            break;
        }


        case CE_MenuBarItem: {
            QMenuItem *mi = opt.menuItem();
            bool active  = flags & Style_Active;
            bool focused = flags & Style_HasFocus;
            bool down = flags & Style_Down;
            const int text_flags =
                AlignVCenter | AlignHCenter | ShowPrefix | DontClip | SingleLine;

            p->fillRect(r, cg.background());

            if (active && focused) {
                if (down) {
                    drawPrimitive(PE_ButtonTool, p, r, cg, flags|Style_Down, opt);
                } else {
                    drawPrimitive(PE_ButtonTool, p, r, cg, flags, opt);
                }
            }

            p->setPen(cg.foreground() );
            p->drawText(r, text_flags, mi->text());
            break;
        }


        case CE_PopupMenuItem: {
            const QPopupMenu *popupmenu = static_cast< const QPopupMenu * >( widget );
            QMenuItem *mi = opt.menuItem();

            if ( !mi )
            {
                // Don't leave blank holes if we set NoBackground for the QPopupMenu.
                // This only happens when the popupMenu spans more than one column.
                if (! ( widget->erasePixmap() && !widget->erasePixmap()->isNull() ) )
                    p->fillRect( r, cg.background().light( 105 ) );

                break;
            }
            int  tab        = opt.tabWidth();
            int  checkcol   = opt.maxIconWidth();
            bool enabled    = mi->isEnabled();
            bool checkable  = popupmenu->isCheckable();
            bool active     = flags & Style_Active;
            bool etchtext   = styleHint( SH_EtchDisabledText );
            bool reverse    = QApplication::reverseLayout();
            if ( checkable )
                checkcol = QMAX( checkcol, 20 );

            // Draw the menu item background
            if (active) {
                if (enabled) {
                    p->fillRect(r, cg.highlight() );
                }
                else {
                    if ( widget->erasePixmap() && !widget->erasePixmap()->isNull() )
                        p->drawPixmap( r.topLeft(), *widget->erasePixmap(), r );
                    else p->fillRect( r, cg.background().light(105) );
                    if(_drawFocusRect)
                        p->drawWinFocusRect( r );
                }
            }
            // Draw the transparency pixmap
            else if ( widget->erasePixmap() && !widget->erasePixmap()->isNull() )
                p->drawPixmap( r.topLeft(), *widget->erasePixmap(), r );
            // Draw a solid background
            else
                p->fillRect( r, cg.background().light( 105 ) );
            // Are we a menu item separator?
            if ( mi->isSeparator() )
            {
                p->setPen( cg.mid() );
                p->drawLine( r.x()+5, r.y() + 1, r.right()-5, r.y() + 1 );
                p->setPen( cg.light() );
                p->drawLine( r.x()+5, r.y() + 2, r.right()-5 , r.y() + 2 );
                break;
            }

            QRect cr = visualRect( QRect( r.x() + 2, r.y() + 2, checkcol - 1, r.height() - 4 ), r );
            // Do we have an icon?
            if ( mi->iconSet() )
            {
                QIconSet::Mode mode;

                // Select the correct icon from the iconset
                if (active)
                    mode = enabled?QIconSet::Active:QIconSet::Disabled;
                else
                    mode = enabled?QIconSet::Normal:QIconSet::Disabled;

                // Do we have an icon and are checked at the same time?
                // Then draw a "pressed" background behind the icon
                if ( checkable && /*!active &&*/ mi->isChecked() )
                    qDrawShadePanel( p, cr.x(), cr.y(), cr.width(), cr.height(),
                                        cg, true, 1, &cg.brush(QColorGroup::Midlight) );
                // Draw the icon
                QPixmap pixmap = mi->iconSet()->pixmap(QIconSet::Small, mode);
                QRect pmr( 0, 0, pixmap.width(), pixmap.height() );
                pmr.moveCenter( cr.center() );
                p->drawPixmap( pmr.topLeft(), pixmap );
            }

            // Are we checked? (This time without an icon)
            else if ( checkable && mi->isChecked() )
            {
                // We only have to draw the background if the menu item is inactive -
                // if it's active the "pressed" background is already drawn
            // if ( ! active )
                    qDrawShadePanel( p, cr.x(), cr.y(), cr.width(), cr.height(), cg, true, 1,
                                        &cg.brush(QColorGroup::Midlight) );

                // Draw the checkmark
                SFlags cflags = Style_On;
                if (enabled)
                    cflags |= Style_Enabled;
                drawPrimitive( PE_CheckMark, p, cr, cg, cflags );
            }

            // Time to draw the menu item label...
            int xm = 2 + checkcol + 2; // X position margin

            int xp = reverse ? // X position
                    r.x() + tab + rightBorder + itemHMargin + itemFrame - 1 :
                    r.x() + xm;

            int offset = reverse ? -1 : 1; // Shadow offset for etched text

            // Label width (minus the width of the accelerator portion)
            int tw = r.width() - xm - tab - arrowHMargin - itemHMargin * 3 - itemFrame + 1;

            // Set the color for enabled and disabled text
            // (used for both active and inactive menu items)
            p->setPen( enabled ? cg.buttonText() : cg.mid() );

            // This color will be used instead of the above if the menu item
            // is active and disabled at the same time. (etched text)
            QColor discol = cg.mid();

            // Does the menu item draw it's own label?
            if ( mi->custom() ) {
                int m = 2;
                // Save the painter state in case the custom
                // paint method changes it in some way
                p->save();

                // Draw etched text if we're inactive and the menu item is disabled
                if ( etchtext && !enabled && !active ) {
                    p->setPen( cg.foreground() );
                    mi->custom()->paint( p, cg, active, enabled, xp+offset, r.y()+m+1, tw, r.height()-2*m );
                    p->setPen( discol );
                }
                mi->custom()->paint( p, cg, active, enabled, xp, r.y()+m, tw, r.height()-2*m );
                p->restore();
            }
            else {
                // The menu item doesn't draw it's own label
                QString s = mi->text();
                // Does the menu item have a text label?
                if ( !s.isNull() ) {
                    int t = s.find( '\t' );
                    int m = 2;
                    int text_flags = AlignVCenter | ShowPrefix | DontClip | SingleLine;
                    text_flags |= reverse ? AlignRight : AlignLeft;

                    //QColor draw = cg.text();
                    QColor draw = (active && enabled) ? cg.highlightedText () : cg.foreground();
                    p->setPen(draw);


                    // Does the menu item have a tabstop? (for the accelerator text)
                    if ( t >= 0 ) {
                        int tabx = reverse ? r.x() + rightBorder + itemHMargin + itemFrame :
                            r.x() + r.width() - tab - rightBorder - itemHMargin - itemFrame;

                        // Draw the right part of the label (accelerator text)
                        if ( etchtext && !enabled ) {
                            // Draw etched text if we're inactive and the menu item is disabled
                            p->setPen( cg.light() );
                            p->drawText( tabx+offset, r.y()+m+1, tab, r.height()-2*m, text_flags, s.mid( t+1 ) );
                            p->setPen( discol );
                        }
                        p->drawText( tabx, r.y()+m, tab, r.height()-2*m, text_flags, s.mid( t+1 ) );
                        s = s.left( t );
                    }

                    // Draw the left part of the label (or the whole label
                    // if there's no accelerator)
                    if ( etchtext && !enabled ) {
                        // Etched text again for inactive disabled menu items...
                        p->setPen( cg.light() );
                        p->drawText( xp+offset, r.y()+m+1, tw, r.height()-2*m, text_flags, s, t );
                        p->setPen( discol );
                    }


                    p->drawText( xp, r.y()+m, tw, r.height()-2*m, text_flags, s, t );

                    p->setPen(cg.text());

                }

                // The menu item doesn't have a text label
                // Check if it has a pixmap instead
                else if ( mi->pixmap() ) {
                    QPixmap *pixmap = mi->pixmap();

                    // Draw the pixmap
                    if ( pixmap->depth() == 1 )
                        p->setBackgroundMode( OpaqueMode );

                    int diffw = ( ( r.width() - pixmap->width() ) / 2 )
                                    + ( ( r.width() - pixmap->width() ) % 2 );
                    p->drawPixmap( r.x()+diffw, r.y()+1, *pixmap );

                    if ( pixmap->depth() == 1 )
                        p->setBackgroundMode( TransparentMode );
                }
            }

            // Does the menu item have a submenu?
            if ( mi->popup() ) {
                PrimitiveElement arrow = reverse ? PE_ArrowLeft : PE_ArrowRight;
                int dim = pixelMetric(PM_MenuButtonIndicator) - 1;
                QRect vr = visualRect( QRect( r.x() + r.width() - 5 - 1 - dim,
                            r.y() + r.height() / 2 - dim / 2, dim, dim), r );

                // Draw an arrow at the far end of the menu item
                if ( active ) {
                    if ( enabled )
                        discol = cg.buttonText();

                    QColorGroup g2( discol, cg.highlight(), white, white,
                                    enabled ? white : discol, discol, white );

                    drawPrimitive( arrow, p, vr, g2, Style_Enabled );
                } else
                    drawPrimitive( arrow, p, vr, cg,
                            enabled ? Style_Enabled : Style_Default );
            }
            break;
        }

        // Menu and dockwindow empty space
        case CE_DockWindowEmptyArea:
            p->fillRect(r, cg.background());
            break;

        case CE_MenuBarEmptyArea:
            p->fillRect(r, cg.background());

//             if ( _drawToolBarSeparator ) {
//                 p->setPen( getColor(cg, PanelDark) );
//                 p->drawLine( r.left(), r.bottom(), r.right(), r.bottom() );
//             }

            break;

        default:
          KStyle::drawControl(element, p, widget, r, cg, flags, opt);
    }
}

void Plastik2Style::drawControlMask(ControlElement element,
                                  QPainter *p,
                                  const QWidget *w,
                                  const QRect &r,
                                  const QStyleOption &opt) const
{
    switch (element) {
        case CE_PushButton: {
                    p->fillRect (r, color0);
                    renderMask(p, r, color1,
                            Round_UpperLeft|Round_UpperRight|Round_BottomLeft|Round_BottomRight);
                    break;
        }

        default: {
            KStyle::drawControlMask (element, p, w, r, opt);
        }
    }
}

void Plastik2Style::drawComplexControlMask(ComplexControl c,
                                         QPainter *p,
                                         const QWidget *w,
                                         const QRect &r,
                                         const QStyleOption &o) const
{
    switch (c) {
        case CC_SpinWidget:
        case CC_ListView:
        case CC_ComboBox: {
                p->fillRect (r, color0);
                renderMask(p, r, color1,
                        Round_UpperLeft|Round_UpperRight|Round_BottomLeft|Round_BottomRight);
            break;
        }
        default: {
            KStyle::drawComplexControlMask (c, p, w, r, o);
        }
    }
}

void Plastik2Style::drawComplexControl(ComplexControl control,
                                     QPainter *p,
                                     const QWidget *widget,
                                     const QRect &r,
                                     const QColorGroup &cg,
                                     SFlags flags,
                                     SCFlags controls,
                                     SCFlags active,
                                     const QStyleOption& opt) const
{
    const bool reverseLayout = QApplication::reverseLayout();

    const bool enabled = (flags & Style_Enabled);

    int r_w = r.width();
    int r_h = r.height();
    int r_x, r_y, r_x2, r_y2;
    r.coords(&r_x, &r_y, &r_x2, &r_y2);

    switch(control) {

        case CC_ComboBox:
        case CC_SpinWidget:
        {
        // As CC_ComboBox and CC_SpinWidget have a lot code in common, try to do this work here.
            const bool cbMode = control==CC_ComboBox;

            // collect some information about the widgets, set some cautious default values
            bool editable = cbMode?false:true;
            bool khtmlWidget = false; // ComboBox only
            bool plusMinusSymbol = false; // SpinWidget only
            bool hasFocus = false;
            if (cbMode) {
                const QComboBox *cb = dynamic_cast<const QComboBox *>(widget);
                if (cb) {
                    editable = cb->editable();
                    khtmlWidget = khtmlWidgets.contains(cb);
                    hasFocus = cb->hasFocus();
                }
            } else {
                const QSpinWidget *sw = dynamic_cast<const QSpinWidget *>(widget);
                if (sw) {
                    plusMinusSymbol = sw->buttonSymbols() == QSpinWidget::PlusMinus;
                    hasFocus = sw->hasFocus();
                }
            }

            const QColor buttonColor = enabled?cg.button():cg.background();
            const QColor inputColor = enabled? (editable?cg.base():cg.button()) : cg.background();
            QColor extContourColor = getColor(cg, ButtonContour, enabled);

            const bool mouseOver = (widget == hoverWidget) || (flags & Style_MouseOver);

            // contour
            uint contourFlags = 0;
            uint surfaceFlags = 0;
            if(khtmlWidget)
                contourFlags |= Draw_AlphaBlend;
            if (mouseOver) {
                contourFlags |= Is_Highlight;
                surfaceFlags |= Is_Highlight;
                extContourColor = extContourColor.light(140);
            }

            if (_inputFocusHighlight && hasFocus && editable && enabled)
            {
                QRect editField = querySubControlMetrics(control, widget, cbMode?SC_ComboBoxEditField:SC_SpinWidgetEditField);
                QRect editFrame = r;
                QRect buttonFrame = r;

                uint editFlags = contourFlags;
                uint buttonFlags = contourFlags;

                // Hightlight only the part of the contour next to the control button
                if (reverseLayout)
                {
                    // querySubControlMetrics doesn't work right for reverse Layout
                    int dx = r.right() - editField.right();
                    editFrame.setLeft(editFrame.left() + dx);
                    buttonFrame.setRight(editFrame.left() - 1);
                    editFlags |= Draw_Right|Draw_Top|Draw_Bottom;
                    buttonFlags |= Draw_Left|Draw_Top|Draw_Bottom|Round_UpperLeft|Round_BottomLeft;
                }
                else
                {
                    editFrame.setRight(editField.right());
                    buttonFrame.setLeft(editField.right() + 1);

                    editFlags |= Draw_Left|Draw_Top|Draw_Bottom;
                    buttonFlags |= Draw_Right|Draw_Top|Draw_Bottom|Round_UpperRight|Round_BottomRight;
                }
                renderShade(p, editFrame, cg.background(), editFlags|Is_Sunken|Fill_Edges|Is_HighlightShade, getColor(cg,FocusHighlight,enabled));
                renderContour(p, SHRINK_RECT(editFrame, 1),
                               cg.background(), getColor(cg,FocusHighlight,enabled), editFlags);

                renderShade(p, buttonFrame, cg.background(), buttonFlags|Is_Sunken|Fill_Edges);
                renderContour(p, SHRINK_RECT(buttonFrame, 1),
                               cg.background(), getColor(cg, ButtonContour, enabled), buttonFlags);
            }
            else
            {
                contourFlags |= Draw_Left|Draw_Right|Draw_Top|Draw_Bottom;
                if (!editable) {
                    contourFlags |= Round_UpperLeft|Round_BottomLeft|Round_UpperRight|Round_BottomRight;
                } else {
                    if (reverseLayout) {
                        contourFlags |= Round_UpperLeft|Round_BottomLeft;
                    } else {
                        contourFlags |= Round_UpperRight|Round_BottomRight;
                    }
                }
                renderShade(p, r, cg.background(), contourFlags|Is_Sunken|Fill_Edges);
                renderContour(p, SHRINK_RECT(r, 1),
                               cg.background(), getColor(cg, ButtonContour, enabled), contourFlags);
            }

            //extend the contour: between input and handler...
            p->setPen(extContourColor);
            if(reverseLayout) {
                p->drawLine(r.left()+cbButtonWidth+2, r.top()+1, r.left()+cbButtonWidth+2, r.bottom()-1);
            } else {
                p->drawLine(r.right()-cbButtonWidth-2, r.top()+1, r.right()-cbButtonWidth-2, r.bottom()-1);
            }

            // the rect of the button area...
            const QRect RbuttonSurface(reverseLayout?r.left()+2:r.right()-cbButtonWidth-1, r.top()+1+1,
                                       cbButtonWidth, r.height()-2-2);

            // This contains code that is specific to ComboBoxes or SpinWidgets
            if (cbMode) {
                const QRect RcontentSurface(reverseLayout?r.left()+1+cbButtonWidth+2:r.left()+1+1, r.top()+1+1,
                                            r.width()-cbButtonWidth-3-1-1, r.height()-2-2);

                // handler

                surfaceFlags |= Draw_Left|Draw_Right|Draw_Top|Draw_Bottom|Is_Horizontal;
                if(reverseLayout) {
                    surfaceFlags |= Round_UpperLeft|Round_BottomLeft;
                } else {
                    surfaceFlags |= Round_UpperRight|Round_BottomRight;
                }

                if(!enabled) surfaceFlags|=Is_Disabled;

                renderSurface(p, RbuttonSurface,
                            cg.background(), buttonColor, getColor(cg,MouseOverHighlight), _contrast,
                            surfaceFlags);

                if(!editable) {
                    surfaceFlags = Draw_Left|Draw_Right|Draw_Top|Draw_Bottom|Is_Horizontal;
                    if(reverseLayout) {
                        surfaceFlags |= Round_UpperRight|Round_BottomRight;
                    } else {
                        surfaceFlags |= Round_UpperLeft|Round_BottomLeft;
                    }

                    if ((widget == hoverWidget) || (flags & Style_MouseOver)) {
                        surfaceFlags |= Is_Highlight;
                        surfaceFlags |= Highlight_Top|Highlight_Bottom;
                    }
                    renderSurface(p, RcontentSurface,
                                cg.background(), buttonColor, getColor(cg,MouseOverHighlight), _contrast,
                                surfaceFlags);
                    if (hasFocus) {
                        drawPrimitive(PE_FocusRect, p,
                                      SHRINK_RECT(RcontentSurface, 2), cg);
                    }
                }

                p->setPen(cg.foreground());
                drawPrimitive(PE_SpinWidgetDown, p, RbuttonSurface, cg, Style_Default|Style_Enabled|Style_Raised);

                // QComboBox draws the text using cg.text(), we can override this
                // from here
                p->setPen( cg.buttonText() );
                p->setBackgroundColor( cg.button() );

            } else {

                SFlags sflags = flags;
                PrimitiveElement pe;

                const bool heightDividable = ((r.height()%2) == 0);
                const int buttonHeight = RbuttonSurface.height()/2;

                //extend the contour: between up/down buttons
                p->setPen(extContourColor);
                p->drawLine(reverseLayout?r.left()+1:r.right()-cbButtonWidth-1, r.top()+1+(r.height()-2)/2,
                            reverseLayout?r.left()+cbButtonWidth:r.right()-1, r.top()+1+(r.height()-2)/2);
                if(heightDividable)
                    p->drawLine(reverseLayout?r.left()+1:r.right()-cbButtonWidth-1, r.top()+1+(r.height()-2)/2-1,
                                reverseLayout?r.left()+cbButtonWidth:r.right()-1, r.top()+1+(r.height()-2)/2-1);

                // surface
                QRect upRect = QRect(RbuttonSurface.x(), RbuttonSurface.y(),
                                        RbuttonSurface.width(), buttonHeight);
                QRect downRect = QRect(RbuttonSurface.x(),
                                        heightDividable? RbuttonSurface.y()+buttonHeight : RbuttonSurface.y()+buttonHeight+1,
                                        RbuttonSurface.width(), buttonHeight);
                if(heightDividable) {
                    upRect = QRect(upRect.left(), upRect.top(), upRect.width(), upRect.height()-1 );
                    downRect = QRect(downRect.left(), downRect.top()+1, downRect.width(), downRect.height()-1 );
                }

                surfaceFlags |= Draw_Left|Draw_Right|Draw_Top|Draw_Bottom|Is_Horizontal;
                if(reverseLayout) {
                    surfaceFlags |= Round_UpperLeft;
                } else {
                    surfaceFlags |= Round_UpperRight;
                }

                if (active==SC_SpinWidgetUp) surfaceFlags|=Is_Sunken;
                if(!enabled) surfaceFlags|=Is_Disabled;
                renderSurface(p, upRect, cg.background(), buttonColor, getColor(cg,MouseOverHighlight),
                            _contrast, surfaceFlags);
                surfaceFlags = Draw_Left|Draw_Right|Draw_Top|Draw_Bottom|Is_Horizontal;
                if(reverseLayout) {
                    surfaceFlags |= Round_BottomLeft;
                } else {
                    surfaceFlags |= Round_BottomRight;
                }
                if ((widget == hoverWidget) || (sflags & Style_MouseOver)) {
                    surfaceFlags |= Is_Highlight;
                    surfaceFlags |= Highlight_Bottom|Highlight_Left|Highlight_Right;
                }
                if (active==SC_SpinWidgetDown) surfaceFlags|=Is_Sunken;
                if(!enabled) surfaceFlags|=Is_Disabled;
                renderSurface(p, downRect, cg.background(), buttonColor, getColor(cg,MouseOverHighlight),
                            _contrast, surfaceFlags);

                // icons...
                sflags = Style_Default | Style_Enabled;
                if (active == SC_SpinWidgetUp) {
                    sflags |= Style_On;
                    sflags |= Style_Sunken;
                } else {
                    sflags |= Style_Raised;
                }
                if (plusMinusSymbol)
                    pe = PE_SpinWidgetPlus;
                else
                    pe = PE_SpinWidgetUp;
                p->setPen(cg.foreground());
                drawPrimitive(pe, p, upRect, cg, sflags);

                sflags = Style_Default | Style_Enabled;
                if (active == SC_SpinWidgetDown) {
                    sflags |= Style_On;
                    sflags |= Style_Sunken;
                } else {
                    sflags |= Style_Raised;
                }
                if (plusMinusSymbol)
                    pe = PE_SpinWidgetMinus;
                else
                    pe = PE_SpinWidgetDown;
                p->setPen(cg.foreground());
                drawPrimitive(pe, p, downRect, cg, sflags);

            }

            break;
        }


        case CC_ToolButton: {
            const QToolButton *tb = (const QToolButton *) widget;

            QRect button, menuarea;
            button   = querySubControlMetrics(control, widget, SC_ToolButton, opt);
            menuarea = querySubControlMetrics(control, widget, SC_ToolButtonMenu, opt);

            SFlags bflags = flags,
                    mflags = flags;

            if (kornMode) {
                drawPrimitive(PE_ButtonTool, p, button, cg, bflags, opt);
                break;
            } else {
                // don't want to have the buttoncolor as the background...
                p->fillRect(r, cg.background());
                bflags &= ~Style_MouseOver;
            }

            if (active & SC_ToolButton)
                bflags |= Style_Down;

            if (active & SC_ToolButtonMenu)
                mflags |= Style_Down;

            if (controls & SC_ToolButton) {
            // If we're pressed, on, or raised...
                if (bflags & (Style_Down | Style_On | Style_Raised) || widget==hoverWidget ) {
                    drawPrimitive(PE_ButtonTool, p, button, cg, bflags, opt);
                } else if (tb->parentWidget() &&
                            tb->parentWidget()->backgroundPixmap() &&
                            !tb->parentWidget()->backgroundPixmap()->isNull()) {
                    QPixmap pixmap = *(tb->parentWidget()->backgroundPixmap());
                    p->drawTiledPixmap( r, pixmap, tb->pos() );
                }
            }

            // Draw a toolbutton menu indicator if required
            if (controls & SC_ToolButtonMenu) {
                if (mflags & (Style_Down | Style_On | Style_Raised)) {
                    drawPrimitive(PE_ButtonDropDown, p, menuarea, cg, mflags, opt);
                }
                drawPrimitive(PE_ArrowDown, p, menuarea, cg, mflags, opt);
            }

            if (tb->hasFocus() && !tb->focusProxy()) {
                QRect fr = tb->rect();
                fr.addCoords(2, 2, -2, -2);
                drawPrimitive(PE_FocusRect, p, fr, cg);
            }

            // Set the color for the ToolButton menu indicator
            p->setPen(cg.buttonText() );

            break;
        }

        default:
            KStyle::drawComplexControl(control, p, widget,
                                        r, cg, flags, controls,
                                        active, opt);
            break;
    }
}


QRect Plastik2Style::subRect(SubRect r, const QWidget *widget) const
{
    switch (r) {
        case SR_ComboBoxFocusRect: {
            return querySubControlMetrics( CC_ComboBox, widget, SC_ComboBoxEditField );
        }

        // Don't use KStyles progressbar subrect
        // TODO:
        case SR_ProgressBarGroove: {
            return QRect(widget->rect());
        }
        case SR_ProgressBarContents:
        case SR_ProgressBarLabel: {
            return SHRINK_RECT(widget->rect(), 2);
        }

        default: {
            return KStyle::subRect(r, widget);
        }
    }
}

QRect Plastik2Style::querySubControlMetrics(ComplexControl control,
                                          const QWidget *widget,
                                          SubControl subcontrol,
                                          const QStyleOption &opt) const
{
    if (!widget) {
        return QRect();
    }

    QRect r(widget->rect());
    switch (control) {

        case CC_ComboBox: {
            switch (subcontrol) {
                case SC_ComboBoxEditField: {
                    return QRect(r.left()+cbFrameWidth, r.top()+cbFrameWidth,
                                 r.width()-2*cbFrameWidth-cbButtonWidth-1, r.height()-2*cbFrameWidth);
                    // 1 px "fake" contour                              ^
                }
                case SC_ComboBoxArrow: {
                    return QRect(r.right()-cbFrameWidth-cbButtonWidth, r.top(),
                                 cbButtonWidth+cbFrameWidth, r.height() );
                }
                default: {
                    return KStyle::querySubControlMetrics(control, widget, subcontrol, opt);
                }
            }
            break;
        }
        case CC_SpinWidget: {

            int bh = r.height()/2; // button height
            if (r.height()%2 == 0)
                bh -= 1;

            const int buttonsWidth = cbButtonWidth+cbFrameWidth;
            const int buttonsLeft = r.right()-cbButtonWidth-cbFrameWidth;

            switch (subcontrol) {
                case SC_SpinWidgetUp: {
                    return QRect(buttonsLeft, r.top(), buttonsWidth, bh);
                }
                case SC_SpinWidgetDown: {
                    return QRect(buttonsLeft, r.bottom()+1-bh, buttonsWidth, bh);
                }
                case SC_SpinWidgetEditField: {
                    return QRect(r.left()+cbFrameWidth, r.top()+cbFrameWidth,
                                 r.width()-2*cbFrameWidth-cbButtonWidth-1, r.height()-2*cbFrameWidth);
                    // 1 px "fake" contour                              ^
                }
                case SC_SpinWidgetButtonField: {
                    return QRect(r.right()-cbFrameWidth-cbButtonWidth, r.top(),
                                 cbButtonWidth+cbFrameWidth, r.height() );
                }
                default: {
                    return KStyle::querySubControlMetrics(control, widget, subcontrol, opt);
                }
            }
            break;
        }
        default: {
            return KStyle::querySubControlMetrics(control, widget, subcontrol, opt);
        }
    }
}

int Plastik2Style::pixelMetric(PixelMetric m, const QWidget *widget) const
{
    switch(m) {

        case PM_TabBarTabVSpace: {
            const QTabBar * tb = (const QTabBar *) widget;
            if (tb->shape() == QTabBar::RoundedAbove ||
                tb->shape() == QTabBar::RoundedBelow)
                return 9;
            else
                return 4;
        }

        case PM_TabBarTabOverlap:
            return 1;

#if (QT_VERSION >= 0x030300) // requires Qt 3.3
    // extra space between menubar items
        case PM_MenuBarItemSpacing: {
            return 6;
        }
#endif

// #if (QT_VERSION >= 0x030300) // requires Qt 3.3
//     // extra space between toolbar items
//         case PM_ToolBarItemSpacing: {
//             return 4;
//         }
// #endif

        case PM_ScrollBarSliderMin: // ScrollBar slider length
            return 21;

        case PM_ScrollBarExtent: // ScrollBar "width"
            return 17;

        // TODO: what exactly does this do?
        case PM_DockWindowSeparatorExtent:
            return 6;


        case PM_SplitterWidth:
            return 6;


        case PM_ProgressBarChunkWidth:
            return 10;


        // Slider
        case PM_SliderThickness: // Thickness/height
            return 17;
        case PM_SliderLength:
            return 19;


        // TODO: what exactly does this do?
        case PM_MenuButtonIndicator:
            return 8;


        // RadioButton size
        case PM_ExclusiveIndicatorWidth:
        case PM_ExclusiveIndicatorHeight:
            return 15;

        // Checkbox size
        case PM_IndicatorWidth:
        case PM_IndicatorHeight:
            return 15;


        case PM_SpinBoxFrameWidth:
            return cbFrameWidth;

        case PM_MenuBarFrameWidth:
            return 1;

        case PM_DefaultFrameWidth: {
            if(widget && ::qt_cast<QPopupMenu*>(widget))
                return 1;
            else
                return 2;
        }

        case PM_ButtonDefaultIndicator:
            return 0;

        case PM_ButtonMargin:
            return 2;

        case PM_ButtonShiftVertical:
            return 1;
        case PM_ButtonShiftHorizontal:
            return 0;

        default:
            return KStyle::pixelMetric(m, widget);
    }
}


QSize Plastik2Style::sizeFromContents(ContentsType t,
                                    const QWidget *widget,
                                    const QSize &s,
                                    const QStyleOption &opt) const
{
    switch (t) {
        case CT_PopupMenuItem: {
            if (!widget || opt.isDefault())
                return s;

            const QPopupMenu *popup = dynamic_cast<const QPopupMenu *>(widget);
            QMenuItem *mi = opt.menuItem();
            int maxpmw = opt.maxIconWidth();
            int w = s.width(), h = s.height();
            bool checkable = popup->isCheckable();

            if (mi->custom()) {
                w = mi->custom()->sizeHint().width();
                h = mi->custom()->sizeHint().height();
                if (!mi->custom()->fullSpan() )
                    h += 4;
            } else if (mi->widget()) {
            // don't change the size in this case.
            } else if (mi->isSeparator()) {
                w = 20;
                h = 2;
            } else {
                if (mi->pixmap()) {
                    h = QMAX(h, mi->pixmap()->height() + 2);
                } else {
                    h = QMAX(h, 16 + 2 );
                    h = QMAX(h, popup->fontMetrics().height() + 4 );
                }

                if (mi->iconSet()) {
                    h = QMAX(h, mi->iconSet()->pixmap(QIconSet::Small, QIconSet::Normal).height() + 2);
                }
            }

            if (!mi->text().isNull() && (mi->text().find('\t') >= 0)) {
                w += itemHMargin + itemFrame*2 + 7;
            } else if (mi->popup()) {
                w += 2 * arrowHMargin;
            }

            if (maxpmw) {
                w += maxpmw + 6;
            }
            if (checkable && maxpmw < 20) {
                w += 20 - maxpmw;
            }
            if (checkable || maxpmw > 0) {
                w += 12;
            }

            w += rightBorder;

            return QSize(w, h);
        }

        case CT_PushButton:
        {
            const QPushButton* btn = static_cast<const QPushButton*>(widget);

            int w = s.width() + 2 * pixelMetric(PM_ButtonMargin, widget);
            int h = s.height() + 2 * pixelMetric(PM_ButtonMargin, widget);
            if ( btn->text().isEmpty() && s.width() < 32 ) return QSize(w, h);

            return QSize( w+25, h+5 );
        }

        case CT_ToolButton:
        {
            if(widget->parent() && ::qt_cast<QToolBar*>(widget->parent()) )
                return QSize( s.width()+2*4, s.height()+2*4 );
            else
                return KStyle::sizeFromContents (t, widget, s, opt);
        }

        default:
            return KStyle::sizeFromContents (t, widget, s, opt);
    }

    return KStyle::sizeFromContents (t, widget, s, opt);
}

int Plastik2Style::styleHint( StyleHint stylehint,
                                   const QWidget *widget,
                                   const QStyleOption &option,
                                   QStyleHintReturn* returnData ) const
{
    switch (stylehint) {
        case SH_PopupMenu_SubMenuPopupDelay:
            return 96; // Motif-like delay...

        default:
            return KStyle::styleHint(stylehint, widget, option, returnData);
    }
}

bool Plastik2Style::eventFilter(QObject *obj, QEvent *ev)
{
    if (KStyle::eventFilter(obj, ev) )
        return true;

    if (!obj->isWidgetType() ) return false;
 
    // focus highlight
    if ( ::qt_cast<QLineEdit*>(obj) ) {
        QWidget* widget = static_cast<QWidget*>(obj);

        if ( ::qt_cast<QSpinWidget*>(widget->parentWidget()) )
        {
            QWidget* spinbox = widget->parentWidget();
            if ((ev->type() == QEvent::FocusIn) || (ev->type() == QEvent::FocusOut))
            {
                spinbox->repaint(false);
            }
            return false;
        }

        if ((ev->type() == QEvent::FocusIn) || (ev->type() == QEvent::FocusOut))
        {
            widget->repaint(false);
        }
        return false;
    }
    
    //Hover highlight... use qt_cast to check if the widget inheits one of the classes.
    if ( ::qt_cast<QPushButton*>(obj) || ::qt_cast<QComboBox*>(obj) ||
            ::qt_cast<QSpinWidget*>(obj) || ::qt_cast<QCheckBox*>(obj) ||
            ::qt_cast<QRadioButton*>(obj) || ::qt_cast<QToolButton*>(obj) || obj->inherits("QSplitterHandle") )
    {
        if ((ev->type() == QEvent::Enter) && static_cast<QWidget*>(obj)->isEnabled())
        {
            QWidget* button = static_cast<QWidget*>(obj);
            hoverWidget = button;
            button->repaint(false);
        }
        else if ((ev->type() == QEvent::Leave) && (obj == hoverWidget) )
        {
            QWidget* button = static_cast<QWidget*>(obj);
            hoverWidget = 0;
            button->repaint(false);
        }
        return false;
    }
    if ( ::qt_cast<QTabBar*>(obj) ) {
        if ((ev->type() == QEvent::Enter) && static_cast<QWidget*>(obj)->isEnabled())
        {
            QWidget* tabbar = static_cast<QWidget*>(obj);
            hoverWidget = tabbar;
            hoverTab = 0;
            tabbar->repaint(false);
        }
        else if (ev->type() == QEvent::MouseMove)
        {
            QTabBar *tabbar = dynamic_cast<QTabBar*>(obj);
            QMouseEvent *me = dynamic_cast<QMouseEvent*>(ev);

            if (tabbar && me) {
                // avoid unnecessary repaints (which otherwise would occour on every
                // MouseMove event causing high cpu load).

                bool repaint = true;

                    // go through the tabbar and see which tabs are hovered by the mouse.
                    // tabs are overlapping 1 px, so it's possible that 2 tabs are under the mouse.
                    int tabCount = 0;
                    for (int i = 0; i < tabbar->count(); ++i) {
                        QTab *tab = tabbar->tab(i);
                        if (tab && tab->rect().contains(me->pos() ) ) {
                            ++ tabCount;
    
                            if (tabCount < 2) {
                                // good, only one tab under the mouse.
                                if (hoverTab==tab)
                                    repaint = false; // has been updated before, no repaint necessary
                                hoverTab = tab;
                            } else {
                                // uhh! there's another tab under the mouse, repaint...
                                repaint = true;
                                hoverTab = 0; // make sure the tabbar is repainted next time too.
                            }
    
                        }
                    }

                if (repaint)
                    tabbar->repaint(false);
            }
        }
        else if (ev->type() == QEvent::Leave)
        {
            QWidget* tabbar = static_cast<QWidget*>(obj);
            hoverWidget = 0;
            hoverTab = 0;
            tabbar->repaint(false);
        }
        return false;
    }
    // Track show events for progress bars
    if ( _animateProgressBar && ::qt_cast<QProgressBar*>(obj) )
    {
        if ((ev->type() == QEvent::Show) && !animationTimer->isActive())
        {
            animationTimer->start( 50, false );
        }
    }
    if ( !qstrcmp(obj->name(), "kde toolbar widget") )
    {
        QWidget* lb = static_cast<QWidget*>(obj);
        if (lb->backgroundMode() == Qt::PaletteButton)
            lb->setBackgroundMode(Qt::PaletteBackground);
        lb->removeEventFilter(this);
    }

    return false;
}

QColor Plastik2Style::getColor(const QColorGroup &cg, const ColorType t, const bool enabled)const
{
    return getColor(cg, t, enabled?IsEnabled:IsDisabled);
}

QColor Plastik2Style::getColor(const QColorGroup &cg, const ColorType t, const WidgetState s)const
{
    const bool enabled = (s != IsDisabled) &&
            ((s == IsEnabled) || (s == IsPressed) || (s == IsHighlighted));
    const bool pressed = (s == IsPressed);
    const bool highlighted = (s == IsHighlighted);
    switch(t) {
        case DefaultButtonContour:
            return enabled ? cg.foreground()
                           : cg.background().dark(190+_contrast*10);
        case ButtonContour:
            return enabled ? cg.button().dark(130+_contrast*8)
                           : cg.background().dark(120+_contrast*8);
        case DragButtonContour: {
            if(enabled) {
                if(pressed)
                    return cg.button().dark(130+_contrast*6); // bright
                else if(highlighted)
                    return cg.button().dark(130+_contrast*9); // dark
                else
                    return cg.button().dark(130+_contrast*8); // normal
            } else {
                return cg.background().dark(120+_contrast*8);
            }
        }
        case DragButtonSurface: {
            if(enabled) {
                if(pressed)
                    return cg.button().dark(100-_contrast);  // bright
                else if(highlighted)
                    return cg.button().light(100+_contrast); // dark
                else
                    return cg.button();                      // normal
            } else {
                return cg.background();
            }
        }
        case PanelContour:
            return cg.background().dark(160+_contrast*8);
        case PanelDark:
            return alphaBlendColors(cg.background(), cg.background().dark(120+_contrast*5), 110);
        case PanelLight:
            return alphaBlendColors(cg.background(), cg.background().light(120+_contrast*5), 110);
        case ShadeLight:
            return cg.background().light(105+_contrast);
        case ShadeDark:
            return cg.background().dark(105+_contrast);
        case MouseOverHighlight:
            if( _customOverHighlightColor )
                return _overHighlightColor;
            else
                return cg.highlight();
        case FocusHighlight:
            if( _customFocusHighlightColor )
                return _focusHighlightColor;
            else
                return cg.highlight();
        case CheckMark:
            if( _customCheckMarkColor )
                return _checkMarkColor;
            else
                return cg.foreground();
    }

    return Qt::color0;
}

QColor Plastik2Style::alphaBlendColors(const QColor &bgColor, const QColor &fgColor, const int a) const
{
    // normal button...
    QRgb rgb = bgColor.rgb();
    QRgb rgb_b = fgColor.rgb();
    int alpha = a;
    if(alpha>255) alpha = 255;
    if(alpha<0) alpha = 0;
    int inv_alpha = 255 - alpha;

    QColor result  = QColor( qRgb(qRed(rgb_b)*inv_alpha/255 + qRed(rgb)*alpha/255,
                             qGreen(rgb_b)*inv_alpha/255 + qGreen(rgb)*alpha/255,
                             qBlue(rgb_b)*inv_alpha/255 + qBlue(rgb)*alpha/255) );

    return result;
}

// kate: indent-width 4; replace-tabs on; tab-width 4; space-indent on;
