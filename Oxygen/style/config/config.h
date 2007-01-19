#ifndef CONFIG_H
#define CONFIG_H

#include "ui_config.h"
#include <QDialog>

class Config : public QDialog
{
   Q_OBJECT
public:
   Config(QWidget *parent = 0L);
private:
   Ui::Config ui;
   void generateColorModes(QComboBox *box);
private slots:
   void bgModeSet(int bgMode);
   void loadSettings();
   void save();
};

#endif
