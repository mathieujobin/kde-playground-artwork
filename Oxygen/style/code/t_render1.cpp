   
#ifndef BTM_OVERLAP
#define BTM_OVERLAP blw = (blw*r.width())/(blw+brw); brw = r.width() - blw;
#endif
#ifndef RGT_OVERLAP
#define RGT_OVERLAP trh = (trh*r.height())/(trh+brh); brh = r.height() - trh;
#endif

   int rOff = 0, xOff, yOff, w, h;
   
   r.getRect(&xOff, &yOff, &w, &h);
   int tlh = height(TopLeft), blh = height(BtmLeft),
      trh = height(TopRight), brh = height(BtmLeft),
      tlw = width(TopLeft), blw = width(BtmLeft),
      trw = width(TopRight), brw = width(BtmRight);
   
   if (pf & Left)
   {
      w -= width(TopLeft);
      xOff += width(TopLeft);
      if (pf & (Top | Bottom) && tlh + blh > r.height()) // vertical edge overlap
      {
         tlh = (tlh*r.height())/(tlh+blh);
         blh = r.height() - tlh;
      }
   }
   if (pf & Right)
   {
      w -= width(TopRight);
      if (pf & (Top | Bottom) && trh + brh > r.height()) // vertical edge overlap
      {
         RGT_OVERLAP
      }
   }
   
   if (pf & Top)
   {
      if (pf & (Left | Right) && tlw + trw > r.width()) // horizontal edge overlap
      {
         tlw = tlw*r.width()/(tlw+trw);
         trw = r.width() - tlw;
      }
      rOff = r.right()-trw+1;
      yOff += tlh;
      h -= tlh;
      if (pf & Left) // upper left
         DRAW_PIXMAP(r.x(),r.y(),pixmap[TopLeft], 0, 0, tlw, tlh);
      if (pf & Right) // upper right
         DRAW_PIXMAP(rOff, r.y(), pixmap[TopRight], width(TopRight)-trw, 0, trw, trh);
      
      // upper line
      if (w > 0)
         DRAW_TILED_PIXMAP(xOff, r.y(), w, height(TopMid), pixmap[TopMid]);
   }
   if (pf & Bottom)
   {
      if (pf & (Left | Right) && blw + brw > r.width()) // horizontal edge overlap
      {
         BTM_OVERLAP
      }
      rOff = r.right()-brw+1;
      int bOff = r.bottom()-blh+1;
      h -= blh;
      if (pf & Left) // lower left
         DRAW_PIXMAP(r.x(), bOff, pixmap[BtmLeft], 0, height(BtmLeft)-blh, blw, blh);
      if (pf & Right) // lower right
         DRAW_PIXMAP(rOff, bOff, pixmap[BtmRight], width(BtmRight)-brw, height(BtmRight)-brh, brw, brh);
      
      // lower line
      if (w > 0)
         DRAW_TILED_PIXMAP(xOff, bOff, w, height(BtmMid), pixmap[BtmMid]);
   }
   
   if (h > 0)
   {
      if ((pf & Center) && (w > 0)) // center part
         DRAW_TILED_PIXMAP(xOff, yOff, w, h, pixmap[MidMid]);
      if (pf & Left) // left line
         DRAW_TILED_PIXMAP(r.x(), yOff, width(MidLeft), h, pixmap[MidLeft]);
      rOff = r.right()-width(MidRight)+1;
      if (pf & Right) // right line
         DRAW_TILED_PIXMAP(rOff, yOff, width(MidRight), h, pixmap[MidRight]);
   }

#undef BTM_OVERLAP
#undef RGT_OVERLAP
