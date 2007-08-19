/* coKoon theme KWin decoration
  Copyright (C) 2006 Sandro Giessl <giessl@kde.org>

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public
  License as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; see the file COPYING.  If not, write to
  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
  Boston, MA 02110-1301, USA.
 */

#ifndef COKOON_DECORATION_H
#define COKOON_DECORATION_H

#include <qfont.h>
#include <qcolor.h>
#include <kdecoration.h>
#include <kdecorationfactory.h>
#include <kcommondecoration.h>

#include <cokoon/document.h>
#include <cokoon/bridges.h>

#include "CokoonDecorationSpec.h"
#include <kcomponentdata.h>

class Cokoon::Object;

namespace KWinCokoon {

class Client;
class Handler;

class Handler: public QObject, public KDecorationFactory
{
    Q_OBJECT
public:
    Handler();
    ~Handler();
    virtual bool reset( unsigned long changed );

    virtual KDecoration* createDecoration( KDecorationBridge* );
    virtual bool supports( Ability ability );

    bool reverseLayout() { return m_reverse; }

    QList< Handler::BorderSize >  borderSizes() const;

    CokoonDecorationSpec::SpecDocument theme;
    KComponentData componentData;
    QString themeName;
    bool menuClose;
    bool customBtnSize;
    bool respectBorderSize;
    double borderSize;
    Qt::Alignment titleAlignment;
private:
    void readConfig();

    bool m_reverse;
};

class Button;
class Client : public KCommonDecoration,
               public Cokoon::ExpressionVariableBridge,
               public Cokoon::SpecialCellBridge
{
    public:
        Client(KDecorationBridge* bridge, Handler* handler);
        ~Client();

        void captionChange();

        virtual QString visibleName() const;
        virtual QString defaultButtonsLeft() const;
        virtual QString defaultButtonsRight() const;
        virtual bool decorationBehaviour(DecorationBehaviour behaviour) const;
        virtual int layoutMetric(LayoutMetric lm, bool respectWindowState = true, const KCommonDecorationButton * = 0) const;
        virtual QRegion cornerShape(WindowCorner corner);
        virtual KCommonDecorationButton *createButton(ButtonType type);

        virtual void init();
        virtual void reset( unsigned long changed );

        virtual void paintEvent(QPaintEvent *e);

        // ExpressionVariableBridge
        virtual QVariant getVariableValue(int idx) const;

        // SpecialCellBridge
        virtual void paintCell(int id, QPainter *p,
                               int left, int top, int width, int height) const;

    private:
        const Cokoon::Object *getObj(int objId) const;

        int themeObjectId() const;

        friend class Button;

        Handler *m_handler;

        void updCaption();
        const QFont m_titleFont;
        QString m_caption;
};

class Button : public KCommonDecorationButton
{
    public:
        Button(ButtonType type, Client *parent);
        ~Button();

        int themeObjectId() const;

        void reset(unsigned long changed);

    protected:
        void paintEvent(QPaintEvent *);

    private:
        void enterEvent(QEvent *e);
        void leaveEvent(QEvent *e);

    private:
        bool hover;
        Handler *m_handler;
        Client *m_client;
};

} // KWinCokoon

#endif // COKOON_DECORATION_H

// kate: space-indent on; indent-width 4; mixedindent off; indent-mode cstyle;
