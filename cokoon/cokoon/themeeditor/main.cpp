#include <QApplication>
#include <QDebug>

#include <KDE/KAboutData>
#include <KDE/KCmdLineArgs>
#include <KDE/KApplication>

#include "specificationstateselector.h"
#include "previewvariableeditor.h"
#include "themeitemview.h"
#include "thememodel.h"
#include "mainwindow.h"

using namespace Cokoon;

int main(int argc, char **argv) {
    KAboutData aboutData("cokoonthemeeditor", 0, ki18n("CokoonThemeEditor"), "0.1", ki18n("A graphical editor for Cokoon themes"), KAboutData::License_GPL, ki18n("(c) 2007  Sandro Giessl"));
    KCmdLineArgs::init(argc, argv, &aboutData);
    KCmdLineOptions options;
    options.add("+[File]", ki18n("Theme file to open"), "");
    KCmdLineArgs::addCmdLineOptions( options ); // Add our own options.

    KApplication app;

    MainWindow *mw = new MainWindow();
    mw->show();

    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

    if (args->count()>=1) {
        mw->loadFile(args->arg(0));
    }

    return app.exec();
}
