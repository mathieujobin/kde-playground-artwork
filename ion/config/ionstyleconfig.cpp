//////////////////////////////////////////////////////////////////////////////
// ionstyleconfig.cpp
// -------------------
// Config dialog for Ion widget style
// -------------------
// Copyright (c) 2007 Matthew Woehlke <mw_triad@sourceforge.net>
//
//   Based on the Phase style:
//     Copyright (c) 2004 David Johnson <david@usermode.org>
//
// Please see the header file for copyright and license information.
//////////////////////////////////////////////////////////////////////////////

#include <QtCore/QSettings>
#include <KLocale>
#include <KGlobal>

#include "ionstyleconfig.h"

//////////////////////////////////////////////////////////////////////////////
// IonStyleConfig Class                                                     //
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// IonStyleConfig()
// ----------------
// Constructor

IonStyleConfig::IonStyleConfig(QWidget* parent) : Ui::StyleDialog()
{
    setupUi(parent);

    KGlobal::locale()->insertCatalog("kstyle_ion_config");

    QSettings settings;
    oldhlfocus =
        settings.value("/ionstyle/Settings/hlfocus", true).toBool();
    hlfocus->setChecked(oldhlfocus);

    oldembossed =
        settings.value("/ionstyle/Settings/embossed", true).toBool();
    embossed->setChecked(oldembossed);

    oldgradients =
        settings.value("/ionstyle/Settings/gradients", true).toBool();
    gradients->setChecked(oldgradients);

    oldhighlights =
        settings.value("/ionstyle/Settings/highlights", true).toInt();
    highlights->setValue(oldhighlights);

    // connections
    connect(hlfocus, SIGNAL(toggled(bool)),
            this, SLOT(updateChanged()));
    connect(embossed, SIGNAL(toggled(bool)),
            this, SLOT(updateChanged()));
    connect(gradients, SIGNAL(toggled(bool)),
            this, SLOT(updateChanged()));
    connect(highlights, SIGNAL(valueChanged(int)),
            this, SLOT(updateChanged()));
}

//////////////////////////////////////////////////////////////////////////////
// ~IonStyleConfig()
// -----------------
// Destructor

IonStyleConfig::~IonStyleConfig()
{
    KGlobal::locale()->removeCatalog("kstyle_ion_config");
}

//////////////////////////////////////////////////////////////////////////////
// selectionChanged()
// ------------------
// Selection has changed

void IonStyleConfig::updateChanged()
{
    bool update = false;

    if ((hlfocus->isChecked() != oldhlfocus) ||
        (embossed->isChecked() != oldembossed) ||
        (gradients->isChecked() != oldgradients) ||
        (highlights->value() != oldhighlights)) {
        update = true;
    }

    emit changed(update);
}

//////////////////////////////////////////////////////////////////////////////
// save()
// ------
// Save the settings

void IonStyleConfig::save()
{
    QSettings settings;
    settings.setValue("/ionstyle/Settings/hlfocuss", hlfocus->isChecked());
    settings.setValue("/ionstyle/Settings/embossed", embossed->isChecked());
    settings.setValue("/ionstyle/Settings/gradients", gradients->isChecked());
    settings.setValue("/ionstyle/Settings/highlights", highlights->value());
}

//////////////////////////////////////////////////////////////////////////////
// defaults()
// ----------
// Set to the defaults

void IonStyleConfig::defaults()
{
    hlfocus->setChecked(true);
    embossed->setChecked(true);
    gradients->setChecked(true);
    highlights->setValue(2);
}

//////////////////////////////////////////////////////////////////////////////
// Plugin Stuff                                                             //
//////////////////////////////////////////////////////////////////////////////

extern "C"
{
    KDE_EXPORT QObject* allocate_kstyle_config(QWidget* parent) {
        return(new IonStyleConfig(parent));
    }
}

#include "ionstyleconfig.moc"
