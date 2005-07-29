/* Plastik2 widget style for Qt 4
   Copyright (C) 2005 Sandro Giessl <sandro@giessl.com>

   Adapted from Plastik for KDE 3

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

#ifndef __PLASTIK_H
#define __PLASTIK_H

#include <QCommonStyle>

#include <QMap>

#define DRAW_BITMAP( painter, pos_x, pos_y, color, bitmapname ) \
{ \
    QBitmap bmp(bitmapname##_width, bitmapname##_height, bitmapname##_bits, true); \
    bmp.setMask(bmp); \
    painter->setPen(color); \
    painter->drawPixmap(pos_x, pos_y, bmp); \
}

#define MODIFY_RECT( rect, mod_x, mod_y, mod_w, mod_h ) \
    QRect(rect.x()+mod_x, rect.y()+mod_y, rect.width()+mod_w, rect.height()+mod_h)

#define SHRINK_RECT( rect, by ) \
    MODIFY_RECT(rect, by, by, -2*by, -2*by)

class QTab;

class Plastik2Style : public QCommonStyle
{
    Q_OBJECT

    public:
        Plastik2Style();
        virtual ~Plastik2Style();

        void polish (QWidget *widget);
        void unpolish (QWidget *widget);
        bool eventFilter(QObject *obj, QEvent *ev);

        void drawComplexControl (ComplexControl cc, const QStyleOptionComplex *opt, QPainter *p, const QWidget *widget = 0) const;
        void drawControl (ControlElement ce, const QStyleOption *opt, QPainter *p, const QWidget *widget = 0) const;
        void drawPrimitive (PrimitiveElement pe, const QStyleOption *opt, QPainter *p, const QWidget *widget = 0) const;
        QRect subElementRect (SubElement se, const QStyleOption *option, const QWidget *widget = 0) const;
        QRect subControlRect(ComplexControl cc, const QStyleOptionComplex *opt, SubControl sc, const QWidget *widget = 0) const;
        QSize sizeFromContents (ContentsType type, const QStyleOption *option, const QSize &contentsSize, const QWidget *widget = 0) const;
        int pixelMetric (PixelMetric metric, const QStyleOption * option = 0, const QWidget * widget = 0) const;

    protected:

        // for passing options to the custom render*-functions
        enum SurfaceFlags {
            Draw_Left =          0x00000001,
            Draw_Right =         0x00000002,
            Draw_Top =           0x00000004,
            Draw_Bottom =        0x00000008,
            Highlight_Left =     0x00000010, // surface
            Highlight_Right =    0x00000020, // surface
            Highlight_Top =      0x00000040, // surface
            Highlight_Bottom =   0x00000080, // surface
            Is_Sunken =          0x00000100, // surface
            Is_Horizontal =      0x00000200, // surface
            Is_Highlight =       0x00000400, // surface
            Is_Default =         0x00000800, // surface
            Is_Disabled =        0x00001000,
            Round_UpperLeft =    0x00002000,
            Round_UpperRight =   0x00004000,
            Round_BottomLeft =   0x00008000,
            Round_BottomRight =  0x00010000,
            Draw_AlphaBlend =    0x00020000,
            Fill_Edges =         0x00040000,
            Is_HighlightShade =  0x00080000
        };

        void renderShade(QPainter *p, const QRect &r, const QColor &background, const uint flags, const QColor &highlight = Qt::color0) const;
        void renderContour(QPainter *p, const QRect &r, const QColor &background, const QColor &contour,const uint flags) const;
        void renderSurface(QPainter *p, const QRect &r, const QColor &background, const QColor &button, const QColor &highlight, int intensity, const uint flags) const;
        void renderGradient(QPainter *p, const QRect &r, const QColor &c1, const QColor &c2, bool horizontal) const;

        void renderButton(QPainter *p, const QRect &r, const QPalette &pal, bool sunken, bool mouseOver, bool horizontal, bool enabled/*, bool khtmlMode*/) const;
        void renderComboSpinBoxFrame(QPainter *p, const QRect &r, const QRect &editField, const QPalette &pal, bool enabled, bool editable, bool mouseOver, bool focusHighlight, bool reverseLayout) const;
        void renderFrame(QPainter *p, const QRect &r, const QPalette &pal, bool pseudo3d, bool sunken) const;
        void renderProgressBar(QPainter *p, QRect &r, const QColor &fg, const QColor &bg, double percentage, int animShift, bool reverseLayout) const;
        void renderDot(QPainter *p, int x, int y, const QColor &c, const bool sunken) const;

    protected slots:
        // Progressbar animation
        void updateProgressPos();
        void progressBarDestroyed(QObject* bar);

    protected:

        enum ColorType
        {
            DefaultButtonContour,
            ButtonContour,
            DragButtonContour,
            DragButtonSurface,
            PanelContour,
            PanelLight,
            PanelDark,
            ShadeLight,
            ShadeDark,
            MouseOverHighlight,
            FocusHighlight,
            CheckMark
        };

        enum WidgetState
        {
            IsEnabled,
            IsPressed,     // implies IsEnabled
            IsHighlighted, // implies IsEnabled
            IsDisabled
        };

        enum Direction
        {
            North,
            East,
            South,
            West
        };

        inline QColor getColor(const QPalette &pal, const ColorType t, const bool enabled = true)const;
        inline QColor getColor(const QPalette &pal, const ColorType t, const WidgetState s)const;
        inline QColor alphaBlendColors(const QColor &backgroundColor, const QColor &foregroundColor, int alpha)const;

    private:
// Disable copy constructor and = operator
        Plastik2Style( const Plastik2Style & );
        Plastik2Style& operator=( const Plastik2Style & );

        int _contrast;
        bool _animateProgressBar;

        // progress bar animation
        QMap<QWidget*, int> progAnimWidgets;
        QTimer *animationTimer;

        QWidget *hoverWidget;
//         QTab *hoverTab;
};

#endif // __PLASTIK_H

// kate: indent-width 4; replace-tabs on; tab-width 4; space-indent on;
