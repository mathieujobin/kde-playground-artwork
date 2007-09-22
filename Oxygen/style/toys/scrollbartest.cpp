#include <QtGui>
#include <KApplication>
#include <KCmdLineArgs>
#include <KAboutData>
#include <KColorScheme>
#include <KColorUtils>

#include <math.h>

#include "tileset.h"

int r = 0, g = 128, b = 0, o = 0;

//BEGIN Helpers
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
//END Helpers

//BEGIN OxygenScrollbar
class OxygenScrollbar {
public:
    OxygenScrollbar(const QColor&);

    TileSet horizontal(int size, int width, int offset) const;
    TileSet vertical(int size, int width, int offset) const;

private:
    QLinearGradient shimmerGradient() const;
    QPixmap bevel(int width, int height, double w, double h, int rx, int ry) const;

    QColor color;
    QColor light;
    QColor dark;
    QColor shadow;
    QColor highlight;
};

OxygenScrollbar::OxygenScrollbar(const QColor &c) : color(c),
    light(KColorScheme::shade(c, KColorScheme::MidlightShade, 0.7)),
    dark(KColorScheme::shade(c, KColorScheme::DarkShade, 0.7)),
    shadow(KColorScheme::shade(c, KColorScheme::ShadowShade, 0.7)),
    highlight(Qt::white)
{
}

QPixmap OxygenScrollbar::bevel(int width, int height, double w, double h, int rx, int ry) const
{
    QPixmap pixmap(width, height);
    pixmap.fill(Qt::transparent);

    QPainter p(&pixmap);
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(Qt::NoPen);
    p.setWindow(0, 0, w, h);

    QRectF rect(0, 0, w, h);

    // anti-highlight
    QLinearGradient ahGradient(0, 0, 0, 8);
    ahGradient.setColorAt(0.0, dark);
    ahGradient.setColorAt(0.9, dark);
    ahGradient.setColorAt(1.0, shadow);
    p.setBrush(ahGradient);
    p.drawRect(rect);

    // anti-highlight mask
    p.setCompositionMode(QPainter::CompositionMode_DestinationOut);
    p.setBrush(Qt::black);
    p.drawRoundRect(rect.adjusted(0, 1, 0, -1), rx, ry);

    // bevel
    QLinearGradient bevelGradient(0, 0, 0, 8);
    bevelGradient.setColorAt(0.0, alphaColor(highlight, 0.4));
    bevelGradient.setColorAt(0.6, alphaColor(highlight, 0.4));
    bevelGradient.setColorAt(1.0, alphaColor(shadow, 0.8));
    p.setBrush(bevelGradient);
    p.setCompositionMode(QPainter::CompositionMode_DestinationOver);
    p.drawRect(rect);

    // mask
    p.setCompositionMode(QPainter::CompositionMode_DestinationOut);
    p.setBrush(Qt::black);
    p.drawRoundRect(rect.adjusted(1, 2, -1, -2), rx, ry);

    p.end();
    return pixmap;
}

TileSet OxygenScrollbar::vertical(int size, int width, int offset) const
{
    offset %= (size * 12);

    int s = size/2;
    int length = s*22;
    double w = 12.0 * double(width)/double(s*2);
    double o = -4.0 * double(offset) / double(size);
    const int h = 6*22;

    QPixmap pixmap(width, length);
    pixmap.fill(Qt::transparent);

    QPainter p(&pixmap);
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(Qt::NoPen);
    QRectF rect(0, 0, w, h);
    p.setWindow(0, 0, w, h);

    // mask; never draw outside this, hence SourceAtop
    // drawRoundRect is too bloody hard to control to get the corners perfectly
    // square (i.e. circles not ellipses), so draw the mask in parts with real
    // circles
    p.setBrush(Qt::black); // color doesn't matter
    p.drawRect(rect.adjusted(7,0,-7,0));
    p.drawRect(rect.adjusted(0,7,0,-7));
    p.drawEllipse(QRectF(0,0,14,14));
    p.drawEllipse(QRectF(w-14,0,14,14));
    p.drawEllipse(QRectF(0,h-14,14,14));
    p.drawEllipse(QRectF(w-14,h-14,14,14));
    p.setCompositionMode(QPainter::CompositionMode_SourceAtop);

    // base
    QLinearGradient baseGradient(0, 0, w*0.6, 0);
    baseGradient.setColorAt(0.0, color);
    baseGradient.setColorAt(1.0, dark);
    p.setBrush(baseGradient);
    p.drawRect(rect);

    // shine
    QLinearGradient shineGradient(0, 0, w*2.0, 0);
    shineGradient.setColorAt(0.0, light);
    shineGradient.setColorAt(0.5, alphaColor(color, 0.5));
    shineGradient.setColorAt(1.0, color);
    p.setBrush(shineGradient);
    p.drawRoundRect(QRectF(w*0.4, 0, w*0.6, h), int(2000.0/w), 12);
    p.setClipping(false);

    // shimmer - should tile every 48 units, with 1:3 slope
    QLinearGradient shimmerGradient(0, o, 14.4/2.0, 43.2/2.0 + o);
    shimmerGradient.setSpread(QGradient::ReflectSpread);
    shimmerGradient.setColorAt(0.0, alphaColor(dark, 0.40));
    shimmerGradient.setColorAt(0.6, alphaColor(dark, 0.10));
    shimmerGradient.setColorAt(1.0, alphaColor(dark, 0.00));
    p.setBrush(shimmerGradient);
    p.drawRect(rect);

    // dim edges
    QLinearGradient dimGradient(0, 0, 0, h/2);
    dimGradient.setSpread(QGradient::ReflectSpread);
    dimGradient.setColorAt(0.00, alphaColor(dark, 1.0));
    dimGradient.setColorAt(0.19, alphaColor(dark, 0.3));
    dimGradient.setColorAt(0.27, alphaColor(dark, 0.0));
    p.setBrush(dimGradient);
    p.drawRect(rect);

    // bevel
    p.setWindow(0, 0, width, length);
    p.drawPixmap(0, 0, bevel(width, length, w, h, int(1400.0/w), 9));
    p.setWindow(0, 0, w, h);

    // highlight
    p.setBrush(alphaColor(highlight, 0.2));
    p.drawRoundRect(QRectF(w-3, 7, 1.5, h-14), 100, 5);
    p.drawRoundRect(QRectF(1.5, 7, 1.5, h-14), 100, 5);

    return TileSet(pixmap, 1, s*3, width-2, s*16);
}

TileSet vertical(const QColor &color, int size, int width, int offset)
{
    return OxygenScrollbar(color).vertical(size, width, offset);
}
//END OxygenScrollbar

//BEGIN Widget
class Widget : public QWidget
{
    Q_OBJECT
public:
    Widget(QWidget *parent=0) : QWidget(parent) {}

    QSize sizeHint() const { return QSize(600, 800); }

protected:
    void renderVertical(QPainter &p, const QColor &color, int size, int width, QRect rect)
    {
        p.save();

        int l = ceil(double(size)*1.5);
        int offset = (o * size) / 8;

        TileSet ts1 = vertical(color, size, width, offset);
        p.setClipRect(rect.adjusted(0, 0, 0, -l));
        ts1.render(rect, p);

        TileSet ts2 = vertical(color, size, width, offset+(rect.height()+size)*3);
        p.setClipRect(rect.left(), rect.bottom() - l + 1, rect.width(), l+1);
        ts2.render(rect, p);

        p.restore();
    }

    void paintEvent(QPaintEvent *e)
    {
        QColor color(r, g, b);
        QPainter p(this);
        QRect rect = e->rect();
        p.setWindow(rect);
        p.setClipRect(rect);

        //p.fillRect(rect, color);

        // vertical
        renderVertical(p, color, 8, 14, QRect(2, 2, 14, 200));
        renderVertical(p, color, 80, 140, QRect(32, 2, 140, 766));
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
        QLabel *c[4];
        c[0] = new QLabel("Red");
        c[1] = new QLabel("Green");
        c[2] = new QLabel("Blue");
        c[3] = new QLabel("Offset");
        l->addWidget(c[0], 0, 0);
        l->addWidget(c[1], 1, 0);
        l->addWidget(c[2], 2, 0);
        l->addWidget(c[3], 4, 0);
        QSpinBox *s[4];
        s[0] = new QSpinBox; s[0]->setRange(0, 255); s[0]->setValue(r);
        s[1] = new QSpinBox; s[1]->setRange(0, 255); s[1]->setValue(g);
        s[2] = new QSpinBox; s[2]->setRange(0, 255); s[2]->setValue(b);
        s[3] = new QSpinBox; s[3]->setRange(0, 100); s[3]->setValue(o);
        connect(s[0], SIGNAL(valueChanged(int)), this, SLOT(   redChanged(int)));
        connect(s[1], SIGNAL(valueChanged(int)), this, SLOT( greenChanged(int)));
        connect(s[2], SIGNAL(valueChanged(int)), this, SLOT(  blueChanged(int)));
        connect(s[3], SIGNAL(valueChanged(int)), this, SLOT(offsetChanged(int)));
        l->addWidget(s[0], 0, 1);
        l->addWidget(s[1], 1, 1);
        l->addWidget(s[2], 2, 1);
        l->addWidget(s[3], 4, 1);
        setLayout(l);
    }

public slots:
    void    redChanged(int value) { r = value; w->update(); }
    void  greenChanged(int value) { g = value; w->update(); }
    void   blueChanged(int value) { b = value; w->update(); }
    void offsetChanged(int value) { o = value; w->update(); }
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

#include "scrollbartest.moc"

int main(int argc, char **argv)
{
    KAboutData about("oxyscrollbartest", 0, ki18n("oxyscrollbartest"), "0.1",
                     ki18n("Oxygen style scrollbar test application"),
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
