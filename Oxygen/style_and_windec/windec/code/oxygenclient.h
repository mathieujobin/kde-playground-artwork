//////////////////////////////////////////////////////////////////////////////
// exampleclient.h
// -------------------
// Minimalistic window decoration for KDE
// -------------------
// Copyright (c) 2003, 2004 David Johnson <david@usermode.org>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//////////////////////////////////////////////////////////////////////////////

#ifndef EXAMPLECLIENT_H
#define EXAMPLECLIENT_H

#include <qabstractbutton.h>
#include <qlayout.h>
//Added by qt3to4:
#include <QResizeEvent>
#include <QShowEvent>
#include <QEvent>
#include <QMouseEvent>
#include <QPaintEvent>
#include <kdecoration.h>
#include <kdecorationfactory.h>

class QSpacerItem;
class QPoint;

namespace Minimalistic {

class MinimalisticClient;

enum ButtonType {
    ButtonHelp=0,
    ButtonMax,
    ButtonMin,
    ButtonClose, 
    ButtonMenu,
    ButtonSticky,
    ButtonTypeCount
};

// MinimalisticFactory /////////////////////////////////////////////////////////////

class MinimalisticFactory: public KDecorationFactory
{
public:
    MinimalisticFactory();
    virtual ~MinimalisticFactory();
    virtual KDecoration *createDecoration(KDecorationBridge *b);
    virtual bool reset(unsigned long changed);

    static bool initialized();
    static Qt::Alignment titleAlign();

private:
    bool readConfig();

private:
    static bool initialized_;
    static Qt::Alignment titlealign_;
};

inline bool MinimalisticFactory::initialized()
    { return initialized_; }

inline Qt::Alignment MinimalisticFactory::titleAlign()
    { return titlealign_; }

// MinimalisticButton //////////////////////////////////////////////////////////////

class MinimalisticButton : public QAbstractButton
{
public:
    MinimalisticButton(MinimalisticClient *parent=0, const char *name=0,
                  const QString &tip=NULL,
                  ButtonType type=ButtonHelp,
                  const unsigned char *bitmap=0);
    ~MinimalisticButton();

    void setBitmap(const unsigned char *bitmap);
    QSize sizeHint() const;
    int lastMousePress() const;
    void reset();

private:
    void enterEvent(QEvent *e);
    void leaveEvent(QEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void drawButton(QPainter *painter);

private:
    MinimalisticClient *client_;
    ButtonType type_;
    QBitmap *deco_;
    int lastmouse_;
};

inline int MinimalisticButton::lastMousePress() const
    { return lastmouse_; }

inline void MinimalisticButton::reset()
    { repaint(false); }

// MinimalisticClient //////////////////////////////////////////////////////////////

class MinimalisticClient : public KDecoration
{
    Q_OBJECT
public:
    MinimalisticClient(KDecorationBridge *b, KDecorationFactory *f);
    virtual ~MinimalisticClient();

    virtual void init();

    virtual void activeChange();
    virtual void desktopChange();
    virtual void captionChange();
    virtual void iconChange();
    virtual void maximizeChange();
    virtual void shadeChange();

    virtual void borders(int &l, int &r, int &t, int &b) const;
    virtual void resize(const QSize &size);
    virtual QSize minimumSize() const;
    virtual Position mousePosition(const QPoint &point) const;

private:
    void addButtons(Q3BoxLayout* layout, const QString& buttons);

    bool eventFilter(QObject *obj, QEvent *e);
    void mouseDoubleClickEvent(QMouseEvent *e);
    void paintEvent(QPaintEvent *e);
    void resizeEvent(QResizeEvent *);
    void showEvent(QShowEvent *);
    void doShape();
private slots:
    void maxButtonPressed();
    void menuButtonPressed();

private:
    MinimalisticButton *button[ButtonTypeCount];
    QSpacerItem *titlebar_;
};

} // namespace Minimalistic

#endif // EXAMPLECLIENT_H
