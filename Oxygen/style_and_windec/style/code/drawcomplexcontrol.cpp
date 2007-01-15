#include <QComboBox>
#include <QPainter>
// #include <QPixmapCache>
#include <QStyleOptionComplex>
#include <cmath>
#include "oxygen.h"

using namespace Oxygen;

extern Config config;

#include "inlinehelp.cpp"

static int calcBigLineSize(int radius)
{
   int bigLineSize = radius / 6;
   if (bigLineSize < 4)
      bigLineSize = 4;
   if (bigLineSize > radius / 2)
      bigLineSize = radius / 2;
   return bigLineSize;
}

/**gradients or glosses*/
inline const QPixmap OxygenStyle::colorRun(const QColor &c, const int size, Qt::Orientation o, Orientation3D o3D, bool smooth) const
{
   if (smooth)
      return gradient(c, size, o, o3D);
   return gloss(c, size, o, o3D);
}

static QPolygonF calcLines(const QStyleOptionSlider *dial, const QWidget *)
{
   QPolygonF poly;
   int width = dial->rect.width();
   int height = dial->rect.height();
   qreal r = qMin(width, height) / 2.0;
   int bigLineSize = calcBigLineSize(int(r));
   
   qreal xc = width / 2.0;
   qreal yc = height / 2.0;
   int ns = dial->tickInterval;
   int notches = (dial->maximum + ns - 1 - dial->minimum) / ns;
   if (notches <= 0)
      return poly;
   poly.resize(2 + 2 * notches);
   int smallLineSize = bigLineSize / 2;
   for (int i = 0; i <= notches; ++i)
   {
      qreal angle = dial->dialWrapping ? M_PI * 3 / 2 - i * 2 * M_PI / notches
         : (M_PI * 8 - i * 10 * M_PI / notches) / 6;
      qreal s = sin(angle);
      qreal c = cos(angle);
   if (i == 0 || (((ns * i) % (dial->pageStep ? dial->pageStep : 1)) == 0))
   {
      poly[2 * i] = QPointF(xc + (r - bigLineSize) * c,
                            yc - (r - bigLineSize) * s);
      poly[2 * i + 1] = QPointF(xc + r * c, yc - r * s);
   }
      else
      {
      poly[2 * i] = QPointF(xc + (r - 1 - smallLineSize) * c,
                            yc - (r - 1 - smallLineSize) * s);
      poly[2 * i + 1] = QPointF(xc + (r - 1) * c, yc -(r - 1) * s);
      }
   }
   return poly;
}

static QPolygonF calcArrow(const QStyleOptionSlider *dial, qreal &a)
{
   int width = dial->rect.width();
   int height = dial->rect.height();
   int r = qMin(width, height) / 2;
   if (dial->maximum == dial->minimum)
      a = M_PI / 2;
   else if (dial->dialWrapping)
      a = M_PI * 3 / 2 - (dial->sliderValue - dial->minimum) * 2 * M_PI
      / (dial->maximum - dial->minimum);
   else
      a = (M_PI * 8 - (dial->sliderValue - dial->minimum) * 10 * M_PI
           / (dial->maximum - dial->minimum)) / 6;
   
   int xc = width / 2;
   int yc = height / 2;
   
   int len = r - calcBigLineSize(r) - 5;
   if (len < 5)
      len = 5;
   int back = len / 2;
   
   QPolygonF arrow(3);
   arrow[0] = QPointF(0.5 + xc + len * cos(a),
                      0.5 + yc - len * sin(a));
   arrow[1] = QPointF(0.5 + xc + back * cos(a + M_PI * 5 / 6),
                      0.5 + yc - back * sin(a + M_PI * 5 / 6));
   arrow[2] = QPointF(0.5 + xc + back * cos(a - M_PI * 5 / 6),
                      0.5 + yc - back * sin(a - M_PI * 5 / 6));
   return arrow;
}

void OxygenStyle::drawComplexControl ( ComplexControl control, const QStyleOptionComplex * option, QPainter * painter, const QWidget * widget) const
{
#define PAL option->palette
#define RECT option->rect
   
   Q_ASSERT(option);
   Q_ASSERT(painter);
   
   bool sunken = option->state & State_Sunken;
   bool isEnabled = option->state & State_Enabled;
   bool hover = isEnabled && (option->state & State_MouseOver);
   bool hasFocus = option->state & State_HasFocus;
   
   switch ( control )
   {
   case CC_SpinBox: // A spinbox, like QSpinBox
      if (const QStyleOptionSpinBox *sb =
          qstyleoption_cast<const QStyleOptionSpinBox *>(option))
      {
         QStyleOptionSpinBox copy = *sb;
         QRect rect;
         int uh = 0;
         
         if (sb->subControls & SC_SpinBoxUp)
         {
            copy.subControls = SC_SpinBoxUp;
            rect = subControlRect(CC_SpinBox, sb, SC_SpinBoxUp, widget);
            uh = rect.height();
            if (option->direction == Qt::RightToLeft)
               copy.rect.setLeft(RECT.left()+2*rect.width()/3);
            else
               copy.rect.setRight(rect.left()+rect.width()/3);
         }
         
         if (sb->frame && (sb->subControls & SC_SpinBoxFrame))
            drawPrimitive ( PE_PanelLineEdit, &copy, painter, widget );
         
         if (sb->subControls & SC_SpinBoxUp)
         {
            copy.rect = rect;
            
            renderFrame( painter, copy.rect, Raised, Top | Left | Right);
            
            copy.rect.adjust(2,2,-2,0);
            painter->drawTiledPixmap(copy.rect, gradient(PAL.color(QPalette::Window), copy.rect.height()*2, Qt::Vertical, (sb->activeSubControls == SC_SpinBoxUp) && sunken ? Raised : Sunken)); // sic!
            
            copy.rect.adjust(copy.rect.width()/4,copy.rect.height()/4,-copy.rect.width()/4,-copy.rect.height()/4);
            if (!(sb->stepEnabled & QAbstractSpinBox::StepUpEnabled))
               painter->setPen(PAL.color(QPalette::Disabled, QPalette::Text));
            else if (sb->activeSubControls == SC_SpinBoxUp)
               painter->setPen(PAL.color(QPalette::WindowText));
            else
               painter->setPen(midColor(PAL.color(QPalette::Window),PAL.color(QPalette::WindowText),1,3));
            drawPrimitive(PE_IndicatorSpinUp, &copy, painter, widget);
         }
         
         if (sb->subControls & SC_SpinBoxDown)
         {
            copy.subControls = SC_SpinBoxDown;
            copy.rect = subControlRect(CC_SpinBox, sb, SC_SpinBoxDown, widget);
            
            renderFrame( painter, copy.rect, Raised, Bottom | Left | Right);
            
            copy.rect.adjust(2,0,-2,-2);
            painter->drawTiledPixmap(copy.rect, gradient(PAL.color(QPalette::Window), copy.rect.height()*2, Qt::Vertical, (sb->activeSubControls == SC_SpinBoxDown) && sunken ? Raised : Sunken), QPoint(0,-uh)); // sic!
            
            copy.rect.adjust(copy.rect.width()/4,copy.rect.height()/4,-copy.rect.width()/4,-copy.rect.height()/4);
            if (!(sb->stepEnabled & QAbstractSpinBox::StepDownEnabled))
               painter->setPen(PAL.color(QPalette::Disabled, QPalette::Text));
            else if (sb->activeSubControls == SC_SpinBoxDown)
               painter->setPen(PAL.color(QPalette::WindowText));
            else
               painter->setPen(midColor(PAL.color(QPalette::Window),PAL.color(QPalette::WindowText),1,3));
            drawPrimitive(PE_IndicatorSpinDown, &copy, painter, widget);
         }
      }
      break;
   case CC_GroupBox:
      if (const QStyleOptionGroupBox *groupBox =
          qstyleoption_cast<const QStyleOptionGroupBox *>(option))
      {
         // Draw frame
         QRect textRect = subControlRect(CC_GroupBox, option, SC_GroupBoxLabel, widget);
         QRect checkBoxRect = subControlRect(CC_GroupBox, option, SC_GroupBoxCheckBox, widget);
         if (groupBox->subControls & QStyle::SC_GroupBoxFrame)
         {
            QStyleOptionFrameV2 frame;
            frame.QStyleOption::operator=(*groupBox);
            frame.features = groupBox->features;
            frame.lineWidth = groupBox->lineWidth;
            frame.midLineWidth = groupBox->midLineWidth;
            frame.rect = subControlRect(CC_GroupBox, option, SC_GroupBoxFrame, widget);
            drawPrimitive(PE_FrameGroupBox, &frame, painter, widget);
         }
         
         // Draw title
         if ((groupBox->subControls & QStyle::SC_GroupBoxLabel) && !groupBox->text.isEmpty())
         {
            if (!groupBox->text.isEmpty())
            {
               QColor textColor = groupBox->textColor;
               if (textColor.isValid())
                  painter->setPen(textColor);
               QFont tmpfnt = painter->font(); tmpfnt.setBold(true);
               painter->setFont ( tmpfnt );
               int alignment = int(groupBox->textAlignment);
               if (!styleHint(QStyle::SH_UnderlineShortcut, option, widget))
                  alignment |= Qt::TextHideMnemonic;
               
               drawItemText(painter, textRect,  Qt::TextShowMnemonic | Qt::AlignHCenter | alignment,
                            groupBox->palette, isEnabled, groupBox->text,
                            textColor.isValid() ? QPalette::NoRole : QPalette::Foreground);

            }
         }
         
            // Draw checkbox
         if (groupBox->subControls & SC_GroupBoxCheckBox)
         {
            QStyleOptionButton box;
            box.QStyleOption::operator=(*groupBox);
            box.rect = checkBoxRect;
            drawPrimitive(PE_IndicatorCheckBox, &box, painter, widget);
         }
      }
      break;
   case CC_ComboBox: // A combobox, like QComboBox
      if (const QStyleOptionComboBox *cmb =
          qstyleoption_cast<const QStyleOptionComboBox *>(option))
      {
         if ((cmb->subControls & SC_ComboBoxFrame) && cmb->frame)
         {
            if (cmb->editable)
            {
               drawPrimitive(PE_PanelLineEdit, option, painter, widget);
            }
            else
            {
               Orientation3D o3D = (cmb->state & State_On) ? Raised : Relief;
               if (hover || cmb->state & State_On)
               {
                  QRegion ir = round_frames[o3D].innerRegion(RECT);
                  painter->save();
                  painter->setClipRegion(ir);
                  painter->drawTiledPixmap(ir.boundingRect(), gradient(PAL.color(QPalette::Window), RECT.height(), Qt::Vertical, Sunken));
                  painter->restore();
               }
               renderFrame( painter, RECT, o3D, Full, widget, false, true);
            }
         }
         
         if ((cmb->subControls & SC_ComboBoxArrow) &&
             (!(widget && qobject_cast<const QComboBox*>(widget)) || ((const QComboBox*)widget)->count() > 0))
         {
            painter->save();
            QStyleOptionComboBox tmpOpt = *cmb;
            hover = hover && (cmb->activeSubControls == SC_ComboBoxArrow);
            QRect ar = subControlRect(CC_ComboBox, cmb, SC_ComboBoxArrow, widget);
            if (!cmb->editable)
            {
               if (hasFocus)
                  renderFrame( painter, ar, Sunken, Full, widget, true);
               ar.adjust(2,2,-2,-2);
               renderFrame( painter, ar, Raised, Full, widget, false);
               ar.adjust(2,2,-2,-2);
               painter->drawTiledPixmap(ar, gloss(btnBgColor(PAL, isEnabled, hasFocus, hover), ar.height(), Qt::Vertical, sunken?Sunken:Raised));
               painter->setPen(btnFgColor(PAL, isEnabled, hasFocus, hover));
               tmpOpt.rect =  ar.adjusted(ar.width()/3,ar.height()/3,-ar.width()/3,-ar.height()/3);
            }
            else
            {
               if (hover && !sunken)
                  painter->setPen( PAL.color(QPalette::Text));
               else
                  painter->setPen( midColor(PAL.color(QPalette::Text), PAL.color(QPalette::Base)) );
               tmpOpt.rect =  ar.adjusted(ar.width()/3,ar.height()/3,-ar.width()/3,-ar.height()/3);
            }
            painter->setRenderHint ( QPainter::Antialiasing, true );
            drawPrimitive(PE_IndicatorArrowDown, &tmpOpt, painter, widget);
            painter->restore();
         }
      }
      break;
//    case CC_ScrollBar: // A scroll bar, like QScrollBar
   case CC_Slider: // A slider, like QSlider
      if (const QStyleOptionSlider *slider =
          qstyleoption_cast<const QStyleOptionSlider *>(option))
      {
         QRect groove = QCommonStyle::subControlRect(CC_Slider, slider, SC_SliderGroove, widget);
         QRect handle = QCommonStyle::subControlRect(CC_Slider, slider, SC_SliderHandle, widget);
         hover = hover && (slider->activeSubControls & SC_SliderHandle);
         sunken = sunken && (slider->activeSubControls & SC_SliderHandle);
         
         if ((slider->subControls & SC_SliderGroove) && groove.isValid())
         {
            QPalette::ColorRole lc = isEnabled ? QPalette::Highlight : QPalette::Window, rc = QPalette::Window;

            if ( slider->orientation == Qt::Horizontal )
            {
               // this is a workaround, qslider doesn't send the direction
               if (widget && widget->layoutDirection() == Qt::RightToLeft)
               // TODO: activate this one, remove the above
//                if (option->direction == Qt::RightToLeft)
               {
                  lc = QPalette::Window; rc = isEnabled ? QPalette::Highlight : QPalette::Window;
               }
               painter->setPen(PAL.color(lc).dark(120));
               int y = groove.y()+(groove.height()>>1);
               int left = groove.left()+3;
               int right = groove.right()-3;
               int swapPt = handle.left()+3;
               painter->drawLine(left,y,swapPt,y);
               painter->setPen(PAL.color(rc).dark(120));
               painter->drawLine(swapPt,y,right,y);
               y++;
               painter->setPen(PAL.color(lc).light(130));
               painter->drawLine(left,y,swapPt,y);
               painter->setPen(PAL.color(rc).light(130));
               painter->drawLine(swapPt,y,right,y);
            }
            else
            {
               painter->setPen(PAL.color(lc).dark(120));
               int x = groove.x()+(groove.width()>>1)-1;
               int top = groove.top()+3;
               int btm = groove.bottom()-3;
               int swapPt = handle.bottom()-3;
               painter->drawLine(x,swapPt,x,btm);
               painter->setPen(PAL.color(rc).dark(120));
               painter->drawLine(x,top,x,swapPt);
               x++;
               painter->setPen(PAL.color(lc).light(130));
               painter->drawLine(x,swapPt,x,btm);
               painter->setPen(PAL.color(rc).light(130));
               painter->drawLine(x,top,x,swapPt);
            }
         }
         
         if (slider->subControls & SC_SliderTickmarks)
         {
            QStyleOptionSlider tmpSlider = *slider;
            tmpSlider.subControls = SC_SliderTickmarks;
            QCommonStyle::drawComplexControl(control, &tmpSlider, painter, widget);
         }
         
         if (slider->subControls & SC_SliderHandle)
         {
            QPalette::ColorRole c1 = QPalette::Window, c2 = QPalette::WindowText;
            QRect innerRect;
            QRegion innerRegion;
            painter->save();
            if (isEnabled)
            {
               c1 = QPalette::Button, c2 = QPalette::ButtonText;
//                if (hasFocus)
//                {
//                   c1 = QPalette::Highlight, c2 = QPalette::HighlightedText;
//                }
//                else
//                {
//                   c1 = QPalette::Button, c2 = QPalette::ButtonText;
//                }
               innerRegion = round_frames[Raised].innerRegion(handle);
               innerRect = innerRegion.boundingRect();
               painter->setClipRegion(innerRegion);
               painter->drawTiledPixmap(innerRect, gloss(PAL.color(c1), innerRect.height(),
                  handle.height() > handle.width() ? Qt::Horizontal : Qt::Vertical,
                  hover && !sunken ? Raised : Sunken));
               if (hasFocus)
               {
                  painter->setBrush(PAL.color(QPalette::Highlight));
                  painter->setPen(Qt::NoPen);
                  painter->setRenderHint ( QPainter::Antialiasing, true );
                  int w = innerRect.width()/3; int h = innerRect.height()/3;
                  painter->drawEllipse ( innerRect.adjusted(w,h,-w,-h) );
               }
               painter->setClipping(false);
               renderFrame(painter, handle, Raised, Full, widget, false, true);
//                if (slider->orientation == Qt::Horizontal)
//                {
//                   int x = innerRect.center().x()-1;
//                   painter->setPen(midColor(PAL.color(c1),PAL.color(c2),3,1));
//                   painter->drawLine(x,innerRect.y()+2,x,innerRect.bottom()-2);
//                   x +=2;
//                   painter->drawLine(x,innerRect.y()+2,x,innerRect.bottom()-2);
//                   x -= 1;
//                   painter->setPen(midColor(PAL.color(c1),PAL.color(c2)));
//                   painter->drawLine(x,innerRect.y()+2,x,innerRect.bottom()-2);
//                }
//                else
//                {
//                   int y = innerRect.center().y()-1;
//                   painter->setPen(midColor(PAL.color(c1),PAL.color(c2),3,1));
//                   painter->drawLine(innerRect.x()+2,y,innerRect.right()-2,y);
//                   y +=2;
//                   painter->drawLine(innerRect.x()+2,y,innerRect.right()-2,y);
//                   y -= 1;
//                   painter->setPen(midColor(PAL.color(c1),PAL.color(c2)));
//                   painter->drawLine(innerRect.x()+2,y,innerRect.right()-2,y);
//                }
            }
            else
            {
               handle = handle.adjusted(1,2,-1,0);
               innerRegion = round_frames[Relief].innerRegion(handle);
               innerRect = innerRegion.boundingRect();
               painter->setClipRegion(innerRegion);
               painter->drawTiledPixmap(innerRect, gradient(PAL.color(QPalette::Background),innerRect.height(),
                  handle.height() > handle.width() ? Qt::Horizontal : Qt::Vertical, Sunken));
               painter->setClipping(false);
               renderFrame(painter, handle, Relief, Full, widget, false, true );
            }
            painter->restore();
         }
      }
      break;
   case CC_ToolButton: // A tool button, like QToolButton
      if (widget && widget->parent() && qobject_cast<QTabBar*>(widget->parent()))
      {
      // this is a qtabbar scrollbutton and needs to be erased...
         if (config.bgMode > Scanlines)
            painter->drawPixmap(RECT, widget->palette().brush(QPalette::Window).texture(), RECT.translated(widget->mapTo ( widget->topLevelWidget(), QPoint(0,0))));
         else
            painter->fillRect(RECT, PAL.brush(QPalette::Window));
//          painter->drawTiledPixmap(RECT.adjusted(0,2,0,-2), gradient(PAL.color(QPalette::Window), RECT.height()-4, Qt::Vertical, Raised));
      }
      if (const QStyleOptionToolButton *toolbutton
          = qstyleoption_cast<const QStyleOptionToolButton *>(option))
      {
         QRect button, menuarea;
         button = subControlRect(control, toolbutton, SC_ToolButton, widget);
         menuarea = subControlRect(control, toolbutton, SC_ToolButtonMenu, widget);
         
         State bflags = toolbutton->state;
             
         if (bflags & State_AutoRaise)
         {
            if (!hover)
            {
               bflags &= ~State_Raised;
            }
         }
         
         State mflags = bflags;
             
         if (toolbutton->activeSubControls & SC_ToolButton)
            bflags |= State_Sunken;
         if (toolbutton->activeSubControls & SC_ToolButtonMenu)
            mflags |= State_Sunken;
             
         QStyleOption tool(0);
         tool.palette = toolbutton->palette;
         if (toolbutton->subControls & SC_ToolButton)
         {
            if (isEnabled && (bflags & (State_Sunken | State_On | State_Raised | State_HasFocus)))
            {
               tool.rect = button;
               tool.state = bflags;
               drawPrimitive(PE_PanelButtonTool, &tool, painter, widget);
            }
         }
         
         if (toolbutton->subControls & SC_ToolButtonMenu)
         {
            tool.rect = menuarea;
            tool.state = mflags;
            if (mflags & (State_Sunken | State_On | State_Raised))
               drawPrimitive(PE_IndicatorButtonDropDown, &tool, painter, widget);
            drawPrimitive(PE_IndicatorArrowDown, &tool, painter, widget);
         }

         QStyleOptionToolButton label = *toolbutton;
         int fw = pixelMetric(PM_DefaultFrameWidth, option, widget);
         label.rect = button.adjusted(fw, fw, -fw, -fw);
//          label.rect = button.adjusted(4, 4, -4, -4);
         drawControl(CE_ToolButtonLabel, &label, painter, widget);
      }
      break;
   case CC_TitleBar: // A Title bar, like what is used in Q3Workspace
      if (const QStyleOptionTitleBar *tb =
          qstyleoption_cast<const QStyleOptionTitleBar *>(option))
      {
         painter->fillRect(RECT, PAL.brush(QPalette::Window));
         QRect ir;
         if (option->subControls & SC_TitleBarLabel)
         {
            ir = subControlRect(CC_TitleBar, tb, SC_TitleBarLabel, widget);
            painter->setPen(PAL.color(QPalette::WindowText));
            painter->drawText(ir.x() + 2, ir.y(), ir.width() - 2, ir.height(), Qt::AlignCenter | Qt::TextSingleLine, tb->text);
         }
         
         
#define PAINT_WINDOW_BUTTON(_scbtn_, _spbtn_)\
         {\
            tmpOpt.rect = subControlRect(CC_TitleBar, tb, _scbtn_, widget);\
            if (tb->activeSubControls & _scbtn_)\
               tmpOpt.state = tb->state;\
            else\
               tmpOpt.state &= ~(State_Sunken | State_MouseOver);\
            pm = standardPixmap ( _spbtn_, &tmpOpt, widget );\
            painter->drawPixmap(tmpOpt.rect.topLeft(), pm);\
         }
         
         QPixmap pm;
         QStyleOptionTitleBar tmpOpt = *tb;
         if (tb->subControls & SC_TitleBarCloseButton)
            PAINT_WINDOW_BUTTON(SC_TitleBarCloseButton, SP_TitleBarCloseButton)
         
         if (tb->subControls & SC_TitleBarMaxButton
             && tb->titleBarFlags & Qt::WindowMaximizeButtonHint)
         {
            if (tb->titleBarState & Qt::WindowMaximized)
               PAINT_WINDOW_BUTTON(SC_TitleBarNormalButton, SP_TitleBarNormalButton)
            else
               PAINT_WINDOW_BUTTON(SC_TitleBarMaxButton, SP_TitleBarMaxButton)
         }
         
         if (tb->subControls & SC_TitleBarMinButton
             && tb->titleBarFlags & Qt::WindowMinimizeButtonHint)
         {
            if (tb->titleBarState & Qt::WindowMinimized)
               PAINT_WINDOW_BUTTON(SC_TitleBarNormalButton, SP_TitleBarNormalButton)
            else
               PAINT_WINDOW_BUTTON(SC_TitleBarMinButton, SP_TitleBarMinButton)
         }
         
         if (tb->subControls & SC_TitleBarNormalButton &&
             tb->titleBarFlags & Qt::WindowMinMaxButtonsHint)
            PAINT_WINDOW_BUTTON(SC_TitleBarNormalButton, SP_TitleBarNormalButton)
         
         if (tb->subControls & SC_TitleBarShadeButton)
            PAINT_WINDOW_BUTTON(SC_TitleBarShadeButton, SP_TitleBarShadeButton)
         
         if (tb->subControls & SC_TitleBarUnshadeButton)
            PAINT_WINDOW_BUTTON(SC_TitleBarUnshadeButton, SP_TitleBarUnshadeButton)
         
         if (tb->subControls & SC_TitleBarContextHelpButton
             && tb->titleBarFlags & Qt::WindowContextHelpButtonHint)
            PAINT_WINDOW_BUTTON(SC_TitleBarContextHelpButton, SP_TitleBarContextHelpButton)
         
         if (tb->subControls & SC_TitleBarSysMenu
             && tb->titleBarFlags & Qt::WindowSystemMenuHint)
         {
            if (!tb->icon.isNull())
            {
               ir = subControlRect(CC_TitleBar, tb, SC_TitleBarSysMenu, widget);
               tb->icon.paint(painter, ir);
            }
            else
               PAINT_WINDOW_BUTTON(SC_TitleBarSysMenu, SP_TitleBarMenuButton)
         }
      }
      break;
//    case CC_Q3ListView: // Used for drawing the Q3ListView class
   case CC_Dial: // A dial, like QDial
      if (const QStyleOptionSlider *dial =
          qstyleoption_cast<const QStyleOptionSlider *>(option))
      {
         painter->save();

         if (isEnabled)
         {
            if (RECT.height() < 300)
            {
               painter->setClipRegion(QRegion(RECT, QRegion::Ellipse), Qt::IntersectClip);
               painter->drawTiledPixmap(RECT, gloss(PAL.background().color(), RECT.height(), Qt::Vertical, Raised ));
               painter->setClipping(false);
            }
            else
            {
               /* for big dials the thing above became horriby slow
                  because of the clipping (might be nvidia XServer issue?)
                  - strange enough only for the vertical size...
                  So instead we paint two chords
                  (don't try to use the gloss or a QGradientBrush as brush and paint a full ellipse,
                  i guess Qt uses clipping for this internal - it's however not
                  about the pixmap itself, unclipped painting of the pixmap is fast as it can be
                  - also it's not about the clipregion generation (tried to cache that)) */
               painter->setPen(Qt::NoPen);
               painter->setBrush(PAL.color(QPalette::Window).light(105));
               painter->drawPie(RECT, 0, 180*16);
               painter->setBrush(PAL.color(QPalette::Window).dark(110));
               painter->drawPie(RECT, -180*16, 180*16);
               painter->setBrush(Qt::NoBrush);
            }
         }
         painter->setRenderHint(QPainter::Antialiasing);
         painter->setPen(/*hasFocus ? PAL.color(QPalette::Highlight) :*/
                         midColor(PAL.color(QPalette::Window),PAL.color(QPalette::WindowText)));
         painter->drawEllipse(RECT);

         // draw notches
         if (dial->subControls & QStyle::SC_DialTickmarks)
            painter->drawLines(calcLines(dial, widget)); // ### calcLines could be cached...
         
         qreal a;
         QPolygonF arrow(calcArrow(dial, a));
         painter->setPen(Qt::NoPen);
         QColor c;
         QPalette::ColorRole cr = hasFocus ? QPalette::Highlight : QPalette::WindowText;
         if (RECT.height() < 300)
         {
            c = PAL.color(cr);
            if (hover)
               c.setAlpha(128);
            else
               c.setAlpha(64);
            painter->setBrush(c);
            painter->drawPolygon(arrow);
         }
         else
         {
            // true alpha channel can be slow for large objects, we're gonna trick for the moment (as we do on the bg anyway)
            c = midColor(PAL.color(QPalette::Window), PAL.color(cr), 1, hover?2:1);
            painter->setClipRect(RECT.x(),RECT.y(),RECT.width(),RECT.height()/2);
            painter->setBrush(c.light(110));
            painter->drawPolygon(arrow);
            painter->setClipRect(RECT.x(),RECT.y()+RECT.height()/2,RECT.width(),RECT.height()/2);
            painter->setBrush(c.dark(110));
            painter->drawPolygon(arrow);
            painter->setClipping(false);
            painter->setBrush(c);
         }
         int r = qMin(RECT.width(),RECT.height())/30;
         painter->drawEllipse(RECT.center().x()+1-r, RECT.center().y()+1-r, 2*r, 2*r);

         painter->restore();
      }
      break;
   default:
      QCommonStyle::drawComplexControl( control, option, painter, widget );
   } // switch
#undef RECT
#undef PAL
}
