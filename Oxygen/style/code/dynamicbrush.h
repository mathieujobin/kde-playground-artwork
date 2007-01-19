#ifndef DYNAMICBRUSH_H
#define DYNAMICBRUSH_H

class QTimer;
class QImage;
class QPixmap;
class QRect;

#include <QObject>
#include <QSize>
#include <QRect>
#include <X11/Xlib.h>
#include <fixx11h.h>

class DynamicBrush : public QObject
{
   Q_OBJECT
public:
   enum Mode {Tiled = 0, XRender, OpenGL, Tiled2};
   DynamicBrush(Mode mode, QObject *parent = 0);
   DynamicBrush(Pixmap pixmap = -1, Pixmap shadow = -1, int bgYoffset = 0, QObject *parent = 0);
   DynamicBrush(const QImage &leftCenter, const QImage &leftTile, QObject *parent = 0);
   QPixmap shadow(const QRect &rect);
   void setMode(Mode);
   void setXPixmap(Pixmap pixmap = -1, Pixmap shadow = -1);
   virtual bool eventFilter ( QObject * watched, QEvent * event );
private:
   void updateBrushTiled();
   void updateBrushGL();
   void updateBrushRender();
   void updateBrushEdMetal();
   QPixmap glPixmap(const QRect &rect, int darkness = 0);
   QSize _size;
   Pixmap _pixmap, _shadow;
   int _bgYoffset;
   Mode _mode;
   QTimer *_timer;
   QPixmap *_center[2][2];
   QPixmap *_tile[2][2];
   QPixmap *_glShadow;
   QRect _lastShadowRect;
private slots:
   void wipeBackground();
};

#endif //DYNAMICBRUSH_H
