
#include "config.h"
#include "config.moc"
#include <QApplication>
#include <QSettings>

Config::Config(QWidget *parent) : QDialog(parent)
{
   ui.setupUi(this);
   connect(ui.buttonBox, SIGNAL(accepted()), this, SLOT(save()));
   connect(ui.bgMode, SIGNAL(currentIndexChanged(int)), this, SLOT(bgModeSet(int)));
   generateColorModes(ui.cmProgress);
   generateColorModes(ui.cmTab);
   generateColorModes(ui.cmScroll);
   loadSettings();
}

void Config::bgModeSet(int bgMode)
{
   ui.acceleration->setEnabled(bgMode == 3);
}

void Config::loadSettings()
{
   QSettings settings("Oxygen", "Style");
   settings.beginGroup("Style");
   
   ui.bgMode->setCurrentIndex(settings.value("BackgroundMode", 3).toInt());
   ui.acceleration->setCurrentIndex(settings.value("Acceleration", 0).toInt());
   ui.tabDirection->setCurrentIndex(settings.value("TabWidget3D", 1).toInt());
   ui.tabTransition->setCurrentIndex(settings.value("TabTransition", 2).toInt());
   
   ui.inverseButtons->setChecked(settings.value("InverseButtons",false).toBool());
   ui.roundButtons->setChecked(settings.value("RoundButtons", false).toBool());
   ui.raisedButtons->setChecked(settings.value("RaisedButtons", true).toBool());
   
   ui.cmProgress->setCurrentIndex(ui.cmProgress->findData(settings.value("ProgressColor", QPalette::WindowText).toInt()));
   ui.cmTab->setCurrentIndex(ui.cmTab->findData(settings.value("TabColor", QPalette::Window).toInt()));
   ui.cmScroll->setCurrentIndex(ui.cmScroll->findData(settings.value("ScrollbarColor", QPalette::Window).toInt()));
   settings.endGroup();
}

void Config::save()
{
   QSettings settings("Oxygen", "Style");
   settings.beginGroup("Style");
   settings.setValue("BackgroundMode", ui.bgMode->currentIndex());
   settings.setValue("Acceleration", ui.acceleration->currentIndex());
   settings.setValue("TabWidget3D", ui.tabDirection->currentIndex());
   settings.setValue("TabTransition", ui.tabTransition->currentIndex());
   settings.setValue("InverseButtons", ui.inverseButtons->isChecked());
   settings.setValue("RoundButtons", ui.roundButtons->isChecked());
   settings.setValue("RaisedButtons", ui.raisedButtons->isChecked());
   settings.setValue("ProgressColor", ui.cmProgress->itemData(ui.cmProgress->currentIndex()));
   settings.setValue("TabColor", ui.cmTab->itemData(ui.cmTab->currentIndex()));
   settings.setValue("ScrollbarColor", ui.cmScroll->itemData(ui.cmScroll->currentIndex()));
   settings.endGroup();
}

void Config::generateColorModes(QComboBox *box)
{
   box->clear();
   box->addItem("Window", QPalette::Window);
   box->addItem("Window Text", QPalette::WindowText);
   box->addItem("Base (text editor)", QPalette::Base);
   box->addItem("Text (text editor)", QPalette::Text);
   box->addItem("Button", QPalette::Button);
   box->addItem("Button Text", QPalette::ButtonText);
   box->addItem("Highlight", QPalette::Highlight);
   box->addItem("Highlighted Text", QPalette::HighlightedText);
}


int main(int argc, char *argv[])
{
   QApplication app(argc, argv);
   Config *window = new Config;
   window->show();
   return app.exec();
}
