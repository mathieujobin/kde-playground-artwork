/***************************************************************************
 *   Copyright (C) 2006-2007 by Thomas L�bking                             *
 *   thomas.luebking@web.de                                                *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

const QPixmap &OxygenStyle::gradient(const QColor &c, int size, Qt::Orientation o, GradientType type) const
{
   if (size <= 0)
   {
      qWarning("NULL Pixmap requested, size was %d",size);
      return nullPix;
   }
   
   QColor iC = c;
   
   PixmapCache *cache = &(const_cast<OxygenStyle*>( this )->gradients[o == Qt::Horizontal][type]);
   int v = colorValue(c);
   if (v < 80) // very dark colors won't make nice buttons ;)
   {
      int h,s;
      c.getHsv(&h,&s,&v);
      iC.setHsv(h,s,80);
   }
   
   /* ==================================================
   Ok, we want to cache the gradients, but unfortunately we have no idea about
   what kind of gradients will be demanded in the future
   Thus creating a 2 component map (uint for color and uint for size)
   would be some overhead and cause a lot of unused space in the dictionaries
   So we store all the gradients by a uint index
   Therefore we substitute the alpha component (byte << 24) of the demanded color with the demanded size
   As this would limit the size to 255/256 pixels we'll be a bit sloppy,
   depending on the users resolution (e.g. use 0 to store a gradient with 2px,
   usefull for demand of 1px or 2px) and shift the index
   (e.g. gradients from 0 to 6 px size will hardly be needed - maybe add statistics on this)
   So the handled size is actually demandedSize + (demandedSize % sizeSloppyness),
   beeing at least demanded size and the next sloppy size above at max
   ====================================================== */
   
   int sizeSloppyness = 1; uint magicNumber = 0;
   QPixmap *pix = 0;
   if (size < 105884) {
   // this is where our dictionary reaches - should be enough for the moment ;)
      int frameBase = 0, frameSize = 20;

      while ((frameBase += frameSize) < size) {
         sizeSloppyness++;
         frameSize += 20;
      }
      
      frameBase -=frameSize; frameSize -= 20;

      // The mapping is done by the "magicNumber"
      // basically we use the first 11 bits to store the size and the remaining 21 bits for the color (7bpc)
      magicNumber =  (((frameSize + (size - frameBase)/sizeSloppyness) & 0xff) << 21) |
         (((iC.red() >> 1) & 0x7f) << 14) |
         (((iC.green() >> 1) & 0x7f) << 7 ) |
         ((iC.blue() >> 1) & 0x7f);
      
      pix = cache->object(magicNumber);
      if (pix)
         return *pix;
   }
   else {
      qWarning("gradient with more than 105883 steps requested, returning NULL pixmap");
      return nullPix;
   }
   
   // no cache entry found, so let's create one =)
   int add = size % sizeSloppyness; if (!add) add = sizeSloppyness;
   size += add;
   
   QPoint start, stop;
   if (type == GradRadialGloss)
      pix = new QPixmap(size, size);
   else {
      if (o == Qt::Horizontal) {
         pix = new QPixmap(size, 32);
         start = QPoint(0,32); stop = QPoint(pix->width(),32);
      }
      else {
         pix = new QPixmap(32, size);
         start = QPoint(32, 0); stop = QPoint(32, pix->height());
      }
   }
   
   if (type == GradGloss || type == GradRadialGloss || type == GradGlass) {
   // many XRender implementations cannot do this... FUCK!
      // calculate the determining colors
      QColor d,dd,b,bb;
      int h,s, ch,cs,cv, delta, add;
      b = iC; d = b;
      
      iC.getHsv(&h,&s,&v);
      
      add = ((180-qGray(b.rgb()))>>1);
      if (add < 0) add = -add/2;
      if (type == GradGlass)
         add = add>>4;

      cv = v+27+add;
      if (cv > 255) {
         delta = cv-255; cv = 255;
         cs = s - delta; if (cs < 0) cs = 0;
         ch = h - delta/6; if (ch < 0) ch = 360+ch;
      }
      else {
         ch = h; cs = s;
      }
      bb.setHsv(ch,cs,cv);
      
      cv = v - 14-add; if (cv < 0) cv = 0;
      cs = s*13/7; if (cs > 255) cs = 255;
      dd.setHsv(h,cs,cv);
      
      if (type == GradRadialGloss) {
         QRadialGradient rg(2*pix->width()/3, pix->height(), pix->height());
         rg.setColorAt(0,d); rg.setColorAt(0.8,dd);
         rg.setColorAt(0.8, b); rg.setColorAt(1, bb);
         QPainter p(pix); p.fillRect(pix->rect(), rg); p.end();
      }
      else {
         QLinearGradient lg(start, stop);
         lg.setColorAt(0,bb); lg.setColorAt(0.5,b);
         lg.setColorAt(0.5, dd);
         (type == GradGlass) ? lg.setColorAt(1, b) : lg.setColorAt(.90, bb);
         QPainter p(pix); p.fillRect(pix->rect(), lg); p.end();
      }
   }
   else {
      ColorArray colors; PointArray stops;
      switch (type) {
      case GradButton:
      case GradButtonHover:
      case GradButtonDisabled: {
         int h,s, inc, dec;
         iC.getHsv(&h,&s,&v);
         QColor tc;
         if (type == GradButtonDisabled) {
            inc = 5; dec = 2;
            if (v+5 > 255) {
               inc = 255-v; dec += (5-inc);
            }
         }
         else {
            inc = 15; dec = 6;
            if (v+15 > 255) {
               inc = 255-v; dec += (15-inc);
            }
         }
         tc.setHsv(h,s,v+inc);
         iC.setHsv(h,s,v-dec);
         colors << tc << iC;
         if (type == GradButtonHover)
            stops << 0.30 << 0.75;
         else
            stops << 0 << 0.75;
         break;
      }
      case GradGloss:
      case GradRadialGloss: {
         // calculate the determining colors
         QColor d,dd,b,bb;
         int h,s, ch,cs,cv, delta, add;
         b = iC; d = b;
         
         iC.getHsv(&h,&s,&v);
         
         add = ((180-qGray(b.rgb()))>>1);
         if (add < 0) add = -add/2;
   
         cv = v+27+add;
         if (cv > 255) {
            delta = cv-255; cv = 255;
            cs = s - delta; if (cs < 0) cs = 0;
            ch = h - delta/6; if (ch < 0) ch = 360+ch;
         }
         else {
            ch = h; cs = s;
         }
         bb.setHsv(ch,cs,cv);
         
         cv = v - 14-add; if (cv < 0) cv = 0;
         cs = s*13/7; if (cs > 255) cs = 255;
         dd.setHsv(h,cs,cv);
   
         colors << bb << b << dd << d;
         stops << 0 <<  0.5 << 0.5 << 1;
         
         break;
      }
      case GradSunken:
         colors << iC.dark(100+config.gradientIntensity*20/100)
            << iC.light(100+config.gradientIntensity*60/100);
         break;
      case GradGroup: {
         if (o == Qt::Horizontal) {
            pix = new QPixmap(32, size);
            pix->fill(Qt::transparent);
            start = QPoint(32, 0); stop = QPoint(32, pix->height());
            iC = c; iC.setAlpha(0);
            colors << c << iC;
            break;
         }
         colors << c.light(100+(qMin(3,size/15))) << c;
         break;
      }
      case GradSimple:
      default:
         colors << iC.light(100+config.gradientIntensity*60/100)
            << iC.dark(100+config.gradientIntensity*20/100);
      }
      
      OXPicture grad = OXRender::gradient(start, stop, colors, stops);
      OXRender::composite (grad, None, *pix, 0, 0, 0, 0, 0, 0, pix->width(), pix->height());
      OXRender::freePicture(grad);
   }
   
   // cache for later ;)
   cache->insert(magicNumber, pix);
   return *pix;
}

const QPixmap &OxygenStyle::btnAmbient(int height) const {
   if (height <= 0) {
      qWarning("NULL Pixmap requested, height was %d",height);
      return nullPix;
   }
   QPixmap *pix = _btnAmbient.object(height);
   if (pix)
      return *pix;
      
   pix = new QPixmap(16*height/9,height); //golden mean relations
   pix->fill(Qt::transparent);
   ColorArray colors = ColorArray() << QColor(255,255,255,128) << QColor(255,255,255,0);
   OXPicture grad = OXRender::gradient(QPoint(pix->width(), pix->height()),
                                       QPoint(pix->width()/2,pix->height()/2), colors);
   OXRender::composite (grad, None, *pix, 0, 0, 0, 0, 0, 0, pix->width(), pix->height());
   OXRender::freePicture(grad);
   
   // cache for later ;)
   PixmapCache *cache = &(const_cast<OxygenStyle*>( this )->_btnAmbient);
   cache->insert(height, pix);
   return *pix;
}

const QPixmap &OxygenStyle::tabShadow(int height, bool bottom) const {
   if (height <= 0) {
      qWarning("NULL Pixmap requested, height was %d",height);
      return nullPix;
   }
   uint val = height + bottom*0x80000000;
   QPixmap *pix = _tabShadow.object(val);
   if (pix)
      return *pix;
      
   pix = new QPixmap(height/3,height);
   pix->fill(Qt::transparent);
   ColorArray colors = ColorArray() << QColor(0,0,0,75) << QColor(0,0,0,0);
   float hypo = sqrt(pow(pix->width(),2)+pow(pix->height(),2));
   float cosalpha = (float)(pix->height())/hypo;
   QPoint p1, p2;
   if (bottom) {
      p1 = QPoint(0, 0);
      p2 = QPoint((int)(pix->width()*pow(cosalpha, 2)),
                  (int)(pow(pix->width(), 2)*cosalpha/hypo));
   }
   else {
      p1 = QPoint(0, pix->height());
      p2 = QPoint((int)(pix->width()*pow(cosalpha, 2)),
                  (int)pix->height() - (int)(pow(pix->width(), 2)*cosalpha/hypo));
   }
   OXPicture grad = OXRender::gradient(p1, p2, colors);
   OXRender::composite (grad, None, *pix, 0, 0, 0, 0, 0, 0, pix->width(), pix->height());
   OXRender::freePicture(grad);
   
   // cache for later ;)
   PixmapCache *cache = &(const_cast<OxygenStyle*>( this )->_tabShadow);
   cache->insert(val, pix);
   return *pix;
}
