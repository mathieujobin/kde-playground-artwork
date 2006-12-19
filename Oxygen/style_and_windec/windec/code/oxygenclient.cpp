//////////////////////////////////////////////////////////////////////////////
// exampleclient.cc
// -------------------
// Minimalistic window decoration for KDE
// -------------------
// Copyright (c) 2003, 2004 David Johnson
// Please see the header file for copyright and license information.
//////////////////////////////////////////////////////////////////////////////

#include <kconfig.h>
#include <kglobal.h>
#include <kglobalsettings.h>
#include <klocale.h>
#include <kdebug.h>

#include <qbitmap.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qpainter.h>
#include <qtooltip.h>
//Added by qt3to4:
#include <Q3HBoxLayout>
#include <Q3GridLayout>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QEvent>
#include <QShowEvent>
#include <QPaintEvent>

#include "oxygenclient.h"

using namespace Minimalistic;

// global constants

static const int BUTTONSIZE      = 18;
static const int DECOSIZE        = 8;
static const int TITLESIZE       = 18;
static const int TFRAMESIZE       = 8;
static const int BFRAMESIZE       = 7;
static const int LFRAMESIZE       = 8;
static const int RFRAMESIZE       = 7;
static const int FRAMEBUTTONSPACE       = 67;



// this method "stolen" from Plastic theme
void renderDot(QPainter *p,
                             const QPoint &point,
                             const QColor &baseColor,
                             const bool sunken)
{
    const QColor topColor = sunken?baseColor.dark(130):baseColor.light(120);
    const QColor bottomColor = sunken?baseColor.light(120):baseColor.dark(130);
    const QColor halftopColor = sunken?baseColor.dark(115):baseColor.light(110);
    const QColor halfbottomColor = sunken?baseColor.light(110):baseColor.dark(115);
    const QColor midColor = baseColor.light(110);

    p->setPen(topColor );
    p->drawPoint(point.x()+1, point.y());
    p->drawPoint(point.x(), point.y()+1);
    p->setPen(bottomColor );
    p->drawPoint(point.x()+1, point.y()+2);
    p->drawPoint(point.x()+2, point.y()+1);
    p->setPen(halftopColor );
    p->drawPoint(point.x(), point.y());
    p->setPen(halfbottomColor );
    p->drawPoint(point.x()+2, point.y()+2);
}

// window button decorations

static const unsigned char close_bits[] = {
    0xc3, 0x66, 0x7e, 0x3c, 0x3c, 0x7e, 0x66, 0xc3};

static const unsigned char help_bits[] = {
    0x7e, 0x7e, 0x60, 0x78, 0x78, 0x00, 0x18, 0x18};

static const unsigned char max_bits[] = {
    0x00, 0x18, 0x3c, 0x7e, 0xff, 0xff, 0x00, 0x00};

static const unsigned char min_bits[] = {
//    0x00, 0x00, 0xff, 0xff, 0x7e, 0x3c, 0x18, 0x00};
    0x00, 0x00, 0x38, 0x38, 0x38, 0x00, 0x00, 0x00};

static const unsigned char minmax_bits[] = {
    0x00, 0x02, 0x06, 0x0e, 0x1e, 0x3e, 0x7e, 0x00};

static const unsigned char stickydown_bits[] = {
    0x00, 0x18, 0x18, 0x7e, 0x7e, 0x18, 0x18, 0x00};

static const unsigned char sticky_bits[] = {
    0x00, 0x00, 0x00, 0x7e, 0x7e, 0x00, 0x00, 0x00};

//////////////////////////////////////////////////////////////////////////////
// MinimalisticFactory Class                                                     //
//////////////////////////////////////////////////////////////////////////////

bool MinimalisticFactory::initialized_              = false;
Qt::AlignmentFlags MinimalisticFactory::titlealign_ = Qt::AlignHCenter;

extern "C" KDecorationFactory* create_factory()
{
    return new Minimalistic::MinimalisticFactory();
}

//////////////////////////////////////////////////////////////////////////////
// MinimalisticFactory()
// ----------------
// Constructor

MinimalisticFactory::MinimalisticFactory()
{
    readConfig();
    initialized_ = true;
}

//////////////////////////////////////////////////////////////////////////////
// ~MinimalisticFactory()
// -----------------
// Destructor

MinimalisticFactory::~MinimalisticFactory() { initialized_ = false; }

//////////////////////////////////////////////////////////////////////////////
// createDecoration()
// -----------------
// Create the decoration

KDecoration* MinimalisticFactory::createDecoration(KDecorationBridge* b)
{
    return new MinimalisticClient(b, this);
}

//////////////////////////////////////////////////////////////////////////////
// reset()
// -------
// Reset the handler. Returns true if decorations need to be remade, false if
// only a repaint is necessary

bool MinimalisticFactory::reset(unsigned long changed)
{
    // read in the configuration
    initialized_ = false;
    bool confchange = readConfig();
    initialized_ = true;

    if (confchange ||
        (changed & (SettingDecoration | SettingButtons | SettingBorder))) {
        return true;
    } else {
        resetDecorations(changed);
        return false;
    }
}

//////////////////////////////////////////////////////////////////////////////
// readConfig()
// ------------
// Read in the configuration file

bool MinimalisticFactory::readConfig()
{
    // create a config object
    KConfig config("kwinexamplerc");
    config.setGroup("General");

    // grab settings
    Qt::AlignmentFlags oldalign = titlealign_;
    QString value = config.readEntry("TitleAlignment", "AlignHCenter");
    if (value == "AlignLeft") titlealign_ = Qt::AlignLeft;
    else if (value == "AlignHCenter") titlealign_ = Qt::AlignHCenter;
    else if (value == "AlignRight") titlealign_ = Qt::AlignRight;

    if (oldalign == titlealign_)
        return false;
    else
        return true;
}

//////////////////////////////////////////////////////////////////////////////
// MinimalisticButton Class                                                      //
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// MinimalisticButton()
// ---------------
// Constructor

MinimalisticButton::MinimalisticButton(MinimalisticClient *parent, const char *name,
                             const QString& tip, ButtonType type,
                             const unsigned char *bitmap)
    : QButton(parent->widget(), name), client_(parent), type_(type),
      deco_(0), lastmouse_(0)
{
    setBackgroundMode(Qt::NoBackground);
    setFixedSize(BUTTONSIZE, BUTTONSIZE);
    setCursor(Qt::arrowCursor);
    if (bitmap) setBitmap(bitmap);
    QToolTip::add(this, tip);
}

MinimalisticButton::~MinimalisticButton()
{
    if (deco_) delete deco_;
}

//////////////////////////////////////////////////////////////////////////////
// setBitmap()
// -----------
// Set the button decoration

void MinimalisticButton::setBitmap(const unsigned char *bitmap)
{
    if (!bitmap) return; // no bitmap, probably the menu button

    if (deco_) delete deco_;
    deco_ = new QBitmap(DECOSIZE, DECOSIZE, bitmap, true);
    deco_->setMask(*deco_);
    repaint(false);
}

//////////////////////////////////////////////////////////////////////////////
// sizeHint()
// ----------
// Return size hint

QSize MinimalisticButton::sizeHint() const
{
    return QSize(BUTTONSIZE, BUTTONSIZE);
}

//////////////////////////////////////////////////////////////////////////////
// enterEvent()
// ------------
// Mouse has entered the button

void MinimalisticButton::enterEvent(QEvent *e)
{
    // if we wanted to do mouseovers, we would keep track of it here
    QButton::enterEvent(e);
}

//////////////////////////////////////////////////////////////////////////////
// leaveEvent()
// ------------
// Mouse has left the button

void MinimalisticButton::leaveEvent(QEvent *e)
{
    // if we wanted to do mouseovers, we would keep track of it here
    QButton::leaveEvent(e);
}

//////////////////////////////////////////////////////////////////////////////
// mousePressEvent()
// -----------------
// Button has been pressed

void MinimalisticButton::mousePressEvent(QMouseEvent* e)
{
    lastmouse_ = e->button();

    // translate and pass on mouse event
    int button = Qt::LeftButton;
    if ((type_ != ButtonMax) && (e->button() != Qt::LeftButton)) {
        button = Qt::NoButton; // middle & right buttons inappropriate
    }
    QMouseEvent me(e->type(), e->pos(), e->globalPos(),
                   button, e->state());
    QButton::mousePressEvent(&me);
}

//////////////////////////////////////////////////////////////////////////////
// mouseReleaseEvent()
// -----------------
// Button has been released

void MinimalisticButton::mouseReleaseEvent(QMouseEvent* e)
{
    lastmouse_ = e->button();

    // translate and pass on mouse event
    int button = Qt::LeftButton;
    if ((type_ != ButtonMax) && (e->button() != Qt::LeftButton)) {
        button = Qt::NoButton; // middle & right buttons inappropriate
    }
    QMouseEvent me(e->type(), e->pos(), e->globalPos(), button, e->state());
    QButton::mouseReleaseEvent(&me);
}

//////////////////////////////////////////////////////////////////////////////
// drawButton()
// ------------
// Draw the button

void MinimalisticButton::drawButton(QPainter *painter)
{
    if (!MinimalisticFactory::initialized()) return;

    QColorGroup group;
    int dx, dy;

    // paint a plain box with border
    group = KDecoration::options()->colorGroup(KDecoration::ColorFrame, client_->isActive());
    painter->fillRect(rect(), group.button());
    
    if (type_ == ButtonMenu) {
        // we paint the mini icon (which is 16 pixels high)
        dx = (width() - 16) / 2;
        dy = (height() - 16) / 2;
        painter->drawPixmap(dx, dy, client_->icon().pixmap(QIcon::Small,
                                                           QIcon::Normal));
    } else {
//       painter->fillRect(rect(), group.button());
       int x,y,w,h;
       rect().rect(&x, &y, &w, &h);
       int x2=x+w-1;
       int y2=y+h-1;
       painter->setPen(group.dark());
       painter->drawEllipse(x, y, w, h);

       if (deco_) {
           // otherwise we paint the deco
           dx = (width() - DECOSIZE) / 2;
           dy = (height() - DECOSIZE) / 2;
           painter->setPen(group.dark());
           painter->drawPixmap(dx, dy, *deco_);
       }
    }
}

//////////////////////////////////////////////////////////////////////////////
// MinimalisticClient Class                                                      //
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// MinimalisticClient()
// ---------------
// Constructor

MinimalisticClient::MinimalisticClient(KDecorationBridge *b, KDecorationFactory *f)
    : KDecoration(b, f) { ; }

MinimalisticClient::~MinimalisticClient()
{
    for (int n=0; n<ButtonTypeCount; n++) {
        if (button[n]) delete button[n];
    }
}

//////////////////////////////////////////////////////////////////////////////
// init()
// ------
// Actual initializer for class

void MinimalisticClient::init()
{
    createMainWidget(Qt::WResizeNoErase | Qt::WNoAutoErase);
    widget()->installEventFilter(this);

    // for flicker-free redraws
    widget()->setBackgroundMode(Qt::NoBackground);

    // setup layout
    Q3GridLayout *mainlayout = new Q3GridLayout(widget(), 4, 3); // 4x3 grid
    Q3HBoxLayout *titlelayout = new Q3HBoxLayout();
    titlebar_ = new QSpacerItem(1, TITLESIZE, QSizePolicy::Expanding,
                                QSizePolicy::Fixed);

    mainlayout->setResizeMode(QLayout::SetNoConstraint);
    mainlayout->addRowSpacing(0, TFRAMESIZE);
    mainlayout->addRowSpacing(3, BFRAMESIZE);
    mainlayout->addColSpacing(0, LFRAMESIZE);
    mainlayout->addColSpacing(2, RFRAMESIZE);

    mainlayout->addLayout(titlelayout, 1, 1);
    if (isPreview()) {
        mainlayout->addWidget(
        new QLabel(i18n("<b><center>Minimalistic preview</center></b>"),
        widget()), 2, 1);
    } else {
        mainlayout->addItem(new QSpacerItem(0, 0), 2, 1);
    }

    // the window should stretch
    mainlayout->setRowStretch(2, 10);
    mainlayout->setColStretch(1, 10);

    // setup titlebar buttons
    for (int n=0; n<ButtonTypeCount; n++) button[n] = 0;
    addButtons(titlelayout, options()->titleButtonsLeft());
    titlelayout->addItem(titlebar_);
    addButtons(titlelayout, options()->titleButtonsRight());
}

//////////////////////////////////////////////////////////////////////////////
// addButtons()
// ------------
// Add buttons to title layout

void MinimalisticClient::addButtons(Q3BoxLayout *layout, const QString& s)
{
    const unsigned char *bitmap;
    QString tip;

    if (s.length() > 0) {
        for (unsigned n=0; n < s.length(); n++) {
            layout->addSpacing(5);
            switch (s[n]) {
              case 'M': // Menu button
                  if (!button[ButtonMenu]) {
                      button[ButtonMenu] =
                          new MinimalisticButton(this, "menu", i18n("Menu"),
                                            ButtonMenu, 0);
                      connect(button[ButtonMenu], SIGNAL(pressed()),
                              this, SLOT(menuButtonPressed()));
                      layout->addWidget(button[ButtonMenu]);
                  }
                  break;

              case 'S': // Sticky button
                  if (!button[ButtonSticky]) {
              if (isOnAllDesktops()) {
              bitmap = stickydown_bits;
              tip = i18n("Un-Sticky");
              } else {
              bitmap = sticky_bits;
              tip = i18n("Sticky");
              }
                      button[ButtonSticky] =
                          new MinimalisticButton(this, "sticky", tip,
                                            ButtonSticky, bitmap);
                      connect(button[ButtonSticky], SIGNAL(clicked()),
                              this, SLOT(toggleOnAllDesktops()));
                      layout->addWidget(button[ButtonSticky]);
                  }
                  break;

              case 'H': // Help button
                  if ((!button[ButtonHelp]) && providesContextHelp()) {
                      button[ButtonHelp] =
                          new MinimalisticButton(this, "help", i18n("Help"),
                                            ButtonHelp, help_bits);
                      connect(button[ButtonHelp], SIGNAL(clicked()),
                              this, SLOT(showContextHelp()));
                      layout->addWidget(button[ButtonHelp]);
                  }
                  break;

              case 'I': // Minimize button
                  if ((!button[ButtonMin]) && isMinimizable())  {
                      button[ButtonMin] =
                          new MinimalisticButton(this, "iconify", i18n("Minimize"),
                                            ButtonMin, min_bits);
                      connect(button[ButtonMin], SIGNAL(clicked()),
                              this, SLOT(minimize()));
                      layout->addWidget(button[ButtonMin]);
                  }
                  break;

              case 'A': // Maximize button
                  if ((!button[ButtonMax]) && isMaximizable()) {
              if (maximizeMode() == MaximizeFull) {
              bitmap = minmax_bits;
              tip = i18n("Restore");
              } else {
              bitmap = max_bits;
              tip = i18n("Maximize");
              }
                      button[ButtonMax]  =
                          new MinimalisticButton(this, "maximize", tip,
                                            ButtonMax, bitmap);
                      connect(button[ButtonMax], SIGNAL(clicked()),
                              this, SLOT(maxButtonPressed()));
                      layout->addWidget(button[ButtonMax]);
                  }
                  break;

              case 'X': // Close button
                  if ((!button[ButtonClose]) && isCloseable()) {
                      button[ButtonClose] =
                          new MinimalisticButton(this, "close", i18n("Close"),
                                            ButtonClose, close_bits);
                      connect(button[ButtonClose], SIGNAL(clicked()),
                              this, SLOT(closeWindow()));
                      layout->addWidget(button[ButtonClose]);
                  }
                  break;

              case '_': // Spacer item
                  layout->addSpacing(FRAMEBUTTONSPACE);
            }
        }
    }
    layout->addSpacing(10);
}

//////////////////////////////////////////////////////////////////////////////
// activeChange()
// --------------
// window active state has changed

void MinimalisticClient::activeChange()
{
    for (int n=0; n<ButtonTypeCount; n++)
        if (button[n]) button[n]->reset();
    widget()->repaint(false);
}

//////////////////////////////////////////////////////////////////////////////
// captionChange()
// ---------------
// The title has changed

void MinimalisticClient::captionChange()
{
    widget()->repaint(titlebar_->geometry(), false);
}

//////////////////////////////////////////////////////////////////////////////
// desktopChange()
// ---------------
// Called when desktop/sticky changes

void MinimalisticClient::desktopChange()
{
    bool d = isOnAllDesktops();
    if (button[ButtonSticky]) {
        button[ButtonSticky]->setBitmap(d ? stickydown_bits : sticky_bits);
    QToolTip::remove(button[ButtonSticky]);
    QToolTip::add(button[ButtonSticky], d ? i18n("Un-Sticky") : i18n("Sticky"));
    }
}

//////////////////////////////////////////////////////////////////////////////
// iconChange()
// ------------
// The title has changed

void MinimalisticClient::iconChange()
{
    if (button[ButtonMenu]) {
        button[ButtonMenu]->setBitmap(0);
        button[ButtonMenu]->repaint(false);
    }
}

//////////////////////////////////////////////////////////////////////////////
// maximizeChange()
// ----------------
// Maximized state has changed

void MinimalisticClient::maximizeChange()
{
    bool m = (maximizeMode() == MaximizeFull);
    if (button[ButtonMax]) {
        button[ButtonMax]->setBitmap(m ? minmax_bits : max_bits);
    QToolTip::remove(button[ButtonMax]);
    QToolTip::add(button[ButtonMax], m ? i18n("Restore") : i18n("Maximize"));
    }
}

//////////////////////////////////////////////////////////////////////////////
// shadeChange()
// -------------
// Called when window shading changes

void MinimalisticClient::shadeChange()
{ ; }

//////////////////////////////////////////////////////////////////////////////
// borders()
// ----------
// Get the size of the borders

void MinimalisticClient::borders(int &l, int &r, int &t, int &b) const
{
    l = LFRAMESIZE;
    r = RFRAMESIZE;
    t = TITLESIZE + TFRAMESIZE;
    b = BFRAMESIZE;
}

//////////////////////////////////////////////////////////////////////////////
// resize()
// --------
// Called to resize the window

void MinimalisticClient::resize(const QSize &size)
{
    widget()->resize(size);
}

//////////////////////////////////////////////////////////////////////////////
// minimumSize()
// -------------
// Return the minimum allowable size for this window

QSize MinimalisticClient::minimumSize() const
{
    return widget()->minimumSize();
}

//////////////////////////////////////////////////////////////////////////////
// mousePosition()
// ---------------
// Return logical mouse position

KDecoration::Position MinimalisticClient::mousePosition(const QPoint &point) const
{
    const int corner = 24;
    Position pos;

    if (point.y() <= TFRAMESIZE) {
        // inside top frame
        if (point.x() <= corner)                 pos = PositionTopLeft;
        else if (point.x() >= (width()-corner))  pos = PositionTopRight;
        else                                     pos = PositionTop;
    } else if (point.y() >= (height()-BFRAMESIZE)) {
        // inside handle
        if (point.x() <= corner)                 pos = PositionBottomLeft;
        else if (point.x() >= (width()-corner))  pos = PositionBottomRight;
        else                                     pos = PositionBottom;
    } else if (point.x() <= LFRAMESIZE) {
        // on left frame
        if (point.y() <= corner)                 pos = PositionTopLeft;
        else if (point.y() >= (height()-corner)) pos = PositionBottomLeft;
        else                                     pos = PositionLeft;
    } else if (point.x() >= width()-RFRAMESIZE) {
        // on right frame
        if (point.y() <= corner)                 pos = PositionTopRight;
        else if (point.y() >= (height()-corner)) pos = PositionBottomRight;
        else                                     pos = PositionRight;
    } else {
        // inside the frame
        pos = PositionCenter;
    }
    return pos;
}

//////////////////////////////////////////////////////////////////////////////
// eventFilter()
// -------------
// Event filter

bool MinimalisticClient::eventFilter(QObject *obj, QEvent *e)
{
    if (obj != widget()) return false;

    switch (e->type()) {
      case QEvent::MouseButtonDblClick: {
          mouseDoubleClickEvent(static_cast<QMouseEvent *>(e));
          return true;
      }
      case QEvent::MouseButtonPress: {
          processMousePressEvent(static_cast<QMouseEvent *>(e));
          return true;
      }
      case QEvent::Paint: {
          paintEvent(static_cast<QPaintEvent *>(e));
          return true;
      }
      case QEvent::Resize: {
          resizeEvent(static_cast<QResizeEvent *>(e));
          return true;
      }
      case QEvent::Show: {
          showEvent(static_cast<QShowEvent *>(e));
          return true;
      }
      default: {
          return false;
      }
    }

    return false;
}

//////////////////////////////////////////////////////////////////////////////
// mouseDoubleClickEvent()
// -----------------------
// Doubleclick on title

void MinimalisticClient::mouseDoubleClickEvent(QMouseEvent *e)
{
    if (titlebar_->geometry().contains(e->pos())) titlebarDblClickOperation();
}

//////////////////////////////////////////////////////////////////////////////
// paintEvent()
// ------------
// Repaint the window

void MinimalisticClient::paintEvent(QPaintEvent*)
{
    if (!MinimalisticFactory::initialized()) return;

    doShape();

    QColorGroup group;
    QPainter painter(widget());

    // draw the titlebar+buttton background
    group = options()->colorGroup(KDecoration::ColorTitleBar, isActive());
    painter.fillRect(QRect(0,TFRAMESIZE,width(), TITLESIZE+TFRAMESIZE), group.background());

    // draw title text
    QRect title(titlebar_->geometry());
    painter.setFont(options()->font(isActive(), false));
    painter.setPen(options()->color(KDecoration::ColorFont, isActive()));
    painter.drawText(title.x() + LFRAMESIZE, title.y(),
                     title.width() - RFRAMESIZE, title.height(),
                     MinimalisticFactory::titleAlign() | Qt::AlignVCenter,
                     caption());

    // draw frame
    group = options()->colorGroup(KDecoration::ColorFrame, isActive());

    QRect frame(0, 0, width(), TFRAMESIZE);
    painter.fillRect(frame, group.background());
    frame.setRect(0, 0, LFRAMESIZE, height());
    painter.fillRect(frame, group.background());
    frame.setRect(0, height() - BFRAMESIZE, width(), BFRAMESIZE);
    painter.fillRect(frame, group.background());
    frame.setRect(width()-RFRAMESIZE, 0, RFRAMESIZE, height());
    painter.fillRect(frame, group.background());

    
    // outline the frame
    frame = widget()->rect();
    int x,y,w,h;
    frame.rect(&x, &y, &w, &h);
    int x2=x+w-1;
    int y2=y+h-1;
    painter.setPen(group.background().light(140));
    painter.drawLine(x, y, x2, y);
    painter.drawLine(x, y, x, y2);
    painter.drawPoint(x+4, y+1);
    painter.drawPoint(x+3, y+1);
    painter.drawPoint(x+2, y+2);
    painter.drawPoint(x+1, y+3);
    painter.drawPoint(x+1, y+4);
    painter.setPen(group.background().dark(140));
    painter.drawLine(x, y2, x2, y2);
    painter.drawLine(x2, y, x2, y2);
    painter.drawPoint(x2-4, y2-1);
    painter.drawPoint(x2-3, y2-1);
    painter.drawPoint(x2-2, y2-2);
    painter.drawPoint(x2-1, y2-3);
    painter.drawPoint(x2-1, y2-4);

    int numDotGroups = 1;
    if (frame.width() >700)
        numDotGroups = 3;
    int step = frame.width() / (numDotGroups + 1);

    for(int j = 0, x = step; j < numDotGroups; j++, x += step)
    {
        renderDot(&painter, QPoint(x-5, 2), group.background(), true);
        renderDot(&painter, QPoint(x, 2), group.background(), true);
        renderDot(&painter, QPoint(x+5, 2), group.background(), true);

        renderDot(&painter, QPoint(x-5, frame.height()-5), group.background(), true);
        renderDot(&painter, QPoint(x, frame.height()-5), group.background(), true);
        renderDot(&painter, QPoint(x+5, frame.height()-5), group.background(), true);
    }

    numDotGroups = 1;
    if (frame.height() >700)
        numDotGroups = 3;
    step = frame.height() / (numDotGroups + 1);

    for(int j = 0, y = step; j < numDotGroups; j++, y += step)
    {
        renderDot(&painter, QPoint(3, y-5), group.background(), true);
        renderDot(&painter, QPoint(3, y), group.background(), true);
        renderDot(&painter, QPoint(3, y+5), group.background(), true);

        renderDot(&painter, QPoint(frame.width()-5, y-5), group.background(), true);
        renderDot(&painter, QPoint(frame.width()-5, y), group.background(), true);
        renderDot(&painter, QPoint(frame.width()-5, y+5), group.background(), true);
    }

    renderDot(&painter, QPoint(3, 8), group.background(), true);
    renderDot(&painter, QPoint(4, 4), group.background(), true);
    renderDot(&painter, QPoint(8, 3), group.background(), true);

    renderDot(&painter, QPoint(3, frame.height()-11), group.background(), true);
    renderDot(&painter, QPoint(4, frame.height()-7), group.background(), true);
    renderDot(&painter, QPoint(8, frame.height()-5), group.background(), true);

    renderDot(&painter, QPoint(frame.width()-5, 8), group.background(), true);
    renderDot(&painter, QPoint(frame.width()-7, 4), group.background(), true);
    renderDot(&painter, QPoint(frame.width()-11, 3), group.background(), true);

    renderDot(&painter, QPoint(frame.width()-5, frame.height()-11), group.background(), true);
    renderDot(&painter, QPoint(frame.width()-7, frame.height()-7), group.background(), true);
    renderDot(&painter, QPoint(frame.width()-11, frame.height()-5), group.background(), true);
}

void MinimalisticClient::doShape()
{
  int r=widget()->width();
  int b=widget()->height();
QRegion mask(0,0,r,b);
    // Remove top-left corner.
    mask -= QRegion(0, 0, 5, 1);
    mask -= QRegion(0, 1, 3, 1);
    mask -= QRegion(0, 2, 2, 1);
    mask -= QRegion(0, 3, 1, 2);

    // Remove top-right corner.
    mask -= QRegion(r - 5, 0, 5, 1);
    mask -= QRegion(r - 3, 1, 3, 1);
    mask -= QRegion(r - 2, 2, 2, 1);
    mask -= QRegion(r - 1, 3, 1, 2);

    // Remove bottom-left corner.
    mask -= QRegion(0, b-1-0, 5, b-1-1);
    mask -= QRegion(0, b-1-1, 3, b-1-1);
    mask -= QRegion(0, b-1-2, 2, b-1-1);
    mask -= QRegion(0, b-1-3, 1, b-1-2);

    // Remove bottom-right corner.
    mask -= QRegion(r - 5, b-1-0, 5, b-1-1);
    mask -= QRegion(r - 3, b-1-1, 3, b-1-1);
    mask -= QRegion(r - 2, b-1-2, 2, b-1-1);
    mask -= QRegion(r - 1, b-1-3, 1, b-1-2);

    setMask(mask);
}

//////////////////////////////////////////////////////////////////////////////
// resizeEvent()
// -------------
// Window is being resized

void MinimalisticClient::resizeEvent(QResizeEvent *)
{
    if (widget()->isShown()) {
        QRegion region = widget()->rect();
        region = region.subtract(titlebar_->geometry());
    widget()->erase(region);
    }
}

//////////////////////////////////////////////////////////////////////////////
// showEvent()
// -----------
// Window is being shown

void MinimalisticClient::showEvent(QShowEvent *)
{
   widget()->repaint();
}

//////////////////////////////////////////////////////////////////////////////
// maxButtonPressed()
// -----------------
// Max button was pressed

void MinimalisticClient::maxButtonPressed()
{
    if (button[ButtonMax]) {
        switch (button[ButtonMax]->lastMousePress()) {
          case Qt::MidButton:
              maximize(maximizeMode() ^ MaximizeVertical);
              break;
          case Qt::RightButton:
              maximize(maximizeMode() ^ MaximizeHorizontal);
              break;
          default:
              (maximizeMode() == MaximizeFull) ? maximize(MaximizeRestore)
                  : maximize(MaximizeFull);
        }
    }
}

//////////////////////////////////////////////////////////////////////////////
// menuButtonPressed()
// -------------------
// Menu button was pressed (popup the menu)

void MinimalisticClient::menuButtonPressed()
{
    if (button[ButtonMenu]) {
        QPoint p(button[ButtonMenu]->rect().bottomLeft().x(),
                 button[ButtonMenu]->rect().bottomLeft().y());
        KDecorationFactory* f = factory();
        showWindowMenu(button[ButtonMenu]->mapToGlobal(p));
        if (!f->exists(this)) return; // decoration was destroyed
        button[ButtonMenu]->setDown(false);
    }
}

//#include "oxygenclient.moc"
