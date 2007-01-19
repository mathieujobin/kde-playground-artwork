#ifndef INLINEHELP_CPP
#define INLINEHELP_CPP

#ifndef CLAMP
#define CLAMP(x,l,u) (x) < (l) ? (l) :\
(x) > (u) ? (u) :\
(x)
#endif

#ifndef QABS
#define QABS(x) (x) > 0 ? (x) : (-(x))
#endif

#ifndef _PRINTFLAGS_
#define _PRINTFLAGS_ option ? qWarning("State Flags:\n%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s",\
option->state & State_Active ? "Active, " : "",\
option->state & State_AutoRaise ? "AutoRaise, " : "",\
option->state & State_Bottom ? "Bottom, " : "",\
option->state & State_Children ? "Children, " : "",\
option->state & State_None ? "None, " : "",\
option->state & State_DownArrow ? "DownArrow, " : "",\
option->state & State_Editing ? "Editing, " : "",\
option->state & State_Enabled ? "Enabled, " : "",\
option->state & State_FocusAtBorder ? "FocusAtBorder, " : "",\
option->state & State_HasFocus ? "HasFocus, " : "",\
option->state & State_Horizontal ? "Horizontal, " : "",\
option->state & State_Item ? "Item, " : "",\
option->state & State_MouseOver ? "MouseOver, " : "",\
option->state & State_NoChange ? "NoChange, " : "",\
option->state & State_Off ? "Off, " : "",\
option->state & State_On ? "On, " : "",\
option->state & State_Open ? "Open, " : "",\
option->state & State_Raised ? "Raised, " : "",\
option->state & State_Selected ? "Selected, " : "",\
option->state & State_Sibling ? "Sibling, " : "",\
option->state & State_Sunken ? "Sunken, " : "",\
option->state & State_Top ? "Top, " : "",\
option->state & State_UpArrow ? "UpArrow, " : "",\
option->state & State_KeyboardFocusChange ? "KeyboardFocusChange, " : "",\
option->state & State_ReadOnly ? "ReadOnly, " : "") : qWarning("MISSING OPTIONS")
#endif

/**Internal calculates hsv v from color, faster than calling qcolor.hsv if you only want v*/
inline int colorValue(const QColor &c)
{
   int v = c.red();
   if (c.green() > v) v = c.green();
   if (c.blue() > v) v = c.blue();
   return v;
}

/**Internal, calcs weighted mid of two colors*/
inline QColor midColor(const QColor &oc1, const QColor &c2, int w1 = 1, int w2 = 1)
{
   int sum = (w1+w2);
   QColor c1 = oc1; //QColor c2 = oc2;
   int h,s, v = colorValue(c1);
   if (v < 70)
   {
      c1.getHsv(&h,&s,&v);
      c1.setHsv(h,s,70);
   }
//    v = colorValue(c2);
//    if (v < 70)
//    {
//       c2.getHsv(&h,&s,&v);
//       c2.setHsv(h,s,70);
//    }
   return QColor((w1*c1.red() + w2*c2.red())/sum,(w1*c1.green() + w2*c2.green())/sum,(w1*c1.blue() + w2*c2.blue())/sum);
}

/**Internal, calcs button color depending on state*/

inline QColor btnBgColor(const QPalette &pal, bool isEnabled, bool hasFocus, bool hover)
{
   if (!isEnabled)
      return pal.color(QPalette::Window);
   if (hover)
   {
      if (hasFocus)
         return pal.color(QPalette::Highlight);
      return pal.color(QPalette::ButtonText);
   }
   return pal.color(QPalette::Button);
}

/**Internal, calcs buttonText color depending on state*/
inline QColor btnFgColor(const QPalette &pal, bool isEnabled, bool hasFocus, bool hover)
{
   if (!isEnabled)
      return midColor(pal.color(QPalette::Window), pal.color(QPalette::WindowText),1,3);
   if (hover)
   {
      if (hasFocus)
         return pal.color(QPalette::HighlightedText);
      return pal.color(QPalette::Button);
   }
   return pal.color(QPalette::ButtonText);
}

/**Internal, calcs a contrasted color to a qcolor*/
inline QColor emphasize(const QColor &c, int value = 10)
{
   int h,s,v;
   QColor ret;
   c.getHsv(&h,&s,&v);
   if (v < 75+value)
   {
      ret.setHsv(h,s,CLAMP(85+value,85,255));
      return ret;
   }
   if (v > 200)
   {
      if (s > 30)
      {
         h -= 5; if (h < 0) h = 360 + h;
         s = (s<<3)/9;
         v += value;
         ret.setHsv(h,CLAMP(s,30,255),CLAMP(v,0,255));
         return ret;
      }
      if (v > 230)
      {
         ret.setHsv(h,s,CLAMP(v-value,0,255));
         return ret;
      }
   }
   if (v > 128)
      ret.setHsv(h,s,CLAMP(v+value,0,255));
   else
      ret.setHsv(h,s,CLAMP(v-value,0,255));
   return ret;
}

/**Internal, calcs a lightned version of a qcolor*/
inline QColor light(const QColor &c, int value)
{
   int h,s,v;
   c.getHsv(&h,&s,&v);
   QColor ret;
   if (v < 255-value)
   {
      ret.setHsv(h,s,CLAMP(v+value,0,255)); //value could be negative
      return ret;
   }
   // psychovisual uplightning, i.e. shift hue and lower saturation
   if (s > 30)
   {
      h -= (value*5/20); if (h < 0) h = 400 + h;
      s = CLAMP((s<<3)/9,30,255);
      ret.setHsv(h,s,255);
      return ret;
   }
   else // hue shifting has no sense, half saturation (btw, white won't get brighter :)
      ret.setHsv(h,s>>1,255);
   return ret;
}

/**Internal, generates a 1bit alpha mask for a pixmap - useless on qt4?*/
#if 0
static inline QBitmap alphaMask(const QPixmap &pix)
{
   QImage src = pix.toImage();
   QImage dst(pix.size(), QImage::Format_ARGB32);
   
   unsigned int *data, *data2;// = ( unsigned int * ) src.bits();
   unsigned int *destData = ( unsigned int* ) dst.bits();
//    int total = src.width() * src.height();
   int col, row;
   
   
   for (row = 0; row < src.height(); ++row)
   {
      data = ( unsigned int * )src.scanLine(row);
      if (row == 0 || row == src.height()-1)
         data2 = 0;
      else if (row < (src.height()>>1))
         data2 = ( unsigned int * )src.scanLine(row-1);
      else
         data2 = ( unsigned int * )src.scanLine(row+1);
      for ( col = 0; col < src.width(); ++col )
      {
         if (qAlpha(data[ col ]) ||
             (data2 && qAlpha(data2[col])) xor
             ((col > 0 && col < (src.width()>>1) && qAlpha(data[ col-1 ])) ||
              (col > (src.width()>>1) && col < src.width()-1 && qAlpha(data[ col+1 ]))))
            destData[ row*src.width() + col ] = qRgba( 0, 0, 0, 255 );
         else
            destData[ row*src.width() + col ] = qRgba( 255, 255, 255, 255 );
      }
   }
   QBitmap ret;
   ret = dst;
   return ret;
}
#endif

#if 0
/**Internal, checks if there's contrast between two colors*/
static bool thereIsContrastBetween(const QColor &a, const QColor &b)
{
   int ar,ag,ab,br,bg,bb;
   a.getRgb(&ar,&ag,&ab);
   b.getRgb(&br,&bg,&bb);
   
   int diff = (299*(ar-br) + 587*(ag-bg) + 114*(ab-bb));
   
   if (QABS(diff) < 91001)
      return false;
   
   diff = qMax(ar,br) + qMax(ag,bg) + qMax(ab,bb)
      - (qMin(ar,br) + qMin(ag,bg) + qMin(ab,bb));
   
   return (diff > 300);
}
#endif

#endif //INLINEHELP_CPP
