/* kate: hl c++; indent-mode cstyle; space-indent on; indent-width 4; replace-tabs on;
  Bluecurve style.
  Copyright (c) 2007 Matthew Woehlke <mw_triad@sourceforge.net>

  Released under the GNU General Public License (GPL) v2.

  Based on the Bluecurve style:
  Copyright (c) 2002 Red Hat, Inc.
  And on the Phase style:
  Copyright (c) 2004 David Johnson <david@usermode.org>

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
#ifndef IONSTYLE_H
#define IONSTYLE_H

#include <kstyle.h>
#include <qcolor.h>
#include <qpixmap.h>
#include <qimage.h>

#include <qtabbar.h>
#include <q3toolbar.h>
#include <QEvent>

extern QImage *
genImage(
    QRgb color,
    int size,
    unsigned char *alpha,
    unsigned char *intensity = NULL,
    QRgb dark = 0,
    QRgb light = 0xffffff
                );

extern QImage *
genGradient(
    QRgb c1,
    QRgb c2,
    bool horizontal,
    int w, int h,
    double bias = 0.5
           );

extern QRgb   shade(const QRgb    color, double lk, double sk = 1.0);
extern QColor shade(const QColor &color, double lk, double sk = 1.0);

extern QRgb   shadei(const QRgb    color, double lk, double sk = 1.0);
extern QColor shadei(const QColor &color, double lk, double sk = 1.0);

extern QRgb   blend(const QRgb    c1, const QRgb    c2, double k);
extern QColor blend(const QColor &c1, const QColor &c2, double k);

enum PIXMAP_ENUM {
    IM_UARROW = 0,
    IM_DARROW,
    IM_LARROW,
    IM_RARROW,
    IM_UARROW_D,
    IM_DARROW_D,
    IM_LARROW_D,
    IM_RARROW_D,
    IM_CHECK,
    IM_CHECK_FLAT,
    IM_DOT,
    IM_CIRCLE_FLAT,
    IM_CIRCLE_D,
    IM_CIRCLE_M,
    IM_CIRCLE_L,
    IM_CIRCLE_B,
    IM_COUNT
};

class IonStyle : public KStyle
{
    Q_OBJECT
public:
    IonStyle();
    virtual ~IonStyle();

    void polish(QApplication* app);
    void polish(QWidget *widget);
    void polish(QPalette &pal);
    void unPolish(QWidget *widget);

    void drawPrimitive(PrimitiveElement element,
            QPainter *painter,
            const QRect &rect,
            const QColorGroup &group,
            SFlags flags = Style_Default,
            const QStyleOption &option = QStyleOption::Default) const;

    void drawKStylePrimitive(KStylePrimitive element,
            QPainter *painter,
            const QWidget *widget,
            const QRect &rect,
            const QColorGroup &group,
            SFlags flags = Style_Default,
            const QStyleOption &option = QStyleOption::Default) const;

    void drawControl(ControlElement element,
            QPainter *painter,
            const QWidget *widget,
            const QRect &rect,
            const QColorGroup &group,
            SFlags flags = Style_Default,
            const QStyleOption &option = QStyleOption::Default) const;

    void drawControlMask(ControlElement element,
            QPainter *painter,
            const QWidget *widget,
            const QRect &rect,
            const QStyleOption &option = QStyleOption::Default) const;

    void drawComplexControl(ComplexControl control,
            QPainter *painter,
            const QWidget *widget,
            const QRect &rect,
            const QColorGroup &group,
            SFlags flags = Style_Default,
            SCFlags controls = SC_All,
            SCFlags active = SC_None,
            const QStyleOption &option = QStyleOption::Default) const;

    void drawComplexControlMask(ComplexControl control,
            QPainter *painter,
            const QWidget *widget,
            const QRect &rect,
            const QStyleOption &option = QStyleOption::Default) const;

    int pixelMetric(PixelMetric metric,
            const QWidget *widget = 0) const;

    QRect subRect(SubRect rect, const QWidget *widget) const;

    QRect querySubControlMetrics(ComplexControl control,
            const QWidget *widget,
            SubControl subcontrol,
            const QStyleOption &option = QStyleOption::Default) const;

    QSize sizeFromContents(ContentsType contents,
            const QWidget *widget,
            const QSize &contentsize,
            const QStyleOption& option = QStyleOption::Default) const;

private:
    IonStyle(const IonStyle &);
    IonStyle& operator=(const IonStyle &);

    void  drawIonEdge(QPainter *painter,
            int x1, int y1, int x2, int y2,
            const QColor &mid,
            const QColor &dark,
            const QColor &light,
            bool sunken=false,
            bool rotated=false) const;

    void drawIonBevel(QPainter *painter,
            int x, int y, int w, int h,
            const QColorGroup &group,
            const QColor &fill,
            const QColor &glow,
            bool sunken=false,
            bool horizontal=true,
            bool reverse=false) const;

    void drawIonButton(QPainter *painter,
            int x, int y, int w, int h,
            const QColorGroup &group,
            const QColor &fill,
            const QColor &glow,
            bool sunken=false,
            bool isDefault=false) const;

    void drawIonPanel(QPainter *painter,
            int x, int y, int w, int h,
            const QColorGroup &group,
            bool sunken=false,
            const QBrush *fill=NULL,
            bool active=false) const;

    void IonStyle::drawIonPanel(QPainter *painter,
            int x, int y, int w, int h,
            const QColor &color,
            double k,
            bool sunken = false) const;

    void drawIonProgressBar(QPainter *painter,
            int x, int y, int w, int h,
            const QColorGroup &group) const;

    void drawIonTab(QPainter *painter,
            int x, int y, int w, int h,
            const QColorGroup &group,
            const QTabBar *bar,
            const QStyleOption &option,
            const SFlags flags) const;

    void drawIonGradient(QPainter *painter,
            const QRect &rect,
            QColor color,
            QColor glow,
            bool horizontal=true,
            bool reverse=false,
            int px=0, int py=0,
            int pw=0, int ph=0) const;

    void drawIonPixmap(QPainter *painter,
            PIXMAP_ENUM which,
            const QColor &color,
            int x, int y) const;

    bool flatToolbar(const Q3ToolBar *toolbar) const;

    bool eventFilter(QObject *object, QEvent *event);

private:
    QPixmap *arrowPix[4];

    QWidget *hover_;
    QTab *hovertab_;
    bool hlfocus_;
    bool embossed_;
    bool gradients_;
    int  highlights_;
    bool reverse_;
    bool kicker_;
};

class GradientSet
{
public:
    GradientSet(const QColor &color, int size);
    ~GradientSet();

    QPixmap* gradient(bool horizontal, bool reverse, bool alpha);

private:
    static const int gtVBit = 1;
    static const int gtRBit = 2;
    static const int gtABit = 4;
    static const int gtCount = 8;

    QPixmap *set[gtCount];
    QColor color_;
    int size_;
};

class PixmapSet
{
public:
    PixmapSet(const QColor &color);
    ~PixmapSet();

    QPixmap* pixmap(PIXMAP_ENUM which);

private:
    static const int gtVBit = 1;
    static const int gtRBit = 2;
    static const int gtABit = 4;
    static const int gtCount = 8;

    QPixmap *set[IM_COUNT];
    QColor color_;
};

#endif // IONSTYLE_H
