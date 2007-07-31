#include <QtGui>
#include <KApplication>
#include <KCmdLineArgs>
#include <KAboutData>
#include <KColorScheme>
#include <KColorUtils>

int r = 224, g = 224, b = 224;

//BEGIN TileCache
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
    return KColorScheme::shade(color, KColorScheme::MidShade, 0.7);
}
//END TileCache

//BEGIN Widget
class Widget : public QWidget
{
    Q_OBJECT
public:
    Widget(QWidget *parent=0) : QWidget(parent) {}

    QSize sizeHint() const { return QSize(100, 100); }

protected:
    void paintEvent(QPaintEvent *e)
    {
        QColor color(r, g, b);
        QPainter p(this);
        QRect rect = e->rect();
        p.setClipRect(rect);
        p.setRenderHints(QPainter::Antialiasing);

        //* comment/uncomment to switch between zoomed and actual size
        if (rect.width() < rect.height())
            p.setWindow(0, 0, 20, 20 * rect.height() / rect.width());
        else
            p.setWindow(0, 0, 20 * rect.width() / rect.height(), 20);
        /*/
        p.setWindow(rect);
        //*/

        p.fillRect(rect, color);
        p.setPen(Qt::NoPen);

        // shadow
        QRadialGradient shadowGradient(10, 11, 9, 10, 12);
        shadowGradient.setColorAt(0.0, QColor(0,0,0,80));
        shadowGradient.setColorAt(1.0, QColor(0,0,0,0));
        p.setBrush(shadowGradient);
        p.drawEllipse(QRectF(0, 0, 20, 20));

        // outline
        QRadialGradient edgeGradient(10, 10, 9, 10, 10);
        edgeGradient.setColorAt(0.0, QColor(0,0,0,60));
        edgeGradient.setColorAt(0.9, QColor(0,0,0,20));
        edgeGradient.setColorAt(1.0, QColor(0,0,0,0));
        p.setBrush(edgeGradient);
        p.drawEllipse(QRectF(0, 0, 20, 20));

        // base (for anti-shadow)
        p.setBrush(color);
        p.drawEllipse(QRectF(2.4,2.4,15.2,15.2));

        // bevel
        QLinearGradient bevelGradient(0, 0, 0, 20);
        bevelGradient.setColorAt(0.45, calcLightColor(color));
        bevelGradient.setColorAt(0.55, color);
        bevelGradient.setColorAt(0.65, calcDarkColor(color));
        p.setBrush(QBrush(bevelGradient));
        p.drawEllipse(QRectF(2.4,2.4,15.2,15.0));

        // inside
        QLinearGradient innerGradient(0, 0, 0, 20);
        innerGradient.setColorAt(0.0, color);
        innerGradient.setColorAt(1.0, calcLightColor(color));
        p.setOpacity(0.4);
        p.setBrush(QBrush(innerGradient));
        p.drawEllipse(QRectF(3.2,3.2,13.6,13.6));
        p.setOpacity(1.0);
        p.setBrush(QBrush(innerGradient));
        p.drawEllipse(QRectF(4.0,4.0,12.0,12.0));
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
