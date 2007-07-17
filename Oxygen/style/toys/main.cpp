#include <QtGui>
#include <KApplication>
#include <KCmdLineArgs>
#include <KAboutData>
#include <KColorScheme>
#include <KColorUtils>

//BEGIN class declarations
// TODO merge with implementation
class Controller : public QWidget
{
    Q_OBJECT
public:
    Controller(QWidget *parent=0);
public slots:
    void khaChanged(double value);
    void khbChanged(double value);
    void kraChanged(double value);
    void krbChanged(double value);
    void modeChanged(int value);
};
//END class declarations

int r = 224, g = 224, b = 224;
double kha = 0.04, khb = 0.20, kra = 0.10, krb = 0.60;
int mode = 1;

//BEGIN TileCache
class TileCache
{
    private:
        TileCache();

    public:
        ~TileCache() {}
        static TileCache *instance();

        void clear();
        QPixmap verticalGradient(const QColor &color, int height);
        QPixmap radialGradient(const QColor &color, int width);

    private:
        static TileCache *s_instance;
        QCache<quint64, QPixmap> m_cache;
};

TileCache *TileCache::s_instance = 0;

TileCache::TileCache()
{
    m_cache.setMaxCost(64);
}

TileCache *TileCache::instance()
{
    if (!s_instance)
        s_instance = new TileCache;

    return s_instance;
}

void TileCache::clear()
{
    m_cache.clear();
}

QPixmap TileCache::verticalGradient(const QColor &color, int height)
{
    quint64 key = (quint64(color.rgba()) << 32) | height | 0x8000;
    QPixmap *pixmap = m_cache.object(key);

    if (!pixmap)
    {
        qreal y = KColorUtils::luma(color);
        pixmap = new QPixmap(32, height);

        QLinearGradient gradient(0, 0, 0, height);
        if (mode == 0)
            gradient.setColorAt(0, color.lighter(115));
        else
            gradient.setColorAt(0, KColorUtils::shade(color, kha + khb*y));
        gradient.setColorAt(1, color);

        QPainter p(pixmap);
        p.setCompositionMode(QPainter::CompositionMode_Source);
        p.fillRect(pixmap->rect(), gradient);

        m_cache.insert(key, pixmap);
    }

    return *pixmap;
}

QPixmap TileCache::radialGradient(const QColor &color, int width)
{
    quint64 key = (quint64(color.rgba()) << 32) | width | 0xb000;
    QPixmap *pixmap = m_cache.object(key);

    if (!pixmap)
    {
        qreal y = KColorUtils::luma(color);
        width /= 2;
        pixmap = new QPixmap(width, 64);
        pixmap->fill(QColor(0,0,0,0));
        QColor radialColor;
        if (mode == 0)
            radialColor = color.lighter(140);
        else
            radialColor = KColorUtils::shade(color, kra + krb*y);
        radialColor.setAlpha(255);
        QRadialGradient gradient(64, 0, 64);
        gradient.setColorAt(0, radialColor);
        radialColor.setAlpha(101);
        gradient.setColorAt(0.5, radialColor);
        radialColor.setAlpha(37);
        gradient.setColorAt(0.75, radialColor);
        radialColor.setAlpha(0);
        gradient.setColorAt(1, radialColor);

        QPainter p(pixmap);
        p.scale(width/128.0,1);
        p.fillRect(pixmap->rect(), gradient);

        m_cache.insert(key, pixmap);
    }

    return *pixmap;
}
//END TileCache

//BEGIN Widget
class Widget : public QWidget
{
    Q_OBJECT
public:
    Widget(QWidget *parent=0) : QWidget(parent) {}

    QSize sizeHint() const { return QSize(600, 600); }
public Q_SLOTS:
    void update()
    {
        TileCache::instance()->clear();
        QWidget::update();
    }

protected:
    void paintEvent(QPaintEvent *e)
    {
        QPainter p(this);
        QRect rect = e->rect();
        p.setClipRect(rect);
        QColor color(r, g, b);

        p.setWindow(rect);
        rect.adjust(1,1,0,0);
        int splitY = qMin(300, 3*rect.height()/4);

        QRect upperRect = QRect(0, 0, rect.width(), splitY);
        QPixmap tile = TileCache::instance()->verticalGradient(color, splitY);
        p.drawTiledPixmap(upperRect, tile);

        QRect lowerRect = QRect(0,splitY, rect.width(), rect.height() - splitY);
        p.fillRect(lowerRect, color);

        int radialW = qMin(600, rect.width());
        tile = TileCache::instance()->radialGradient(color, radialW);
        QRect radialRect = QRect((rect.width() - radialW) / 2, 0, radialW, 64);
        p.drawPixmap(radialRect, tile);

        p.setPen(Qt::black);
        p.drawRect(rect.adjusted(-1,-1,-1,-1));
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

Controller::Controller(QWidget *parent )
    : QWidget(parent)
{
#define NUM_PARAMS 4
    const char *names[NUM_PARAMS] = {"H Offset", "H Scale", "R Offset", "R Scale"};
    QGridLayout *l = new QGridLayout;
    QLabel *c[NUM_PARAMS];
    QDoubleSpinBox *k[NUM_PARAMS];
    for (int i=0; i<NUM_PARAMS; i++) {
        c[i] = new QLabel(names[i]);
        k[i] = new QDoubleSpinBox;
        k[i]->setMinimum(-1.0);
        k[i]->setMaximum(1.0);
        k[i]->setDecimals(2);
        k[i]->setSingleStep(0.01);
        l->addWidget(c[i], i, 0);
        l->addWidget(k[i], i, 1);
    }
    k[0]->setValue(kha); k[1]->setValue(khb);
    k[2]->setValue(kra); k[3]->setValue(krb);
    connect(k[0], SIGNAL(valueChanged(double)), this, SLOT(khaChanged(double)));
    connect(k[1], SIGNAL(valueChanged(double)), this, SLOT(khbChanged(double)));
    connect(k[2], SIGNAL(valueChanged(double)), this, SLOT(kraChanged(double)));
    connect(k[3], SIGNAL(valueChanged(double)), this, SLOT(krbChanged(double)));
    QGroupBox *f = new QGroupBox("Mode");
    QButtonGroup *g = new QButtonGroup;
    QRadioButton *m[2];
    m[0] = new QRadioButton("Qt");
    m[1] = new QRadioButton("KCU");
    QVBoxLayout *v = new QVBoxLayout;
    v->addWidget(m[0]); g->addButton(m[0]); g->setId(m[0], 0);
    v->addWidget(m[1]); g->addButton(m[1]); g->setId(m[1], 1);
    m[mode]->setChecked(true);
    connect(g, SIGNAL(buttonClicked(int)), this, SLOT(modeChanged(int)));
    f->setLayout(v);
    l->addWidget(f, NUM_PARAMS, 0, 1, 2);
    setLayout(l);
}

void Controller::khaChanged(double value) { kha = value; w->update(); }
void Controller::khbChanged(double value) { khb = value; w->update(); }
void Controller::kraChanged(double value) { kra = value; w->update(); }
void Controller::krbChanged(double value) { krb = value; w->update(); }
void Controller::modeChanged(int value) { mode = value; w->update(); }

//BEGIN MyLayout
class MyLayout : public QWidget
{
    Q_OBJECT
public:
    MyLayout(QWidget *parent=0) : QWidget(parent)
    {
        QVBoxLayout *lv = new QVBoxLayout;
        lv->addWidget(new Picker);
        lv->addWidget(new Controller);

        QHBoxLayout *lh = new QHBoxLayout;
        lh->addLayout(lv);
        w = new Widget;
        lh->addWidget(w);
        setLayout(lh);
    }
};
//END MyLayout

#include "main.moc"

int main(int argc, char **argv)
{
    KAboutData about("oxybgtest", 0, ki18n("oxybgtest"), "0.1",
                     ki18n("Oxygen style background test application"),
                     KAboutData::License_GPL, ki18n(
                     "(C) 2007 Matthew Woehlke, "
                     "(C) 2007 Casper Boemann, "
                     "(C) 2007 Fredrik Höglund"));
    about.addAuthor( ki18n("Matthew Woehlke"), KLocalizedString(), "mw_triad@users.sourceforge.net" );
    about.addAuthor( ki18n("Casper Boemann"),  KLocalizedString(), "cbr@boemann.dk" );
    about.addAuthor( ki18n("Fredrik Höglund"), KLocalizedString(), "<fredrik@kde.org>" );
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
