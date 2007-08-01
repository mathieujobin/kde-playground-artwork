#include <QtGui>
#include <KApplication>
#include <KCmdLineArgs>
#include <KAboutData>
#include <KColorScheme>
#include <KColorUtils>

int r = 224, g = 224, b = 224;

#define NUM_MODES 3
#define MAX_PARAMS 6
int params[NUM_MODES] = { 3, 6, 3 };

const char* knames[NUM_MODES][MAX_PARAMS] = {
    { "H+ Amount", "H- Amount", "R Amount", "(unused)", "(unused)", "(unused)" },
    { "H+ Offset", "H+ Scale", "H- Offset", "H- Scale", "R Offset", "R Scale" },
    { "H+ Scale", "H- Scale", "R Scale", "(unused)", "(unused)", "(unused)" }
};

double k[NUM_MODES][MAX_PARAMS] = {
    { 0.05, 0.11, 0.40 },
    { 0.04, 0.20, 0.04, 0.20, 0.10, 0.60 },
    { 1.0, -0.6, 1.0 }
};

int mode = 2;

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

bool lowThreshold(const QColor& color)
{
    QColor darker = KColorScheme::shade(color, KColorScheme::MidShade, 0.5);
    return KColorUtils::luma(darker) > KColorUtils::luma(color);
}

QColor calcLightColor(const QColor &color)
{
    qreal y = KColorUtils::luma(color);
    switch (mode) {
        case 0:
            return color.lighter(100 + int(100.0 * k[0][2]));
        case 1:
            return KColorUtils::shade(color, k[1][4] + k[1][5]*y);
        default:
            if (lowThreshold(color))
                return KColorScheme::shade(color, KColorScheme::LightShade, 0.0);
            else
                return KColorScheme::shade(color, KColorScheme::LightShade, 0.7 * k[2][2]);
    }
}

QColor calcMidlightColor(const QColor &color)
{
    qreal y = KColorUtils::luma(color);
    switch (mode) {
        case 0:
            return color.lighter(100 + int(100.0 * k[0][0]));
        case 1:
            return KColorUtils::shade(color, k[1][0] + k[1][1]*y);
        default:
            if (lowThreshold(color))
                return KColorScheme::shade(color, KColorScheme::MidlightShade, 0.0);
            else
                return KColorScheme::shade(color, KColorScheme::MidlightShade, 0.7 * k[2][0]);
    }
}

QColor calcMidColor(const QColor &color)
{
    qreal y = KColorUtils::luma(color);
    switch (mode) {
        case 0:
            return color.darker(100 + int(100.0 * k[0][1]));
        case 1:
            return KColorUtils::shade(color, -k[1][2] - k[1][3]*y);
        default:
            if (lowThreshold(color))
                return KColorScheme::shade(color, KColorScheme::MidShade, 0.0);
            else
                return KColorScheme::shade(color, KColorScheme::MidShade, 0.7 * k[2][1]);
    }
}

QPixmap TileCache::verticalGradient(const QColor &color, int height)
{
    quint64 key = (quint64(color.rgba()) << 32) | height | 0x8000;
    QPixmap *pixmap = m_cache.object(key);

    if (!pixmap)
    {
        pixmap = new QPixmap(32, height);

        QLinearGradient gradient(0, 0, 0, height);
        gradient.setColorAt(0.0, calcMidlightColor(color));
        gradient.setColorAt(0.5, color);
        gradient.setColorAt(1.0, calcMidColor(color));

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
        width /= 2;
        pixmap = new QPixmap(width, 64);
        pixmap->fill(QColor(0,0,0,0));
        QColor radialColor = calcLightColor(color);
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
protected:
    friend class Picker;
    QLineEdit *_r[3];
public:
    Widget(QWidget *parent=0) : QWidget(parent) {}

    QSize sizeHint() const { return QSize(600, 600); }
public Q_SLOTS:
    void update()
    {
        TileCache::instance()->clear();
        QWidget::update();
        QColor color(r, g, b);
        _r[0]->setText(calcLightColor(color).name());
        _r[1]->setText(calcMidlightColor(color).name());
        _r[2]->setText(calcMidColor(color).name());
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
        p.fillRect(lowerRect, calcMidColor(color));

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
        w->_r[0] = new QLineEdit();
        w->_r[1] = new QLineEdit();
        w->_r[2] = new QLineEdit();
        w->_r[0]->setReadOnly(true);
        w->_r[1]->setReadOnly(true);
        w->_r[2]->setReadOnly(true);
        l->addWidget(w->_r[0], 3, 0, 1, 2);
        l->addWidget(w->_r[1], 4, 0, 1, 2);
        l->addWidget(w->_r[2], 5, 0, 1, 2);
        setLayout(l);
    }

public slots:
    void   redChanged(int value) { r = value; w->update(); }
    void greenChanged(int value) { g = value; w->update(); }
    void  blueChanged(int value) { b = value; w->update(); }
};
//END Picker

//BEGIN Controller
class Controller : public QWidget
{
    Q_OBJECT
protected:
    QDoubleSpinBox *_s[MAX_PARAMS];
    QLabel *_c[MAX_PARAMS];
    bool _noUpdate;
public:
    Controller(QWidget *parent=0) : QWidget(parent)
    {
        _noUpdate = false;
        QGridLayout *l = new QGridLayout;
        // modes
        QGroupBox *f = new QGroupBox("Mode");
        QButtonGroup *g = new QButtonGroup;
        QRadioButton *m[3];
        m[0] = new QRadioButton("Qt");
        m[1] = new QRadioButton("KCU");
        m[2] = new QRadioButton("KCS");
        QVBoxLayout *v = new QVBoxLayout;
        v->addWidget(m[0]); g->addButton(m[0]); g->setId(m[0], 0);
        v->addWidget(m[1]); g->addButton(m[1]); g->setId(m[1], 1);
        v->addWidget(m[2]); g->addButton(m[2]); g->setId(m[2], 2);
        m[mode]->setChecked(true);
        connect(g, SIGNAL(buttonClicked(int)), this, SLOT(modeChanged(int)));
        f->setLayout(v);
        l->addWidget(f, 0, 0, 1, 2);
        // params
        for (int i=0; i<MAX_PARAMS; i++) {
            _c[i] = new QLabel(knames[mode][i]);
            _s[i] = new QDoubleSpinBox;
            _s[i]->setMinimum(-1.0);
            _s[i]->setMaximum(1.0);
            _s[i]->setDecimals(2);
            _s[i]->setSingleStep(0.01);
            _s[i]->setValue(k[mode][i]);
            l->addWidget(_c[i], i+1, 0);
            l->addWidget(_s[i], i+1, 1);
            connect(_s[i], SIGNAL(valueChanged(double)), this, SLOT(kChanged(double)));
            _c[i]->setEnabled(i < params[mode]);
            _s[i]->setEnabled(i < params[mode]);
        }
        setLayout(l);
    }
public slots:
    void kChanged(double value)
    {
        Q_UNUSED(value);
        if (_noUpdate)
            return;
        for (int i=0; i<MAX_PARAMS; i++)
            k[mode][i] = _s[i]->value();
        w->update();
    }
    void modeChanged(int value)
    {
        _noUpdate = true;
        mode = value;
        for (int i=0; i<MAX_PARAMS; i++) {
            _s[i]->setValue(k[mode][i]);
            _c[i]->setText(knames[mode][i]);
            _c[i]->setEnabled(i < params[mode]);
            _s[i]->setEnabled(i < params[mode]);
        }
        w->update();
        _noUpdate = false;
    }
};
//END Controller

//BEGIN MyLayout
class MyLayout : public QWidget
{
    Q_OBJECT
public:
    MyLayout(QWidget *parent=0) : QWidget(parent)
    {
        w = new Widget; // needed by Picker

        QVBoxLayout *lv = new QVBoxLayout;
        lv->addWidget(new Picker);
        lv->addWidget(new Controller);

        QHBoxLayout *lh = new QHBoxLayout;
        lh->addLayout(lv);
        lh->addWidget(w);
        setLayout(lh);
        w->update();
    }
};
//END MyLayout

#include "bgtest.moc"

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
