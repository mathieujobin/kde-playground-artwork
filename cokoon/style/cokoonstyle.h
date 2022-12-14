/* coKoon theme widget style
   Copyright © 2006 Sandro Giessl <giessl@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
 */

#ifndef COKOON_STYLE_H
#define COKOON_STYLE_H

#include <kstyle.h>

#include <cokoon/document.h>
#include <cokoon/bridges.h>

#include "CokoonStyleSpec.h"

class CokoonStyle : public KStyle
{
    public:
        CokoonStyle(const QString &themeName);
        virtual ~CokoonStyle();

        void polish(QWidget* widget);
        void unpolish(QWidget* widget);

/*     virtual void drawKStylePrimitive(WidgetType widgetType, int primitive,  */
/*                                      const QStyleOption* opt, */
/*                                      QRect r, QPalette pal, State flags, */
/*                                      QPainter* p,  */
/*                                      const QWidget* widget = 0, */
/*                                      Option* kOpt    = 0) const; */


        virtual void drawKStylePrimitive(WidgetType widgetType, int primitive,
                                         const QStyleOption* opt,
                                         const QRect &r, const QPalette &pal,
                                         State flags, QPainter* p,
                                         const QWidget* widget = 0,
                                         Option* kOpt = 0) const;

    protected:
        QString themeName;
        CokoonStyleSpec::SpecDocument theme;
};

#endif // COKOON_STYLE_H

// kate: space-indent on; indent-width 4; mixedindent off; indent-mode cstyle;
