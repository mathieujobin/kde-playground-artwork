#include <QtGui/QScrollBar>
#include <KApplication>
#include <KCmdLineArgs>
#include <KAboutData>

#include "ui_scrollbar.h"

class Widget : public QWidget
{
    Q_OBJECT
public:
    Widget(QWidget *parent=0) : QWidget(parent)
    {
        Ui::scrollbars ui;
        ui.setupUi(this);
    }
};

#include "scrollbardemo.moc"

int main(int argc, char **argv)
{
    KAboutData about("oxyscrollbartest", 0, ki18n("oxyscrollbartest"), "0.1",
                     ki18n("Oxygen style scrollbar test application"),
                     KAboutData::License_GPL, ki18n("(C) 2007 Matthew Woehlke"));
    about.addAuthor( ki18n("Matthew Woehlke"), KLocalizedString(), "mw_triad@users.sourceforge.net" );
    KCmdLineArgs::init(argc, argv, &about);
    KApplication app;

    Widget w;
    w.show();

    return app.exec();
}
