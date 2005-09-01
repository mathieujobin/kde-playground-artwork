/* Plastik widget style for KDE 4
   Copyright (C) 2005 Sandro Giessl <sandro@giessl.com>

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

#include <QPainter>
#include <QStyleOption>

#include "plastik.h"
// #include "plastik.moc"

K_EXPORT_STYLE("Plastik42", Plastik2Style)


Plastik2Style::Plastik2Style()
{
    setWidgetLayoutProp(WT_Generic, Generic::DefaultFrameWidth, 2);

    setWidgetLayoutProp(WT_PushButton, PushButton::DefaultIndicatorMargin, 1);
    setWidgetLayoutProp(WT_PushButton, PushButton::FocusMargin, 3);
}

Plastik2Style::~Plastik2Style()
{
}

void Plastik2Style::drawKStylePrimitive(WidgetType widgetType, int primitive,
                                       const QStyleOption* opt,
                                       QRect r, QPalette pal, State flags,
                                       QPainter* p,
                                       const QWidget* widget,
                                       KStyle::Option* kOpt) const
{
//     bool reverseLayout = opt->direction == Qt::RightToLeft;
// 
//     bool enabled = flags & State_Enabled;

    switch (widgetType)
    {
        case WT_PushButton:
        {
            switch (primitive)
            {
                case Generic::Bevel:
                {


//                     return;
                }

                case PushButton::DefaultButtonBevel:
                {


//                     return;
                }
            }
        }
        break;



    }

    // default fallback
    KStyle::drawKStylePrimitive(widgetType, primitive, opt,
                                r, pal, flags, p, widget, kOpt);
}
