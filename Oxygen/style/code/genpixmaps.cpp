void OxygenStyle::generatePixmaps()
{
   QPixmap tmp; QPainter p;
   
   // PUSHBUTTON =====================================
   // shadow
   int $1 = dpi.$1, $2 = dpi.$2, $2_2 = lround($2/2.0);
   int $9 = dpi.$9, $9_2 = ($9-1)/2;
   tmp = QPixmap($9,$9);
   for (int i = 0; i < 2; ++i)
   {
      tmp.fill(Qt::transparent);
      p.begin(&tmp);
      p.setPen(Qt::NoPen);
      p.setRenderHint(QPainter::Antialiasing);
      p.setBrush(QColor(0,0,0,(i+1)*9));
      p.drawRoundRect(0,0,$9,$9,60,60);
      p.setBrush(QColor(0,0,0,(i+1)*20));
      p.drawRoundRect($2_2,$2_2,$9-$2,$9-$2,80,80);
      p.end();
      shadows.button[i] = Tile::Set(tmp,$9_2,$9_2,$9-2*$9_2,$9-2*$9_2);
   }
   
   // -> sunken
   QImage tmpImg($9,$9, QImage::Format_ARGB32);
   
   for (int i = 0; i < 2; ++i)
   {
      int add = i*30;
      tmpImg.fill(Qt::transparent);
   
      p.begin(&tmpImg);
      p.setPen(Qt::NoPen);
      p.setRenderHint(QPainter::Antialiasing);
      p.setBrush(QColor(0,0,0,55+add)); p.drawRoundRect(0,0,$9,$9,80,80);
      p.setCompositionMode( QPainter::CompositionMode_DestinationOut );
      add = 30 - add;
      p.setBrush(QColor(0,0,0,120+add)); p.drawRoundRect(0,$1,$9,dpi.$8,75,75);
      p.setBrush(QColor(0,0,0,140+add)); p.drawRoundRect(0,$2,$9,dpi.$7,80,80);
      p.setBrush(QColor(0,0,0,160+add)); p.drawRoundRect($1,dpi.$3,dpi.$7,dpi.$6,85,85);
      p.setBrush(QColor(0,0,0,180+add)); p.drawRoundRect($2,dpi.$4,dpi.$3,dpi.$5,90,90);
      p.end();
   
      shadows.lineEdit[i] = Tile::Set(QPixmap::fromImage(tmpImg),$9_2,$9_2,$9-2*$9_2,$9-2*$9_2);
   }
   
   // relief
   tmp = QPixmap($9,$9);
   tmp.fill(Qt::transparent);
   p.begin(&tmp);
   QPen pen = p.pen(); pen.setWidth($1); p.setPen(pen);
   p.setRenderHint(QPainter::Antialiasing);
   p.setBrush(Qt::NoBrush);
   p.setPen(QColor(255,255,255,40));
   p.drawRoundRect(0,0,$9-$2,$9-$2,60,60);
   p.setPen(QColor(255,255,255,60));
   p.drawRoundRect($2,$2,$9-$2,$9-$2,60,60);
   p.setPen(QColor(0,0,0,50));
   p.drawRoundRect($1,$1,$9-dpi.$3,$9-dpi.$3,60,60);
   p.end();
   shadows.relief = Tile::Set(tmp,$9_2,$9_2,$9-2*$9_2,$9-2*$9_2);

   
   // outlines
   tmp = QPixmap($9,$9);
   for (int i = 0; i < 2; ++i)
   {
      tmp.fill(Qt::transparent);
      p.begin(&tmp);
      p.setRenderHint(QPainter::Antialiasing);
      p.setPen(QColor(255,255,255,100+i*60));
      p.setBrush(Qt::NoBrush);
      p.drawRoundRect(0,0,$9,2*$9,75,38);
      p.end();
      frames.button[i] = Tile::Set(tmp,$9_2,$9_2,$9-2*$9_2,$9-2*$9_2);
   }
//    frames.button[0] = Tile::Nuno(100);
//    frames.button[1] = Tile::Nuno(160);
   
   // mask
   tmp = QPixmap($9,$9);
   tmp.fill(Qt::transparent);
   p.begin(&tmp);
   p.setPen(Qt::NoPen);
   p.setRenderHint(QPainter::Antialiasing);
   p.setBrush(QColor(0,0,0,255));
   p.drawRoundRect(0,0,$9,$9,60,60);
   p.end();
   masks.button = Tile::Mask(tmp,$9_2,$9_2,$9-2*$9_2,$9-2*$9_2,0,0,0,0,60,60);
   
   // toplight
   int $49 = SCALE(49);
   int $49_2 = ($49-1)/2;
   tmp = QPixmap($49,$49);
   tmp.fill(Qt::transparent);
   QRadialGradient rg( tmp.rect().center(), $49_2 );
   rg.setColorAt ( 0, QColor(255,255,255,160) );
   rg.setColorAt ( 1, QColor(255,255,255,0) );
   p.begin(&tmp);
   p.fillRect(0,0,$49,$49,rg);
   p.end();
   tmp = tmp.scaled( $49, dpi.$5, Qt::IgnoreAspectRatio, Qt::SmoothTransformation );
   tmp = tmp.copy(0,$2,$49,dpi.$3);
   lights.top = Tile::Line(tmp,Qt::Horizontal,$49_2,-$49_2);
   
   // ================================================================
   
   // RADIOUTTON =====================================
   int rw = dpi.ExclusiveIndicator;
   int rh = dpi.ExclusiveIndicator;
   // shadow
   for (int i = 0; i < 2; ++i)
   {
      shadows.radio[i] = QPixmap(rw, rh);
      shadows.radio[i].fill(Qt::transparent);
      p.begin(&shadows.radio[i]);
      p.setPen(Qt::NoPen);
      p.setRenderHint(QPainter::Antialiasing);
      p.setBrush(QColor(0,0,0,(i+1)*9));
      p.drawEllipse(0,0,rw,rh);
      p.setBrush(QColor(0,0,0,(i+1)*20));
      p.drawEllipse($2_2,$2_2,rw-$2,rh-$2);
      p.end();
   }
   
   // mask
   rw -= dpi.$4; rh -= dpi.$4;
   masks.radio = QPixmap(rw, rh);
   masks.radio.fill(Qt::transparent);
   p.begin(&masks.radio);
   p.setPen(Qt::NoPen);
   p.setRenderHint(QPainter::Antialiasing);
   p.setBrush(QColor(0,0,0,255));
   p.drawEllipse(0,0,rw,rh);
   p.end();
   
   rw -= dpi.$4; rh -= dpi.$4;
   masks.radioGroove = QPixmap(rw, rh);
   masks.radioGroove.fill(Qt::transparent);
   p.begin(&masks.radioGroove);
   p.setPen(Qt::NoPen);
   p.setRenderHint(QPainter::Antialiasing);
   p.setBrush(QColor(0,0,0,255));
   p.drawEllipse(0,0,rw,rh);
   p.end();
   
   // mask fill
   rw -= dpi.$4; rh -= dpi.$4;
   masks.radioIndicator = QPixmap(rw, rh);
   masks.radioIndicator.fill(Qt::transparent);
   p.begin(&masks.radioIndicator);
   p.setPen(Qt::NoPen);
   p.setRenderHint(QPainter::Antialiasing);
   p.setBrush(QColor(0,0,0,255));
   p.drawEllipse(0,0,rw,rh);
   p.end();
   
   // ================================================================
   
   // RECTANGULAR =====================================
   
   // raised
   
   // sunken
   int $3 = dpi.$3, $4 = dpi.$4, $6 = dpi.$6;
   tmp = QPixmap($9,$9);
   tmp.fill(Qt::transparent);
   p.begin(&tmp);
   p.fillRect($1,0,$9-$2,$1, QColor(0,0,0,10));
   p.fillRect($2,$1,$9-$4,$1, QColor(0,0,0,20));
   p.fillRect($2,$2,$9-$4,$1, QColor(0,0,0,40));
   p.fillRect($3,$3,$9-$6,$1, QColor(0,0,0,80));
   
   p.fillRect($1,$9-$1,$9-$2,$1, QColor(255,255,255,10));
   p.fillRect($2,$9-$2,$9-$4,$1, QColor(255,255,255,20));
   p.fillRect($2,$9-$3,$9-$4,$1, QColor(255,255,255,40));
   p.fillRect($3,$9-$4,$9-$6,$1, QColor(255,255,255,80));
   
   p.fillRect(0,$1,$1,$9-$2, QColor(128,128,128,10));
   p.fillRect($1,$2,$1,$9-$4, QColor(128,128,128,20));
   p.fillRect($2,$2,$1,$9-$4, QColor(128,128,128,40));
   p.fillRect($3,$3,$1,$9-$6, QColor(128,128,128,80));
   
   p.fillRect($9-$1,$1,$1,$9-$2, QColor(128,128,128,10));
   p.fillRect($9-$2,$2,$1,$9-$4, QColor(128,128,128,20));
   p.fillRect($9-$3,$2,$1,$9-$4, QColor(128,128,128,40));
   p.fillRect($9-$4,$3,$1,$9-$6, QColor(128,128,128,80));
   
   p.end();
   shadows.sunken = Tile::Set(tmp,$9_2,$9_2,$9-2*$9_2,$9-2*$9_2);
   
   // ================================================================
   
   // TABBAR =====================================
   
   // mask
   int $13 = SCALE(13);
   tmp = QPixmap($13,$13);
   tmp.fill(Qt::transparent);
   p.begin(&tmp);
   p.setPen(Qt::NoPen);
   p.setRenderHint(QPainter::Antialiasing);
   p.setBrush(QColor(0,0,0,255));
   p.drawRoundRect(0,0,$13,$13,99,99);
   p.end();
   int $13_2 = ($13-1)/2;
   masks.tab = Tile::Mask(tmp,$13_2,$13_2,$13-2*$13_2,$13-2*$13_2,0,0,0,0,99,99);
   
   // shadow
   int $17 = SCALE(17);
   tmpImg = QImage($17,$17, QImage::Format_ARGB32);
   tmpImg.fill(Qt::transparent);
   p.begin(&tmpImg);
   p.setPen(Qt::NoPen);
   p.setRenderHint(QPainter::Antialiasing);
   p.setBrush(QColor(0,0,0,15)); p.drawRoundRect(0,0,$17,$17,90,90);
   p.setBrush(QColor(0,0,0,18)); p.drawRoundRect($1,$1,$17-$2,$17-$2,93,93);
   p.setBrush(QColor(0,0,0,23)); p.drawRoundRect($2,$2,$17-dpi.$4,$17-dpi.$4,96,96);
   p.setBrush(QColor(0,0,0,28)); p.drawRoundRect(dpi.$3,dpi.$3,$17-dpi.$6,$17-dpi.$6,99,99);
   p.setCompositionMode( QPainter::CompositionMode_DestinationOut );
   p.setBrush(QColor(0,0,0,255)); p.drawRoundRect($2,$1,$17-dpi.$4,$17-dpi.$5,99,99);
   p.setCompositionMode( QPainter::CompositionMode_SourceOver );
   p.setPen(QColor(255,255,255,170)); p.setBrush(Qt::NoBrush);
   p.drawRoundRect($2,$1,$17-dpi.$4,$17-dpi.$5,99,99);
   p.end();
   int $17_2 = ($17-1)/2;
   shadows.tab = Tile::Set(QPixmap::fromImage(tmpImg),$17_2,$17_2,$17-2*$17_2,$17-2*$17_2);
   
   // GROUPBOX =====================================
   
   // shadow
   tmpImg = QImage($49,$49, QImage::Format_ARGB32);
   tmpImg.fill(Qt::transparent);
   p.begin(&tmpImg);
   p.setPen(Qt::NoPen);
   p.setRenderHint(QPainter::Antialiasing);
   p.setBrush(QColor(0,0,0,5)); p.drawRoundRect(0,0,$49,2*$49,14,7);
   p.setBrush(QColor(0,0,0,9)); p.drawRoundRect($1,$1,$49-$2,2*$49,13,7);
   p.setBrush(QColor(0,0,0,11)); p.drawRoundRect($2,$2,$49-dpi.$4,2*$49,12,6);
   p.setBrush(QColor(0,0,0,13)); p.drawRoundRect(dpi.$3,dpi.$3,$49-dpi.$6,2*$49,48,24);
   p.setCompositionMode( QPainter::CompositionMode_DestinationIn );
   p.setBrush(QColor(0,0,0,0)); p.drawRoundRect(dpi.$4,dpi.$2,$49-dpi.$8,2*$49,11,6);
   p.setCompositionMode( QPainter::CompositionMode_SourceOver );
   p.setPen(QColor(255,255,255,160)); p.setBrush(Qt::NoBrush);
   p.drawRoundRect(dpi.$4,dpi.$2,$49-dpi.$8,2*$49,11,6);
   p.setRenderHint(QPainter::Antialiasing, false);
   p.setCompositionMode( QPainter::CompositionMode_DestinationIn );
   int $33 = SCALE(33);
   for (int i = 1; i < $33; ++i)
   {
      p.setPen(QColor(0,0,0,i*lround(255.0/dpi.$32)));
      p.drawLine(0, $49-i, $49, $49-i);
   }
   p.end();
   int $12 = dpi.$12;
   shadows.group = Tile::Set(QPixmap::fromImage(tmpImg),$12,$12,$49-2*$12,$1);
   
   // mask --> uses buttons
//    int $25 = SCALE(25);
//    tmp = QPixmap($25,$25);
//    tmp.fill(Qt::transparent);
//    p.begin(&tmp);
//    p.setPen(Qt::NoPen);
//    p.setRenderHint(QPainter::Antialiasing);
//    p.setBrush(QColor(0,0,0,255));
//    p.drawRoundRect(0,0,$25,$25,22,22);
//    p.end();
//    $12 = ($25-1)/2;
//    masks.group = Tile::Mask(tmp,$12,$12,$25-2*$12,$25-2*$12,0,0,0,0,22,22);
   
   // shadow line
   int w,h,c1,c2; QLinearGradient lg;
   for (int i = 0; i < 2; ++i) // orientarion
   {
      if (i)
      {
         w = $2; h = $49;
         lg = QLinearGradient(0,0,0,$49);
      }
      else
      {
         w = $49; h = $2;
         lg = QLinearGradient(0,0,$49,0);
      }
      tmp = QPixmap(w,h);
      for (int j = 0; j < 3; ++j) // direction
      {
         c1 = (j > 0) ? 111 : 255; c2 = (j > 0) ? 255 : 111;
         tmp.fill(Qt::transparent);
         p.begin(&tmp);
         QGradientStops stops;
         stops << QGradientStop( 0, QColor(c1,c1,c1,0) )
            << QGradientStop( 0.5, QColor(c1,c1,c1,71) )
            << QGradientStop( 1, QColor(c1,c1,c1,0) );
         lg.setStops(stops);
         if (i)
            p.fillRect(0,0,$1,$49,lg);
         else
            p.fillRect(0,0,$49,$1,lg);
         stops.clear();
         stops << QGradientStop( 0, QColor(c2,c2,c2,0) )
            << QGradientStop( 0.5, QColor(c2,c2,c2,74) )
            << QGradientStop( 1, QColor(c2,c2,c2,0) );
         lg.setStops(stops);
         if (i)
            p.fillRect($1,0,$2-$1,$49,lg);
         else
            p.fillRect(0,$1,$49,$2-$1,lg);
         p.end();
         shadows.line[i][j] = Tile::Line(tmp,i?Qt::Vertical:Qt::Horizontal,$49_2,-$49_2);
      }
   }
   // ================================================================
}
