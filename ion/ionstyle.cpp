/* kate: hl c++; indent-mode cstyle; space-indent on; indent-width 4; replace-tabs on;
  Ion style.
  Copyright (c) 2007 Matthew Woehlke <mw_triad@sourceforge.net>

  Released under the GNU General Public License (GPL) v2.

    Based on the Bluecurve style:
      Copyright (c) 2002 Red Hat, Inc.
    And on the Phase style:
      Copyright (c) 2004 David Johnson

  Permission is hereby granted, free of charge, to any person obtaining a
  copy of this software and associated documentation files (the "Software"),
  to deal in the Software without restriction, including without limitation
  the rights to use, copy, modify, merge, publish, distribute, sublicense,
  and/or sell copies of the Software, and to permit persons to whom the
  Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
  DEALINGS IN THE SOFTWARE.
*/

//////////////////////////////////////////////////////////////////////////////
//
// Some miscellaneous notes
//
// Reimplemented scrollbar metric and drawing routines from KStyle to allow
// better placement of subcontrols. This is because the subcontrols slightly
// overlap to share part of their border.
//
// Menu and toolbars are painted with the background color by default. This
// differs from the Qt default of giving them PaletteButton backgrounds.
// Menubars have normal gradients, toolbars have reverse.
//
// Some toolbars are not part of a QMainWindows, such as in a KDE file dialog.
// In these cases we treat the toolbar as "floating" and paint it flat.
//
//////////////////////////////////////////////////////////////////////////////

#include <kdrawutil.h>

#include <qapplication.h>
#include <q3intdict.h>
#include <qpainter.h>
#include <q3pointarray.h>
#include <qsettings.h>
#include <qstyleplugin.h>

#include <qcheckbox.h>
#include <qcombobox.h>
#include <q3header.h>
#include <qlineedit.h>
#include <q3mainwindow.h>
#include <qmenubar.h>
#include <q3popupmenu.h>
#include <q3progressbar.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qscrollbar.h>
#include <qslider.h>
#include <qsplitter.h>
#include <qtabbar.h>
#include <qtabwidget.h>
#include <q3toolbar.h>
#include <qtoolbox.h>
#include <qtoolbutton.h>
//Added by qt3to4:
#include <QPixmap>
#include <QMouseEvent>
#include <QEvent>
#include <Q3Frame>

#include "ionstyle.h"
#include "bitmaps.h"

static const char* QSPLITTERHANDLE    = "QSplitterHandle";
static const char* QTOOLBAREXTENSION  = "QToolBarExtensionWidget";
static const char* KTOOLBARWIDGET     = "kde toolbar widget";

// some convenient constants
static const int ITEMFRAME       = 1; // menu stuff
static const int ITEMHMARGIN     = 3;
static const int ITEMVMARGIN     = 1;
static const int ARROWMARGIN     = 6;
static const int RIGHTBORDER     = 6;
static const int MINICONSIZE     = 16;
static const int CHECKSIZE       = 9;
static const int MAXGRADIENTSIZE = 64;

static unsigned contrast = 110;

QMap<unsigned int, Q3IntDict<GradientSet> > gradients; // gradients[color][size]
                   Q3IntDict<PixmapSet>     pixmaps;   //   pixmaps[color]

//////////////////////////////////////////////////////////////////////////////
// Construction, Destruction, Initialization                                //
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// IonStyle()
// -----------
// Constructor

IonStyle::IonStyle()
    : KStyle(FilledFrameWorkaround | AllowMenuTransparency,
             WindowsStyleScrollBar), hover_(0), hovertab_(0),
      gradients_(QPixmap::defaultDepth() > 8 ? 2 : 0), kicker_(false)
{
    QSettings settings;
    hlfocus_ =
        settings.readBoolEntry("/ionstyle/Settings/hlfocus", true);
    embossed_ =
        settings.readBoolEntry("/ionstyle/Settings/embossed", true);
    if (gradients_) { // don't bother setting if already false
        gradients_ =
            settings.readBoolEntry("/ionstyle/Settings/gradients", true);
    }
    highlights_ =
        settings.readNumEntry("/ionstyle/Settings/highlights", 2);

    reverse_ = QApplication::reverseLayout();

    // create bitmaps
    bplus = QBitmap(BSIZE_PLUSMINUS, BSIZE_PLUSMINUS, bplus_bits, true);
    bplus.setMask(bplus);
    bminus = QBitmap(BSIZE_PLUSMINUS, BSIZE_PLUSMINUS, bminus_bits, true);
    bminus.setMask(bminus);
    doodad_mid = QBitmap(BSIZE_DOODAD, BSIZE_DOODAD, doodad_mid_bits, true);
    doodad_light = QBitmap(BSIZE_DOODAD, BSIZE_DOODAD, doodad_light_bits, true);
}

IonStyle::~IonStyle() { ; }

//////////////////////////////////////////////////////////////////////////////
// Color Helpers                                                            //
//////////////////////////////////////////////////////////////////////////////

#define DEF_GLOW_K 0.07
#define defaultGlow(g,a) blend(shade(g.a(), (double)contrast * 0.011), \
                               g.highlight(), DEF_GLOW_K)

QColor darken(const QColor &color, double a = 1.0, double b = 0.0)
{
    register double x = (double)(contrast - 100) * 0.01;
    register double k = a*x + b;
    return shade(color, 1.0 - k);
}

QColor lighten(
    const QColor &color,
    double a = 1.0,
    double b = 0.0,
    bool sb = false
              )
{
    register double x = (double)(contrast - 100) * 0.01;
    register double k = a*x + b;
    return shadei(color, 1.0 - k, (sb ? 1.0 - k*0.3 : 1.0));
}

//////////////////////////////////////////////////////////////////////////////
// Polishing                                                                //
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// polish()
// --------
// Initialize application specific

void IonStyle::polish(QApplication* app)
{
    if (!qstrcmp(app->argv()[0], "kicker")) kicker_ = true;
}

//////////////////////////////////////////////////////////////////////////////
// polish()
// --------
// Initialize the appearance of a widget

void IonStyle::polish(QWidget *widget)
{
    if (::qt_cast<QMenuBar*>(widget) ||
        ::qt_cast<Q3PopupMenu*>(widget)) {
        // anti-flicker optimization
        widget->setBackgroundMode(Qt::NoBackground);
    } else if (::qt_cast<QLineEdit*>(widget) ||
               widget->inherits(QTOOLBAREXTENSION) ||
               (!qstrcmp(widget->name(), KTOOLBARWIDGET))) {
        // needs special handling on paint events
        widget->installEventFilter(this);
    } else if (highlights_ &&
               (::qt_cast<QToolButton*>(widget) ||
                ::qt_cast<QPushButton*>(widget) ||
                ::qt_cast<QComboBox*>(widget) ||
                ::qt_cast<Q3SpinWidget*>(widget) ||
                ::qt_cast<QCheckBox*>(widget) ||
                ::qt_cast<QRadioButton*>(widget) ||
                ::qt_cast<QSlider*>(widget) ||
                ::qt_cast<QScrollBar*>(widget) ||
                widget->inherits(QSPLITTERHANDLE))) {
        // mouseover highlighting
        widget->installEventFilter(this);
    } else if (highlights_ && ::qt_cast<QTabBar*>(widget)) {
        // highlighting needing mouse tracking
        widget->setMouseTracking(true);
        widget->installEventFilter(this);
    }

    KStyle::polish(widget);
}

//////////////////////////////////////////////////////////////////////////////
// polish()
// --------
// Initialize the palette

void IonStyle::polish(QPalette &pal)
{
    QSettings settings;

    // clear out gradients on a color change
    gradients.clear();
    contrast = 100 + settings.readNumEntry("/Qt/KDE/contrast", 5);

    // lighten up a bit, so the look is not so "crisp"
    if (QPixmap::defaultDepth() > 8) { // but not on low color displays
        pal.setColor(QPalette::Disabled, QColorGroup::Dark,
            lighten(pal.color(QPalette::Disabled, QColorGroup::Dark), 0.4));
        pal.setColor(QPalette::Active, QColorGroup::Dark,
            lighten(pal.color(QPalette::Active, QColorGroup::Dark), 0.4));
        pal.setColor(QPalette::Inactive, QColorGroup::Dark,
            lighten(pal.color(QPalette::Inactive, QColorGroup::Dark), 0.4));
    }
}

//////////////////////////////////////////////////////////////////////////////
// unPolish()
// ----------
// Undo the initialization of a widget's appearance

void IonStyle::unPolish(QWidget *widget)
{
    if (::qt_cast<QMenuBar*>(widget) ||
        ::qt_cast<Q3PopupMenu*>(widget)) {
        widget->setBackgroundMode(PaletteBackground);
    } else if (::qt_cast<Q3Frame*>(widget) ||
               widget->inherits(QTOOLBAREXTENSION) ||
               (!qstrcmp(widget->name(), KTOOLBARWIDGET))) {
        widget->removeEventFilter(this);
    } else if (highlights_ && // highlighting
               (::qt_cast<QPushButton*>(widget) ||
                ::qt_cast<QComboBox*>(widget) ||
                ::qt_cast<Q3SpinWidget*>(widget) ||
                ::qt_cast<QCheckBox*>(widget) ||
                ::qt_cast<QRadioButton*>(widget) ||
                ::qt_cast<QSlider*>(widget) ||
                widget->inherits(QSPLITTERHANDLE))) {
        widget->removeEventFilter(this);
    } else if (highlights_ && ::qt_cast<QTabBar*>(widget)) {
        widget->setMouseTracking(false);
        widget->removeEventFilter(this);
    }

    KStyle::unPolish(widget);
}

//////////////////////////////////////////////////////////////////////////////
// Drawing                                                                  //
//////////////////////////////////////////////////////////////////////////////

static GradientSet* getGradient(const QColor &color, int size)
{
    GradientSet *set;

    // lazy allocation
    set = gradients[color.rgb()][size];
    if (!set) {
        set = new GradientSet(color, size);
        gradients[color.rgb()].setAutoDelete(true);
        gradients[color.rgb()].insert(size, set);
    }
    return set;
}

static PixmapSet* getPixmap(const QColor &color)
{
    PixmapSet *set;

    // lazy allocation
    set = pixmaps[color.rgb()];
    if (!set) {
        set = new PixmapSet(color);
        pixmaps.setAutoDelete(true);
        pixmaps.insert(color.rgb(), set);
    }
    return set;
}

static void getParentRelativeCoords(
    const QWidget *widget,
    int &x, int &y, int &w, int &h,
    bool horiz
                                   )
{
    if (widget && widget->parentWidget()) {
        const QWidget *parent = widget->parentWidget();
         if (horiz) {
            h = parent->height();
            y = widget->mapToParent(QPoint(0, 0)).y();
         } else {
            w = parent->width();
            x = widget->mapToParent(QPoint(0, 0)).x();
         }
    }
}

//////////////////////////////////////////////////////////////////////////////
// drawIonPixmap()
// ------------------
// Draw pixmap

void IonStyle::drawIonPixmap(
    QPainter *painter,
    PIXMAP_ENUM which,
    const QColor &color,
    int x, int y
                                      ) const
{
    PixmapSet *set = getPixmap(color);
    painter->drawPixmap(x, y, *set->pixmap(which));
}

//////////////////////////////////////////////////////////////////////////////
// drawIonGradient()
// ------------------
// Draw gradient

void IonStyle::drawIonGradient(
    QPainter *painter,
    const QRect &rect,
    QColor color,
    QColor glow,
    bool horizontal,
    bool reverse,
    int px, int py,
    int pw, int ph
                                        ) const
{
    GradientSet *set;

    // px, py, pw, ph used for parent-relative pixmaps
    int size;
    if (horizontal)
        size = (ph > 0) ? ph : rect.height();
    else
        size = (pw > 0) ? pw : rect.width();

    // First (and always) paint the base
    if (size < ((3*highlights_)/2) && glow != color) {
        // If too small for proper glow, use glow color and be done with it
        painter->fillRect(rect, glow);
        return;
    } else {
        painter->fillRect(rect, color);
    }

    // keep it sensible
    if (gradients_ && size <= MAXGRADIENTSIZE) {
        // Ok, paint the regular gradient
        set = getGradient(color, size);
        painter->drawTiledPixmap(rect, *set->gradient(horizontal, reverse,
                                 false), QPoint(px, py));
    }

    if (highlights_ == 0 || glow == color)
        return;

    // Finally paint the glow
    QRect rtop(rect), rbot(rect);
    if (horizontal) {
        rbot.setTop(rect.bottom()+1-highlights_);
        rtop.setHeight(highlights_);
        rbot.setHeight(highlights_);
    } else {
        rbot.setLeft(rect.right()+1-highlights_);
        rtop.setWidth(highlights_);
        rbot.setWidth(highlights_);
    }

    if (gradients_) {
        set = getGradient(glow, highlights_);
        painter->drawTiledPixmap(rtop, *set->gradient(horizontal, false,
                                 true), QPoint(px, py));
        set = getGradient(glow, highlights_);
        painter->drawTiledPixmap(rbot, *set->gradient(horizontal, true,
                                 true), QPoint(px, py));
    } else {
        painter->fillRect(rtop, glow);
        painter->fillRect(rbot, glow);
    }
}

//////////////////////////////////////////////////////////////////////////////
// drawIonEdge()
// ----------------
// Draw a basic Ion edge

void IonStyle::drawIonEdge(
    QPainter *painter,
    int x1, int y1, int x2, int y2,
    const QColor &mid,
    const QColor &dark,
    const QColor &light,
    bool sunken, bool rotated
                                    ) const
{
    if (rotated) {
        painter->setPen(sunken ? dark : light);
        painter->drawLine(x1, y1, x2-1, y1);
        painter->drawLine(x2, y1+1, x2, y2-1);

        painter->setPen(sunken ? light : dark);
        painter->drawLine(x1+1, y2, x2, y2);
        painter->drawLine(x1, y1+1, x1, y2-1);

        painter->setPen(mid);
        painter->drawPoint(x1, y1);
        painter->drawPoint(x2, y2);
    } else {
        painter->setPen(sunken ? dark : light);
        painter->drawLine(x1, y1, x2-1, y1);
        painter->drawLine(x1, y1+1, x1, y2-1);

        painter->setPen(sunken ? light : dark);
        painter->drawLine(x1+1, y2, x2, y2);
        painter->drawLine(x2, y1+1, x2, y2-1);

        painter->setPen(mid);
        painter->drawPoint(x1, y2);
        painter->drawPoint(x2, y1);
    }
}

//////////////////////////////////////////////////////////////////////////////
// drawIonBevel()
// ----------------
// Draw the basic Ion bevel

void IonStyle::drawIonBevel(
    QPainter *painter,
    int x, int y, int w, int h,
    const QColorGroup &group,
    const QColor &fill,
    const QColor &glow,
    bool sunken,
    bool horizontal,
    bool reverse
                                     ) const
{
    int x2 = x + w - 1;
    int y2 = y + h - 1;
    painter->save();

    painter->setPen(group.dark());
    painter->drawRect(x, y, w, h);

    drawIonEdge(painter, x+1, y+1, x2-1, y2-1, group.button(),
                group.mid(), group.light(), sunken);

    if (sunken) {
        // sunken bevels don't get gradients
        painter->fillRect(x+2, y+2, w-4, h-4, fill);
    } else {
        drawIonGradient(painter, QRect(x+2, y+2, w-4, h-4),
                        fill, glow, horizontal, reverse);
    }
    painter->restore();
}

//////////////////////////////////////////////////////////////////////////////
// drawIonButton()
// ----------------
// Draw the basic Ion button

void IonStyle::drawIonButton(
    QPainter *painter,
    int x, int y, int w, int h,
    const QColorGroup &group,
    const QColor &fill,
    const QColor &glow,
    bool sunken,
    bool isDefault
                                      ) const
{
    int x2 = x + w - 1;
    int y2 = y + h - 1;
    painter->save();

    if (embossed_) {
        drawIonEdge(painter, x, y, x2, y2, group.button(),
                    group.mid(), group.midlight(), true);
        x++; y++; w-=2; h-=2; x2-=1; y2-=1;
    }

    if (isDefault) {
        QColor const &mid = group.highlight();
        QColor dark = blend(darken(mid, 2.0, 0.2), group.dark(), 0.5);

        painter->setPen(dark);
        painter->drawRect(x, y, w, h);

        drawIonEdge(painter, x+1, y+1, x2-1, y2-1, mid,
                    darken(mid, 2.0), lighten(mid, 2.0), sunken);

        painter->setPen(mid);
        painter->drawRect(x+2, y+2, w-4, h-4);

        drawIonGradient(painter, QRect(x+3, y+3, w-6, h-6),
                        fill, glow, true, sunken);
    } else {
        painter->setPen(group.dark());
        painter->drawRect(x, y, w, h);

        drawIonEdge(painter, x+1, y+1, x2-1, y2-1, group.button(),
                    group.mid(), group.light(), sunken);

        drawIonGradient(painter, QRect(x+2, y+2, w-4, h-4),
                        fill, glow, true, sunken);
    }

    painter->restore();
}

//////////////////////////////////////////////////////////////////////////////
// drawIonPanel()
// ----------------
// Draw the basic Ion panel

void IonStyle::drawIonPanel(
    QPainter *painter,
    int x, int y, int w, int h,
    const QColorGroup &group,
    bool sunken,
    const QBrush *fill,
    bool active
                                     ) const
{
    int x2 = x + w - 1;
    int y2 = y + h - 1;
    painter->save();

    if (sunken) {
        if (embossed_) {
            if (hlfocus_ && active) {
                QColor const &mid = group.highlight();
                QColor dark = blend(darken(mid, 2.0, 0.2), group.dark(), 0.5);

                painter->setPen(dark);
                painter->drawRect(x+1, y+1, w-2, h-2);

                drawIonEdge(painter, x, y, x2, y2, mid,
                            darken(mid, 2.0), lighten(mid, 2.0), sunken);
            } else {
                painter->setPen(group.dark());
                painter->drawRect(x+1, y+1, w-2, h-2);

                drawIonEdge(painter, x, y, x2, y2, group.background(),
                            group.mid(), group.light(), true);
            }
        } else {
            const QColor &base = (fill ? fill->color() : group.mid());
            QColor dark = (hlfocus_ && active ? group.highlight()
                                              : darken(group.mid(), 1.4, 0.15));

            painter->setPen(dark);
            painter->drawRect(x, y, w, h);

            painter->setPen(blend(dark, base, 0.6));
            painter->drawLine(x+1, y+1, x+1, y2-1);
            painter->drawLine(x+2, y+1, x2-1, y+1);
        }
    } else {
        painter->setPen(group.dark());
        painter->drawRect(x, y, w, h);

        drawIonEdge(painter, x+1, y+1, x2-1, y2-1,
                    group.background(), group.mid(), group.light());
    }

    if (fill) {
        if (!sunken || embossed_)
            painter->fillRect(x+2, y+2, w-4, h-4, fill->color());
        else
            painter->fillRect(x+2, y+2, w-3, h-3, fill->color());
    }
    painter->restore();
}

//////////////////////////////////////////////////////////////////////////////
// drawIonPanel()
// ----------------
// Draw the simple Ion panel (with ramped gradient)

void IonStyle::drawIonPanel(
    QPainter *painter,
    int x, int y, int w, int h,
    const QColor &color,
    double k,
    bool sunken
                                     ) const
{
    int x2 = x + w - 1;
    int y2 = y + h - 1;
    painter->save();

    if (sunken)
        drawIonEdge(painter, x, y, x2, y2, color,
                    lighten(color, 1.0, 0.1, true),
                    darken(color, 1.0, 0.1));
    else
        drawIonEdge(painter, x, y, x2, y2, color,
                    darken(color, 1.0, 0.1),
                    lighten(color, 1.0, 0.1, true));

    QRect rect(x+1, y+1, w-2, h-2);
    painter->fillRect(rect, color);

    GradientSet *set;
    int size = h-2;

    // keep it sensible
    if (gradients_ && size <= MAXGRADIENTSIZE) {
        // Ok, we'll paint the gradient
        QPoint p0(0, 0);

        QRect rtop(x+1, y+1, w-2, (h-2)/2);
        set = getGradient(sunken ? lighten(color, k, 0.0, true)
                                 : darken(color, k),
                          rtop.height());
        painter->drawTiledPixmap(rtop, *set->gradient(true, false, true));

        QRect rbot(x+1, y+(h/2), w-2, h-(h/2)-1);
        set = getGradient(sunken ? darken(color, k)
                                 : lighten(color, k, 0.0, true),
                          rbot.height());
        set = getGradient(lighten(color, k, 0.0, true), rbot.height());
        painter->drawTiledPixmap(rbot, *set->gradient(true, true, true));
    }

    painter->restore();
}

//////////////////////////////////////////////////////////////////////////////
// drawIonProgressBar()
// ----------------
// Draw a Ion style progress bar (filled portion)

void IonStyle::drawIonProgressBar(
    QPainter *painter,
    int x, int y, int w, int h,
    const QColorGroup &group
                                           ) const
{
    QColor color = group.highlight();

    painter->save();

    if (!embossed_) {
        painter->setPen(darken(color, 1.0, 0.2));
        painter->drawRect(x, y, w, h);
        x += 1; y += 1;
        w -= 2; h -= 2;
    }

    drawIonPanel(painter, x, y, w, h, color, 5.0);

    painter->restore();
}

//////////////////////////////////////////////////////////////////////////////
// drawIonTab()
// -------------
// Draw a Ion style tab

void IonStyle::drawIonTab(
    QPainter *painter,
    int x, int y, int w, int h,
    const QColorGroup &group,
    const QTabBar *bar,
    const QStyleOption &option,
    SFlags flags
                                   ) const
{
    const QTabWidget *tabwidget;
    bool selected = (flags & Style_Selected);
    bool edge; // tab is at edge of bar
    const int x2 = x + w - 1;
    const int y2 = y + h - 1;

    painter->save();

    // what position is the tab?
    edge = (bar->count() == 1);
    edge |= (bar->indexOf(option.tab()->identifier()) == 0);

    switch (QTabBar::Shape(bar->shape())) {
        case QTabBar::RoundedNorth:
        case QTabBar::TriangularNorth: {
            // is there a corner widget?
            tabwidget = ::qt_cast<QTabWidget*>(bar->parent());
            if (edge && tabwidget
                && tabwidget->cornerWidget(reverse_ ? Qt::TopRightCorner
                                                    : Qt::TopLeftCorner))
                edge = false;

            if (!selected) // shorten
                y += 2; h -= 2;
            if (selected) {
                painter->setPen(Qt::NoPen);
                painter->fillRect(x+1, y+1, w-1, h-1,
                                  group.brush(QColorGroup::Background));
            } else {
                if (flags & Style_MouseOver)
                    drawIonGradient(painter, QRect(x+1, y+2, w-1, h-4),
                                    blend(darken(group.background()),
                                          group.highlight(), DEF_GLOW_K),
                                    group.highlight());
                else
                    drawIonGradient(painter, QRect(x+1, y+2, w-1, h-4),
                                    darken(group.background(), 1.0, 0.1),
                                    darken(group.background(), 1.0, 0.1));
            }

            // draw tab
            painter->setPen(group.dark());
            painter->drawLine(x, y, x, y2-2);
            painter->drawLine(x+1, y, x2, y);
            painter->drawLine(x2, y+1, x2, y2-2);

            painter->setPen(group.mid());
            painter->drawLine(x2-1, y+2, x2-1, y2-2);

            painter->setPen(group.light());
            painter->drawLine(x+1, y+1, x2-2, y+1);
            if ((selected) || edge) painter->drawLine(x+1, y+2, x+1, y2-2);

            // finish off bottom
            if (selected) {
                painter->setPen(group.dark());
                painter->drawPoint(x, y2-1);
                painter->drawPoint(x2, y2-1);

                painter->setPen(group.light());
                painter->drawPoint(x, y2);
                painter->drawLine(x+1, y2-1, x+1, y2);
                painter->drawPoint(x2, y2);

                painter->setPen(group.mid());
                painter->drawPoint(x2-1, y2-1);

                if (!reverse_ && edge) {
                    painter->setPen(group.dark());
                    painter->drawLine(x, y2-1, x, y2);
                    painter->setPen(group.light());
                    painter->drawPoint(x+1, y2);
                }
            } else {
                painter->setPen(group.dark());
                painter->drawLine(x, y2-1, x2, y2-1);

                painter->setPen(group.light());
                painter->drawLine(x, y2, x2, y2);

                if (!reverse_ && edge) {
                    painter->setPen(group.dark());
                    painter->drawLine(x, y2-1, x, y2);
                }
            }
            if (reverse_ && edge) {
                painter->setPen(group.dark());
                painter->drawPoint(x2, y2);
                painter->setPen(selected ? group.mid() : group.background());
                painter->drawPoint(x2-1, y2);
            }
            break;
        }

        case QTabBar:: RoundedSouth:
        case QTabBar:: TriangularSouth: {
            // is there a corner widget?
            tabwidget = ::qt_cast<QTabWidget*>(bar->parent());
            if (edge && tabwidget
                && tabwidget->cornerWidget(reverse_ ?
                                           Qt::BottomRightCorner : Qt::BottomLeftCorner)) {
                edge = false;
            }

            painter->setBrush((selected || (flags & Style_MouseOver))
                               ? group.background()
                               : darken(group.background()));
            painter->setPen(Qt::NoPen);
            painter->fillRect(x+1, y+1, w-1, h-1, painter->brush());

            // draw tab
            painter->setPen(group.dark());
            painter->drawLine(x, y+1, x, y2);
            painter->drawLine(x+1, y2, x2, y2);
            painter->drawLine(x2, y+1, x2, y2-1);

            painter->setPen(group.mid());
            painter->drawLine(x2-1, y+1, x2-1, y2-1);
            painter->drawLine(x+2, y2-1, x2-1, y2-1);
            painter->drawPoint(x, y);
            painter->drawPoint(x2, y);

            if ((selected) || edge) {
                painter->setPen(group.light());
                painter->drawLine(x+1, y+1, x+1, y2-2);
            }

            // finish off top
            if (selected) {
                if (!reverse_ && edge) {
                    painter->setPen(group.dark());
                    painter->drawPoint(x, y);
                    painter->setPen(group.light());
                    painter->drawPoint(x+1, y);
                }
            } else {
                painter->setPen(group.dark());
                painter->drawLine(x, y+1, x2, y+1);

                painter->setPen(group.mid());
                painter->drawLine(x, y, x2, y);

                if (!reverse_ && edge) {
                    painter->setPen(group.dark());
                    painter->drawPoint(x, y);
                }
            }
            if (reverse_ && edge) {
                painter->setPen(group.dark());
                painter->drawPoint(x2, y);
                painter->setPen(group.mid());
                painter->drawPoint(x2-1, y);
            }
            break;
        }
    }
    painter->restore();
}

//////////////////////////////////////////////////////////////////////////////
// drawPrimitive()
// ---------------
// Draw the primitive element

void IonStyle::drawPrimitive(
    PrimitiveElement element,
    QPainter *painter,
    const QRect &rect,
    const QColorGroup &group,
    SFlags flags,
    const QStyleOption &option
                                 ) const
{
    // common locals
    bool down      = flags & Style_Down;
    bool on        = flags & Style_On;
    bool depress   = (down || on);
    bool enabled   = flags & Style_Enabled;
    bool horiz     = flags & Style_Horizontal;
    bool mouseover = highlights_ && (flags & Style_MouseOver);
    int x, y, w, h, x2, y2, n, cx, cy;
    Q3PointArray parray;
    QWidget* widget;

    rect.rect(&x, &y, &w, &h);
    x2 = rect.right();
    y2 = rect.bottom();

    switch(element) {
        case PE_ButtonBevel:
        case PE_ButtonDefault:
        case PE_ButtonDropDown:
        case PE_ButtonTool:
            // TODO?
            drawIonBevel(painter, x, y, w, h, group, group.button(),
                         group.button(), depress);
            break;

        case PE_ButtonCommand:
            if (mouseover)
                drawIonButton(painter, x, y, w, h, group,
                              defaultGlow(group, button),
                              group.highlight(), depress);
            else
                drawIonButton(painter, x, y, w, h, group,
                              group.button(), group.button(), depress);
            break;

        case PE_FocusRect: {
            widget = dynamic_cast<QWidget*>(painter->device());
            if (widget && widget->inherits("QSlider"))
                break;

            QPen old = painter->pen();
            painter->setPen(darken(group.highlight()));

            painter->drawRect(rect);

            painter->setPen(old);
        } break;

        case PE_HeaderSection: {
          // covers kicker taskbar buttons and menu titles
          Q3Header* header = dynamic_cast<Q3Header*>(painter->device());
          widget = dynamic_cast<QWidget*>(painter->device());

          if (header) {
              horiz = (header->orientation() == Horizontal);
          } else {
              horiz = true;
          }

          if ((widget) && ((widget->inherits("QPopupMenu")) ||
                           (widget->inherits("KPopupTitle")))) {
              // kicker/kdesktop menu titles
              drawIonBevel(painter, x, y, w, h, group,
                           group.background(), group.background(),
                           depress, horiz);
          } else if (kicker_) {
              // taskbar buttons (assuming no normal headers used in kicker)
              if (depress) {
                  painter->setPen(group.dark());
                  painter->setBrush(group.brush(QColorGroup::Mid));
                  painter->drawRect(x-1, y-1, w+1, h+1);
              } else {
                  drawIonBevel(painter, x-1, y-1, w+1, h+1, group,
                               group.button(), group.button(),
                               false, horiz);
              }
          } else {
              // other headers
              if (depress) {
                  painter->setPen(group.dark());
                  painter->setBrush(group.brush(QColorGroup::Mid));
                  painter->drawRect(x-1, y-1, w+1, h+1);
              } else {
                  drawIonBevel(painter, x-1, y-1, w+1, h+1, group,
                               group.background(), group.background(),
                               false, horiz);
              }
          }
          break;
      }

        case PE_HeaderArrow:
            if (flags & Style_Up)
                drawPrimitive(PE_ArrowUp, painter, rect, group, Style_Enabled);
            else
                drawPrimitive(PE_ArrowDown, painter, rect, group, Style_Enabled);
          break;

        case PE_ScrollBarAddPage:
        case PE_ScrollBarSubPage:
            if (h) { // has a height, thus visible
                if (flags & Style_Down)
                    painter->fillRect(rect, group.midlight());
                else
                    painter->fillRect(rect, group.mid());
                if (embossed_)
                    painter->setPen(group.dark());
                else
                    painter->setPen(darken(group.mid(), 1.4, 0.15));
                if (horiz) { // vertical
                    painter->drawLine(x, y, x2, y);
                    painter->drawLine(x, y2, x2, y2);
                    if (!embossed_ && !(flags & Style_Down)) {
                        painter->setPen(darken(group.mid()));
                        painter->drawLine(x, y+1, x2, y+1);
                    }
                } else { // horizontal
                    painter->drawLine(x, y, x, y2);
                    painter->drawLine(x2, y, x2, y2);
                    if (!embossed_ && !(flags & Style_Down)) {
                        painter->setPen(darken(group.mid()));
                        painter->drawLine(x+1, y, x+1, y2);
                    }
                }
            }
            break;

        case PE_ScrollBarAddLine:
        case PE_ScrollBarSubLine: {
            if (mouseover) {
                drawIonBevel(painter, x, y, w, h, group,
                             defaultGlow(group, button),
                             group.highlight(),
                             down, horiz, true);
            } else {
                drawIonBevel(painter, x, y, w, h, group,
                             group.button(), group.button(), down,
                             horiz, true);
            }

            PrimitiveElement arrow = ((horiz) ?
                                      ((element == PE_ScrollBarAddLine) ?
                                       PE_ArrowRight : PE_ArrowLeft) :
                                      ((element == PE_ScrollBarAddLine) ?
                                       PE_ArrowDown : PE_ArrowUp));
             if (down) { // shift arrow
                 x++; y++;
             }

//              if (down) { // shift arrow
//                 switch (arrow) {
//                   case PE_ArrowRight: x++; break;
//                   case PE_ArrowLeft:  x--; break;
//                   case PE_ArrowDown:  y++; break;
//                   case PE_ArrowUp:    y--; break;
//                   default: break;
//                 }
//             }

            drawPrimitive(arrow, painter, QRect(x,y,h,w), group, flags);
            break;
        }

        case PE_ScrollBarSlider:
            if (mouseover) {
                drawIonBevel(painter, x, y, w, h, group,
                             defaultGlow(group, button),
                             group.highlight(),
                             false, horiz, true);
            } else {
                drawIonBevel(painter, x, y, w, h, group,
                             group.button(), group.button(),
                             false, horiz, true);
            }
            // draw doodads
            cx = x + (w-BSIZE_DOODAD)/2; cy = y + (h-BSIZE_DOODAD)/2;
            if (horiz && (w >=20)) {
                for (n = -5; n <= 5; n += 5) {
                    kColorBitmaps(painter, group, cx+n, cy,
                                  &doodad_light, &doodad_mid);
                }
            } else if (!horiz && (h >= 20)) {
                for (n = -5; n <= 5; n += 5) {
                    kColorBitmaps(painter, group, cx, cy+n,
                                  &doodad_light, &doodad_mid);
                }
            }
            break;

        case PE_Indicator: {
            cx = x+(w-BSIZE_CHECK)/2;
            cy = y+(h-BSIZE_CHECK)/2;
            int fw = (embossed_ ? 0 : 1);
            if (mouseover) {
                drawIonPanel(painter, x+fw, y+fw,
                             x+BSIZE_CHECK-fw-1, y+BSIZE_CHECK-fw-1,
                             group, true,
                             &group.brush(QColorGroup::Highlight));
                if ( ( flags & Style_On ) || ( flags & Style_Down ) )
                    drawIonPixmap(painter, IM_CHECK,
                                  group.highlightedText(), cx, cy);
            } else {
                drawIonPanel(painter, x+fw, y+fw,
                             x+BSIZE_CHECK-fw-1, y+BSIZE_CHECK-fw-1,
                             group, true, &group.brush(QColorGroup::Base));
                if ( ( flags & Style_On ) || ( flags & Style_Down ) )
                    drawIonPixmap(painter, IM_CHECK,
                                  group.highlight(), cx+fw-1, cy+fw-1);
            }
            break;
        }

        case PE_IndicatorMask: {
            int fw = (embossed_ ? 0 : 1);
            painter->fillRect(x+fw, y+fw, x+BSIZE_CHECK-fw-1,
                              y+BSIZE_CHECK-fw-1, Qt::color1);
            painter->setPen(Qt::color0);
            break;
        }

        case PE_ExclusiveIndicator: {
            cx = x+(w-BSIZE_CHECK)/2;
            cy = y+(h-BSIZE_CHECK)/2;
            QColor fill = (mouseover ? group.highlight() : group.base());

            painter->fillRect(cx, cy, cx + BSIZE_CHECK, cy + BSIZE_CHECK,
                              group.background());
            if (embossed_) {
                drawIonPixmap(painter, IM_CIRCLE_M, group.mid(), cx, cy);
                drawIonPixmap(painter, IM_CIRCLE_L, group.light(), cx, cy);
                drawIonPixmap(painter, IM_CIRCLE_D, group.dark(), cx, cy);
                drawIonPixmap(painter, IM_CIRCLE_B, fill, cx, cy);
            } else {
                drawIonPixmap(painter, IM_CIRCLE_D,
                              darken(group.mid(), 1.4, 0.15), cx, cy);
                drawIonPixmap(painter, IM_CIRCLE_FLAT, fill, cx, cy);
            }

            if ( ( flags & Style_On ) || ( flags & Style_Down ) )
                drawIonPixmap(painter, IM_DOT,
                              (mouseover ? group.highlightedText()
                                         : group.highlight()),
                              cx, cy);
            break;
        }

        case PE_ExclusiveIndicatorMask:
            cx = x+(w-BSIZE_CHECK)/2;
            cy = y+(h-BSIZE_CHECK)/2;
            painter->fillRect(cx, cy, cx + BSIZE_CHECK, cy + BSIZE_CHECK,
                              Qt::color1);
            painter->setPen(Qt::color0);
            break;

        case PE_DockWindowResizeHandle:
            drawIonPanel(painter, x, y, w, h, group, false,
                         &group.brush(QColorGroup::Background));
            break;

        case PE_Splitter:
            cx = x + (w-BSIZE_DOODAD)/2; cy = y + (h-BSIZE_DOODAD)/2;
            painter->fillRect(rect, (hover_ == painter->device())
                                    ? lighten(group.background())
                                    : group.background());

            if (!horiz && (w >=20)) {
                for (n = -5; n <= 5; n += 5) {
                    kColorBitmaps(painter, group, cx+n, cy,
                                  &doodad_light, &doodad_mid);
                }
            } else if (horiz && (h >= 20)) {
                for (n = -5; n <= 5; n += 5) {
                    kColorBitmaps(painter, group, cx, cy+n,
                                  &doodad_light, &doodad_mid);
                }
            }
            break;

        case PE_Panel:
        case PE_PanelTabWidget:
        case PE_TabBarBase:
            widget = dynamic_cast<QWidget*>(painter->device());
            if (widget && widget->inherits("QScrollView"))
                drawIonPanel(painter, x, y, w, h, group,
                             flags & Style_Sunken,
                             &group.brush(QColorGroup::Base));
            else
                drawIonPanel(painter, x, y, w, h, group,
                             flags & Style_Sunken);
            break;

        case PE_PanelLineEdit:
            drawIonPanel(painter, x, y, w, h, group, flags & Style_Sunken,
                         &group.brush(QColorGroup::Base),
                         flags & Style_HasFocus);
            break;

        case PE_PanelPopup:
        case PE_WindowFrame:
            drawIonPanel(painter, x, y, w, h, group);
            break;

      // TODO?
      case PE_Separator:
          painter->setPen(group.dark());
          if (w < h)
              painter->drawLine(w/2, y, w/2, y2);
          else
              painter->drawLine(x, h/2, x2, h/2);
          break;

      case PE_StatusBarSection:
          painter->setPen(group.mid());
          painter->drawLine(x, y,  x2-1, y);
          painter->drawLine(x, y+1, x, y2-1);
          painter->setPen(group.midlight());
          painter->drawLine(x+1, y2, x2, y2);
          painter->drawLine(x2, y+1, x2, y2-1);
          break;

      case PE_PanelMenuBar:
      case PE_PanelDockWindow:
          if (kicker_ && (w == 2)) { // kicker handle separator
              painter->setPen(group.mid());
              painter->drawLine(x, y,  x, y2);
              painter->setPen(group.midlight());
              painter->drawLine(x+1, y,  x+1, y2);
          } else if (kicker_ && (h == 2)) { // kicker handle separator
              painter->setPen(group.mid());
              painter->drawLine(x, y,  x2, y);
              painter->setPen(group.midlight());
              painter->drawLine(x, y+1,  x2, y+1);
          } else {
              horiz = (w > h);
              if (horiz) {
                  painter->setPen(group.light());
                  painter->drawLine(x, y, x2, y);
                  painter->setPen(group.mid());
                  painter->drawLine(x, y2, x2, y2);
                  y++; h-=2;
              } else {
                  painter->setPen(group.light());
                  painter->drawLine(x, y, x, y2);
                  painter->setPen(group.mid());
                  painter->drawLine(x2, y, x2, y2);
                  x++; w-=2;
              }
              drawIonGradient(painter, QRect(x, y, w, h),
                              group.background(), group.background(),
                              horiz);
          }
          break;

        case PE_DockWindowSeparator: {
            widget = dynamic_cast<QWidget*>(painter->device());
            bool flat = true;

            if (widget && widget->parent() &&
                widget->parent()->inherits("QToolBar")) {
                Q3ToolBar *toolbar = ::qt_cast<Q3ToolBar*>(widget->parent());
                if (toolbar) {
                    // toolbar not floating or in a QMainWindow
                    flat = flatToolbar(toolbar);
                }
            }

            if (flat)
                painter->fillRect(rect, group.background());
            else
                drawIonGradient(painter, rect, group.background(),
                                group.background(), horiz);

            if (horiz) {
                int cx = w/2 - 1;
                painter->setPen(group.mid());
                painter->drawLine(cx, 2, cx, h-3);
                painter->setPen(group.midlight());
                painter->drawLine(cx+1, 2, cx+1, h-3);
            } else {
                int cy = h/2 - 1;
                painter->setPen(group.mid());
                painter->drawLine(2, cy, w-3, cy);
                painter->setPen(group.midlight());
                painter->drawLine(2, cy+1, w-3, cy+1);
            }
            break;
        }

      case PE_SizeGrip: {
          int sw = QMIN(h, w) - 1;
          y = y2 - sw;

          if (reverse_) {
              x2 = x + sw;
              for (int n = 0; n < 4; ++n) {
                  painter->setPen(group.dark());
                  painter->drawLine(x, y, x2, y2);
                  painter->setPen(group.midlight());
                  painter->drawLine(x, y+1, x2-1, y2);
                  y += 3;;
                  x2 -= 3;;
              }
          } else {
              x = x2 - sw;
              for (int n = 0; n < 4; ++n) {
                  painter->setPen(group.dark());
                  painter->drawLine(x, y2, x2, y);
                  painter->setPen(group.midlight());
                  painter->drawLine(x+1, y2, x2, y+1);
                  x += 3;
                  y += 3;
              }
          }

          break;
      }

      case PE_CheckMark:
          drawIonPixmap(painter, IM_CHECK_FLAT, group.text(),
                        x+(w-BSIZE_CHECK)/2, y+(h-BSIZE_CHECK)/2);
          break;

      case PE_SpinWidgetPlus:
      case PE_SpinWidgetMinus:
            painter->setPen(enabled ? group.buttonText() : group.mid());
            painter->drawPixmap(x+(w-BSIZE_PLUSMINUS)/2,
                                y+(h-BSIZE_PLUSMINUS)/2,
                                (element==PE_SpinWidgetPlus) ? bplus : bminus);
          break;

#define drawArrow(kind)                                             \
    if (enabled)                                                    \
        drawIonPixmap(painter, IM_##kind##ARROW,                    \
                      (flags & Style_Sunken ? group.midlight()      \
                                            : group.buttonText()),  \
                       x+(w-BSIZE_ARROW)/2, y+(h-BSIZE_ARROW)/2);   \
    else                                                            \
        drawIonPixmap(painter, IM_##kind##ARROW_D, group.mid(),     \
                      x+(w-BSIZE_ARROW)/2, y+(h-BSIZE_ARROW)/2);

        case PE_SpinWidgetUp:
        case PE_ArrowUp:
            drawArrow(U);
            break;

        case PE_SpinWidgetDown:
        case PE_ArrowDown:
            drawArrow(D);
          break;

        case PE_ArrowLeft:
            drawArrow(L);
            break;

        case PE_ArrowRight:
            drawArrow(R);
            break;

      default:
          KStyle::drawPrimitive(element, painter, rect, group, flags, option);
    }
}

//////////////////////////////////////////////////////////////////////////////
// drawKStylePrimitive()
// ---------------------
// Draw the element

void IonStyle::drawKStylePrimitive(KStylePrimitive element,
                                   QPainter *painter,
                                   const QWidget *widget,
                                   const QRect &rect,
                                   const QColorGroup &group,
                                   SFlags flags,
                                   const QStyleOption &option) const
{
    bool horiz = flags & Style_Horizontal;
    int x, y, w, h, x2, y2, n, cx, cy;

    rect.rect(&x, &y, &w, &h);
    x2 = rect.right();
    y2 = rect.bottom();
    cx = x + w/2;
    cy = y + h/2;

    switch (element) {
        case KPE_ToolBarHandle:
            int px=x-1, py=y-1, pw=w+2, ph=h+2;
            getParentRelativeCoords(widget, px, py, pw, ph, horiz);

            cx=x + (w-BSIZE_DOODAD)/2; cy = y + (h-BSIZE_DOODAD)/2;
            drawIonGradient(painter, rect, group.background(),
                            group.background(), horiz, false,
                            px-1, py-1, pw-2, ph-2);
            if (horiz) {
                for (n = -5; n <= 5; n += 5) {
                    kColorBitmaps(painter, group, cx, cy+n,
                                  &doodad_light, &doodad_mid);
                }
            } else {
                for (n = -5; n <= 5; n += 5) {
                    kColorBitmaps(painter, group, cx+n, cy,
                                  &doodad_light, &doodad_mid);
                }
            }
            break;

        //case KPE_DockWindowHandle:
        case KPE_GeneralHandle:
            cx=x + (w-BSIZE_DOODAD)/2; cy = y + (h-BSIZE_DOODAD)/2;
            painter->fillRect(rect, group.brush(QColorGroup::Background));

            if (horiz) {
                for (n = -5; n <= 5; n += 5) {
                    kColorBitmaps(painter, group, cx, cy+n,
                                  &doodad_light, &doodad_mid);
                }
            } else {
                for (n = -5; n <= 5; n += 5) {
                    kColorBitmaps(painter, group, cx+n, cy,
                                  &doodad_light, &doodad_mid);
                }
            }
            break;

        case KPE_ListViewExpander:
            cx = x+(w-BSIZE_PLUSMINUS)/2;
            cy = y+(h-BSIZE_PLUSMINUS)/2;
            painter->setPen(blend(group.text(), group.base(), 0.6));
            painter->drawRect(cx-2, cy-2,
                              BSIZE_PLUSMINUS+4, BSIZE_PLUSMINUS+4);
            painter->setPen(blend(group.text(), group.base(), 0.4));
            if (flags & Style_On)
                painter->drawPixmap(cx, cy, bplus);
            else
                painter->drawPixmap(cx, cy, bminus);
            break;

        case KPE_ListViewBranch:
            painter->setPen(blend(group.text(), group.base(), 0.8));
            if (flags & Style_Horizontal)
                painter->drawLine(x, cy, x2, cy);
            else
                painter->drawLine(cx, y, cx, y2+1);
            break;

        case KPE_SliderGroove: {
            const QSlider* slider = ::qt_cast<const QSlider*>(widget);
            if (slider) {
                if (slider->orientation() == Horizontal) {
                    y = cy - 3;
                    h = 7;
                } else {
                    x = cx - 3;
                    w = 7;
                }
            }
            drawIonPanel(painter, x, y, w, h, group, true,
                         &group.brush(QColorGroup::Mid), widget->hasFocus());
            } break;

        case KPE_SliderHandle: {
            const QSlider* slider = ::qt_cast<const QSlider*>(widget);
            if (slider) {
                QColor color = (widget==hover_)
                    ? defaultGlow(group, button)
                    : group.button();
                QColor glow = (widget==hover_)
                    ? group.highlight()
                    : group.button();
                if (slider->orientation() == Horizontal) {
                    drawIonBevel(painter, cx-4, y, 8, h, group,
                                 color, glow, false, true);
                } else {
                    drawIonBevel(painter, x, cy-4, w, 8, group,
                                 color, glow, false, false);
                }
            }
            break;
        }

        default:
            KStyle::drawKStylePrimitive(element, painter, widget, rect,
                                        group, flags, option);
    }
}

//////////////////////////////////////////////////////////////////////////////
// drawControl()
// -------------
// Draw the control

void IonStyle::drawControl(ControlElement element,
                           QPainter *painter,
                           const QWidget *widget,
                           const QRect &rect,
                           const QColorGroup &group,
                           SFlags flags,
                           const QStyleOption &option) const
{
    bool active, enabled, depress;
    int x, y, w, h, x2, y2, dx;
    QMenuItem *mi;
    QIcon::Mode mode;
    QIcon::State state;
    QPixmap pixmap;

    rect.rect(&x, &y, &w, &h);
    x2 = rect.right();
    y2 = rect.bottom();

    switch (element) {
        case CE_PushButton: {
            depress = flags & (Style_Down | Style_On);

            if (flags & (Style_Top | Style_Bottom)) {
                // tab buttons ala KDevelop
                bool horizontal = (flags & Style_Horizontal);
                // vertical buttons are actually drawn rotated, so draw them
                // as horizontal and top, but munge the light direction
                bool reversed = false, rotated = false;
                if (!horizontal && w > h) {
                    if (flags & Style_Bottom) {
                        flags &= ~Style_Bottom;
                        reversed = true;
                    }
                    flags |= Style_Top | Style_Horizontal;
                    horizontal = true;
                    rotated = true;
                }
                if (!depress) {
                    painter->fillRect(rect, group.background());
                    if (horizontal) {
                        if (flags & Style_Top)
                            y += 2;
                        h -= 2;
                    } else {
                        if (flags & Style_Top)
                            x += 2;
                        w -= 2;
                    }
                }
                if (widget==hover_) {
                    drawIonEdge(painter, x, y, x+w-1, y+h-1,
                                group.mid(), group.dark(), group.light(),
                                reversed, rotated);
                    QRect rect(x+1, y+1, w-2, h-2);
                    if (depress)
                        drawIonGradient(painter, rect,
                                        defaultGlow(group, button),
                                        group.highlight(), horizontal, true);
                    else
                        drawIonGradient(painter, rect,
                                        blend(darken(group.background()),
                                              group.highlight(), DEF_GLOW_K),
                                        group.highlight(), horizontal);
                } else {
                    drawIonEdge(painter, x, y, x+w-1, y+h-1,
                                group.mid(), group.dark(), group.light(),
                                reversed, rotated);

                    QRect rect(x+1, y+1, w-2, h-2);
                    QColor base = (depress ? lighten(group.button())
                                           : darken(group.background(),
                                                    1.0, 0.1));

                    drawIonGradient(painter, rect, base, base,
                                    horizontal, depress);
                }
            } else {
                bool isDefault = (flags & Style_ButtonDefault ? true : false);

                if (widget==hover_) {
                    drawIonButton(painter, x, y, w, h, group,
                                  defaultGlow(group, button),
                                  group.highlight(), depress, isDefault);
                } else {
                    drawIonButton(painter, x, y, w, h, group,
                                  group.button(), group.button(),
                                  depress, isDefault);
                }

                if (flags & Style_HasFocus) { // draw focus
                    drawPrimitive(PE_FocusRect, painter,
                                  subRect(SR_PushButtonFocusRect, widget),
                                  group, flags);
                }
            }
            break;
        }

        case CE_PushButtonLabel: {
            const QPushButton* button = ::qt_cast<const QPushButton*>(widget);
            if (!button) {
                KStyle::drawControl(element, painter, widget, rect, group,
                                    flags, option);
                return;
            }
            active = button->isOn() || button->isDown();

            if (flags & (Style_Top | Style_Bottom)) {
                // tab buttons ala KDevelop
                bool horizontal = (flags & Style_Horizontal);
                // vertical buttons are actually drawn rotated, so draw them
                // as horizontal and top
                if (!horizontal && w > h) {
                    if (flags & Style_Bottom)
                        flags &= ~Style_Bottom;
                    flags |= Style_Top | Style_Horizontal;
                    horizontal = true;
                }
                if (active) {
                    flags |= Style_Sunken;
                } else {
                    if (horizontal) {
                        if (flags & Style_Top)
                            y += 2;
                        h -= 2;
                    } else {
                        if (flags & Style_Top)
                            x += 2;
                        w -= 2;
                    }
                }
            } else if (active) { // shift contents
                x++; y++;
                flags |= Style_Sunken;
            }

            if (button->isMenuButton()) { // menu indicator
                int dx = pixelMetric(PM_MenuButtonIndicator, widget);
                drawPrimitive(PE_ArrowDown, painter,
                              QRect(x+w-dx-2, y+2, dx, h-4),
                              group, flags, option);
                w -= dx;
            }

            if (button->iconSet() && !button->iconSet()->isNull()) { // draw icon
                if (button->isEnabled()) {
                    if (button->hasFocus()) {
                        mode = QIcon::Active;
                    } else {
                        mode = QIcon::Normal;
                    }
                } else {
                    mode = QIcon::Disabled;
                }

                if (button->isToggleButton() && button->isOn()) {
                    state = QIcon::On;
                } else {
                    state = QIcon::Off;
                }

                pixmap = button->iconSet()->pixmap(QIcon::Small, mode, state);
                if (button->text().isEmpty() && !button->pixmap()) {
                    painter->drawPixmap(x+w/2 - pixmap.width()/2,
                                        y+h/2 - pixmap.height()/2, pixmap);
                } else {
                    painter->drawPixmap(x+4, y+h/2 - pixmap.height()/2, pixmap);
                }
                x += pixmap.width() + 4;
                w -= pixmap.width() + 4;
            }

            if (active || button->isDefault()) { // default button
                for(int n=0; n<2; n++) {
                    drawItem(painter, QRect(x+n, y, w, h),
                             AlignCenter | ShowPrefix,
                             button->colorGroup(),
                             button->isEnabled(),
                             button->pixmap(),
                             button->text(), -1,
                             (button->isEnabled()) ?
                             &button->colorGroup().buttonText() :
                             &button->colorGroup().mid());
                }
            } else { // normal button
                QColor fore;
                if (!button->isEnabled())
                    fore = group.mid();
                else if (flags & (Style_Top | Style_Bottom))
                    fore = group.foreground();
                else
                    fore = group.buttonText();
                drawItem(painter, QRect(x, y, w, h),
                         AlignCenter | ShowPrefix,
                         button->colorGroup(),
                         button->isEnabled(),
                         button->pixmap(),
                         button->text(), -1,
                         &fore);
            }
            break;
        }

        case CE_CheckBox:
            if (widget==hover_)
                flags |= Style_MouseOver;
            drawPrimitive(PE_Indicator, painter, rect, group, flags);
            break;

        case CE_RadioButton: {
            if (widget==hover_)
                flags |= Style_MouseOver;
            drawPrimitive(PE_ExclusiveIndicator, painter, rect, group, flags);
            break;
        }

        case CE_CheckBoxLabel:
        case CE_RadioButtonLabel: {
            const QButton *b = ::qt_cast<const QButton*>(widget);
            if (!b) return;

            int alignment = reverse_ ? AlignRight : AlignLeft;
            drawItem(painter, rect, alignment | AlignVCenter | ShowPrefix,
                     group, flags & Style_Enabled, b->pixmap(), b->text());

            // only draw focus if content (forms on html won't)
            if ((flags & Style_HasFocus) && ((!b->text().isNull()) || b->pixmap())) {
                drawPrimitive(PE_FocusRect, painter,
                              visualRect(subRect(SR_RadioButtonFocusRect,
                                                 widget), widget),
                              group, flags);
            }
            break;
        }

      case CE_DockWindowEmptyArea:  {
          const Q3ToolBar *tb = ::qt_cast<const Q3ToolBar*>(widget);
          if (tb) {
              // toolbar not floating or in a QMainWindow
              if (flatToolbar(tb)) {
                  if (tb->backgroundMode() == PaletteButton)
                      // force default button color to background color
                      painter->fillRect(rect, group.background());
                  else
                      painter->fillRect(rect, tb->paletteBackgroundColor());
              }
          }
          break;
      }

      case CE_MenuBarEmptyArea:
          drawIonGradient(painter, QRect(x, y, w, h),
                          group.background(), group.background(), (w>h));
          break;

        case CE_MenuBarItem: {
            const QMenuBar *mbar = ::qt_cast<const QMenuBar*>(widget);
            if (!mbar) {
                KStyle::drawControl(element, painter, widget, rect, group,
                                    flags, option);
                return;
            }
            mi = option.menuItem();
            QRect prect = mbar->rect();
            const QColor *ctext = NULL;

            if ((flags & Style_Active) && (flags & Style_HasFocus)) {
                if (flags & Style_Down) {
                    drawIonPanel(painter, x, y, w, h, group.highlight(), 5.0);
                    ctext = &group.highlightedText();
                } else {
                    drawIonEdge(painter, x, y, x2, y2, group.background(),
                                group.mid(), group.light());
                    drawIonGradient(painter, QRect(x+1, y+1, w-2, h-2),
                                    lighten(darken(group.background(), -0.8), 0.2, 0.1),
                                    lighten(group.background(), 1.0, 0.2),
                                    true, true);
                }
            } else {
                drawIonGradient(painter, rect,
                               group.background(), group.background(),
                               true, false, x-1, y-1,
                               prect.width()-2, prect.height()-2);
            }
            drawItem(painter, rect,
                     AlignCenter | AlignVCenter |
                     DontClip | ShowPrefix | SingleLine,
                     group, flags & Style_Enabled,
                     mi->pixmap(), mi->text(), -1, ctext);
            break;
        }

        case CE_PopupMenuItem: {
            const Q3PopupMenu *popup = ::qt_cast<const Q3PopupMenu*>(widget);
            if (!popup) {
                KStyle::drawControl(element, painter, widget, rect, group,
                                    flags, option);
                return;
            }

            mi = option.menuItem();
            if (!mi) {
                painter->fillRect(rect, group.button());
                break;
            }

            int tabwidth   = option.tabWidth();
            int checkwidth = option.maxIconWidth();
            bool checkable = popup->isCheckable();
            bool etchtext  = styleHint(SH_EtchDisabledText);
            active         = flags & Style_Active;
            enabled        = mi->isEnabled();
            QRect vrect;

            if (checkable) checkwidth = QMAX(checkwidth, 20);

            // draw background
            if (active && enabled) {
                drawIonPanel(painter, x, y, w, h, group.highlight(), 5.0);
            } else if (widget->erasePixmap() &&
                        !widget->erasePixmap()->isNull()) {
                painter->drawPixmap(x, y, *widget->erasePixmap(), x, y, w, h);
            } else {
                painter->fillRect(x, y, w, h, group.background());
            }

            // draw separator
            if (mi->isSeparator()) {
                painter->setPen(group.light());
                painter->drawLine(x+4, y+4, x2-4, y+4);
                painter->setPen(group.mid());
                painter->drawLine(x+4, y+3, x2-4, y+3);
                break;
            }

            // draw icon
            if (mi->iconSet() && !mi->isChecked()) {
                if (active)
                    mode = enabled ? QIcon::Active : QIcon::Disabled;
                else
                    mode = enabled ? QIcon::Normal : QIcon::Disabled;

                pixmap = mi->iconSet()->pixmap(QIcon::Small, mode);
                QRect pmrect(0, 0, pixmap.width(), pixmap.height());
                vrect = visualRect(QRect(x, y, checkwidth, h), rect);
                pmrect.moveCenter(vrect.center());
                painter->drawPixmap(pmrect.topLeft(), pixmap);
            }

            // draw check
            if (mi->isChecked()) {
                int cx = reverse_ ? x+w - checkwidth : x;
                drawPrimitive(PE_CheckMark, painter,
                                QRect(cx + ITEMFRAME, y + ITEMFRAME,
                                      checkwidth - ITEMFRAME*2, h - ITEMFRAME*2),
                                group, Style_Default |
                                (active ? Style_Enabled : Style_On));
            }

            // draw text
            int xm = ITEMFRAME + checkwidth + ITEMHMARGIN;
            int xp = reverse_ ?
                x + tabwidth + RIGHTBORDER + ITEMHMARGIN + ITEMFRAME - 1 :
                x + xm;
            int offset = reverse_ ? -1 : 1;
            int tw = w - xm - tabwidth - ARROWMARGIN - ITEMHMARGIN * 3
                - ITEMFRAME + 1;

            painter->setPen(enabled ? (active ? group.highlightedText() :
                                       group.buttonText()) : group.mid());

            if (mi->custom()) { // draws own label
                painter->save();
                if (etchtext && !enabled && !active) {
                    painter->setPen(group.light());
                    mi->custom()->paint(painter, group, active, enabled,
                                        xp+offset, y+ITEMVMARGIN+1,
                                        tw, h-2*ITEMVMARGIN);
                    painter->setPen(group.mid());
                }
                mi->custom()->paint(painter, group, active, enabled,
                                    xp, y+ITEMVMARGIN, tw, h-2*ITEMVMARGIN);
                painter->restore();
            } else { // draw label
                QString text = mi->text();

                if (!text.isNull()) {
                    int t = text.find('\t');
                    int tflags = AlignVCenter | DontClip |
                                 ShowPrefix |  SingleLine |
                                 (reverse_ ? AlignRight : AlignLeft);

                    if (t >= 0) {
                        int tabx = reverse_ ?
                            x + RIGHTBORDER + ITEMHMARGIN + ITEMFRAME :
                            x + w - tabwidth - RIGHTBORDER - ITEMHMARGIN
                            - ITEMFRAME;

                        // draw right label (accerator)
                        if (etchtext && !enabled) { // etched
                            painter->setPen(group.light());
                            painter->drawText(tabx+offset, y+ITEMVMARGIN+1,
                                              tabwidth, h-2*ITEMVMARGIN,
                                              tflags, text.mid(t+1));
                            painter->setPen(group.mid());
                        }
                        painter->drawText(tabx, y+ITEMVMARGIN,
                                          tabwidth, h-2*ITEMVMARGIN,
                                          tflags, text.mid(t+1));
                        text = text.left(t);
                    }

                    // draw left label
                    if (etchtext && !enabled) { // etched
                        painter->setPen(group.light());
                        painter->drawText(xp+offset, y+ITEMVMARGIN+1,
                                          tw, h-2*ITEMVMARGIN,
                                          tflags, text, t);
                        painter->setPen(group.mid());
                    }
                    painter->drawText(xp, y+ITEMVMARGIN,
                                      tw, h-2*ITEMVMARGIN,
                                      tflags, text, t);
                } else if (mi->pixmap()) { // pixmap as label
                    pixmap = *mi->pixmap();
                    if (pixmap.depth() == 1)
                        painter->setBackgroundMode(OpaqueMode);

                    dx = ((w - pixmap.width()) / 2) + ((w - pixmap.width()) % 2);
                    painter->drawPixmap(x+dx, y+ITEMFRAME, pixmap);

                    if (pixmap.depth() == 1)
                        painter->setBackgroundMode(TransparentMode);
                }
            }

            if (mi->popup()) { // draw submenu arrow
                PrimitiveElement arrow = reverse_ ? PE_ArrowLeft : PE_ArrowRight;
                int dim = (h-2*ITEMFRAME) / 2;
                vrect = visualRect(QRect(x + w - ARROWMARGIN - ITEMFRAME - dim,
                                         y + h / 2 - dim / 2, dim, dim), rect);
                drawPrimitive(arrow, painter, vrect, group,
                              enabled ? Style_Enabled : Style_Default);
            }
            break;
        }

      case CE_TabBarTab: {
          const QTabBar* tab = ::qt_cast<const QTabBar*>(widget);
          if (tab) {
              if ((widget == hover_) && (option.tab() == hovertab_)) {
                  flags |= Style_MouseOver;
              }
              // this guy can get complicated, we we do it elsewhere
              drawIonTab(painter, x, y, w, h, group, tab, option, flags);
          } else { // not a tabbar
              KStyle::drawControl(element, painter, widget, rect, group,
                                  flags, option);
              return;
          }
          break;
      }

        case CE_ProgressBarGroove: {
            drawIonPanel(painter, x, y, w, h, group, true,
                         &group.brush(QColorGroup::Mid));
            break;
        }

        case CE_ProgressBarLabel:
        case CE_ProgressBarContents: {
            const Q3ProgressBar* pbar = ::qt_cast<const Q3ProgressBar*>(widget);
            if (!pbar) {
                KStyle::drawControl(element, painter, widget, rect, group,
                                    flags, option);
                return;
            }
            int px, pw;
            subRect(SR_ProgressBarContents, widget).rect(&x, &y, &w, &h);
            px = x;

            if (!pbar->totalSteps()) {
                // busy indicator
                int bar = pixelMetric(PM_ProgressBarChunkWidth, widget);
                int progress = pbar->progress() % ((w-bar) * 2);
                if (progress > (w-bar)) progress = 2 * (w-bar) - progress;
                px += progress;
                pw = bar;
            } else {
                double progress = static_cast<double>(pbar->progress()) /
                    static_cast<double>(pbar->totalSteps());
                dx = static_cast<int>(w * progress);
                if (reverse_) px += w - dx;
                pw = dx;
            }
            if (element == CE_ProgressBarContents) {
                if (pw >= 4)
                    drawIonProgressBar(painter, px, y, pw, h, group);
            } else {
                const QString& text = pbar->progressString();

                // Are we drawing inside or out?
                QRect irect = rect.intersect(QRect(x, y, w, h));
                if (irect.width() < 4) {
                    painter->setPen(group.foreground());
                    painter->drawText(rect, AlignCenter, text);
                    break;
                }

                painter->save();

                // Draw first part (if any)
                if (x < px) {
                    painter->setPen(group.buttonText());
                    painter->setClipRect(QRect(x, y, px - x, h));
                    painter->drawText(rect, AlignCenter, text);
                }
                // Draw middle part (if any)
                if (pw > 0) {
                    painter->setPen(group.highlightedText());
                    painter->setClipRect(QRect(px, y, pw, h));
                    painter->drawText(rect, AlignCenter, text);
                }
                // Draw last part (if any)
                if (px + pw < x + w) {
                    painter->setPen(group.buttonText());
                    painter->setClipRect(QRect(px + pw, y,
                                               x + w - px - pw, h));
                    painter->drawText(rect, AlignCenter, text);
                }

                painter->restore();
            }
            break;
        }

#if (QT_VERSION >= 0x030200)
      case CE_ToolBoxTab: {
          const QToolBox *box = ::qt_cast<const QToolBox*>(widget);
          if (!box) {
              KStyle::drawControl(element, painter, widget, rect, group,
                                  flags, option);
              return;
          }

          const int rx = x2 - 20;
          const int cx = rx - h + 1;

          Q3PointArray parray(6);
          parray.putPoints(0, 6,
                           x-1,y, cx,y, rx-2,y2-2, x2+1,y2-2,
                           x2+1,y2+2, x-1,y2+2);

          if (box->currentItem() && (flags & Style_Selected)) {
              painter->setPen(group.dark());
              painter->setBrush(box->currentItem()->paletteBackgroundColor());
              painter->drawConvexPolygon(parray, 0, 6);
              painter->setBrush(Qt::NoBrush);
          } else {
              painter->setClipRegion(parray, QPainter::CoordPainter);
              drawIonGradient(painter, rect,
                              group.background(), group.background());
              painter->setClipping(false);
              painter->drawPolyline(parray, 0, 4);
          }

          parray.putPoints(0, 4, x,y+1, cx,y+1, rx-2,y2-1, x2,y2-1);
          painter->setPen(group.midlight());
          painter->drawPolyline(parray, 0, 4);

          break;
      }
#endif

      default:
          KStyle::drawControl(element, painter, widget, rect, group,
                              flags, option);
    }
}

//////////////////////////////////////////////////////////////////////////////
// drawControlMask()
// -----------------
// Draw a bitmask for the element

void IonStyle::drawControlMask(ControlElement element,
                               QPainter *painter,
                               const QWidget *widget,
                               const QRect &rect,
                               const QStyleOption &option) const
{
    switch (element) {
      case CE_PushButton:
          painter->fillRect(rect, Qt::color1);
          painter->setPen(Qt::color0);
          break;

      default:
          KStyle::drawControlMask(element, painter, widget, rect, option);
    }
}

//////////////////////////////////////////////////////////////////////////////
// drawComplexControl()
// --------------------
// Draw a complex control

void IonStyle::drawComplexControl(ComplexControl control,
                                  QPainter *painter,
                                  const QWidget *widget,
                                  const QRect &rect,
                                  const QColorGroup &group,
                                  SFlags flags,
                                  SCFlags controls,
                                  SCFlags active,
                                  const QStyleOption &option) const
{
    bool down = flags & Style_Down;
    bool on = flags & Style_On;
    bool raised = flags & Style_Raised;
    bool sunken = flags & Style_Sunken;
    QRect subrect;
    int x, y, w, h, x2, y2;
    rect.rect(&x, &y, &w, &h);

    switch (control) {
        case CC_ComboBox: {
            const QComboBox * combo = ::qt_cast<const QComboBox*>(widget);
            if (!combo) {
                KStyle::drawComplexControl(control, painter, widget, rect,
                                           group, flags, controls, active,
                                           option);
                return;
            }

            bool editstyle = (controls & SC_ComboBoxEditField)
                           && combo->editable();
            sunken = (active == SC_ComboBoxArrow);

            // draw editable boxes totally differently
            if (editstyle) {
                drawIonPanel(painter, x, y, w, h, group, true,
                             (combo->isEnabled()
                                     ? &group.brush(QColorGroup::Base)
                                     : &group.brush(QColorGroup::Background)),
                             combo->hasFocus());

                if (controls & SC_ComboBoxArrow) {
                    // update rect to draw just the arrow box
                    subrect = querySubControlMetrics(CC_ComboBox, widget,
                                                     SC_ComboBoxArrow);
                    subrect = visualRect(subrect, widget);
                    subrect.rect(&x, &y, &w, &h);
                    int slot = QMAX(h/3, 10) + (h % 2);
                    int fw = (embossed_ ? 0 : 1);
                    y += (h/2)-(slot/2)+fw;
                    h = slot-fw;
                    w -= 1+fw;
                } else {
                    // skip remaining code if no arrow box
                    painter->setPen(group.buttonText()); // for subsequent text
                    break;
                }
            }

            // not editable, or only drawing the arrow box
            if (widget==hover_) {
                drawIonButton(painter, x, y, w, h, group,
                              defaultGlow(group, button),
                              group.highlight(), sunken);
            } else {
                drawIonButton(painter, x, y, w, h, group,
                              group.button(), group.button(), sunken);
            }

            if (!editstyle) {
                if (controls & SC_ComboBoxArrow) {
                    // draw arrow box
                    subrect = querySubControlMetrics(CC_ComboBox, widget,
                                                     SC_ComboBoxArrow);
                    subrect = visualRect(subrect, widget);
                    subrect.rect(&x, &y, &w, &h);

                    int slot = QMAX(h/4, 6) + (h % 2);
                    int fw = (embossed_ ? 0 : 1);
                    drawIonPanel(painter, x+5-fw, y+(h/2)-(slot/2)+fw,
                                 w-7, slot-fw, group, true,
                                 sunken ? &group.brush(QColorGroup::Midlight)
                                        : &group.brush(QColorGroup::Mid));
                }

                if ((controls & SC_ComboBoxEditField) && combo->hasFocus()) {
                    subrect = visualRect(subRect(SR_ComboBoxFocusRect, combo),
                                         widget);
                    drawPrimitive(PE_FocusRect, painter, subrect, group,
                                  Style_FocusAtBorder,
                                  QStyleOption(group.highlight()));
                 }
            }

            painter->setPen(group.buttonText()); // for subsequent text
            break;
        }

        case CC_ScrollBar: {
            const QScrollBar *sb = ::qt_cast<const QScrollBar*>(widget);
            if (!sb) {
                KStyle::drawComplexControl(control, painter, widget,
                                           rect, group, flags, controls,
                                           active, option);
                return;
            }

            QRect srect;
            bool horizontal = (sb->orientation() == Qt::Horizontal);
            SFlags scrollflags = (horizontal ? Style_Horizontal
                                             : Style_Default);

            if (!sb->isEnabled() || sb->minValue() == sb->maxValue())
                scrollflags &= ~Style_Enabled;
            else {
                scrollflags |= Style_Enabled;
                if (widget == hover_) // No hover unless enabled
                    scrollflags |= Style_MouseOver;
            }

            // addline
            if (controls & SC_ScrollBarAddLine) {
                srect = querySubControlMetrics(control, widget,
                                                SC_ScrollBarAddLine, option);
                if (srect.isValid())
                    drawPrimitive(PE_ScrollBarAddLine, painter, srect, group,
                                  scrollflags | (active == SC_ScrollBarAddLine
                                                ? Style_Down : Style_Default));
            }

            // subline
            if (controls & SC_ScrollBarSubLine) {
                srect = querySubControlMetrics(control, widget,
                                                SC_ScrollBarSubLine, option);
                if (srect.isValid())
                    drawPrimitive(PE_ScrollBarSubLine, painter, srect, group,
                                  scrollflags | (active == SC_ScrollBarSubLine
                                                ? Style_Down : Style_Default));
            }

            // addpage
            if (controls & SC_ScrollBarAddPage) {
                srect = querySubControlMetrics(control, widget,
                                                SC_ScrollBarAddPage, option);
                if (srect.isValid()) {
                    drawPrimitive(PE_ScrollBarAddPage, painter, srect, group,
                                  scrollflags | (active == SC_ScrollBarAddPage
                                                ? Style_Down : Style_Default));
                }
            }

            // subpage
            if (controls & SC_ScrollBarSubPage) {
                srect = querySubControlMetrics(control, widget,
                                                SC_ScrollBarSubPage, option);
                if (srect.isValid()) {
                    drawPrimitive(PE_ScrollBarSubPage, painter, srect, group,
                                  scrollflags | (active == SC_ScrollBarSubPage
                                                ? Style_Down : Style_Default));
                }
            }

            // slider
            if (controls & SC_ScrollBarSlider) {
                if (sb->minValue() == sb->maxValue()) {
                    // maxed out
                    srect = querySubControlMetrics(control, widget,
                                                   SC_ScrollBarGroove, option);
                    // don't draw the slider, draw an empty groove
                    if (srect.isValid()) {
                        drawPrimitive(PE_ScrollBarSubPage, painter, srect,
                                      group, scrollflags);
                    }
                } else {
                    srect = querySubControlMetrics(control, widget,
                                                   SC_ScrollBarSlider, option);
                    if (srect.isValid()) {
                        int pflags = scrollflags;
                        if (active == SC_ScrollBarSlider)
                            pflags |= Style_Down;

                        drawPrimitive(PE_ScrollBarSlider, painter,
                                    srect, group, pflags);
                        // focus
                        if (sb->hasFocus()) {
                            srect.addCoords(2, 2, -2, -2);
                            drawPrimitive(PE_FocusRect, painter, srect, group,
                                        Style_Default);
                        }
                    }
                }
            }
            break;
        }

        case CC_Slider: {
            // adapted from kstyle.cpp, so we can skip drawing the focus rect
            const QSlider *slider = ::qt_cast<const QSlider*>(widget);
            if (!slider) {
                KStyle::drawComplexControl(control, painter, widget,
                                           rect, group, flags, controls,
                                           active, option);
                return;
            }
            QRect groove = querySubControlMetrics(CC_Slider, widget,
                                                  SC_SliderGroove, option);
            QRect handle = querySubControlMetrics(CC_Slider, widget,
                                                  SC_SliderHandle, option);

            painter->fillRect(rect, group.background());

            if ((controls & SC_SliderGroove) && groove.isValid())
                drawKStylePrimitive(KPE_SliderGroove, painter, widget, groove,
                                    group, flags, option);

            // Draw the tickmarks
            if (controls & SC_SliderTickmarks)
                QCommonStyle::drawComplexControl(control, painter, widget,
                                                 rect, group, flags,
                                                 SC_SliderTickmarks,
                                                 active, option);

            // Draw the slider handle
            if ((controls & SC_SliderHandle) && handle.isValid()) {
                if (active == SC_SliderHandle)
                    flags |= Style_Active;
                drawKStylePrimitive(KPE_SliderHandle, painter, widget, handle,
                                    group, flags, option);
            }
            break;
        }

        case CC_SpinWidget: {
            const Q3SpinWidget *spin = ::qt_cast<const Q3SpinWidget*>(widget);
            if (!spin) {
                KStyle::drawComplexControl(control, painter, widget, rect, group,
                                            flags, controls, active, option);
                return;
            }

            PrimitiveElement element;

            // draw frame
            if (controls & SC_SpinWidgetFrame) {
                drawIonPanel(painter, x, y, w, h, group, true,
                             &group.brush(QColorGroup::Base),
                             spin->hasFocus());
            }

            // draw up arrow
            if (controls & SC_SpinWidgetUp) {
                int tflags = flags;
                subrect = spin->upRect();

                sunken = (active == SC_SpinWidgetUp);

                if (widget==hover_) {
                    drawIonBevel(painter, subrect.x(), subrect.y(),
                                 subrect.width(), subrect.height(), group,
                                 defaultGlow(group, button),
                                 group.highlight(), sunken);
                } else {
                    drawIonBevel(painter, subrect.x(), subrect.y(),
                                 subrect.width(), subrect.height(), group,
                                 group.button(), group.button(), sunken);
                }

                if (spin->buttonSymbols() == Q3SpinWidget::PlusMinus)
                    element = PE_SpinWidgetPlus;
                else
                    element = PE_SpinWidgetUp;
                if (!spin->isUpEnabled()) tflags &= ~Style_Enabled;

                drawPrimitive(element, painter, subrect, group, tflags
                              | ((active == SC_SpinWidgetUp)
                                 ? Style_On | Style_Sunken : Style_Raised));
            }

            // draw down button
            if (controls & SC_SpinWidgetDown) {
                int tflags = flags;
                subrect = spin->downRect();

                sunken = (active == SC_SpinWidgetDown);

                if (widget==hover_) {
                    drawIonBevel(painter, subrect.x(), subrect.y(),
                                 subrect.width(), subrect.height(), group,
                                 defaultGlow(group, button),
                                 group.highlight(), sunken);
                } else {
                    drawIonBevel(painter, subrect.x(), subrect.y(),
                                 subrect.width(), subrect.height(), group,
                                 group.button(), group.button(), sunken);
                }

                if (spin->buttonSymbols() == Q3SpinWidget::PlusMinus)
                    element = PE_SpinWidgetMinus;
                else
                    element = PE_SpinWidgetDown;
                if (!spin->isDownEnabled()) tflags &= ~Style_Enabled;

                drawPrimitive(element, painter, subrect, group, tflags
                              | ((active == SC_SpinWidgetDown)
                                 ? Style_On | Style_Sunken : Style_Raised));
            }
            break;
        }

        case CC_ToolButton: {
            const QToolButton *btn = ::qt_cast<const QToolButton*>(widget);
            if (!btn) {
                KStyle::drawComplexControl(control, painter, widget, rect, group,
                                           flags, controls, active, option);
                return;
            }

            Q3ToolBar *toolbar;
            bool horiz = true;
            bool normal = !(down || on || raised); // normal button state

            x2 = rect.right();
            y2 = rect.bottom();

            // check for QToolBar parent
            if (btn->parent() && btn->parent()->inherits("QToolBar")) {
                toolbar = ::qt_cast<Q3ToolBar*>(btn->parent());
                if (toolbar) {
                    horiz = (toolbar->orientation() == Qt::Horizontal);
                    if (normal) { // draw background
                        if (flatToolbar(toolbar)) {
                            // toolbar not floating or in a QMainWindow
                            painter->fillRect(rect, group.background());
                        } else {
                            drawIonGradient(painter, rect,
                                            group.background(),
                                            group.background(),
                                            horiz, false, 0, 0,
                                            toolbar->width()-2,
                                            toolbar->height()-2);
                        }
                    }
                }
            }
            // check for QToolBarExtensionWidget parent
            else if (btn->parent() &&
                    btn->parent()->inherits(QTOOLBAREXTENSION)) {
                QWidget *extension;
                if ((extension = ::qt_cast<QWidget*>(btn->parent()))) {
                    toolbar = ::qt_cast<Q3ToolBar*>(extension->parent());
                    if (toolbar) {
                        horiz = (toolbar->orientation() == Qt::Horizontal);
                        if (normal) { // draw background
                            drawIonGradient(painter, rect,
                                            group.background(),
                                            group.background(),
                                            horiz, true, 0, 0,
                                            toolbar->width()-2,
                                            toolbar->height()-2);
                        }
                    }
                }
            }
            // check for background pixmap
            else if (normal && btn->parentWidget() &&
                     btn->parentWidget()->backgroundPixmap() &&
                     !btn->parentWidget()->backgroundPixmap()->isNull()) {
                QPixmap pixmap = *(btn->parentWidget()->backgroundPixmap());
                painter->drawTiledPixmap(rect, pixmap, btn->pos());
            }
            // everything else
            else if (normal) {
                // toolbutton not on a toolbar
                painter->fillRect(rect, group.background());
            }

            // now draw active buttons
            if (down || on || raised) {
                QRect rfill(x+1, y+1, w-2, h-2);

                drawIonEdge(painter, x, y, x2, y2, group.button(),
                            group.mid(), group.light(), down || on);

                if (raised) {
                    // Raised doesn't mean hover!
                    if (widget==hover_) {
                        drawIonGradient(painter, rfill,
                                        lighten(darken(group.button(), -0.8), 0.2, 0.1),
                                        lighten(group.button(), 1.0, 0.2),
                                        true, true);
                    } else {
                        drawIonGradient(painter, rfill, group.button(),
                                        group.button());
                    }
                } else if (down) {
                    drawIonGradient(painter, rfill, defaultGlow(group, button),
                                    group.highlight(), true, true);
                } else if (on) {
                    drawIonGradient(painter, rfill, group.button(),
                                    group.button(), true, true);
                }
            }
            painter->setPen(group.text());
            break;
        }

        default:
            KStyle::drawComplexControl(control, painter, widget, rect, group,
                                        flags, controls, active, option);
            break;
    }
}

//////////////////////////////////////////////////////////////////////////////
// drawComplexControlMask()
// ------------------------
// Draw a bitmask for the control

void IonStyle::drawComplexControlMask(ComplexControl control,
                                      QPainter *painter,
                                      const QWidget *widget,
                                      const QRect &rect,
                                      const QStyleOption &option) const
{
    switch (control) {
      case CC_ComboBox:
      case CC_ToolButton: {
          painter->fillRect(rect, Qt::color1);
          painter->setPen(Qt::color0);
          break;
      }

      default:
          KStyle::drawComplexControlMask(control,painter,widget,rect,option);
    }
}

//////////////////////////////////////////////////////////////////////////////
// pixelMetric()
// -------------
// Get the pixel metric for metric

int IonStyle::pixelMetric(PixelMetric metric, const QWidget *widget) const
{
    // not using widget's font, so that all metrics are uniform
    int em = QMAX(QApplication::fontMetrics().strikeOutPos() * 3, 17);

    switch (metric) {
      case PM_DefaultFrameWidth:
          return 2;

      case PM_ButtonDefaultIndicator:   // size of default indicator
          return (embossed_ ? 1 : 0);

      case PM_ButtonMargin:             // Space betweeen frame and label
          return 3;

      case PM_TabBarTabOverlap:         // Amount of tab overlap
          return 1;

      case PM_TabBarTabHSpace:          // extra tab spacing
          return 24;

      case PM_TabBarTabVSpace:
          if (const QTabBar *tb = ::qt_cast<const QTabBar*>(widget)) {
              if (tb->shape() == QTabBar::RoundedAbove) {
                  return 10;
              } else {
                  return 6;
              }
          }
          return 0;

      case PM_ExclusiveIndicatorWidth:  // radiobutton size
      case PM_ExclusiveIndicatorHeight:
      case PM_IndicatorWidth:           // checkbox size
      case PM_IndicatorHeight:
      case PM_CheckListButtonSize:      // checkbox size in qlistview items
      case PM_ScrollBarExtent:          // base width of a vertical scrollbar
          return (em & 0xfffe) - 1;

      case PM_SplitterWidth:            // width of spitter
          return (em / 3) & 0xfffe;

      case PM_ScrollBarSliderMin:       // minimum length of slider
          return  em * 2;

      case PM_SliderThickness:          // slider thickness
      case PM_SliderControlThickness:
          return em;

      default:
          return KStyle::pixelMetric(metric, widget);
    }
}

//////////////////////////////////////////////////////////////////////////////
// subRect()
// ---------
// Return subrect for the widget in logical coordinates

QRect IonStyle::subRect(SubRect rect, const QWidget *widget) const
{
    switch (rect) {
        case SR_ComboBoxFocusRect: {
            QRect r = querySubControlMetrics(CC_ComboBox, widget,
                                             SC_ComboBoxEditField);
            r.addCoords(1, 1, -1, -1);
            return r;
        }

        case SR_ProgressBarContents: {
            QRect r = QCommonStyle::subRect(rect, widget);
            if (embossed_)
                r.addCoords(2, 2, -2, -2);
            return r;
        }

        case SR_ProgressBarGroove:
        case SR_ProgressBarLabel:
            return QCommonStyle::subRect(rect, widget);

        default:
            return KStyle::subRect(rect, widget);
    }
}

//////////////////////////////////////////////////////////////////////////////
// querySubControlMetrics()
// ------------------------
// Get metrics for subcontrols of complex controls

QRect IonStyle::querySubControlMetrics(
    ComplexControl control,
    const QWidget *widget,
    SubControl subcontrol,
    const QStyleOption &option
                                           ) const
{
    QRect rect;

    const int ew = (embossed_ ? 1 : 0);
    const int fw = pixelMetric(PM_DefaultFrameWidth, widget) + ew;
    int w = widget->width(), h = widget->height();
    int xc;

    switch (control) {

      case CC_ComboBox: {
          xc = h; // position between edit and arrow

          switch (subcontrol) {
            case SC_ComboBoxFrame: // total combobox area
                rect = widget->rect();
//                 rect.setRect(-1, -1, w+2, h+2);
                break;

            case SC_ComboBoxArrow: // the right side
                rect.setRect(w-xc, fw, xc-fw, h-(fw*2));
                break;

            case SC_ComboBoxEditField: // the left side
                rect.setRect(fw, fw, w-xc-fw-1, h-(fw*2));
                break;

            case SC_ComboBoxListBoxPopup: // the list popup box
                rect = option.rect();
                break;

            default:
                break;
          }
          break;
      }

      case CC_ScrollBar: {
          const QScrollBar *sb = ::qt_cast<const QScrollBar*>(widget);
          if (!sb) break;

          bool horizontal = (sb->orientation() == Qt::Horizontal);
          rect = KStyle::querySubControlMetrics(control, widget,
                                                subcontrol, option);

          // adjust the standard metrics so controls can "overlap"
          if (subcontrol == SC_ScrollBarGroove) {
              if (horizontal) rect.addCoords(-1, 0, 1, 0);
              else            rect.addCoords(0, -1, 0, 1);
          }
          break;
      }

      case CC_SpinWidget: {
          xc = (h * 2 / 3); // position between edit and arrows

          switch (subcontrol) {
            case SC_SpinWidgetButtonField:
                rect.setRect(w-xc, ew, xc-ew, h-2*ew);
                break;

            case SC_SpinWidgetEditField:
                rect.setRect(fw, fw, w-xc-fw, h-(fw*2));
                break;

            case SC_SpinWidgetFrame:
                rect = widget->rect();
                break;

            case SC_SpinWidgetUp:
                rect.setRect(w-xc, ew, xc-ew, (h/2)+1-ew);
                break;

            case SC_SpinWidgetDown:
                rect.setRect(w-xc, (h/2), xc-ew, h-ew-(h/2));
                break;

            default:
                break;
          }
          break;
      }

      default:
          rect = KStyle::querySubControlMetrics(control, widget, subcontrol,
                                                option);
    }

    return rect;
}

//////////////////////////////////////////////////////////////////////////////
// sizeFromContents()
// ------------------
// Returns the size of widget based on the contentsize

QSize IonStyle::sizeFromContents(ContentsType contents,
                                 const QWidget* widget,
                                 const QSize &contentsize,
                                 const QStyleOption &option ) const
{
    int w = contentsize.width();
    int h = contentsize.height();

    switch (contents) {
      case CT_PushButton: {
          const QPushButton* button = ::qt_cast<const QPushButton*>(widget);
          if (!button) {
              return KStyle::sizeFromContents(contents, widget, contentsize,
                                              option);
          }
          int margin = pixelMetric(PM_ButtonMargin, widget)
                     + pixelMetric(PM_DefaultFrameWidth, widget)
                     + (embossed_ ? 5 : 4);

          w += margin + 6; // add room for bold font
          h += margin;

          // standard width and heights
          if (button->isDefault() || button->autoDefault()) {
              if (w < 80 && !button->pixmap()) w = 80;
          }
          if (h < 22) h = 22;
          return QSize(w, h);
      }

      case CT_PopupMenuItem: {
          if (!widget || option.isDefault()) return contentsize;
          const Q3PopupMenu *popup = ::qt_cast<const Q3PopupMenu*>(widget);
          if (!popup) {
              return KStyle::sizeFromContents(contents, widget, contentsize,
                                              option);
          }
          QMenuItem *item = option.menuItem();

          if (item->custom()) {
              w = item->custom()->sizeHint().width();
              h = item->custom()->sizeHint().height();
              if (!item->custom()->fullSpan())
                  h += ITEMVMARGIN*2 + ITEMFRAME*2;
          } else if (item->widget()) { // a menu item that is a widget
              w = contentsize.width() + ITEMVMARGIN*2;
              h = contentsize.height() + ITEMVMARGIN*2;
          } else if (item->isSeparator()) {
              w = 3;
              h = 8;
          } else {
              if (item->pixmap()) {
                  h = QMAX(h, item->pixmap()->height()
                              + ITEMVMARGIN*2 + ITEMFRAME*2);
              } else {
                  h = QMAX(h, MINICONSIZE + ITEMFRAME*2);
                  h = QMAX(h, popup->fontMetrics().height()
                              + ITEMVMARGIN*2 + ITEMFRAME*2);
              }
              if (item->iconSet())
                  h = QMAX(h, item->iconSet()->
                           pixmap(QIcon::Small, QIcon::Normal).height()
                           + ITEMVMARGIN*2 + ITEMFRAME*2);
          }

          if (!item->text().isNull() && item->text().find('\t') >= 0)
              w += 12;
          else if (item->popup())
              w += 2 * ARROWMARGIN;

          if (option.maxIconWidth() || popup->isCheckable()) {
              w += QMAX(option.maxIconWidth(),
                        QIcon::iconSize(QIcon::Small).width())
                  + ITEMHMARGIN*2;
          }
          w += RIGHTBORDER;
          return QSize(w, h);
      }

      default:
          return KStyle::sizeFromContents(contents, widget, contentsize,
                                          option);
    }
}

//////////////////////////////////////////////////////////////////////////////
// Miscellaneous                                                            //
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// flatToolbar()
// -------------
// Is the toolbar "flat"

bool IonStyle::flatToolbar(const Q3ToolBar *toolbar) const
{
    if (!toolbar) return true; // not on a toolbar
    if (!toolbar->isMovingEnabled()) return true; // immobile toolbars are flat
    if (!toolbar->area()) return true; // not docked
    if (toolbar->place() == Q3DockWindow::OutsideDock) return true; // ditto
    if (!toolbar->mainWindow()) return true; // not in a main window
    return false;
}

//////////////////////////////////////////////////////////////////////////////
// eventFilter()
// -------------
// Grab events we are interested in. Most of this routine is to handle the
// exceptions to the normal styling rules.

bool IonStyle::eventFilter(QObject *object, QEvent *event)
{
    if (KStyle::eventFilter(object, event)) return true;
    if (!object->isWidgetType()) return false;

    bool horiz;
    int x, y, w, h;
    Q3ToolBar *toolbar;
    QWidget *widget;

    // painting events
    if (event->type() == QEvent::Paint) {
        // make sure we do the most specific stuff first

        // KDE Toolbar Widget
        // patch by Daniel Brownlees <dbrownlees@paradise.net.nz>
        if (object->parent() && !qstrcmp(object->name(), KTOOLBARWIDGET)) {
            if (0 == (widget = ::qt_cast<QWidget*>(object))) return false;
            QWidget *parent = ::qt_cast<QWidget*>(object->parent());
            int px = widget->x(), py = widget->y();
            // find the toolbar
            while (parent && parent->parent()
                   && !::qt_cast<Q3ToolBar*>(parent)) {
                px += parent->x();
                py += parent->y();
                parent = ::qt_cast<QWidget*>(parent->parent());
            }
            if (!parent) return false;
            widget->rect().rect(&x, &y, &w, &h);
            QRect prect = parent->rect();

            toolbar = ::qt_cast<Q3ToolBar*>(parent);
            horiz = (toolbar) ? (toolbar->orientation() == Qt::Horizontal)
                : (prect.height() < prect.width());
            QPainter painter(widget);
            if (flatToolbar(toolbar)) {
                painter.fillRect(widget->rect(),
                                  parent->colorGroup().background());
            } else {
                drawIonGradient(&painter, widget->rect(),
                                parent->colorGroup().background(),
                                parent->colorGroup().background(),
                                horiz, false, px, py,
                                prect.width(), prect.height());
            }
        }

        // QToolBarExtensionWidget
        else if (object && object->isWidgetType() && object->parent() &&
                 (toolbar = ::qt_cast<Q3ToolBar*>(object->parent()))) {
            if (0 == (widget = ::qt_cast<QWidget*>(object))) return false;
            horiz = (toolbar->orientation() == Qt::Horizontal);
            QPainter painter(widget);
            widget->rect().rect(&x, &y, &w, &h);
            // draw the extension
            drawIonGradient(&painter, widget->rect(),
                            toolbar->colorGroup().background(),
                            toolbar->colorGroup().background(),
                            horiz, false, x, y, w-1, h-1);
            if (horiz) {
                painter.setPen(toolbar->colorGroup().dark());
                painter.drawLine(w-1, 0, w-1, h-1);
                painter.setPen(toolbar->colorGroup().mid());
                painter.drawLine(w-2, 0, w-2, h-2);
                painter.drawLine(x, h-1, w-2, h-1);
                painter.drawLine(x, y, x, h-2);
                painter.setPen(toolbar->colorGroup().midlight());
                painter.drawLine(x+1, y, x+1, h-2);
            } else {
                painter.setPen(toolbar->colorGroup().dark());
                painter.drawLine(0, h-1, w-1, h-1);
                painter.setPen(toolbar->colorGroup().mid());
                painter.drawLine(0, h-2, w-2, h-2);
                painter.drawLine(w-1, y, w-1, h-2);
                painter.drawLine(x, y, w-2, y);
                painter.setPen(toolbar->colorGroup().midlight());
                painter.drawLine(x, y+1, w-2, y+1);
            }
        }

    } else if (hlfocus_ && (
               event->type() == QEvent::FocusIn ||
               event->type() == QEvent::FocusOut)) {
        if (0 != (widget = ::qt_cast<QLineEdit*>(object))) {
            widget->repaint(false);
            if (0 != (widget = ::qt_cast<Q3SpinWidget*>(widget->parent())))
                widget->repaint(false);
        }
    } else if (highlights_) { // "mouseover" events
        if (::qt_cast<QToolButton*>(object) ||
            ::qt_cast<QPushButton*>(object) ||
            ::qt_cast<QComboBox*>(object) ||
            ::qt_cast<Q3SpinWidget*>(object) ||
            ::qt_cast<QCheckBox*>(object) ||
            ::qt_cast<QRadioButton*>(object) ||
            ::qt_cast<QSlider*>(object) ||
            ::qt_cast<QScrollBar*>(object) ||
            object->inherits(QSPLITTERHANDLE)) {
            if (event->type() == QEvent::Enter) {
                if (0 != (widget = ::qt_cast<QWidget*>(object)) &&
                    widget->isEnabled()) {
                    hover_ = widget;
                    widget->repaint(false);
                }
            } else if (event->type() == QEvent::Leave) {
                if (0 != (widget = ::qt_cast<QWidget*>(object))) {
                    hover_ = 0;
                    widget->repaint(false);
                }
            }
        } else if (::qt_cast<QTabBar*>(object)) { // special case for qtabbar
            if (event->type() == QEvent::Enter) {
                if (0 != (widget = ::qt_cast<QWidget*>(object)) &&
                    widget->isEnabled()) {
                    hover_ = widget;
                    hovertab_ = 0;;
                    widget->repaint(false);
                }
            } else if (event->type() == QEvent::Leave) {
                if (0 != (widget = ::qt_cast<QWidget*>(object))) {
                    hover_ = 0;
                    hovertab_ = 0;;
                    widget->repaint(false);
                }
            } else if (event->type() == QEvent::MouseMove) {
                QTabBar *tabbar;
                if (0 != (tabbar = ::qt_cast<QTabBar*>(object))) {
                    QMouseEvent *me;
                    if (0 != (me = dynamic_cast<QMouseEvent*>(event))) {
                        QTab *tab = tabbar->selectTab(me->pos());
                        if (hovertab_ != tab) {
                            hovertab_ = tab;
                            tabbar->repaint(false);
                        }
                    }
                }
            }
        }
    }

    return false;
}

//////////////////////////////////////////////////////////////////////////////
// GradientSet                                                              //
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// GradientSet()
// -------------
// Constructor

GradientSet::GradientSet(const QColor &color, int size)
    : color_(color), size_(size)
{
    for (int n=0; n<gtCount; ++n)  set[n] = NULL;
}

//////////////////////////////////////////////////////////////////////////////
// ~GradientSet()
// --------------
// Destructor

GradientSet::~GradientSet()
{
    for (int n=0; n<gtCount; ++n) if (set[n]) delete set[n];
}

//////////////////////////////////////////////////////////////////////////////
// gradient()
// ----------
// Return the appropriate gradient pixmap

QPixmap* GradientSet::gradient(bool horizontal, bool reverse, bool alpha)
{
    int type = (horizontal ? 0 : gtVBit)
             | (alpha      ? gtABit : 0)
             | (reverse    ? gtRBit : 0);

    // lazy allocate
    if (!set[type]) {
        QRgb c1, c2;
        int hs = (horizontal ? 16 : size_), vs = (horizontal ? size_ : 16);

        if (alpha) {
            c1 = qRgba(color_.red(), color_.green(), color_.blue(), 255);
            c2 = qRgba(color_.red(), color_.green(), color_.blue(), 0);
        } else {
            c1 = lighten(color_, 0.9).rgb();
            c2 = darken(color_).rgb();
        }
        if (reverse) {
            QRgb tmp = c1;
            c1 = c2;
            c2 = tmp;
        }

        QImage *img;
        img = genGradient(c1, c2, horizontal, hs, vs, (reverse ? 1.0 : 0.0));
        set[type] = new QPixmap(*img);
        delete img;
    }
    return (set[type]);
}

//////////////////////////////////////////////////////////////////////////////
// PixmapSet                                                              //
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// PixmapSet()
// -------------
// Constructor

PixmapSet::PixmapSet(const QColor &color)
    : color_(color)
{
    for (int n=0; n<IM_COUNT; ++n)  set[n] = NULL;
}

//////////////////////////////////////////////////////////////////////////////
// ~PixmapSet()
// --------------
// Destructor

PixmapSet::~PixmapSet()
{
    for (int n=0; n<IM_COUNT; ++n) if (set[n]) delete set[n];
}

//////////////////////////////////////////////////////////////////////////////
// gradient()
// ----------
// Return the appropriate gradient pixmap

QPixmap* PixmapSet::pixmap(PIXMAP_ENUM which)
{
    // lazy allocate
    if (!set[(int)which]) {
        QImage *img;
        double a, b;

        switch (which) {
            case IM_UARROW_D:
            case IM_DARROW_D:
            case IM_LARROW_D:
            case IM_RARROW_D:
                a = 3.0; b = 0.0;
                break;
            default:
                a = 2.0; b = 0.2;
        }


        QRgb base = color_.rgb();
        QRgb dark = darken(color_, a, b).rgb();
        QRgb light = lighten(color_, a, b).rgb();

        switch (which) {
            case IM_UARROW:
                img = genImage(color_.rgb(), BSIZE_ARROW, uarrow_alpha);
                break;
            case IM_DARROW:
                img = genImage(base, BSIZE_ARROW, darrow_alpha);
                break;
            case IM_LARROW:
                img = genImage(base, BSIZE_ARROW, larrow_alpha);
                break;
            case IM_RARROW:
                img = genImage(base, BSIZE_ARROW, rarrow_alpha);
                break;
            case IM_UARROW_D:
                img = genImage(base, BSIZE_ARROW, uarrow_disabled_alpha,
                               uarrow_disabled_intensity, dark, light);
                break;
            case IM_DARROW_D:
                img = genImage(base, BSIZE_ARROW, darrow_disabled_alpha,
                               darrow_disabled_intensity, dark, light);
                break;
            case IM_LARROW_D:
                img = genImage(base, BSIZE_ARROW, larrow_disabled_alpha,
                               larrow_disabled_intensity, dark, light);
                break;
            case IM_RARROW_D:
                img = genImage(base, BSIZE_ARROW, rarrow_disabled_alpha,
                               rarrow_disabled_intensity, dark, light);
                break;
            case IM_CHECK:
                img = genImage(base, BSIZE_CHECK, check_alpha,
                               check_intensity, dark, light);
                break;
            case IM_CHECK_FLAT:
                img = genImage(base, BSIZE_CHECK, check_alpha);
                break;
            case IM_DOT:
                img = genImage(base, BSIZE_CHECK, dot_alpha,
                               dot_intensity, dark, light);
                break;
            case IM_CIRCLE_FLAT: // circle (non-embossed)
                img = genImage(darken(color_).rgb(), BSIZE_CHECK,
                               circle_alpha);
                break;
            case IM_CIRCLE_D: // circle outline
                img = genImage(color_.rgb(), BSIZE_CHECK, outline_dark_alpha);
                break;
            case IM_CIRCLE_M: // circle (embossed) light shadow
                img = genImage(color_.rgb(), BSIZE_CHECK, outline_mid_alpha);
                break;
            case IM_CIRCLE_L: // circle (embossed) dark shadow
                img = genImage(color_.rgb(), BSIZE_CHECK, outline_light_alpha);
                break;
            case IM_CIRCLE_B: // circle (embossed) base
                img = genImage(color_.rgb(), BSIZE_CHECK, circle_emboss_alpha);
                break;
            default:
                return NULL;
        }
        set[(int)which] = new QPixmap(*img);
    }
    return (set[(int)which]);
}

//////////////////////////////////////////////////////////////////////////////
// Plugin Stuff                                                             //
//////////////////////////////////////////////////////////////////////////////

class IonStylePlugin : public QStylePlugin
{
    public:
        IonStylePlugin();
        QStringList keys() const;
        QStyle *create(const QString &key);
};

IonStylePlugin::IonStylePlugin() : QStylePlugin() { ; }

QStringList IonStylePlugin::keys() const
{
    return QStringList() << "Ion";
}

QStyle* IonStylePlugin::create(const QString& key)
{
    if (key.lower() == "ion")
        return new IonStyle();
    return 0;
}

KDE_Q_EXPORT_PLUGIN(IonStylePlugin)

#include "ionstyle.moc"
