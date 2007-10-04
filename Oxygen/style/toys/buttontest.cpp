#include <QtGui>
#include <KApplication>
#include <KCmdLineArgs>
#include <KAboutData>
#include <KColorScheme>
#include <KColorUtils>

#include <math.h>

#include "tileset.h"

int r = 224, g = 224, b = 224;

//BEGIN TileCache
bool lowThreshold(const QColor &color)
{
    QColor darker = KColorScheme::shade(color, KColorScheme::MidShade, 0.5);
    return KColorUtils::luma(darker) > KColorUtils::luma(color);
}

QColor calcLightColor(const QColor &color)
{
    return KColorScheme::shade(color, KColorScheme::LightShade, 0.7);
}

QColor calcMidlightColor(const QColor &color)
{
    return KColorScheme::shade(color, KColorScheme::MidlightShade, 0.7);
}

QColor calcMidColor(const QColor &color)
{
    return KColorScheme::shade(color, KColorScheme::MidShade, -0.4);
}

QColor calcDarkColor(const QColor &color)
{
    if (lowThreshold(color))
        return KColorUtils::mix(color, KColorScheme::shade(color, KColorScheme::LightShade, 0.7), 0.3);
    else
        return KColorScheme::shade(color, KColorScheme::MidShade, 0.7);
}

QColor calcShadowColor(const QColor &color)
{
    return KColorScheme::shade(color, KColorScheme::ShadowShade, 0.7);
}

QColor alphaColor(QColor color, double alpha)
{
    color.setAlphaF(alpha);
    return color;
}

void drawShadow(QPainter &p, const QColor &color, int size)
{
    double m = double(size-2)*0.5;

    const double offset = 0.8;
    double k0 = (m-4.0) / m;
    QRadialGradient shadowGradient(m+1.0, m+offset+1.0, m);
    for (int i = 0; i < 8; i++) { // sinusoidal gradient
        double k1 = (k0 * double(8 - i) + double(i)) * 0.125;
        double a = (cos(3.14159 * i * 0.125) + 1.0) * 0.25;
        shadowGradient.setColorAt(k1, alphaColor(color, a));
    }
    shadowGradient.setColorAt(1.0, alphaColor(color, 0.0));
    p.setBrush(shadowGradient);
    p.drawEllipse(QRectF(0, 0, size, size));
}

void drawInverseShadow(QPainter &p, const QColor &color,
                       int pad, int size, double fuzz)
{
    double m = double(size)*0.5;

    const double offset = 0.8;
    double k0 = (m-2.0) / double(m+2.0);
    QRadialGradient shadowGradient(pad+m, pad+m+offset, m+2.0);
    for (int i = 0; i < 8; i++) { // sinusoidal gradient
        double k1 = (double(8 - i) + k0 * double(i)) * 0.125;
        double a = (cos(3.14159 * i * 0.125) + 1.0) * 0.25;
        shadowGradient.setColorAt(k1, alphaColor(color, a));
    }
    shadowGradient.setColorAt(1.0, color);
    p.setBrush(shadowGradient);
    p.drawEllipse(QRectF(pad-fuzz, pad-fuzz, size+fuzz*2.0, size+fuzz*2.0));
}

QPixmap windecoButton(const QColor &color, int size)
{
    QPixmap pixmap(size*2, size*2);
    pixmap.fill(QColor(0,0,0,0));

    QPainter p(&pixmap);
    p.setRenderHints(QPainter::Antialiasing);
    p.setPen(Qt::NoPen);
    p.setWindow(0,0,20,20);

    QColor light = calcLightColor(color);
    QColor dark = calcDarkColor(color);

    // shadow
    drawShadow(p, calcShadowColor(color), 20);

    // bevel
    qreal y = KColorUtils::luma(color);
    qreal yl = KColorUtils::luma(light);
    qreal yd = KColorUtils::luma(dark);
    QLinearGradient bevelGradient(0, 1, 0, 19);
    bevelGradient.setColorAt(0.45, light);
    bevelGradient.setColorAt(0.80, dark);
    if (y < yl && y > yd) // no middle when color is very light/dark
        bevelGradient.setColorAt(0.55, color);
    p.setBrush(QBrush(bevelGradient));
    p.drawEllipse(QRectF(3.0,3.0,14.0,14.0));

    // inside mask
    QRadialGradient maskGradient(10,10,7);
    maskGradient.setColorAt(0.70, QColor(0,0,0,0));
    maskGradient.setColorAt(0.85, QColor(0,0,0,140));
    maskGradient.setColorAt(0.95, QColor(0,0,0,255));
    p.setCompositionMode(QPainter::CompositionMode_DestinationIn);
    p.setBrush(maskGradient);
    p.drawRect(0,0,18,20);

    // inside
    QLinearGradient innerGradient(0, 4, 0, 16);
    innerGradient.setColorAt(0.0, color);
    innerGradient.setColorAt(1.0, light);
    p.setCompositionMode(QPainter::CompositionMode_DestinationOver);
    p.setBrush(innerGradient);
    p.drawEllipse(QRectF(3.0,3.0,14.0,14.0));

    // anti-shadow
    QRadialGradient highlightGradient(10,9.5,8);
    highlightGradient.setColorAt(0.85, alphaColor(light, 0.0));
    highlightGradient.setColorAt(1.00, light);
    p.setCompositionMode(QPainter::CompositionMode_SourceOver);
    p.setBrush(highlightGradient);
    p.drawEllipse(QRectF(3.0,3.0,14.0,14.0));

    p.end();

    return pixmap;
}

QPixmap roundSlab(const QColor &color, int size)
{
    double shade = 0.0;
    QPixmap pixmap(size*3, size*3);
    pixmap.fill(QColor(0,0,0,0));

    QPainter p(&pixmap);
    p.setRenderHints(QPainter::Antialiasing);
    p.setPen(Qt::NoPen);
    p.setWindow(0,0,21,21);

    QColor base = KColorUtils::shade(color, shade);
    QColor light = KColorUtils::shade(calcLightColor(color), shade);
    QColor dark = KColorUtils::shade(calcDarkColor(color), shade);

    // shadow
    drawShadow(p, calcShadowColor(color), 21);

    // bevel, part 1
    qreal y = KColorUtils::luma(base);
    qreal yl = KColorUtils::luma(light);
    qreal yd = KColorUtils::luma(dark);
    QLinearGradient bevelGradient1(0, 10, 0, 18);
    bevelGradient1.setColorAt(0.0, light);
    bevelGradient1.setColorAt(0.9, dark);
    if (y < yl && y > yd) // no middle when color is very light/dark
        bevelGradient1.setColorAt(0.5, base);
    p.setBrush(bevelGradient1);
    p.drawEllipse(QRectF(3.0,3.0,15.0,15.0));

    // bevel, part 2
    QLinearGradient bevelGradient2(0, 7, 0, 28);
    bevelGradient2.setColorAt(0.0, light);
    bevelGradient2.setColorAt(0.9, base);
    p.setBrush(bevelGradient2);
    p.drawEllipse(QRectF(3.6,3.6,13.8,13.8));

    // inside
    QLinearGradient innerGradient(0, -17, 0, 20);
    innerGradient.setColorAt(0.0, light);
    innerGradient.setColorAt(1.0, base);
    p.setBrush(innerGradient);
    p.drawEllipse(QRectF(4.4,4.4,12.2,12.2));

    p.end();

    return pixmap;
}

void drawSlab(QPainter &p, const QColor &color)
{
    double shade = 0.0;
    QColor base = KColorUtils::shade(color, shade);
    QColor light = KColorUtils::shade(calcLightColor(color), shade);
    QColor dark = KColorUtils::shade(calcDarkColor(color), shade);

    // bevel, part 1
    qreal y = KColorUtils::luma(base);
    qreal yl = KColorUtils::luma(light);
    qreal yd = KColorUtils::luma(dark);
    QLinearGradient bevelGradient1(0, 7, 0, 11);
    bevelGradient1.setColorAt(0.0, light);
    bevelGradient1.setColorAt(0.9, dark);
    if (y < yl && y > yd) // no middle when color is very light/dark
        bevelGradient1.setColorAt(0.5, base);
    p.setBrush(bevelGradient1);
    p.drawEllipse(QRectF(3.0,3.0,8.0,8.0));

    // bevel, part 2
    QLinearGradient bevelGradient2(0, 6, 0, 19);
    bevelGradient2.setColorAt(0.0, light);
    bevelGradient2.setColorAt(0.9, base);
    p.setBrush(bevelGradient2);
    p.drawEllipse(QRectF(3.6,3.6,6.8,6.8));

    // inside mask
    p.setCompositionMode(QPainter::CompositionMode_DestinationOut);
    p.setBrush(QBrush(Qt::black));
    p.drawEllipse(QRectF(4.4,4.4,5.2,5.2));
}

TileSet slab(const QColor &color, int size)
{
    QPixmap pixmap(size*2, size*2);
    pixmap.fill(QColor(0,0,0,0));

    QPainter p(&pixmap);
    p.setRenderHints(QPainter::Antialiasing);
    p.setPen(Qt::NoPen);
    p.setWindow(0,0,14,14);

    // shadow
    drawShadow(p, calcShadowColor(color), 14);

    // slab
    drawSlab(p, color);

    p.end();

    return TileSet(pixmap, size-1, size, 2, 1);
}

TileSet sunkenSlab(const QColor &color, int size)
{
    QPixmap pixmap(size*2, size*2);
    pixmap.fill(QColor(0,0,0,0));

    QPainter p(&pixmap);
    p.setRenderHints(QPainter::Antialiasing);
    p.setPen(Qt::NoPen);
    p.setWindow(0,0,14,14);

    // slab
    drawSlab(p, color);

    // shadow
    p.setCompositionMode(QPainter::CompositionMode_SourceOver);
    drawInverseShadow(p, calcShadowColor(color), 3, 8, 0.0);

    p.end();

    return TileSet(pixmap, size-1, size, 2, 1);
}

TileSet inverseSlab(const QColor &color, int size)
{
    double shade = 0.0;
    QPixmap pixmap(size*2, size*2);
    pixmap.fill(QColor(0,0,0,0));

    QPainter p(&pixmap);
    p.setRenderHints(QPainter::Antialiasing);
    p.setPen(Qt::NoPen);
    p.setWindow(0,0,14,14);

    QColor base = KColorUtils::shade(color, shade);
    QColor light = KColorUtils::shade(calcLightColor(color), shade);
    QColor dark = KColorUtils::shade(calcDarkColor(color), shade);

    // bevel, part 2
    QLinearGradient bevelGradient2(0, 8, 0, -8);
    bevelGradient2.setColorAt(0.0, light);
    bevelGradient2.setColorAt(0.9, base);
    p.setBrush(bevelGradient2);
    p.drawEllipse(QRectF(2.6,2.6,8.8,8.8));

    // bevel, part 1
    qreal y = KColorUtils::luma(base);
    qreal yl = KColorUtils::luma(light);
    qreal yd = KColorUtils::luma(dark);
    QLinearGradient bevelGradient1(0, 7, 0, 4);
    bevelGradient1.setColorAt(0.0, light);
    bevelGradient1.setColorAt(0.9, dark);
    if (y < yl && y > yd) // no middle when color is very light/dark
        bevelGradient1.setColorAt(0.5, base);
    p.setBrush(bevelGradient1);
    p.drawEllipse(QRectF(3.4,3.4,7.2,7.2));

    // inside mask
    p.setCompositionMode(QPainter::CompositionMode_DestinationOut);
    p.setBrush(QBrush(Qt::black));
    p.drawEllipse(QRectF(4.0,4.0,6.0,6.0));

    // shadow
    p.setCompositionMode(QPainter::CompositionMode_DestinationOver);
    drawInverseShadow(p, calcShadowColor(color), 4, 6, 0.5);

    p.end();

    return TileSet(pixmap, size-1, size, 2, 1);
}
//END TileCache

//BEGIN Render
void renderTileset(QPainter &p, const QRect &rect, const TileSet &tileset)
{
    tileset.render(rect, p);
}

void renderFilledTileset(QPainter &p, const QRect &rect,
                         const TileSet &tileset, const QColor &color,
                         int size, double shade = 0.0)
{
    int s = int(floor(double(size)*4.0/7.0));
    QRect r = rect.adjusted(s, s, -s, -s);
    int rx = (86*size) / r.width(); // 86 = 2*(7-4)/7
    int ry = (86*size) / r.height();

    p.save();

    QLinearGradient innerGradient(0, rect.top() - rect.height(), 0, rect.bottom());
    innerGradient.setColorAt(0.0, KColorUtils::shade(calcLightColor(color), shade));
    innerGradient.setColorAt(1.0, KColorUtils::shade(color, shade));
    p.setPen(Qt::NoPen);
    p.setBrush(innerGradient);
    p.drawRoundRect(r, rx, ry);

    tileset.render(rect, p);

    p.restore();
}
//END Render

//BEGIN Widget
class Widget : public QWidget
{
    Q_OBJECT
public:
    Widget(QWidget *parent=0) : QWidget(parent) {}

    QSize sizeHint() const { return QSize(860, 420); }

protected:
    void paintEvent(QPaintEvent *e)
    {
        QColor color(r, g, b);
        QPainter p(this);
        QRect rect = e->rect();
        p.setWindow(rect);
        p.setClipRect(rect);

        p.fillRect(rect, color);

        // windeco button
        p.drawPixmap(QRect(2,2,20,20), windecoButton(color, 10));
        p.drawPixmap(QRect(44,0,200,200), windecoButton(color, 100));

        // radio button
        p.drawPixmap(QRect(2,32,21,21), roundSlab(color, 7));
        p.drawPixmap(QRect(244,0,210,210), roundSlab(color, 70));

        // glowing radio button (TODO)
        p.drawPixmap(QRect(24,32,21,21), roundSlab(color, 7));
        p.drawPixmap(QRect(248,210,210,210), roundSlab(color, 70));

        // regular button
        renderFilledTileset(p, QRect(2,62,21,21), slab(color, 7), color, 7);
        renderFilledTileset(p, QRect(458,0,210,210), slab(color, 70), color, 70);

        // glowing button (TODO)
        renderFilledTileset(p, QRect(24,62,21,21), slab(color, 7), color, 7);
        renderFilledTileset(p, QRect(458,210,210,210), slab(color, 70), color, 70);

        // sunken button
        renderFilledTileset(p, QRect(2,92,21,21), sunkenSlab(color, 7), color, 7);
        renderFilledTileset(p, QRect(658,0,210,210), sunkenSlab(color, 70), color, 70);

        // inverted button
        renderTileset(p, QRect(24,92,21,21), inverseSlab(color, 7));
        renderTileset(p, QRect(658,210,210,210), inverseSlab(color, 70));
    }

};
//END Widget

Widget *w;

//BEGIN Picker
class Picker : public QWidget
{
    Q_OBJECT
public:
    Picker(QWidget *parent=0) : QWidget(parent)
    {
        QGridLayout *l = new QGridLayout;
        QLabel *c[3];
        c[0] = new QLabel("Red");
        c[1] = new QLabel("Green");
        c[2] = new QLabel("Blue");
        l->addWidget(c[0], 0, 0);
        l->addWidget(c[1], 1, 0);
        l->addWidget(c[2], 2, 0);
        QSpinBox *s[3];
        s[0] = new QSpinBox; s[0]->setRange(0, 255); s[0]->setValue(r);
        s[1] = new QSpinBox; s[1]->setRange(0, 255); s[1]->setValue(g);
        s[2] = new QSpinBox; s[2]->setRange(0, 255); s[2]->setValue(b);
        connect(s[0], SIGNAL(valueChanged(int)), this, SLOT(  redChanged(int)));
        connect(s[1], SIGNAL(valueChanged(int)), this, SLOT(greenChanged(int)));
        connect(s[2], SIGNAL(valueChanged(int)), this, SLOT( blueChanged(int)));
        l->addWidget(s[0], 0, 1);
        l->addWidget(s[1], 1, 1);
        l->addWidget(s[2], 2, 1);
        setLayout(l);
    }

public slots:
    void   redChanged(int value) { r = value; w->update(); }
    void greenChanged(int value) { g = value; w->update(); }
    void  blueChanged(int value) { b = value; w->update(); }
};
//END Picker

//BEGIN MyLayout
class MyLayout : public QWidget
{
    Q_OBJECT
public:
    MyLayout(QWidget *parent=0) : QWidget(parent)
    {
        w = new Widget; // needed by Picker

        QHBoxLayout *l = new QHBoxLayout;
        l->addWidget(new Picker);
        l->addWidget(w);
        setLayout(l);
        w->update();
    }
};
//END MyLayout

#include "buttontest.moc"

int main(int argc, char **argv)
{
    KAboutData about("oxybuttontest", 0, ki18n("oxybuttontest"), "0.1",
                     ki18n("Oxygen style button test application"),
                     KAboutData::License_GPL, ki18n("Copyright 2007 Matthew Woehlke"));
    about.addAuthor( ki18n("Matthew Woehlke"), KLocalizedString(), "mw_triad@users.sourceforge.net" );
    KCmdLineArgs::init(argc, argv, &about);
    KCmdLineOptions options;
    options.add("+[red]", ki18n("Red component of color"));
    options.add("+[green]", ki18n("Green component of color"));
    options.add("+[blue]", ki18n("Blue component of color"));
    KCmdLineArgs::addCmdLineOptions(options);
    KApplication app;
    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

    if (args->count() > 2) {
        r = args->arg(0).toInt();
        g = args->arg(1).toInt();
        b = args->arg(2).toInt();
    }

    MyLayout l;
    l.show();
    return app.exec();
}
