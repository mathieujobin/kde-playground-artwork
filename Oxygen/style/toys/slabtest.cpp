#include <QtGui>
#include <KApplication>
#include <KCmdLineArgs>
#include <KAboutData>
#include <KColorScheme>
#include <KColorUtils>

#include <math.h>

#include "tileset.h"

QColor *colors;

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
    color.setAlphaF(qMax(0.0, qMin(1.0, alpha)));
    return color;
}

const double shadowGain = 1.5;
const double shadowOffset = 0.8;

void drawShadow(QPainter &p, const QColor &color, int size)
{
    double m = double(size-2)*0.5;

    double k0 = (m-4.0) / m;
    QRadialGradient shadowGradient(m+1.0, m+shadowOffset+1.0, m);
    for (int i = 0; i < 8; i++) { // sinusoidal gradient
        double k1 = (k0 * double(8 - i) + double(i)) * 0.125;
        double a = (cos(3.14159 * i * 0.125) + 1.0) * 0.25;
        shadowGradient.setColorAt(k1, alphaColor(color, a*shadowGain));
    }
    shadowGradient.setColorAt(1.0, alphaColor(color, 0.0));
    p.setBrush(shadowGradient);
    p.drawEllipse(QRectF(0, 0, size, size));
}

void drawInverseShadow(QPainter &p, const QColor &color,
                       int pad, int size, double fuzz)
{
    double m = double(size)*0.5;

    double k0 = (m-2.0) / double(m+2.0);
    QRadialGradient shadowGradient(pad+m, pad+m+shadowOffset, m+2.0);
    for (int i = 0; i < 8; i++) { // sinusoidal gradient
        double k1 = (double(8 - i) + k0 * double(i)) * 0.125;
        double a = (cos(3.14159 * i * 0.125) + 1.0) * 0.25;
        shadowGradient.setColorAt(k1, alphaColor(color, a*shadowGain));
    }
    shadowGradient.setColorAt(k0, alphaColor(color, 0.0));
    p.setBrush(shadowGradient);
    p.drawEllipse(QRectF(pad-fuzz, pad-fuzz, size+fuzz*2.0, size+fuzz*2.0));
}

void drawGlow(QPainter &p, const QColor &color, int size)
{
    QRectF r(0, 0, size, size);
    double m = double(size)*0.5;

    const double width = 3.0;
    const double fuzz = 0.2;
    double k0 = (m-width+0.5) / m;
    QRadialGradient glowGradient(m, m, m);
    for (int i = 0; i < 8; i++) { // inverse parabolic gradient
        double k1 = (k0 * double(8 - i) + double(i)) * 0.125;
        double a = 1.0 - sqrt(i * 0.125);
        glowGradient.setColorAt(k1, alphaColor(color, a));
    }
    glowGradient.setColorAt(1.0, alphaColor(color, 0.0));

    // glow
    p.setBrush(glowGradient);
    p.drawEllipse(r);

    // mask
    p.setCompositionMode(QPainter::CompositionMode_DestinationOut);
    p.setBrush(QBrush(Qt::black));
    p.drawEllipse(r.adjusted(width+fuzz, width+fuzz, -width-fuzz, -width-fuzz));
}

void drawInverseGlow(QPainter &p, const QColor &color, int pad, int size)
{
    QRectF r(pad, pad, size, size);
    double m = double(size)*0.5;

    const double width = 3.0;
    const double bias = 0.5;
    double k0 = (m-width) / (m-bias);
    QRadialGradient glowGradient(pad+m, pad+m, m-bias);
    for (int i = 0; i < 8; i++) { // inverse parabolic gradient
        double k1 = (k0 * double(i) + double(8 - i)) * 0.125;
        double a = 1.0 - sqrt(i * 0.125);
        glowGradient.setColorAt(k1, alphaColor(color, a));
    }
    glowGradient.setColorAt(k0, alphaColor(color, 0.0));
    p.setBrush(glowGradient);
    p.drawEllipse(r);
}

QPixmap windecoButton(const QColor &color, int size)
{
    QPixmap pixmap(size*3, size*3);
    pixmap.fill(QColor(0,0,0,0));

    QPainter p(&pixmap);
    p.setRenderHints(QPainter::Antialiasing);
    p.setPen(Qt::NoPen);
    p.setWindow(0,0,21,21);

    QColor light = calcLightColor(color);
    QColor dark = calcDarkColor(color);

    // shadow
    drawShadow(p, calcShadowColor(color), 21);

    // bevel
    qreal y = KColorUtils::luma(color);
    qreal yl = KColorUtils::luma(light);
    qreal yd = KColorUtils::luma(dark);
    QLinearGradient bevelGradient(0, 1, 0, 20);
    bevelGradient.setColorAt(0.45, light);
    bevelGradient.setColorAt(0.80, dark);
    if (y < yl && y > yd) // no middle when color is very light/dark
        bevelGradient.setColorAt(0.55, color);
    p.setBrush(QBrush(bevelGradient));
    p.drawEllipse(QRectF(3.0,3.0,15.0,15.0));

    // inside mask
    QRadialGradient maskGradient(10.5,10.5,7.5);
    maskGradient.setColorAt(0.70, QColor(0,0,0,0));
    maskGradient.setColorAt(0.85, QColor(0,0,0,140));
    maskGradient.setColorAt(0.95, QColor(0,0,0,255));
    p.setCompositionMode(QPainter::CompositionMode_DestinationIn);
    p.setBrush(maskGradient);
    p.drawRect(0,0,21,21);

    // inside
    QLinearGradient innerGradient(0, 4, 0, 17);
    innerGradient.setColorAt(0.0, color);
    innerGradient.setColorAt(1.0, light);
    p.setCompositionMode(QPainter::CompositionMode_DestinationOver);
    p.setBrush(innerGradient);
    p.drawEllipse(QRectF(3.0,3.0,15.0,15.0));

    // anti-shadow
    QRadialGradient highlightGradient(10.5,10,8.5);
    highlightGradient.setColorAt(0.85, alphaColor(light, 0.0));
    highlightGradient.setColorAt(1.00, light);
    p.setCompositionMode(QPainter::CompositionMode_SourceOver);
    p.setBrush(highlightGradient);
    p.drawEllipse(QRectF(3.0,3.0,15.0,15.0));

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
    p.drawEllipse(QRectF(4.0,4.0,13.0,13.0));

    p.end();

    return pixmap;
}

QPixmap roundSlabGlow(const QColor &color, int size)
{
    QPixmap pixmap(size*3, size*3);
    pixmap.fill(QColor(0,0,0,0));

    QPainter p(&pixmap);
    p.setRenderHints(QPainter::Antialiasing);
    p.setPen(Qt::NoPen);
    p.setWindow(0,0,21,21);

    // glow
    drawGlow(p, color, 21);

    p.end();

    return pixmap;
}

void drawHole(QPainter &p, const QColor &color)
{
    double shade = 0.0;
    QColor base = KColorUtils::shade(color, shade);
    QColor light = KColorUtils::shade(calcLightColor(color), shade);
    QColor dark = KColorUtils::shade(calcDarkColor(color), shade);
    QColor mid = KColorUtils::shade(calcMidColor(color), shade);

    // bevel
    qreal y = KColorUtils::luma(base);
    qreal yl = KColorUtils::luma(light);
    qreal yd = KColorUtils::luma(dark);
    QLinearGradient bevelGradient1(0, 2, 0, 12);
    bevelGradient1.setColorAt(0.2, dark);
    bevelGradient1.setColorAt(0.5, mid);
    bevelGradient1.setColorAt(1.0, light);
    if (y < yl && y > yd) // no middle when color is very light/dark
        bevelGradient1.setColorAt(0.6, base);
    p.setBrush(bevelGradient1);
    p.drawEllipse(2,2,10,10);

    // mask
    QRadialGradient maskGradient(7,7,5);
    maskGradient.setColorAt(0.80, QColor(0,0,0,255));
    maskGradient.setColorAt(0.90, QColor(0,0,0,140));
    maskGradient.setColorAt(1.00, QColor(0,0,0,0));
    p.setCompositionMode(QPainter::CompositionMode_DestinationIn);
    p.setBrush(maskGradient);
    p.drawRect(0,0,14,14);
    p.setCompositionMode(QPainter::CompositionMode_SourceOver);
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
    p.drawEllipse(QRectF(4.0,4.0,6.0,6.0));
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

    return TileSet(pixmap, size, size, size, size, size-1, size, 2, 1);
}

TileSet sunkenSlab(const QColor &color, int size)
{
    QPixmap pixmap(size*2, size*2);
    pixmap.fill(QColor(0,0,0,0));

    QPainter p(&pixmap);
    p.setRenderHints(QPainter::Antialiasing);
    p.setPen(Qt::NoPen);
    p.setWindow(0,0,14,14);

    // hole
    drawHole(p, color);

    // slab
    drawSlab(p, color);

    // shadow
    p.setCompositionMode(QPainter::CompositionMode_SourceOver);
    drawInverseShadow(p, calcShadowColor(color), 3, 8, 0.0);

    p.end();

    return TileSet(pixmap, size, size, size, size, size-1, size, 2, 1);
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

    return TileSet(pixmap, size, size, size, size, size-1, size, 2, 1);
}

TileSet slabGlow(const QColor &color, int size)
{
    QPixmap pixmap(size*2, size*2);
    pixmap.fill(QColor(0,0,0,0));

    QPainter p(&pixmap);
    p.setRenderHints(QPainter::Antialiasing);
    p.setPen(Qt::NoPen);
    p.setWindow(0,0,14,14);

    // glow
    drawGlow(p, color, 14);

    p.end();

    return TileSet(pixmap, size, size, size, size, size-1, size, 2, 1);
}

TileSet hole(const QColor &color, int size)
{
    QPixmap pixmap(size*2, size*2);
    pixmap.fill(QColor(0,0,0,0));

    QPainter p(&pixmap);
    p.setRenderHints(QPainter::Antialiasing);
    p.setPen(Qt::NoPen);
    p.setWindow(0,0,14,14);

    // hole
    drawHole(p, color);

    // hole mask
    p.setCompositionMode(QPainter::CompositionMode_SourceOver);
    p.setBrush(color);
    p.drawEllipse(3,3,8,8);

    // shadow
    drawInverseShadow(p, calcShadowColor(color), 3, 8, 0.0);

    p.end();

    return TileSet(pixmap, size, size, size, size, size-1, size, 2, 1);
}

TileSet holeGlow(const QColor &color, int size)
{
    QPixmap pixmap(size*2, size*2);
    pixmap.fill(QColor(0,0,0,0));

    QPainter p(&pixmap);
    p.setRenderHints(QPainter::Antialiasing);
    p.setPen(Qt::NoPen);
    p.setWindow(0,0,14,14);

    // glow
    drawInverseGlow(p, color, 3, 8);

    p.end();

    return TileSet(pixmap, size, size, size, size, size-1, size, 2, 1);
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
    double s = floor(double(size)*3.6/7.0);
    QRectF r = rect;
    r.adjust(s, s, -s, -s);
    int rx = (int)floor((97*size) / r.width()); // 97 = 2*(7-3.6)/7
    int ry = (int)floor((97*size) / r.height());

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

enum CombineMode {
    Paint,
    Overlay,
    Tint,
    FadeLuma
};

static QPainter::CompositionMode comp;
static CombineMode combine;
static double amount;

static QColor combinedColor()
{
    switch (combine) { // TODO
        case Overlay:
            if (comp == QPainter::CompositionMode_SourceOver) {
                QColor temp1 = KColorUtils::mix(colors[3], colors[2], amount);
                QColor temp2 = KColorUtils::overlayColors(colors[3], colors[2],
                                QPainter::CompositionMode_Screen);
                return KColorUtils::mix(temp1, temp2, 0.7);
            }
            return KColorUtils::overlayColors(colors[3], colors[2], comp);
        case Tint:
            return KColorUtils::tint(colors[3], colors[2], amount);
            case FadeLuma: {
                QColor mixed = KColorUtils::mix(colors[3], colors[2], amount);
                double y2 = KColorUtils::luma(colors[2]);
                double y3 = KColorUtils::luma(colors[3]);
                double yr = KColorUtils::luma(mixed);
                return KColorUtils::shade(mixed, qMax(y2, y3) - yr);
            }
        default:
            return QColor();
    }
}

static QPainter::CompositionMode compositionMode()
{
    return (combine == Paint ? comp : QPainter::CompositionMode_SourceOver);
}

//BEGIN Widget
class Widget : public QWidget
{
    Q_OBJECT
public:
    Widget(QWidget *parent=0) : QWidget(parent) {}

    QSize sizeHint() const { return QSize(1060, 420); }

protected:
    void paintEvent(QPaintEvent *e)
    {
        QPainter p(this);
        QRect rect = e->rect();
        p.setWindow(rect);
        p.setClipRect(rect);

        p.fillRect(rect, colors[0]);

        // windeco button
        p.drawPixmap(QRect(2,2,21,21), windecoButton(colors[1], 7));
        p.drawPixmap(QRect(44,0,210,210), windecoButton(colors[1], 70));

        // glowing windeco button (ok to use roundSlabGlow)
        p.drawPixmap(QRect(24,2,21,21), windecoButton(colors[1], 7));
        p.drawPixmap(QRect(24,2,21,21), roundSlabGlow(colors[2], 7));
        p.drawPixmap(QRect(44,210,210,210), windecoButton(colors[1], 70));
        p.drawPixmap(QRect(44,210,210,210), roundSlabGlow(colors[2], 70));

        // radio button
        p.drawPixmap(QRect(2,32,21,21), roundSlab(colors[1], 7));
        p.drawPixmap(QRect(244,0,210,210), roundSlab(colors[1], 70));

        // glowing radio button
        p.drawPixmap(QRect(24,32,21,21), roundSlab(colors[1], 7));
        p.drawPixmap(QRect(24,32,21,21), roundSlabGlow(colors[3], 7));
        p.drawPixmap(QRect(244,210,210,210), roundSlab(colors[1], 70));
        p.drawPixmap(QRect(244,210,210,210), roundSlabGlow(colors[3], 70));

        // regular button
        renderFilledTileset(p, QRect(2,62,21,21), slab(colors[1], 7), colors[1], 7);
        renderFilledTileset(p, QRect(454,0,210,210), slab(colors[1], 70), colors[1], 70);

        // glowing button
        renderFilledTileset(p, QRect(24,62,21,21), slab(colors[1], 7), colors[1], 7);
        renderFilledTileset(p, QRect(454,210,210,210), slab(colors[1], 70), colors[1], 70);

        p.setCompositionMode(compositionMode());
        QColor mixed = combinedColor();

        if (mixed.isValid()) {
            renderTileset(p, QRect(24,62,21,21), slabGlow(mixed, 7));
            renderTileset(p, QRect(454,210,210,210), slabGlow(mixed, 70));
        }
        else {
            renderTileset(p, QRect(24,62,21,21), slabGlow(colors[2], 7));
            renderTileset(p, QRect(454,210,210,210), slabGlow(colors[2], 70));
            renderTileset(p, QRect(24,62,21,21), slabGlow(colors[3], 7));
            renderTileset(p, QRect(454,210,210,210), slabGlow(colors[3], 70));
        }

        // sunken button
        renderFilledTileset(p, QRect(2,92,21,21), sunkenSlab(colors[1], 7), colors[1], 7);
        renderFilledTileset(p, QRect(644,0,210,210), sunkenSlab(colors[1], 70), colors[1], 70);

        // inverted button
        renderTileset(p, QRect(24,92,21,21), inverseSlab(colors[1], 7));
        renderTileset(p, QRect(644,210,210,210), inverseSlab(colors[1], 70));

        // hole
        renderTileset(p, QRect(2,122,21,21), hole(colors[1], 7));
        renderTileset(p, QRect(834,0,210,210), hole(colors[1], 70));

        // glowing hole
        renderTileset(p, QRect(24,122,21,21), hole(colors[1], 7));
        renderTileset(p, QRect(24,122,21,21), holeGlow(colors[2], 7));
        renderTileset(p, QRect(834,210,210,210), hole(colors[1], 70));
        renderTileset(p, QRect(834,210,210,210), holeGlow(colors[2], 70));
    }

};
//END Widget

Widget *w;

//BEGIN Picker
class Picker : public QGroupBox
{
    Q_OBJECT
protected:
    QColor *_color;
public:
    Picker(QString text, QColor &color, QWidget *parent=0)
    : QGroupBox(text, parent), _color(&color)
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
        s[0] = new QSpinBox; s[0]->setRange(0, 255); s[0]->setValue(_color->red());
        s[1] = new QSpinBox; s[1]->setRange(0, 255); s[1]->setValue(_color->green());
        s[2] = new QSpinBox; s[2]->setRange(0, 255); s[2]->setValue(_color->blue());
        connect(s[0], SIGNAL(valueChanged(int)), this, SLOT(  redChanged(int)));
        connect(s[1], SIGNAL(valueChanged(int)), this, SLOT(greenChanged(int)));
        connect(s[2], SIGNAL(valueChanged(int)), this, SLOT( blueChanged(int)));
        l->addWidget(s[0], 0, 1);
        l->addWidget(s[1], 1, 1);
        l->addWidget(s[2], 2, 1);
        setLayout(l);
    }

public slots:
    void   redChanged(int value) { _color->setRed  (value); w->update(); }
    void greenChanged(int value) { _color->setGreen(value); w->update(); }
    void  blueChanged(int value) { _color->setBlue (value); w->update(); }
};
//END Picker

//BEGIN CompPicker
class CompPicker : public QGroupBox
{
    Q_OBJECT
protected:

public:
    CompPicker(QWidget *parent=0)
    : QGroupBox("Composition", parent)
    {
        QGridLayout *l = new QGridLayout;
        QLabel *n[3];
        n[0] = new QLabel("Combining Mode");
        n[1] = new QLabel("Blending Mode");
        n[2] = new QLabel("Combine Amount");
        l->addWidget(n[0], 0, 0);
        l->addWidget(n[1], 1, 0);
        l->addWidget(n[2], 2, 0);

        QComboBox *m[2];
        m[0] = new QComboBox; m[0]->setEditable(false);
        m[0]->addItem("Paint");
        m[0]->addItem("Overlay");
        m[0]->addItem("Tint");
        m[0]->addItem("Fade (keep luma)");
        connect(m[0], SIGNAL(currentIndexChanged(int)), this, SLOT(combineChanged(int)));
        combine = Overlay;
        m[0]->setCurrentIndex(1);

        m[1] = new QComboBox; m[1]->setEditable(false);
        m[1]->addItem("Normal");
        m[1]->addItem("Plus");
        m[1]->addItem("Screen");
        m[1]->addItem("Hard Light");
        m[1]->addItem("Soft Light");
        connect(m[1], SIGNAL(currentIndexChanged(int)), this, SLOT(blendChanged(int)));
        comp = QPainter::CompositionMode_SourceOver;

        l->addWidget(m[0], 0, 1);
        l->addWidget(m[1], 1, 1);

        // TODO amount slider
        amount = 0.5;

        setLayout(l);
    }

public slots:
    void blendChanged(int mode) {
        switch (mode) {
            case 1: // Plus
                comp = QPainter::CompositionMode_Plus;
                break;
            case 2: // Screen
                comp = QPainter::CompositionMode_Screen;
                break;
            case 3: // Hard Light
                comp = QPainter::CompositionMode_HardLight;
                break;
            case 6: // Soft Light
                comp = QPainter::CompositionMode_SoftLight;
                break;
            default: // must be Normal
                comp = QPainter::CompositionMode_SourceOver;
        }
        w->update();
    }

    void combineChanged(int mode) {
        switch (mode) {
            case 1: // Overlay
                combine = Overlay;
                break;
            case 2: // Tint
                combine = Tint;
                break;
            case 3: // Fade
                combine = FadeLuma;
                break;
            default: // must be Paint
                combine = Paint;
        }
        w->update();
    }
};
//END CompPicker

//BEGIN MyLayout
class MyLayout : public QWidget
{
    Q_OBJECT
public:
    MyLayout(QWidget *parent=0) : QWidget(parent)
    {
        w = new Widget; // needed by Picker

        QVBoxLayout *l = new QVBoxLayout;
        QWidget *c = new QWidget;
        QHBoxLayout *v = new QHBoxLayout;
        v->addWidget(new Picker("Background", colors[0]));
        v->addWidget(new Picker("Base", colors[1]));
        v->addWidget(new Picker("Glow 1", colors[2]));
        v->addWidget(new Picker("Glow 2", colors[3]));
        v->addWidget(new CompPicker());
        c->setLayout(v);
        l->addWidget(c);
        l->addWidget(w);
        setLayout(l);
        w->update();
    }
};
//END MyLayout

#include "slabtest.moc"

int main(int argc, char **argv)
{
    colors = new QColor[4];
    colors[0].setRgb(224, 224, 224);
    colors[1].setRgb(224, 224, 224);
    colors[2].setRgb( 80, 224,  80);
    colors[3].setRgb(224, 160,  80);

    KAboutData about("oxybuttontest", 0, ki18n("oxybuttontest"), "0.1",
                     ki18n("Oxygen style button test application"),
                     KAboutData::License_GPL, ki18n("Copyright 2007 Matthew Woehlke"));
    about.addAuthor( ki18n("Matthew Woehlke"), KLocalizedString(), "mw_triad@users.sourceforge.net" );
    KCmdLineArgs::init(argc, argv, &about);
    KCmdLineOptions options;
    options.add("+[red]", ki18n("Red component of base color"));
    options.add("+[green]", ki18n("Green component of base color"));
    options.add("+[blue]", ki18n("Blue component of base color"));
    options.add("+[red]", ki18n("Red component of background color"));
    options.add("+[green]", ki18n("Green component of background color"));
    options.add("+[blue]", ki18n("Blue component of background color"));
    KCmdLineArgs::addCmdLineOptions(options);
    KApplication app;
    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

    if (args->count() > 2) {
        colors[1].setRed(args->arg(0).toInt());
        colors[1].setGreen(args->arg(1).toInt());
        colors[1].setBlue(args->arg(2).toInt());
    }
    if (args->count() > 5) {
        colors[0].setRed(args->arg(3).toInt());
        colors[0].setGreen(args->arg(4).toInt());
        colors[0].setBlue(args->arg(5).toInt());
    }

    MyLayout l;
    l.show();
    return app.exec();
}
