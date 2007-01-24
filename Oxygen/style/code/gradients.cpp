/***************************************************************************
 *   Copyright (C) 2006-2007 by Thomas Lübking                             *
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

static QPixmap nullPix;

const QPixmap OxygenStyle::gradient(const QColor &oc, const int size, Qt::Orientation o, Orientation3D o3D) const
{
   QColor c = oc;
   PixmapCache *cache = &(const_cast<OxygenStyle*>( this )->gradientCache[o-1][o3D?1:0]);
   int v = colorValue(c);
   if (v < 80) // very dark colors won't make nice buttons ;)
   {
      int h,s;
      c.getHsv(&h,&s,&v);
      c.setHsv(h,s,80);
   }
   /*
   Ok, we want to cache the gradients, but unfortunately we have no idea about what kind of gradients will be demanded in the future
   Therefore creating a 2 component map (uint for color and uint for size) would be some overhead and cause a lot of unused space in the dictionaries
   So we store all the gradients by a uint index
   Therefore we substitute the alpha component (byte << 24) of the demanded color with the demanded size
   As this would limit the size to 255/256 pixels we'll be a bit sloppy, depending on the users resolution (e.g. use 0 to store a gradient with 2px, usefull for demand of 1px or 2px) and shift the index (e.g. gradients from 0 to 6 px size will hardly be needed - maybe add statistics on this)
   So the handled size is actually demandedSize + (demandedSize % sizeSloppyness), beeing at least demanded size and the next sloppy size above at max
   */
   int sizeSloppyness = 1;
   uint magicNumber = 0;
   PixmapCache::const_iterator it;
   if (size < 105884) // this is where our dictionary reaches - should be enough for the moment ;)
   {
      int frameBase = 0;
      int frameSize = 20;

      while ((frameBase += frameSize) < size)
      {
         sizeSloppyness++;
         frameSize += 20;
      }

      frameBase -=frameSize;
      frameSize -= 20;
      // The mapping is done by the "magicNumber"
//       magicNumber =  (((frameSize + (size - frameBase)/sizeSloppyness) & 0xff) << 24) | (c.rgb() & 0xffffff);
      magicNumber =  (((frameSize + (size - frameBase)/sizeSloppyness) & 0xff) << 21) | 
         (((c.red() >> 1) & 0x7f) << 14) |
         (((c.green() >> 1) & 0x7f) << 7 ) |
         ((c.blue() >> 1) & 0x7f);
      
      it = cache->find(magicNumber);
      if (it != cache->end())
         return it.value();
   }
   else
   {
      qWarning("gradient with more than 105883 steps requested, returning NULL pixmap");
      return nullPix;
   }
   /*
   adepted from kpixmapeffect.cpp - WARNING! this is specialised. only use for single column/row pixmaps. pixmaps with depth 8 will become plain (for the moment...)
   also horizontal gradients will become bilinear!
   */
   QPixmap pix;
   if (o == Qt::Horizontal)
      pix = QPixmap(size + (size % sizeSloppyness), 32);
   else
      pix = QPixmap(32, size + (size % sizeSloppyness));
   
   if (pix.isNull())
   {
      qWarning("NULL Pixmap requested, size was %d",size);
      return nullPix;
   }
   
   QPainter p(&pix);
   
   QColor ca;
   QColor cb;
      
   if (o3D == Raised)
   {
      ca = c.light(100+config.gradientIntensity*60/100);
      cb = c.dark(100+config.gradientIntensity*20/100);
   }
   else
   {
      ca = c.dark(100+config.gradientIntensity*20/100);
      cb = c.light(100+config.gradientIntensity*60/100);
   }

   int rDiff, gDiff, bDiff;
   int rca, gca, bca;
   
   register int x, y;

   rDiff = (cb.red())   - (rca = ca.red());
   gDiff = (cb.green()) - (gca = ca.green());
   bDiff = (cb.blue())  - (bca = ca.blue());
   
   register int rl = rca << 16;
   register int gl = gca << 16;
   register int bl = bca << 16;
   
   int rcdelta = ((1<<16) / (o == Qt::Vertical ? pix.height() : pix.width())) * rDiff;
   int gcdelta = ((1<<16) / (o == Qt::Vertical ? pix.height() : pix.width())) * gDiff;
   int bcdelta = ((1<<16) / (o == Qt::Vertical ? pix.height() : pix.width())) * bDiff;
   
   // these for-loops could be merged, but the if's in the inner loop
   // would make it slow
   if (o == Qt::Vertical)
      for ( y = 0; y < pix.height(); y++ )
      {
         rl += rcdelta;
         gl += gcdelta;
         bl += bcdelta;
         
         p.setPen(QColor(rl>>16, gl>>16, bl>>16));
         p.drawLine(0, y, 31 , y);
      }
   else
   {
      for( x = 0; x < pix.width(); x++)
      {
         rl += rcdelta;
         gl += gcdelta;
         bl += bcdelta;
         
         p.setPen(QColor(rl>>16, gl>>16, bl>>16));
         p.drawLine(x, 0, x, 31);
      }
   }
   
//    if (size < 105883) // this is where our dictionary reaches - should be enough for the moment ;)
   {
      // cache for later ;)
      if (cache->size() == cache->capacity())
         // we're full, wipe for this time
         cache->clear();
      it = cache->insert(magicNumber, pix);
      return it.value();
   }
//    return pix;
}

const QPixmap OxygenStyle::gloss(const QColor &oc, const int size, Qt::Orientation o, Orientation3D o3D) const
{
   // SEE gradient(.)
   PixmapCache *cache = &(const_cast<OxygenStyle*>( this )->glossCache[o-1][o3D?1:0]);
   int sizeSloppyness = 1;
   uint magicNumber = 0;
   PixmapCache::const_iterator it;
   if (size < 105884) // this is where our dictionary reaches - should be enough for the moment ;)
   {
      int frameBase = 0;
      int frameSize = 20;
      while ((frameBase += frameSize) < size)
      {
         sizeSloppyness++;
         frameSize += 20;
      }
      frameBase -=frameSize;
      frameSize -= 20;
      // The mapping is done by the "magicNumber"
//       magicNumber =  (((frameSize + (size - frameBase)/sizeSloppyness) & 0xff) << 24) | (oc.rgb() & 0xffffff);
      magicNumber =  (((frameSize + (size - frameBase)/sizeSloppyness) & 0xff) << 21) | 
         (((oc.red() >> 1) & 0x7f) << 14) |
         (((oc.green() >> 1) & 0x7f) << 7 ) |
         ((oc.blue() >> 1) & 0x7f);
      it = cache->find(magicNumber);
      if (it != cache->end())
         return it.value();
   }
   else
   {
      qWarning("gradient with more than 105883 steps requested, returning NULL pixmap");
      return nullPix;
   }
   
   QPixmap pix;
   if (o == Qt::Horizontal)
      pix = QPixmap(size + (size % sizeSloppyness), 32);
   else
      pix = QPixmap(32, size + (size % sizeSloppyness));
   
   if (pix.isNull())
   {
      qWarning("NULL Pixmap requested, size was %d",size);
      return nullPix;
   }
   
   // calculate the determining colors
   QColor d,dd,b,bb;
   int h,s,v, ch,cs,cv, delta, add;
   oc.getHsv(&h,&s,&v);
   if (v < 85) v = 85;
   b.setHsv(h,s,v); d = b;
   if (o3D == Sunken) dd = b;
   
   add = ((180-qGray(b.rgb()))>>1);
   if (add < 0) add = -add/2;
   
   if (o3D == Raised)
   {
      cv = v+27+add;
      if (cv > 255)
      {
         delta = cv-255; cv = 255;
         cs = s - delta; if (cs < 0) cs = 0;
         ch = h - delta/6; if (ch < 0) ch = 360+ch;
      }
      else
      {
         ch = h; cs = s;
      }
      bb.setHsv(ch,cs,cv);
   }
   
   cv = v - 14-add; if (cv < 0) cv = 0;
   cs = s*13/7; if (cs > 255) cs = 255;
   if (o3D == Raised)
      dd.setHsv(h,cs,cv);
   else
      bb.setHsv(h,cs,cv);
   
   // real painting action
   QPainter p(&pix);
   
   int rDiff = b.red() - bb.red();
   int gDiff = b.green() - bb.green();
   int bDiff = b.blue() - bb.blue();
   
   register int rl = (bb.red() << 16);
   register int gl = (bb.green() << 16);
   register int bl = (bb.blue() << 16);
   
   int rcdelta; int gcdelta; int bcdelta;
   
   if (o == Qt::Vertical)
   {
      register int y;
      h = (1<<17) / (pix.height()+1);
      rcdelta = h * rDiff;
      gcdelta = h * gDiff;
      bcdelta = h * bDiff;
      
      for ( y = 0; y < (pix.height()>>1); y++ )
      {
         rl += rcdelta; gl += gcdelta; bl += bcdelta;
         p.setPen(QColor(rl>>16, gl>>16, bl>>16));
         p.drawLine(0, y, 31 , y);
      }
      
      
      rDiff = d.red() - dd.red();
      gDiff = d.green() - dd.green();
      bDiff = d.blue() - dd.blue();
      
      rl = (dd.red() << 16);
      gl = (dd.green() << 16);
      bl = (dd.blue() << 16);
      
      rcdelta = h * rDiff;
      gcdelta = h * gDiff;
      bcdelta = h * bDiff;
      
      for ( ; y < pix.height(); y++ )
      {
         rl += rcdelta; gl += gcdelta; bl += bcdelta;
         p.setPen(QColor(rl>>16, gl>>16, bl>>16));
         p.drawLine(0, y, 31 , y);
      }
   }
   else
   {
      register int x;
      if (o3D == Raised)
      {
         h = (1<<16) / ((pix.width()+1)*8/18);
         s = (pix.width()*8/18);
      }
      else
      {
         h = (1<<17) / (pix.width()+1);
         s = pix.width()>>1;
      }
      rcdelta = h * rDiff;
      gcdelta = h * gDiff;
      bcdelta = h * bDiff;
      
      for ( x = 0; x < s; x++ )
      {
         rl += rcdelta; gl += gcdelta; bl += bcdelta;
         p.setPen(QColor(rl>>16, gl>>16, bl>>16));
         p.drawLine(x, 0, x , 31);
      }
      
      rDiff = d.red() - dd.red();
      gDiff = d.green() - dd.green();
      bDiff = d.blue() - dd.blue();
      
      rl = (dd.red() << 16);
      gl = (dd.green() << 16);
      bl = (dd.blue() << 16);
      
      if (o3D == Raised)
         h = (1<<16) / ((pix.width()+1)*10/18);
      rcdelta = h * rDiff;
      gcdelta = h * gDiff;
      bcdelta = h * bDiff;
      
      for ( ; x < pix.width(); x++ )
      {
         rl += rcdelta; gl += gcdelta; bl += bcdelta;
         p.setPen(QColor(rl>>16, gl>>16, bl>>16));
         p.drawLine(x, 0, x , 31);
      }
   }

   // store the result
   // 1768 is where our dictionary reaches - should be enough for the moment ;)
//    if (size < 105883)
   {

      // cache for later ;)
      if (cache->size() == cache->capacity())
      // we're full, wipe for this time
         cache->clear();
      it = cache->insert(magicNumber, pix);

      return it.value();
   }
//    return pix;
}
