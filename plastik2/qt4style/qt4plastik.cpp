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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
 */

#include <qstyleplugin.h>

#include <QBitmap>
#include <QEvent>
#include <QMatrix>
#include <QMouseEvent>
#include <QPainter>
#include <QSettings>
#include <QStyleOption>
#include <QTimer>

#include <QAbstractSpinBox>
#include <QCheckBox>
#include <QComboBox>
#include <QLineEdit>
#include <QProgressBar>
#include <QPushButton>
#include <QRadioButton>

#include "qt4plastik.h"


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

class Plastik2StylePlugin : public QStylePlugin
{
    public:
        Plastik2StylePlugin() {}
        ~Plastik2StylePlugin() {}

        QStringList keys() const {
            return QStringList() << "Plastik2";
        }

        QStyle* create(const QString& id) {
            QStringList names = keys();
            //check whether included in the keys
            if (names.contains(id, Qt::CaseInsensitive))
                return new Plastik2Style;

            return 0;
        }
};

Q_EXPORT_PLUGIN( Plastik2StylePlugin )

Plastik2Style::Plastik2Style()
    : QCommonStyle(),
      hoverWidget(0)/*,
      hoverTab(0)*/
{
    QSettings settings;
    _contrast = settings.value("/Qt/KDE/contrast", 6).toInt();
    settings.beginGroup("/plastik2style/Settings");
    _animateProgressBar = settings.value("/animateProgressBar", true).toBool();

    if ( _animateProgressBar )
    {
        animationTimer = new QTimer( this );
        connect( animationTimer, SIGNAL(timeout()), this, SLOT(updateProgressPos()) );
    }
}

Plastik2Style::~Plastik2Style()
{
}

void Plastik2Style::polish (QWidget *widget)
{
    // install eventFilters
    if (::qobject_cast<QCheckBox*>(widget) || ::qobject_cast<QRadioButton*>(widget) || ::qobject_cast<QPushButton*>(widget) ||
        ::qobject_cast<QComboBox*>(widget) || ::qobject_cast<QAbstractSpinBox*>(widget) ) {
        // mouseOver highlight
        widget->installEventFilter(this);
    } else if (::qobject_cast<QTabBar*>(widget)) {
        // mouseOver highlight of individual tabs
        widget->setMouseTracking(true);
        widget->installEventFilter(this);
    }
    else if (::qobject_cast<QLineEdit*>(widget) ) {
        // focus indicator
        widget->installEventFilter(this);
    }

    // progressbar animation
    if( _animateProgressBar && ::qobject_cast<QProgressBar*>(widget) )
    {
        widget->installEventFilter(this);
        progAnimWidgets[widget] = 0;
        connect(widget, SIGNAL(destroyed(QObject*)), this, SLOT(progressBarDestroyed(QObject*)));
        if (!animationTimer->isActive()) {
            animationTimer->start(50);
        }
    }

    QCommonStyle::polish(widget);
}

void Plastik2Style::unpolish (QWidget *widget)
{
    // remove eventFilters
    if (::qobject_cast<QCheckBox*>(widget) || ::qobject_cast<QRadioButton*>(widget) || ::qobject_cast<QPushButton*>(widget) ||
        ::qobject_cast<QComboBox*>(widget) || ::qobject_cast<QAbstractSpinBox*>(widget) ) {
        widget->removeEventFilter(this);
    } else if (::qobject_cast<QTabBar*>(widget)) {
        widget->setMouseTracking(false);
        widget->removeEventFilter(this);
    }
    if (::qobject_cast<QLineEdit*>(widget) ) {
        widget->removeEventFilter(this);
    }

    // progressbar animation
    if ( ::qobject_cast<QProgressBar*>(widget) )
    {
        progAnimWidgets.remove(widget);
    }

    QCommonStyle::unpolish(widget);
}

void Plastik2Style::progressBarDestroyed(QObject* obj)
{
    progAnimWidgets.remove(static_cast<QWidget*>(obj));
}

void Plastik2Style::updateProgressPos()
{
    QProgressBar* pb;
    //Update the registered progressbars.
    QMap<QWidget*, int>::iterator iter;
    bool visible = false;
    for (iter = progAnimWidgets.begin(); iter != progAnimWidgets.end(); iter++)
    {   
        if ( !::qobject_cast<QProgressBar*>(iter.key()) )
            continue;
        
        pb = dynamic_cast<QProgressBar*>(iter.key());
        if ( iter.key() -> isEnabled() && 
             pb->value() != pb->maximum() )
        {
            // update animation Offset of the current Widget
            iter.value() = (iter.value() + 1) % (2*pixelMetric(PM_ProgressBarChunkWidth) );
            iter.key()->update();
        }
        if (iter.key()->isVisible())
            visible = true;
    }
    if (!visible)
        animationTimer->stop();
}

bool Plastik2Style::eventFilter(QObject *obj, QEvent *ev)
{
    if (!obj || !obj->isWidgetType() )
        return QCommonStyle::eventFilter(obj, ev);

    QWidget *w = static_cast<QWidget*>(obj);

    // mouseOver highlight
    if (::qobject_cast<QCheckBox*>(obj) || ::qobject_cast<QRadioButton*>(obj) || ::qobject_cast<QPushButton*>(obj) ||
        ::qobject_cast<QComboBox*>(obj) || ::qobject_cast<QAbstractSpinBox*>(obj) )
    {
        if ((ev->type() == QEvent::Enter) && static_cast<QWidget*>(obj)->isEnabled())
        {
            hoverWidget = w;
            w->update();
        }
        else if ((ev->type() == QEvent::Leave) && (obj == hoverWidget) )
        {
            hoverWidget = 0;
            w->update();
        }
    }

    // TODO: there is no way to get the hoverTab...
    else if (::qobject_cast<QTabBar*>(obj) ) {
        if ( (ev->type() == QEvent::Enter) && static_cast<QWidget*>(obj)->isEnabled() )
        {
            hoverWidget = w;
//             hoverTab = 0;
            w->update();
        }
        else if (ev->type() == QEvent::MouseMove)
        {
//             QTabBar *tabbar = dynamic_cast<QTabBar*>(obj);
//             QMouseEvent *me = dynamic_cast<QMouseEvent*>(ev);
// 
//             QTab * t = tabbar->selectTab(me->pos());
// 
//             if (tabbar && me) {
//                 avoid unnecessary repaints (which otherwise would occour on every
//                 MouseMove event causing high cpu load).
// 
//                 bool repaint = true;
// 
//                     go through the tabbar and see which tabs are hovered by the mouse.
//                     tabs are overlapping 1 px, so it's possible that 2 tabs are under the mouse.
//                 int tabCount = 0;
//                 for (int i = 0; i < tabbar->count(); ++i) {
//                     QTab *tab = tabbar->tab(i);
//                     if (tab && tab->rect().contains(me->pos() ) ) {
//                         ++ tabCount;
// 
//                         if (tabCount < 2) {
//                             good, only one tab under the mouse.
//                             if (hoverTab==tab)
//                                 repaint = false; // has been updated before, no repaint necessary
//                             hoverTab = tab;
//                         } else {
//                             uhh! there's another tab under the mouse, repaint...
//                             repaint = true;
//                             hoverTab = 0; // make sure the tabbar is repainted next time too.
//                         }
// 
//                     }
//                 }
// 
//                 if (repaint)
//                     tabbar->update();
//             }
            w->update();
        }
        else if (ev->type() == QEvent::Leave)
        {
            hoverWidget = 0;
//             hoverTab = 0;
            w->update();
        }
    }

    // focus highlight
    if ( ::qobject_cast<QLineEdit*>(obj) ) {
        if ( ::qobject_cast<QAbstractSpinBox*>(w->parentWidget()) )
        {
            QWidget* spinbox = w->parentWidget();
            if ((ev->type() == QEvent::FocusIn) || (ev->type() == QEvent::FocusOut))
            {
                spinbox->update();
            }
        }

        if ((ev->type() == QEvent::FocusIn) || (ev->type() == QEvent::FocusOut))
        {
            w->update();
        }
    }

    // Track show events for progress bars
    if ( _animateProgressBar && ::qobject_cast<QProgressBar*>(obj) )
    {
        if ((ev->type() == QEvent::Show) && !animationTimer->isActive())
        {
            animationTimer->start(50);
        }
    }

    return QCommonStyle::eventFilter(obj, ev);
}

void Plastik2Style::drawComplexControl (ComplexControl cc, const QStyleOptionComplex *opt, QPainter *p, const QWidget *widget) const
{
    QPalette pal = opt->palette;
    QRect r = opt->rect;
    State state = opt->state;

    const bool enabled = state&State_Enabled;

    const bool reverseLayout = opt->direction == Qt::RightToLeft;

    switch (cc) {

        case CC_ComboBox:
        {
            if (const QStyleOptionComboBox *cb = qstyleoption_cast<const QStyleOptionComboBox *>(opt) ) {
                const bool editable = cb->editable;
                const bool hasFocus = state&State_HasFocus;
                const bool focusHighlight = hasFocus && editable && enabled;

                const bool mouseOver = (widget == hoverWidget) || (state&State_MouseOver);

                QColor extContourColor = getColor(pal, ButtonContour, enabled);
                if (mouseOver)
                    extContourColor = extContourColor.light(140);

                QRect editField = visualRect(reverseLayout ? Qt::RightToLeft : Qt::LeftToRight, r,
                                                subControlRect(cc,opt,SC_ComboBoxEditField,widget) );

                if (cb->subControls & SC_ComboBoxFrame) {
                    renderComboSpinBoxFrame(p, r, editField, pal, enabled, editable, mouseOver, focusHighlight, reverseLayout);

                    //extend the contour: between input and handler...
                    p->setPen(extContourColor);
                    if(reverseLayout) {
                        p->drawLine(editField.left()-1, r.top()+1, editField.left()-1, r.bottom()-1);
                    } else {
                        p->drawLine(editField.right()+1, r.top()+1, editField.right()+1, r.bottom()-1);
                    }
                }

                // the rect of the button area...
                int sx = reverseLayout ? r.left()+2: editField.right()+2;
                int sw = reverseLayout ? editField.left()-1-sx : r.right()-1-sx;
                const QRect RbuttonSurface(sx, r.top()+1+1, sw, r.height()-2-2);

                const QRect RcontentSurface = editField; /*(reverseLayout?r.left()+1+cbButtonWidth+2:r.left()+1+1, r.top()+1+1,
                        r.width()-cbButtonWidth-3-1-1, r.height()-2-2);*/

                // handler button
                uint surfaceFlags = 0;
                if (cb->subControls & SC_ComboBoxArrow) {
                    surfaceFlags = Draw_Left|Draw_Right|Draw_Top|Draw_Bottom|Is_Horizontal;
                    if(reverseLayout) {
                        surfaceFlags |= Round_UpperLeft|Round_BottomLeft;
                    } else {
                        surfaceFlags |= Round_UpperRight|Round_BottomRight;
                    }
//                     if (cb->activeSubControls&SC_ComboBoxArrow)
//                         surfaceFlags |= Is_Sunken;

                    if (mouseOver)
                        surfaceFlags |= Is_Highlight;

                    if(!enabled) surfaceFlags|=Is_Disabled;

                    renderSurface(p, RbuttonSurface,
                                    pal.background().color(), pal.button().color(), getColor(pal,MouseOverHighlight), _contrast,
                                    surfaceFlags);

                    p->setPen(pal.foreground().color() );
                    QStyleOption arrowOpt;
                    arrowOpt.palette = pal;
                    arrowOpt.rect = RbuttonSurface;
                    arrowOpt.state = 0/*|State_Default*//*|State_Enabled*//*|State_Raised*/;
                    if (enabled)
                        arrowOpt.state |= State_Enabled;
                    drawPrimitive(PE_IndicatorSpinDown, &arrowOpt, p);
                }

                if (cb->subControls & SC_ComboBoxEditField) {
                    if(!editable) {
                        surfaceFlags = Draw_Left|Draw_Right|Draw_Top|Draw_Bottom|Is_Horizontal;
                        if(reverseLayout) {
                            surfaceFlags |= Round_UpperRight|Round_BottomRight;
                        } else {
                            surfaceFlags |= Round_UpperLeft|Round_BottomLeft;
                        }

                        if (mouseOver) {
                            surfaceFlags |= Is_Highlight;
                            surfaceFlags |= Highlight_Top|Highlight_Bottom;
                        }
                        renderSurface(p, RcontentSurface,
                                        pal.background().color(), pal.button().color(), getColor(pal,MouseOverHighlight), _contrast,
                                        surfaceFlags);
                            // TODO: focus rect
    //                         if (hasFocus) {
    //                             drawPrimitive(PE_FocusRect, p,
    //                                             SHRINK_RECT(RcontentSurface, 2), cg);
    //                         }
                    }
                }

                    // TODO: check: does this have an effect?
//                     // QComboBox draws the text using cg.text(), we can override this
//                     // from here
//                     p->setPen( pal.buttonText().color() );
//                     p->setBackgroundColor( pal.button().color() );

            }
            break;
        }

        case CC_SpinBox:
        {
            if (const QStyleOptionSpinBox *sb = qstyleoption_cast<const QStyleOptionSpinBox *>(opt) ) {
                const bool showFrame = sb->showFrame;
                const bool plusMinusSymbol = sb->buttonSymbols == QAbstractSpinBox::PlusMinus;
                const bool hasFocus = state&State_HasFocus;
                const bool focusHighlight = hasFocus && enabled;
                const bool sliderIndicator = sb->showSliderIndicator;

                const bool mouseOver = (widget == hoverWidget) || (state&State_MouseOver);

                const bool activeSbUp = sb->activeSubControls&SC_SpinBoxUp;
                const bool activeSbDown = sb->activeSubControls&SC_SpinBoxDown;

                QColor extContourColor = getColor(pal, ButtonContour, enabled);
                if (mouseOver)
                    extContourColor = extContourColor.light(140);

                // contour
                QRect editField = visualRect(reverseLayout ? Qt::RightToLeft : Qt::LeftToRight, r,
                                             subControlRect(cc,opt,SC_SpinBoxEditField,widget) );

                if (sb->subControls & SC_SpinBoxFrame) {
                    if (showFrame)
                        renderComboSpinBoxFrame(p, r, editField, pal, enabled, true, mouseOver, focusHighlight, reverseLayout);

                    //extend the contour: between input and handler...
                    p->setPen(extContourColor);
                    if(reverseLayout) {
                        p->drawLine(editField.left()-1, r.top()+1, editField.left()-1, r.bottom()-1);
                    } else {
                        p->drawLine(editField.right()+1, r.top()+1, editField.right()+1, r.bottom()-1);
                    }
                }

                // the rect of the button area...
                int sx = reverseLayout ? r.left()+2: editField.right()+2;
                int sw = reverseLayout ? editField.left()-1-sx : r.right()-1-sx;
                const QRect RbuttonSurface(sx, r.top()+1+1, sw, r.height()-2-2);

                const bool heightDividable = ((r.height()%2) == 0);
                const int buttonHeight = RbuttonSurface.height()/2;

                //extend the contour: between up/down buttons
                p->setPen(extContourColor);
                p->drawLine(sx, r.top()+1+(r.height()-2)/2,
                            sx+sw, r.top()+1+(r.height()-2)/2);
                if(heightDividable)
                    p->drawLine(sx, r.top()+1+(r.height()-2)/2-1,
                                sx+sw, r.top()+1+(r.height()-2)/2-1);

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

                uint surfaceFlags = 0;
                if (sb->subControls & SC_SpinBoxUp) {
                    surfaceFlags = Draw_Left|Draw_Right|Draw_Top|Draw_Bottom|Is_Horizontal;
                    if (mouseOver)
                        surfaceFlags |= Is_Highlight;
                    if(reverseLayout) {
                        surfaceFlags |= Round_UpperLeft;
                    } else {
                        surfaceFlags |= Round_UpperRight;
                    }

                    if (activeSbUp) surfaceFlags|=Is_Sunken;
                    if(!enabled) surfaceFlags|=Is_Disabled;
                    renderSurface(p, upRect, pal.background().color(), pal.button().color(), getColor(pal,MouseOverHighlight),
                                _contrast, surfaceFlags);
                }

                if (sb->subControls & SC_SpinBoxDown) {
                    surfaceFlags = Draw_Left|Draw_Right|Draw_Top|Draw_Bottom|Is_Horizontal;
                    if (mouseOver)
                        surfaceFlags |= Is_Highlight;
                    if(reverseLayout) {
                        surfaceFlags |= Round_BottomLeft;
                    } else {
                        surfaceFlags |= Round_BottomRight;
                    }
                    if (mouseOver) {
                        surfaceFlags |= Is_Highlight;
                        surfaceFlags |= Highlight_Bottom|Highlight_Left|Highlight_Right;
                    }
                    if (activeSbDown) surfaceFlags|=Is_Sunken;
                    if(!enabled) surfaceFlags|=Is_Disabled;
                    renderSurface(p, downRect, pal.background().color(), pal.button().color(), getColor(pal,MouseOverHighlight),
                                _contrast, surfaceFlags);
                }

                // icons...
                PrimitiveElement pe;
                if (sb->subControls & SC_SpinBoxUp) {
                    QStyleOptionSpinBox opt2 = *sb;
                    opt2.rect = upRect;
                    if (!(sb->stepEnabled & QAbstractSpinBox::StepUpEnabled) ) {
                        opt2.palette.setCurrentColorGroup(QPalette::Disabled);
                        opt2.state &= ~State_Enabled;
                    }
                    if (activeSbUp) {
                        opt2.state |= State_On;
                        opt2.state |= State_Sunken;
                    } else {
                        opt2.state |= State_Raised;
                    }
                    if (plusMinusSymbol)
                        pe = PE_IndicatorSpinPlus;
                    else
                        pe = PE_IndicatorSpinUp;
                    drawPrimitive(pe, &opt2, p);
                }

                if (sb->subControls & SC_SpinBoxDown) {
                    QStyleOptionSpinBox opt2 = *sb;
                    opt2.rect = downRect;
                    if (!(sb->stepEnabled & QAbstractSpinBox::StepDownEnabled) ) {
                        opt2.palette.setCurrentColorGroup(QPalette::Disabled);
                        opt2.state &= ~State_Enabled;
                    }
                    if (activeSbDown) {
                        opt2.state |= State_On;
                        opt2.state |= State_Sunken;
                    } else {
                        opt2.state |= State_Raised;
                    }
                    if (plusMinusSymbol)
                        pe = PE_IndicatorSpinMinus;
                    else
                        pe = PE_IndicatorSpinDown;
                    drawPrimitive(pe, &opt2, p);
                }

                if (sb->subControls & SC_SpinBoxSlider) {
                    // progress bar
                    if (sliderIndicator) {
                        QRect rSlider = visualRect(reverseLayout ? Qt::RightToLeft : Qt::LeftToRight, r,
                                                    subControlRect(cc,opt,SC_SpinBoxSlider,widget) );
                        renderProgressBar(p, rSlider, pal.highlight().color(), pal.base().color(), sb->percentage, 0, reverseLayout);
                    }
                }

            }
            break;
        }

        case CC_Slider:
        {
            if (const QStyleOptionSlider *slider = qstyleoption_cast<const QStyleOptionSlider *>(opt)) {
                QRect groove = QCommonStyle::subControlRect(CC_Slider, slider,
                        SC_SliderGroove, widget);
                QRect handle = QCommonStyle::subControlRect(CC_Slider, slider,
                        SC_SliderHandle, widget);

                bool horizontal = slider->orientation == Qt::Horizontal;

                // TODO: focus rect!

                if ((slider->subControls & SC_SliderGroove) && groove.isValid()) {
                    renderShade(p, groove, pal.background().color(), Draw_Left|Draw_Right|Draw_Top|Draw_Bottom|Is_Sunken);
                }

                if (slider->subControls & SC_SliderTickmarks) {
                    QStyleOptionSlider tmpSlider = *slider;
                    tmpSlider.subControls = SC_SliderTickmarks;
                    QCommonStyle::drawComplexControl(cc, &tmpSlider, p, widget);
                }

                if (slider->subControls & SC_SliderHandle) {
                    const bool pressed = (state&State_Active);
                    const WidgetState s = enabled?(pressed?IsPressed:IsEnabled):IsDisabled;
                    const QColor contour = getColor(pal,DragButtonContour,s),
                    surface = getColor(pal,DragButtonSurface,s);

                    int cx = handle.center().x();
                    int cy = handle.center().y();

                    if (horizontal) {
                        renderContour(p, SHRINK_RECT(handle, 1),
                                      pal.background().color(), contour, Draw_Left|Draw_Right|Draw_Top|Draw_Bottom);
                        renderSurface(p, SHRINK_RECT(handle, 2),
                                      pal.background().color(), surface, getColor(pal,MouseOverHighlight), _contrast,
                                      Draw_Left|Draw_Right|Draw_Top|Draw_Bottom|Is_Horizontal);


                        renderDot(p, cx, cy-2, surface, true );
                        renderDot(p, cx, cy+2, surface, true );

                    } else {
                        renderContour(p, SHRINK_RECT(handle, 1),
                                      pal.background().color(), contour, Draw_Left|Draw_Right|Draw_Top|Draw_Bottom);
                        renderSurface(p, SHRINK_RECT(handle, 2),
                                      pal.background().color(), surface, getColor(pal,MouseOverHighlight), _contrast,
                                      Draw_Left|Draw_Right|Draw_Top|Draw_Bottom);

                        renderDot(p, cx-2, cy, surface, true );
                        renderDot(p, cx+2, cy, surface, true );
                    }
                }

            }

            break;
        }

        default:
            return QCommonStyle::drawComplexControl(cc, opt, p, widget);
    }
}


void Plastik2Style::drawControl (ControlElement ce, const QStyleOption *opt, QPainter *p, const QWidget *widget) const
{
    QPalette pal = opt->palette;
    QRect r = opt->rect;
    State state = opt->state;

    const bool reverseLayout = opt->direction == Qt::RightToLeft;
    const bool enabled = state&State_Enabled;
    const bool on = state&State_On;
    const bool down = state&State_Down;

    int r_w = r.width();
    int r_h = r.height();
    int r_x, r_y, r_x2, r_y2;
    r.getCoords(&r_x, &r_y, &r_x2, &r_y2);

    switch (ce) {
        case CE_ProgressBarGroove:
        {
            uint contourFlags = Is_Sunken|Draw_Left|Draw_Right|Draw_Top|Draw_Bottom;
            renderShade(p, r, pal.background().color(), contourFlags );
            renderContour(p, SHRINK_RECT(r, 1),
                          pal.background().color(), getColor(pal, ButtonContour, enabled), contourFlags );
            break;
        }

        case CE_ProgressBarContents:
        {
            if (const QStyleOptionProgressBar *pb = qstyleoption_cast<const QStyleOptionProgressBar *>(opt) ) {
//             const QProgressBar *pb = dynamic_cast<const QProgressBar*>(widget);
                bool reverseLayout = pb->direction == Qt::RightToLeft;

                const QColor bg = enabled ? pal.base().color() : pal.background().color(); // background
                const QColor fg = enabled ? pal.highlight().color() : pal.background().color().dark(115); // foreground

                if(pb->minimum == 0 && pb->maximum == 0) {
                // Busy indicator

                    int cw = 5;

                    int w = r.width();
                    int x = pb->progress % (w * 2);
                    if (x > w)
                        x = 2 * w - x;
                    x = reverseLayout ? r.right()-x-cw : x+r.x();

                    p->fillRect(x, r.y(), cw, r.height(), fg );

                } else {
                    int steps = pb->maximum;
                    if (steps <= 0)
                        steps = 1;

                    double percent = static_cast<double>(pb->progress) / static_cast<double>(steps);

                    int animShift = 0;
                    if (_animateProgressBar) {
                    // find the animation Offset for the current Widget
                        QWidget* nonConstWidget = const_cast<QWidget*>(widget);
                        QMap<QWidget*, int>::const_iterator iter = progAnimWidgets.find(nonConstWidget);
                        if (iter != progAnimWidgets.end())
                            animShift = iter.value();
                    }

                    renderProgressBar(p, r, fg, bg, percent, animShift, reverseLayout);

                }
            }

            break;
        }

        case CE_TabBarTabShape:
        {
            if (const QStyleOptionTab *tab = qstyleoption_cast<const QStyleOptionTab *>(opt) ) {

                const bool selected = tab->state & State_Selected;
                const bool nextIsSelected = tab->selectedPosition == QStyleOptionTab::NextIsSelected;

                Direction dir;
                if (tab->shape == QTabBar::TriangularNorth || tab->shape == QTabBar::RoundedNorth)
                    dir = North;
                else if (tab->shape == QTabBar::TriangularEast || tab->shape == QTabBar::RoundedEast)
                    dir = East;
                else if (tab->shape == QTabBar::TriangularSouth || tab->shape == QTabBar::RoundedSouth)
                    dir = South;
                else
                    dir = West;

                const bool above = dir == North;
                bool isFirst = tab->position == QStyleOptionTab::Beginning;
                bool isLast = tab->position == QStyleOptionTab::End;
//                 bool isSingle = tab->position == QStyleOptionTab::OnlyOneTab;
                bool triangular = tab->shape == QTabBar::TriangularNorth || tab->shape == QTabBar::TriangularEast || tab->shape == QTabBar::TriangularSouth || tab->shape == QTabBar::TriangularWest;
                bool mouseOver = state&State_MouseOver;

                bool cornerWidget = false;

                // TODO: check for cornerWidget.
//                 bool cornerWidget = false;
//                 if( ::qobject_cast<QTabWidget*>(tb->parent()) ) {
//                     const QTabWidget *tw = (const QTabWidget*)tb->parent();
//                 is there a corner widget in the (top) left edge?
//                     QWidget *cw = tw->cornerWidget(Qt::TopLeft);
//                     if(cw)
//                         cornerWidget = true;
//                 }


            // TODO: draw shading around tab bars...

                const bool leftAligned = (isFirst&&!reverseLayout&&!cornerWidget); // aligned to the panel
                const bool rightAligned = (isFirst&&reverseLayout&&!cornerWidget);
                const bool left = (isFirst&&!reverseLayout) || (isLast&&reverseLayout);
                const bool right = (isFirst&&reverseLayout) || (isLast&&!reverseLayout);

                const int mBelow = 1;
                int mBefore = 0, mAfter = 0, mAbove = 0; // margins...

                // the contour rect...
                QRect rC;
                if (selected) {
                    if (leftAligned)
                        mBefore = 1;
                    if (rightAligned)
                        mAfter = 1;
                } else {
                    if (!triangular)
                        mAbove = 2;
                    if (isFirst && !reverseLayout && !cornerWidget)
                        mBefore = 2;
                    if (isFirst && reverseLayout && !cornerWidget)
                        mAfter = 2;
                }

                if (dir == North)
                    rC = MODIFY_RECT(r, mBefore, mAbove, -mBefore-mAfter, -mAbove-mBelow);
                else if (dir == South)
                    rC = MODIFY_RECT(r, mBefore, mBelow, -mBefore-mAfter, -mBelow-mAbove);
                else if (dir == East)
                    rC = MODIFY_RECT(r, mBelow, mBefore, -mBelow-mAbove, -mBefore-mAfter);
                else
                    rC = MODIFY_RECT(r, mAbove, mBefore, -mAbove-mBelow, -mBefore-mAfter);

                if (selected) {
            // is selected

                // the top part of the tab which is nearly the same for all positions
                    uint contourFlags = 0;
                    if(dir == North) {
                        contourFlags |= Draw_Top|Draw_Left|Draw_Right;
                        if (!triangular)
                            contourFlags |= Round_UpperLeft|Round_UpperRight;
                    } else if (dir == East) {
                        contourFlags = Draw_Top|Draw_Right|Draw_Bottom;
                        if (!triangular)
                            contourFlags |= Round_UpperRight|Round_BottomRight;
                    } else if (dir == South) {
                        contourFlags |= Draw_Bottom|Draw_Left|Draw_Right;
                        if (!triangular)
                            contourFlags |= Round_BottomLeft|Round_BottomRight;
                    } else {
                        contourFlags |= Draw_Left|Draw_Top|Draw_Bottom;
                        if (!triangular)
                            contourFlags |= Round_UpperLeft|Round_BottomLeft;
                    }
                    renderContour(p, rC,
                                  pal.background().color(), getColor(pal,PanelContour),
                                  contourFlags);

                // some "position specific" paintings...
                    // aligned connection from the tab to the panel border
                    if(leftAligned) {
                        p->setPen(getColor(pal,PanelContour) );
                        if (dir == North)
                            p->drawPoint(rC.x(), r.bottom() );
                        else if (dir == South)
                            p->drawPoint(rC.x(), r.top() );
                        else if (dir == East)
                            p->drawPoint(r.left(), rC.y() );
                        else
                            p->drawPoint(r.right(), rC.y() );
                    } else if(rightAligned) {
                        p->setPen(getColor(pal,PanelContour) );
                        if (dir == North)
                            p->drawPoint(rC.right(), r.bottom() );
                        else if (dir == South)
                            p->drawPoint(rC.right(), r.top() );
                        else if (dir == East)
                            p->drawPoint(r.left(), rC.y() );
                        else
                            p->drawPoint(r.right(), rC.y() );
                    }

                } else {
            // inactive tabs

                    // surface rect
                    QRect rS;
                    mBefore = 0;
                    mAfter = 0;
                    if (left)
                        mBefore = 1;
                    if (!nextIsSelected)
                        mAfter = 1;

                    if (dir == North)
                        rS = MODIFY_RECT(rC, mBefore, 1, -mBefore-mAfter, -1);
                    else if (dir == South)
                        rS = MODIFY_RECT(rC, mBefore, 0, -mBefore-mAfter, -1);
                    else if (dir == East)
                        rS = MODIFY_RECT(rC, 0, mBefore, -1, -mBefore-mAfter);
                    else
                        rS = MODIFY_RECT(rC, 1, mBefore, -1, -mBefore-mAfter);

                    uint contourFlags = 0;
                    uint surfaceFlags = Draw_Left|Draw_Top|Draw_Bottom|Draw_Right;

                    if (dir == North || dir == South) {
                        if (!nextIsSelected)
                            contourFlags |= Draw_Right;

                        surfaceFlags |= Is_Horizontal;

                    } else {
                        if (!nextIsSelected)
                            contourFlags |= Draw_Bottom;
                    }

                    if(dir == North) {
                        contourFlags |= Draw_Top;
                        if (left) {
                            contourFlags |= Draw_Left|Round_UpperLeft;
                            surfaceFlags |= Round_UpperLeft;
                        } else if ( right ) {
                            contourFlags |= Round_UpperRight;
                            surfaceFlags |= Round_UpperRight;
                        }
                    } else if (dir == East) {
                        contourFlags |= Draw_Right;
                        if (left) {
                            contourFlags |= Draw_Top|Round_UpperRight;
                            surfaceFlags |= Round_UpperRight;
                        } else if ( right ) {
                            contourFlags |= Round_BottomRight;
                            surfaceFlags |= Round_BottomRight;
                        }
                    } else if (dir == South) {
                        contourFlags |= Draw_Bottom;
                        if (left) {
                            contourFlags |= Draw_Left|Round_BottomLeft;
                            surfaceFlags |= Round_BottomLeft;
                        } else if (right) {
                            contourFlags |= Round_BottomRight;
                            surfaceFlags |= Round_BottomRight;
                        }
                    } else {
                        contourFlags |= Draw_Left;
                        if (left) {
                            contourFlags |= Draw_Top|Round_UpperLeft;
                            surfaceFlags |= Round_UpperLeft;
                        } else if (right) {
                            contourFlags |= Round_BottomLeft;
                            surfaceFlags |= Round_BottomLeft;
                        }
                    }

                    if(mouseOver) {
                        surfaceFlags |= Is_Highlight;
                    }

                    renderContour(p, rC,
                                  pal.background().color(), getColor(pal, ButtonContour),
                                  contourFlags);

                    renderSurface(p, rS,
                                  pal.background().color(), pal.button().color(), getColor(pal,MouseOverHighlight), _contrast,
                                  surfaceFlags);

            // some "position specific" paintings...
                // fake parts of the panel border
                    mBefore = 0;
                    mAfter = 0;
                    if (isFirst && !reverseLayout && !cornerWidget) {
                        mBefore = 2;
                    }
                    if (isFirst && reverseLayout && !cornerWidget) {
                        mAfter = 2;
                    }
                    p->setPen(getColor(pal,PanelContour) );
                    if (dir == North)
                        p->drawLine (r.left()+mBefore, r.bottom(), r.right()-mAfter, r.bottom() );
                    else if (dir == South)
                        p->drawLine (r.left()+mBefore, r.top(), r.right()-mAfter, r.top() );
                    else if (dir == East)
                        p->drawLine (r.left(), r.top()+mBefore, r.left(), r.bottom()-mAfter);
                    else
                        p->drawLine (r.right(), r.top()+mBefore, r.right(), r.bottom()-mAfter);

                }

            }

            break;
        }

        case CE_ScrollBarSubLine:
        case CE_ScrollBarAddLine:
        {
            const bool horizontal = state & State_Horizontal;

            uint contourFlags = Draw_Left|Draw_Right|Draw_Top|Draw_Bottom;
            uint surfaceFlags = Draw_Left|Draw_Right|Draw_Top|Draw_Bottom;
            if(down) surfaceFlags|=Is_Sunken;
            if(!enabled) {
                contourFlags|=Is_Disabled;
                surfaceFlags|=Is_Disabled;
            }

            QRect sr; // the rect where the actual scrollbar is painted.

            // fake part of the scrollBar groove: "empty" 1px wide area between button surface and panel contour
            p->setPen(pal.background().color() ); // for the empty area...
            if(horizontal) {
                contourFlags |= Is_Horizontal;
                surfaceFlags |= Is_Horizontal;
            }

            // fake part of the scrollBar groove: "empty" 1px wide area between button surface and panel contour
            p->setPen(pal.background().color() );
            if (ce == CE_ScrollBarAddLine) {
                if(horizontal) {
                    sr = QRect(r_x, r_y, r_w-1, r_h-1);
                    p->drawLine(r_x2, r_y, r_x2, r_y2);
                    p->drawLine(r_x, r_y2, r_x2-1, r_y2);
                } else {
                    sr = QRect(r_x, r_y, r_w-1, r_h-1);
                    p->drawLine(r_x2, r_y, r_x2, r_y2);
                    p->drawLine(r_x, r_y2, r_x2-1, r_y2);
                }
            } else {
                if(horizontal) {
                    // TODO: change subElementRect etc. to make the second subLine-button a bit shorter.
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
            if (horizontal) {
                p->setPen(getColor(pal,ShadeDark) );
                p->drawPoint(sr.x(), sr.y() );
                p->drawPoint(sr.right(), sr.y() );
                p->setPen(getColor(pal,ShadeLight) );
                p->drawPoint(sr.x(), sr.bottom() );
                p->drawPoint(sr.right(), sr.bottom() );
            } else {
                p->setPen(getColor(pal,ShadeDark) );
                p->drawPoint(sr.x(), sr.y() );
                p->drawPoint(sr.x(), sr.bottom() );
                p->setPen(getColor(pal,ShadeLight) );
                p->drawPoint(sr.right(), sr.y() );
                p->drawPoint(sr.right(), sr.bottom() );
            }

            renderContour(p, sr, pal.background().color(), getColor(pal, ButtonContour), contourFlags);
            renderSurface(p, SHRINK_RECT(sr, 1),
                          pal.background().color(), pal.button().color(), getColor(pal,MouseOverHighlight), _contrast,
                          surfaceFlags);

            // arrows
            PrimitiveElement pe;
            if (ce == CE_ScrollBarAddLine)
                pe = horizontal ? PE_IndicatorArrowRight : PE_IndicatorArrowDown;
            else
                pe = horizontal ? PE_IndicatorArrowLeft : PE_IndicatorArrowUp;
            p->setPen(pal.foreground().color() );
            drawPrimitive(pe, opt, p, widget);

            break;
        }

        case CE_ScrollBarSubPage:
        case CE_ScrollBarAddPage:
        {
            const bool horizontal = state & State_Horizontal;

            QRect sr;
            if(horizontal) {
                sr = MODIFY_RECT(r, 0, +1, 0, -3);

                p->setPen(pal.background().color() );
                p->drawLine(r_x, r_y2, r_x2, r_y2); // fill the empty area...
                p->setPen(getColor(pal,ShadeDark) );
                p->drawLine(r_x, r_y, r_x2, r_y); // dark shade
                p->setPen(getColor(pal,ShadeLight) );
                p->drawLine(r_x, r_y2-1, r_x2, r_y2-1); // light shade
            } else {
                sr = MODIFY_RECT(r, +1, 0, -3, 0);

                p->setPen(pal.background().color() );
                p->drawLine(r_x2, r_y, r_x2, r_y2); // fill the empty area...
                p->setPen(getColor(pal,ShadeDark) );
                p->drawLine(r_x, r_y, r_x, r_y2); // dark shade
                p->setPen(getColor(pal,ShadeLight) );
                p->drawLine(r_x2-1, r_y, r_x2-1, r_y2); // li
            }

            QColor color;
            if (on || down) {
                color = pal.background().color().dark(100+2*_contrast);
            } else {
                color = pal.background().color();
            }

            renderGradient(p, sr, color.dark(100+_contrast/2), color.light(100+_contrast/2), horizontal);

            break;
        }

        case CE_ScrollBarSlider:
        {
            const bool horizontal = state & State_Horizontal;

            const WidgetState s = enabled?(down?IsPressed:IsEnabled):IsDisabled;
            const QColor surface = getColor(pal, DragButtonSurface, s);

            QRect sr; // the rect where the actual scrollbar is painted.
            p->setPen(pal.background().color() );
            if(horizontal) {
                // TODO: check reverse layout.
                sr = QRect(r_x, r_y, r_w, r_h-1);
                p->drawLine(r_x, r_y2, r_x2, r_y2); // fill the empty area...
            } else {
                sr = QRect(r_x, r_y, r_w-1, r_h);
                p->drawLine(r_x2, r_y, r_x2, r_y2); // fill the empty area...
            }

            uint contourFlags = Draw_Left|Draw_Right|Draw_Top|Draw_Bottom;
            if(!enabled) contourFlags|=Is_Disabled;

            renderContour(p, sr, pal.background().color(), getColor(pal, DragButtonContour, s), contourFlags);

            uint surfaceFlags = Draw_Left|Draw_Right|Draw_Top|Draw_Bottom;
            if(horizontal) surfaceFlags|=Is_Horizontal;
            if(!enabled) surfaceFlags|=Is_Disabled;
            if(r.height() >= 4) {
                renderSurface(p, SHRINK_RECT(sr, 1),
                              pal.background().color(), surface, pal.background().color(), _contrast, surfaceFlags);
                // fill the remaining edges
                if (horizontal) {
                    p->setPen(getColor(pal,ShadeDark) );
                    p->drawPoint(sr.x(), sr.y() );
                    p->drawPoint(sr.right(), sr.y() );
                    p->setPen(getColor(pal,ShadeLight) );
                    p->drawPoint(sr.x(), sr.bottom() );
                    p->drawPoint(sr.right(), sr.bottom() );
                } else {
                    p->setPen(getColor(pal,ShadeDark) );
                    p->drawPoint(sr.x(), sr.y() );
                    p->drawPoint(sr.x(), sr.bottom() );
                    p->setPen(getColor(pal,ShadeLight) );
                    p->drawPoint(sr.right(), sr.y() );
                    p->drawPoint(sr.right(), sr.bottom() );
                }
            }

            const int d = 5;
            int n = ((horizontal?sr.width():sr.height() )-8)/d;
            if(n>5) n=5;
            if(!horizontal) {
                for(int j = 0; j < n; j++) {
                    int yPos = sr.center().y()-(n*d)/2+d*j+2;
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

        case CE_HeaderSection:
        {
            const bool horizontal = true;
            const bool sunken = (on||down);

            // detect if this is the right most header item
            bool rightAlignedSection = false;
//             QHeader *header = dynamic_cast<QHeader*>(p->device() );
//             if (header) {
//                 bool resizeFull = false;
//                 QListView *lv = dynamic_cast<QListView*>(header->parent() );
//                 if (lv)
//                     resizeFull = lv->resizeMode() == QListView::AllColumns ||
//                             lv->resizeMode() == QListView::LastColumn;
//                 if (resizeFull)
//                     rightAlignedSection = header->mapToIndex(header->sectionAt(r_x) )==(header->count()-1);
//             }

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
//                     if(mouseOver) {
//                         surfaceFlags|=Is_Highlight;
//                         if(horizontal) {
//                             surfaceFlags|=Highlight_Top;
//                             surfaceFlags|=Highlight_Bottom;
//                         } else {
//                             surfaceFlags|=Highlight_Left;
//                             surfaceFlags|=Highlight_Right;
//                         }
//                     }
                }
            }

            // if it's the right most header item, don't draw the right contour.
            renderContour(p, r,
                          pal.background().color(), getColor(pal,ButtonContour), contourFlags);
            renderSurface(p, MODIFY_RECT(r, 0, 0, (rightAlignedSection?0:-1), -1),
                          pal.background().color(), pal.button().color(), getColor(pal,MouseOverHighlight), _contrast, surfaceFlags);

            break;
        }

        case CE_MenuItem:
        {
            if (const QStyleOptionMenuItem *mi = qstyleoption_cast<const QStyleOptionMenuItem *>(opt) ) {
                bool selected = state & State_Selected;

                // separator
                if (mi->menuItemType == QStyleOptionMenuItem::Separator) {
                    p->setPen(getColor(pal, ShadeDark) );
                    p->drawLine(r_x, r_y, r_x2, r_y);
                    p->setPen(getColor(pal, ShadeLight) );
                    p->drawLine(r_x, r_y + 1, r_x2, r_y + 1);
                    break;
                }

                // background
                if (selected) {
                    p->fillRect(r_x, r_y, r_w, r_h, pal.highlight().color() );
                }


                // TODO: checkmark, icon, text, accelerator

                // submenu arrow
                if (mi->menuItemType == QStyleOptionMenuItem::SubMenu) {
                    int indicatorMetric = pixelMetric(PM_MenuButtonIndicator);
                    // TODO
                    QRect indicatorRect = visualRect(opt->direction, r, QRect(r_x2-5-indicatorMetric, r_y+ (r_h-indicatorMetric)/2,
                                                            indicatorMetric, indicatorMetric) );

                    PrimitiveElement pe = reverseLayout ? PE_IndicatorArrowLeft : PE_IndicatorArrowRight;
                    QStyleOptionMenuItem optMI = *mi;
                    optMI.rect = indicatorRect;
                    optMI.state = enabled ? State_Enabled : State_None;
                    if (selected) {
                        optMI.palette.setColor(QPalette::ButtonText,
                                               optMI.palette.highlightedText().color() );
                    }
                    drawPrimitive(pe, &optMI, p, widget);
                }

            }
        }

        default:
            return QCommonStyle::drawControl(ce, opt, p, widget);
    }
}

void Plastik2Style::drawPrimitive (PrimitiveElement pe, const QStyleOption *opt, QPainter *p, const QWidget *widget) const
{
    QPalette pal = opt->palette;
    QRect r = opt->rect;
    State state = opt->state;

    const QColor cBase = pal.color(QPalette::Base);
    const QColor cBackground = pal.color(QPalette::Background);
    const QColor cButton = pal.color(QPalette::Button);

    const bool enabled = state&State_Enabled;
    const bool mouseOver = state&State_MouseOver;

    switch (pe) {
        case PE_PanelButtonCommand:
        case PE_PanelButtonBevel:
        case PE_PanelButtonTool:
        case PE_IndicatorButtonDropDown:
        {
            const bool on = state&State_On;
            const bool down = state&State_Down;
            renderButton(p, r, pal, (on||down), widget==hoverWidget, true, enabled/*, khtmlMode */);

            break;
        }

        case PE_IndicatorToolBarHandle:
        {
            int counter = 1;

            if(state & State_Horizontal) {
                int center = r.left()+r.width()/2;
                for(int j = r.top()+2; j <= r.bottom()-3; j+=5) {
                    renderDot(p, center-1, j, pal.background().color(), true);
                    counter++;
                }
            } else {
                int center = r.top()+r.height()/2;
                for(int j = r.left()+2; j <= r.right()-3; j+=5) {
                    renderDot(p, j, center-1, pal.background().color(), true);
                    counter++;
                }
            }

            break;
        }

        case PE_Frame:
        {
            if (const QStyleOptionFrame *frame = qstyleoption_cast<const QStyleOptionFrame *>(opt) ) {
                if (frame->lineWidth == 2) {
                    renderFrame(p, r, pal, true, frame->state&State_Sunken);
                } else {
                    QCommonStyle::drawPrimitive(pe, opt, p, widget);
                }
            }

            break;
        }

        case PE_FrameLineEdit:
        {
            bool isReadOnly = false;
            bool isEnabled = true;
            // panel is highlighted by default if it has focus, but if we have access to the
            // widget itself we can try to avoid highlighting in case it's readOnly or disabled.
            // TODO: check if this information can be get using QStyleOption
            QLineEdit* lineEdit = dynamic_cast<QLineEdit*>(p->device());
            if (lineEdit)
            {
                isReadOnly = lineEdit->isReadOnly();
                isEnabled = lineEdit->isEnabled();
            }

            const bool hasFocus = state & State_HasFocus;
            const bool focusHighlight = hasFocus && !isReadOnly && isEnabled;

            uint contourFlags = Draw_Left|Draw_Right|Draw_Top|Draw_Bottom|
                    Is_Sunken|Fill_Edges;
            if (focusHighlight)
                contourFlags |= Is_HighlightShade;

            renderShade(p, r, pal.background().color(), contourFlags, getColor(pal,FocusHighlight,enabled) );
            renderContour(p, SHRINK_RECT(r, 1),
                          pal.background().color(), getColor(pal, focusHighlight?FocusHighlight:ButtonContour ,enabled), contourFlags);

            break;
        }

        case PE_FrameTabWidget:
        {
            renderFrame(p, r, pal, true, state&State_Sunken);

            break;
        }

        case PE_FrameGroupBox:
        {
            renderShade(p, r, pal.background().color(),
                        Draw_Left|Draw_Right|Draw_Top|Draw_Bottom/*|Is_Sunken*/);

            break;
        }

        case PE_FrameMenu:
        {
            // TODO: see if the menu can be masked, otherwise remove round corners
            renderContour(p, r, pal.background().color(), getColor(pal,PanelContour),
                          Draw_Left|Draw_Right|Draw_Top|Draw_Bottom|
                                  Round_UpperLeft|Round_BottomLeft|Round_UpperRight|Round_BottomRight);

            break;
        }

        case PE_FrameFocusRect:
        {
            p->save();

            // TODO: real dotted line...
            p->setPen(pal.foreground().color() );
            p->setPen(Qt::DotLine);
            p->drawRect(MODIFY_RECT(r, 0,0, -1, -1) );

            p->restore();
            break;
        }

        case PE_IndicatorSpinUp:
        case PE_IndicatorSpinDown:
        case PE_IndicatorArrowUp:
        case PE_IndicatorArrowDown:
        case PE_IndicatorArrowRight:
        case PE_IndicatorArrowLeft:
        {
            const QMatrix oldMatrix( p->matrix() );

            p->translate((r.x()+r.width()/2), (r.y()+r.height()/2));

            p->setPen(pal.buttonText().color() );

            bool up = true;

            switch (pe)
            {
                case PE_IndicatorSpinUp:
                case PE_IndicatorArrowUp:
                    up = true;
                    break;
                case PE_IndicatorSpinDown:
                case PE_IndicatorArrowDown:
                    up = false;
                    break;
                case PE_IndicatorArrowRight:
                    up = true;
                    p->rotate(90);
                    break;
                case PE_IndicatorArrowLeft:
                    up = false;
                    p->rotate(90);
                    break;
                default:
                    break;
            }

            if (state & State_Down) {
                p->translate(pixelMetric(PM_ButtonShiftHorizontal),
                             pixelMetric(PM_ButtonShiftVertical));
            }

            if (up) {
                p->drawLine(-1,-2, 0,-2);
                p->drawLine(-2,-1, 1,-1);
                p->drawLine(-3,0, 2,0);
                p->drawLine(-3,1, 2,1);
            } else {
                p->drawLine(-1,1, 0,1);
                p->drawLine(-2,0, 1,0);
                p->drawLine(-3,-1, 2,-1);
                p->drawLine(-3,-2, 2,-2);
            }

            p->setMatrix( oldMatrix );

            break;
        }

        // TODO: PE_IndicatorRadioButtonMask
        case PE_IndicatorCheckBox:
        {
            QColor contentColor = enabled?cBase:cBackground;
            QColor checkmarkColor = enabled?getColor(pal,CheckMark):cBackground;

            uint contourFlags = Draw_Left|Draw_Right|Draw_Top|Draw_Bottom|Is_Sunken|Fill_Edges;
            if(!enabled) {
                contourFlags |= Is_Disabled;
            }
            renderShade(p, r, cBackground, contourFlags);
            renderContour(p, SHRINK_RECT(r, 1), cBackground, getColor(pal, ButtonContour), contourFlags);

            // surface
            if (mouseOver) {
                contentColor = contentColor.dark(105+_contrast*2);
            }
            p->fillRect(SHRINK_RECT(r, 2), contentColor );

            // check mark
            if(state & State_Down) {
                checkmarkColor = alphaBlendColors(contentColor, checkmarkColor, 150);
            }

            int x = r.center().x() - 4, y = r.center().y() - 4;
            if( state & State_On ) {
                DRAW_BITMAP( p, x, y, checkmarkColor, checkmark )
            } else if ( state & State_Off ) {
                // empty
            } else { // tristate
                DRAW_BITMAP( p, x, y, checkmarkColor, tristatemark )
            }

            break;
        }

        case PE_IndicatorRadioButton:
        {
            // RadioButton mark
            QColor contentColor = enabled ? pal.base().color() : pal.background().color();
            QColor checkmarkColor = enabled ? getColor(pal,CheckMark) : pal.background().color();
            if (mouseOver) {
                contentColor = contentColor.dark(105+_contrast*2);
            }
            if(state&State_Down) {
                checkmarkColor = alphaBlendColors(contentColor, checkmarkColor, 150);
            }

            // surface
            DRAW_BITMAP( p, r.x(), r.y(), contentColor, radio_bg );
            // contour
            DRAW_BITMAP( p, r.x(), r.y(), getColor(pal, ButtonContour, enabled), radio_contour );
            // shading
            DRAW_BITMAP( p, r.x(), r.y(), pal.background().color().dark(105+_contrast), radio_shade_top );
            DRAW_BITMAP( p, r.x(), r.y(), pal.background().color().light(105+_contrast), radio_shade_bottom );

            // radio mark
            if (state&State_On || state&State_Down) {
                int x = r.center().x() - 4;
                int y = r.center().y() - 4;
                DRAW_BITMAP( p, x, y, checkmarkColor, radiomark )
            }

            break;
        }
//         case PE_IndicatorRadioButtonMask: {
//             p->fillRect(r, Qt::color0);
//             DRAW_BITMAP( p, r.x(), r.y(), Qt::color1, radio_mask );
// 
//             break;
//         }

        default:
            return QCommonStyle::drawPrimitive(pe, opt, p, widget);
    }
}

QRect Plastik2Style::subElementRect (SubElement se, const QStyleOption *option, const QWidget *widget) const
{
    switch (se) {
        case SE_ProgressBarContents:
        {
            QRect groove = subElementRect(SE_ProgressBarGroove, option, widget);
            return SHRINK_RECT(groove, 2);
        }

        default:
            return QCommonStyle::subElementRect(se, option, widget);
    }
}

QRect Plastik2Style::subControlRect(ComplexControl cc, const QStyleOptionComplex *opt, SubControl sc, const QWidget *widget) const
{
    switch (cc) {
        case CC_ComboBox:
        {
            if (const QStyleOptionComboBox *cb = qstyleoption_cast<const QStyleOptionComboBox *>(opt)) {
                QRect r = cb->rect;

                switch (sc) {
                    case SC_ComboBoxFrame:
                        return cb->rect;
                    case SC_ComboBoxArrow:
                        // TODO: replace cbButtonWidth with something r.height()-dependent?
                        return QRect(r.right()-cbFrameWidth-cbButtonWidth, r.top(),
                                     cbButtonWidth+cbFrameWidth, r.height() );
                    case SC_ComboBoxEditField:
                        return QRect(r.left()+cbFrameWidth, r.top()+cbFrameWidth,
                                     r.width()-2*cbFrameWidth-cbButtonWidth-1, r.height()-2*cbFrameWidth);
                        // 1 px "fake" contour                              ^
                    case SC_ComboBoxListBoxPopup:
                        return cb->popupRect;
                }
            }

            break;
        }

        case CC_SpinBox:
        {
            if (const QStyleOptionSpinBox *sb = qstyleoption_cast<const QStyleOptionSpinBox *>(opt)) {
                QRect r = sb->rect;

                int fw = sb->showFrame ? pixelMetric(PM_SpinBoxFrameWidth, sb, widget) : 0;
                int slider = sb->showSliderIndicator ? qMax(int(r.height() / 20),
                        pixelMetric(PM_SpinBoxSliderHeight, sb, widget) )
                    : 0;
                QSize bs;

                int bh = r.height()/2; // button height
                if (r.height()%2 == 0)
                    bh -= 1;

                const int buttonsWidth = cbButtonWidth+fw;
                const int buttonsLeft = r.right()-cbButtonWidth-cbFrameWidth;

                switch (sc) {
                    case SC_SpinBoxUp:
                        return QRect(buttonsLeft, r.top(), buttonsWidth, bh);
                    case SC_SpinBoxDown:
                        return QRect(buttonsLeft, r.bottom()+1-bh, buttonsWidth, bh);
                    case SC_SpinBoxButtonField:
                        return QRect(buttonsLeft, r.top(), buttonsWidth, r.height() );
                    case SC_SpinBoxEditField:
                        return QRect(r.left()+fw, r.top()+fw,
                                     r.width()-2*fw-cbButtonWidth-1, r.height()-2*fw - slider);
                    case SC_SpinBoxSlider:
                        return (slider > 0 ? QRect(r.left()+fw, r.height()-fw-slider, r.width()-2*fw-cbButtonWidth-1, slider) : QRect());
                    case SC_SpinBoxFrame:
                        return sb->showFrame ? r : QRect();
                }
            }

            break;
        }

        default:
            return QCommonStyle::subControlRect(cc, opt, sc, widget);
    }
}

QSize Plastik2Style::sizeFromContents (ContentsType type, const QStyleOption *option, const QSize &contentsSize, const QWidget *widget) const
{
    switch (type) {
        case CT_MenuBarItem:
        {
            if (contentsSize.isValid() )
                return contentsSize + QSize(7*2, 4*2);
        }

        default:
            return QCommonStyle::sizeFromContents(type, option, contentsSize, widget);
    }
}

int Plastik2Style::pixelMetric (PixelMetric metric, const QStyleOption * opt, const QWidget * widget) const
{
    switch (metric) {
        // RadioButton size
        case PM_ExclusiveIndicatorWidth:
        case PM_ExclusiveIndicatorHeight:
            return 15;

        // Checkbox size
        case PM_IndicatorWidth:
        case PM_IndicatorHeight:
            return 15;

        // pressed button label shift
        case PM_ButtonShiftVertical:
            return 1;
        case PM_ButtonShiftHorizontal:
            return 0;

        // tab bar
// TODO:
//             PM_TabBarTabHSpace
//             PM_TabBarBaseHeight
//             PM_TabBarBaseOverlap
//             PM_TabBarScrollButtonWidth
//             PM_TabBarTabShiftHorizontal
//             PM_TabBarTabShiftVertical
        case PM_TabBarTabOverlap:
            return 0;

        case PM_TabBarTabVSpace:
        {
            if (const QStyleOptionTab *tab = qstyleoption_cast<const QStyleOptionTab *>(opt) ) {
                bool triangular = tab->shape == QTabBar::TriangularNorth || tab->shape == QTabBar::TriangularEast || tab->shape == QTabBar::TriangularSouth || tab->shape == QTabBar::TriangularWest;
                if (triangular)
                    return 4;
                else
                    return 9;
            }
        }

        case PM_MenuPanelWidth:
            return 2;

        case PM_SpinBoxFrameWidth:
        case PM_DefaultFrameWidth:
            return 2;

        case PM_ProgressBarChunkWidth:
            return 10;

        case PM_ScrollBarExtent:
            return 17;

        case PM_SliderControlThickness:
            return 15;
        case PM_SliderLength:
            return 19;

        case PM_ToolBarItemSpacing:
            return 4;

        case PM_SplitterWidth:
            return 6;

        default:
            return QCommonStyle::pixelMetric(metric, opt, widget);
    }
}

void Plastik2Style::renderShade(QPainter *p, const QRect &r, const QColor &background, const uint flags, const QColor &hl) const
{
    int r_w = r.width();
    int r_h = r.height();
    int r_x, r_y, r_x2, r_y2;
    r.getCoords(&r_x, &r_y, &r_x2, &r_y2);

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

    QColor cShadeTop /*= Qt::black*/, cShadeBottom /*= Qt::white*/;
    if (highlight) {
        cShadeTop = cShadeBottom = alphaBlendColors(background, hl, 150);

        // TODO: check alpha blended painting...
//         const int a = 100;
//         cShadeTop = cShadeBottom = hl;
//         cShadeTop.setAlpha(a);
//         cShadeBottom.setAlpha(a);
    } else {
        cShadeTop = sunken ? alphaBlendColors(background, Qt::black, 230) : alphaBlendColors(background, Qt::white, 140);
        cShadeBottom = sunken ? alphaBlendColors(background, Qt::white, 140) : alphaBlendColors(background, Qt::black, 230);

        // TODO: check alpha blended painting...
//         const int a = 30;
//         cShadeTop =    sunken ? QColor(0,0,0,       a) : QColor(255,255,255, a);
//         cShadeBottom = sunken ? QColor(255,255,255, a) : QColor(0,0,0,       a);
    }

    p->setPen(cShadeTop);

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


    p->setPen(cShadeBottom);

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

void Plastik2Style::renderContour(QPainter *p, const QRect &r, const QColor &background, const QColor &contour, const uint flags) const
{
    int r_w = r.width();
    int r_h = r.height();
    int r_x, r_y, r_x2, r_y2;
    r.getCoords(&r_x, &r_y, &r_x2, &r_y2);

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
//     const bool alphaBlend = flags&Draw_AlphaBlend;

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
    r.getCoords(&r_x, &r_y, &r_x2, &r_y2);

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

void Plastik2Style::renderButton(QPainter *p, const QRect &r, const QPalette &pal, bool sunken, bool mouseOver,
                                 bool horizontal, bool enabled /*,bool khtmlMode*/) const
{
    int r_w = r.width();
    int r_h = r.height();
    int r_x, r_y, r_x2, r_y2;
    r.getCoords(&r_x, &r_y, &r_x2, &r_y2);

//     // small fix for the kicker buttons...
//     if(kickerMode) enabled = true;

    const QPen oldPen( p->pen() );

    uint contourFlags = Draw_Left|Draw_Right|Draw_Top|Draw_Bottom;
    if(!enabled) contourFlags|=Is_Disabled;
//     if(khtmlMode) contourFlags|=Draw_AlphaBlend;

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

//     if (flatMode) {
//         renderContour(p, r, pal.background(), pal.color(QPalette::Button).dark(105+_contrast*3),
//                       contourFlags);
//         renderSurface(p, SHRINK_RECT(r, 1)/*QRect(r.left()+1, r.top()+1, r.width()-2, r.height()-2)*/,
//                       pal.background(), pal.button(), getColor(g,MouseOverHighlight), _contrast/2, surfaceFlags);
// 
//         flatMode = false;
//     } else {
        contourFlags |= Round_UpperLeft|Round_UpperRight|Round_BottomLeft|Round_BottomRight;
        surfaceFlags |= Round_UpperLeft|Round_UpperRight|Round_BottomLeft|Round_BottomRight;

//         if (defaultMode) {
//             contourFlags |= Is_Default;
//             surfaceFlags |= Is_Default;
//         }

        renderShade(p, r, pal.color(QPalette::Background), contourFlags|Is_Sunken|Fill_Edges);
        renderContour(p, SHRINK_RECT(r, 1),
                      pal.color(QPalette::Background), getColor(pal,/*defaultMode?DefaultButtonContour:*/ButtonContour), contourFlags);
        renderSurface(p, SHRINK_RECT(r, 2),
                      pal.color(QPalette::Background), pal.color(QPalette::Button), getColor(pal,MouseOverHighlight), _contrast, surfaceFlags);

//         if (defaultMode)
//             defaultMode = false;

//     }

    p->setPen(oldPen);
}

void Plastik2Style::renderComboSpinBoxFrame(QPainter *p, const QRect &r, const QRect &editField, const QPalette &pal, bool enabled, bool editable, bool mouseOver, bool focusHighlight, bool reverseLayout) const
{
    uint contourFlags = Draw_Top|Draw_Bottom;
    if (mouseOver) {
        contourFlags |= Is_Highlight;
    }
    if (!editable) {
        contourFlags |= Round_UpperLeft|Round_BottomLeft|Round_UpperRight|Round_BottomRight;
    } else {
        if (reverseLayout) {
            contourFlags |= Round_UpperLeft|Round_BottomLeft;
        } else {
            contourFlags |= Round_UpperRight|Round_BottomRight;
        }
    }

    if (focusHighlight)
    {
        QRect editFrame = r;
        QRect buttonFrame = r;

        uint editFlags = contourFlags;
        uint buttonFlags = contourFlags;

        // Hightlight only the part of the contour next to the control button
        if (reverseLayout) {
            buttonFrame.setRight(editField.left()-1 );
            editFrame.setLeft(editField.left()-1 );

            editFlags |= Draw_Right;
            buttonFlags |= Draw_Left;

        } else {
            editFrame.setRight(editField.right()+1);
            buttonFrame.setLeft(editField.right()+1);

            editFlags |= Draw_Left;
            buttonFlags |= Draw_Right;
        }
        renderShade(p, editFrame, pal.background().color(),
                    editFlags|Is_Sunken|Fill_Edges|Is_HighlightShade,
                    getColor(pal,FocusHighlight,enabled));
        renderContour(p, reverseLayout ? MODIFY_RECT(editFrame, 0, 1, -1, -2) : MODIFY_RECT(editFrame, 1, 1, -1, -2),
                      pal.background().color(), getColor(pal,FocusHighlight,enabled), editFlags);

        renderShade(p, buttonFrame, pal.background().color(), buttonFlags|Is_Sunken|Fill_Edges);
        renderContour(p, reverseLayout ? MODIFY_RECT(buttonFrame, 1, 1, -1, -2) : MODIFY_RECT(buttonFrame, 0, 1, -1, -2),
                      pal.background().color(),
                      getColor(pal, ButtonContour, enabled), buttonFlags);
    } else {
        contourFlags |= Draw_Left|Draw_Right;
        renderShade(p, r, pal.background().color(), contourFlags|Is_Sunken|Fill_Edges);
        renderContour(p, SHRINK_RECT(r, 1), pal.background().color(),
                      getColor(pal, ButtonContour, enabled), contourFlags);
    }
}

void Plastik2Style::renderFrame(QPainter *p, const QRect &r, const QPalette &pal, bool pseudo3d, bool sunken) const
{
    if (pseudo3d) {
        uint shadeFlags = Draw_Left|Draw_Right|Draw_Top|Draw_Bottom;
        if (sunken)
            shadeFlags |= Is_Sunken;
        renderShade (p, r, pal.background().color(), shadeFlags);
    }

    uint contourFlags = Draw_Left|Draw_Right|Draw_Top|Draw_Bottom;
    renderContour (p, SHRINK_RECT(r, 1), pal.background().color(), getColor(pal, PanelContour), contourFlags);
}

void Plastik2Style::renderProgressBar(QPainter *p, QRect &r, const QColor &fg, const QColor &bg, double percentage, int animShift, bool reverseLayout) const
{
    int w = static_cast<int>(r.width() * percentage);
    // set a minimal progressbar width...
    if(w<2) w = 2;
    int w2 = r.width()-(r.width()-w);

    QRect rBar(reverseLayout?r.right()-w2+1:r.left(), r.top(), w2, r.height() );
    QRect rEmpty(reverseLayout?r.left():r.left()+w-1, r.top(), r.width()-w+1, r.height() );

    p->fillRect(rEmpty, bg);

    const int cw = pixelMetric(PM_ProgressBarChunkWidth);

    int x = rBar.x() - 2*cw;
    int count = 0;

    QColor c1 = fg;
    QColor c2 = fg.light(100+_contrast);

    QRegion mask(rBar);
    p->setClipRegion(mask);

    while (x < (rBar.right()+2*cw) ) {
        p->fillRect(QRect(reverseLayout?x-animShift:x+animShift, rBar.y(), cw, rBar.height() ),
                    (count%2==0)?c1:c2);
        x += cw;
        count++;
    }

    p->setClipping(false);
}

void Plastik2Style::renderGradient(QPainter *p, const QRect &r, const QColor &c1, const QColor &c2, bool horizontal) const
{
    if((r.width() <= 0)||(r.height() <= 0))
        return;

    QBrush brush;

    if (horizontal) {
        QLinearGradient gradient(r.topLeft(), r.bottomLeft() );
        gradient.appendStop(0, c1);
        gradient.appendStop(1, c2);
        brush = QBrush(gradient);
    } else {
        QLinearGradient gradient(r.topLeft(), r.topRight() );
        gradient.appendStop(0, c1);
        gradient.appendStop(1, c2);
        brush = QBrush(gradient);
    }

    p->fillRect(r, brush);
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

QColor Plastik2Style::getColor(const QPalette &pal, const ColorType t, const bool enabled)const
{
    return getColor(pal, t, enabled?IsEnabled:IsDisabled);
}

QColor Plastik2Style::getColor(const QPalette &pal, const ColorType t, const WidgetState s)const
{
    const bool enabled = (s != IsDisabled) &&
            ((s == IsEnabled) || (s == IsPressed) || (s == IsHighlighted));
    const bool pressed = (s == IsPressed);
    const bool highlighted = (s == IsHighlighted);
    switch(t) {
        case DefaultButtonContour:
            return enabled ? pal.color(QPalette::Foreground)
            : pal.color(QPalette::Background).dark(190+_contrast*10);
        case ButtonContour:
            return enabled ? pal.color(QPalette::Button).dark(130+_contrast*8)
            : pal.color(QPalette::Background).dark(120+_contrast*8);
            case DragButtonContour: {
                if(enabled) {
                    if(pressed)
                        return pal.color(QPalette::Button).dark(130+_contrast*6); // bright
                    else if(highlighted)
                        return pal.color(QPalette::Button).dark(130+_contrast*9); // dark
                    else
                        return pal.color(QPalette::Button).dark(130+_contrast*8); // normal
                } else {
                    return pal.color(QPalette::Background).dark(120+_contrast*8);
                }
            }
            case DragButtonSurface: {
                if(enabled) {
                    if(pressed)
                        return pal.color(QPalette::Button).dark(100-_contrast);  // bright
                    else if(highlighted)
                        return pal.color(QPalette::Button).light(100+_contrast); // dark
                    else
                        return pal.color(QPalette::Button);                      // normal
                } else {
                    return pal.color(QPalette::Background);
                }
            }
        case PanelContour:
            return pal.color(QPalette::Background).dark(160+_contrast*8);
        case PanelDark:
            return alphaBlendColors(pal.color(QPalette::Background), pal.color(QPalette::Background).dark(120+_contrast*5), 110);
        case PanelLight:
            return alphaBlendColors(pal.color(QPalette::Background), pal.color(QPalette::Background).light(120+_contrast*5), 110);
        case ShadeLight:
            return pal.color(QPalette::Background).light(105+_contrast);
        case ShadeDark:
            return pal.color(QPalette::Background).dark(105+_contrast);
        case MouseOverHighlight:
//             if( _customOverHighlightColor )
//                 return _overHighlightColor;
//             else
                return pal.color(QPalette::Highlight);
        case FocusHighlight:
//             if( _customFocusHighlightColor )
//                 return _focusHighlightColor;
//             else
                return pal.color(QPalette::Highlight);
        case CheckMark:
//             if( _customCheckMarkColor )
//                 return _checkMarkColor;
//             else
                return pal.color(QPalette::Foreground);
    }

    return Qt::color0;
}

QColor Plastik2Style::alphaBlendColors(const QColor &bgColor, const QColor &fgColor, int alpha) const
{
    // normal button...
    QRgb rgb = bgColor.rgb();
    QRgb rgb_b = fgColor.rgb();
    if(alpha>255) alpha = 255;
    if(alpha<0) alpha = 0;
    int inv_alpha = 255 - alpha;

    QColor result  = QColor( qRed(rgb_b)*inv_alpha/255 + qRed(rgb)*alpha/255,
                             qGreen(rgb_b)*inv_alpha/255 + qGreen(rgb)*alpha/255,
                             qBlue(rgb_b)*inv_alpha/255 + qBlue(rgb)*alpha/255,
                             qAlpha(rgb_b)*inv_alpha/255 + qAlpha(rgb)*alpha/255);

    return result;
}

// kate: indent-width 4; replace-tabs on; tab-width 4; space-indent on;
