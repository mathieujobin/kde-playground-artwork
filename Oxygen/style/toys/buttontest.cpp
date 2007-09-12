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
    int m = size>>1;

    const double offset = 0.8;
    double k0 = (double(m) - 4.0) / double(m);
    QRadialGradient shadowGradient(m, m+offset, m, m, m+offset);
    for (int i = 0; i < 8; i++) { // sinusoidal gradient
        double k1 = k0 * double(8 - i) * 0.125 + double(i) * 0.125;
        double a = (cos(3.14159 * i * 0.125) + 1.0) * 0.25;
        shadowGradient.setColorAt(k1, alphaColor(color, a));
    }
    shadowGradient.setColorAt(1.0, alphaColor(color, 0.0));
    p.setBrush(shadowGradient);
    p.drawEllipse(QRectF(0, offset, size, size+offset));
}

QPixmap windecoButton(const QColor &color, int size)
{
    QPixmap pixmap(size, (int)ceil(double(size)*10.0/9.0));
    pixmap.fill(QColor(0,0,0,0));

    QPainter p(&pixmap);
    p.setRenderHints(QPainter::Antialiasing);
    p.setPen(Qt::NoPen);
    p.setWindow(0,0,18,20);

    QColor light = calcLightColor(color);
    QColor dark = calcDarkColor(color);

    // shadow
    drawShadow(p, calcShadowColor(color), 18);

    // bevel
    qreal y = KColorUtils::luma(color);
    qreal yl = KColorUtils::luma(light);
    qreal yd = KColorUtils::luma(light);
    QLinearGradient bevelGradient(0, 0, 0, 18);
    bevelGradient.setColorAt(0.45, light);
    bevelGradient.setColorAt(0.80, dark);
    if (y < yl && y > yd) // no middle when color is very light/dark
        bevelGradient.setColorAt(0.55, color);
    p.setBrush(QBrush(bevelGradient));
    p.drawEllipse(QRectF(2.0,2.0,14.0,14.0));

    // inside mask
    QRadialGradient maskGradient(9,9,7,9,9);
    maskGradient.setColorAt(0.70, QColor(0,0,0,0));
    maskGradient.setColorAt(0.85, QColor(0,0,0,140));
    maskGradient.setColorAt(0.95, QColor(0,0,0,255));
    p.setCompositionMode(QPainter::CompositionMode_DestinationIn);
    p.setBrush(maskGradient);
    p.drawRect(0,0,18,20);

    // inside
    QLinearGradient innerGradient(0, 3, 0, 15);
    innerGradient.setColorAt(0.0, color);
    innerGradient.setColorAt(1.0, light);
    p.setCompositionMode(QPainter::CompositionMode_DestinationOver);
    p.setBrush(innerGradient);
    p.drawEllipse(QRectF(2.0,2.0,14.0,14.0));

    // anti-shadow
    QRadialGradient highlightGradient(9,8.5,8,9,8.5);
    highlightGradient.setColorAt(0.85, alphaColor(light, 0.0));
    highlightGradient.setColorAt(1.00, light);
    p.setCompositionMode(QPainter::CompositionMode_SourceOver);
    p.setBrush(highlightGradient);
    p.drawEllipse(QRectF(2.0,2.0,14.0,14.0));

    return pixmap;
}

QPixmap roundSlab(const QColor &color, int size)
{
    double shade = 0.0;
    QPixmap pixmap(size, (int)ceil(double(size)*10.0/9.0));
    pixmap.fill(QColor(0,0,0,0));

    QPainter p(&pixmap);
    p.setRenderHints(QPainter::Antialiasing);
    p.setPen(Qt::NoPen);
    p.setWindow(0,0,18,20);

    QColor base = KColorUtils::shade(color, shade);
    QColor light = KColorUtils::shade(calcLightColor(color), shade);
    QColor dark = KColorUtils::shade(calcDarkColor(color), shade);

    // shadow
    drawShadow(p, calcShadowColor(color), 18);

    // bevel, part 1
    qreal y = KColorUtils::luma(base);
    qreal yl = KColorUtils::luma(light);
    qreal yd = KColorUtils::luma(light);
    QLinearGradient bevelGradient1(0, 9, 0, 16);
    bevelGradient1.setColorAt(0.0, light);
    bevelGradient1.setColorAt(0.9, dark);
    if (y < yl && y > yd) // no middle when color is very light/dark
        bevelGradient1.setColorAt(0.5, base);
    p.setBrush(bevelGradient1);
    p.drawEllipse(QRectF(2.0,2.0,14.0,14.0));

    // bevel, part 2
    QLinearGradient bevelGradient2(0, 6, 0, 26);
    bevelGradient2.setColorAt(0.0, light);
    bevelGradient2.setColorAt(0.9, base);
    p.setBrush(bevelGradient2);
    p.drawEllipse(QRectF(2.6,2.6,12.8,12.8));

    // inside
    QLinearGradient innerGradient(0, -18, 0, 18);
    innerGradient.setColorAt(0.0, light);
    innerGradient.setColorAt(1.0, base);
    p.setBrush(innerGradient);
    p.drawEllipse(QRectF(3.4,3.4,11.2,11.2));

    return pixmap;
}

TileSet slab(const QColor &color, int size)
{
    int s = size/2;
    double shade = 0.0;
    QPixmap pixmap(size & -2, (int)ceil(double(size)*14.0/12.0));
    pixmap.fill(QColor(0,0,0,0));

    QPainter p(&pixmap);
    p.setRenderHints(QPainter::Antialiasing);
    p.setPen(Qt::NoPen);
    p.setWindow(0,0,12,14);

    QColor base = KColorUtils::shade(color, shade);
    QColor light = KColorUtils::shade(calcLightColor(color), shade);
    QColor dark = KColorUtils::shade(calcDarkColor(color), shade);

    // shadow
    drawShadow(p, calcShadowColor(color), 12);

    // bevel, part 1
    qreal y = KColorUtils::luma(base);
    qreal yl = KColorUtils::luma(light);
    qreal yd = KColorUtils::luma(light);
    QLinearGradient bevelGradient1(0, 6, 0, 10);
    bevelGradient1.setColorAt(0.0, light);
    bevelGradient1.setColorAt(0.9, dark);
    if (y < yl && y > yd) // no middle when color is very light/dark
        bevelGradient1.setColorAt(0.5, base);
    p.setBrush(bevelGradient1);
    p.drawEllipse(QRectF(2.0,2.0,8.0,8.0));

    // bevel, part 2
    QLinearGradient bevelGradient2(0, 5, 0, 18);
    bevelGradient2.setColorAt(0.0, light);
    bevelGradient2.setColorAt(0.9, base);
    p.setBrush(bevelGradient2);
    p.drawEllipse(QRectF(2.6,2.6,6.8,6.8));

    // inside mask
    QRadialGradient maskGradient(6,6,2.56,6,6);
    maskGradient.setColorAt(0.99, QColor(0,0,0,0));
    maskGradient.setColorAt(1.0, QColor(0,0,0,255));
    p.setCompositionMode(QPainter::CompositionMode_DestinationIn);
    p.setBrush(maskGradient);
    p.drawRect(0,0,12,14);

    return TileSet(pixmap, s-1, s, 2, 1);
}

//END TileCache

//BEGIN Render
void renderSlab(QPainter &p, const QRect &rect, const QColor &color, int size, double shade = 0.0)
{
    TileSet tileset = slab(color, size);
    int s1 = size/4;
    int s2 = s1 + (int)ceil(double(size)*2.0/14.0);

    p.save();

    QLinearGradient innerGradient(0, rect.top() - rect.height(), 0, rect.bottom());
    innerGradient.setColorAt(0.0, KColorUtils::shade(calcLightColor(color), shade));
    innerGradient.setColorAt(1.0, color);
    p.setPen(Qt::NoPen);
    p.setBrush(/*innerGradient*/QBrush(Qt::red));
    p.drawRect(rect/*.adjusted(s1, s1, -s1, -s2)*/);

    p.setCompositionMode(QPainter::CompositionMode_SourceOver);
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

    QSize sizeHint() const { return QSize(610, 200); }

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
        p.drawPixmap(QRect(2,2,18,20), windecoButton(color, 18));
        p.drawPixmap(QRect(24,0,180,200), windecoButton(color, 180));

        // radio button
        p.drawPixmap(QRect(2,32,18,20), roundSlab(color, 18));
        p.drawPixmap(QRect(224,0,180,200), roundSlab(color, 180));

        // regular button
        renderSlab(p, QRect(2,62,18,20), color, 12);
        renderSlab(p, QRect(424,0,180,200), color, 120);
        // TODO
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
                     ki18n("Oxygen style background test application"),
                     KAboutData::License_GPL, ki18n("(C) 2007 Matthew Woehlke"));
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
