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

using namespace Qt;

//////////////////////////////////////////////////////////////////////////////
// IonStyleConfig Class                                                     //
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// IonStyleConfig()
// ----------------
// Constructor

IonStyleConfig::IonStyleConfig(QWidget* parent) : QObject(parent)
{
    ui.setupUi(parent);

    KGlobal::locale()->insertCatalog("kstyle_ion_config");

    QSettings settings;
    oldhlfocus =
        settings.value("/ionstyle/Settings/hlfocus", true).toBool();
    ui.hlfocus->setChecked(oldhlfocus);

    oldembossed =
        settings.value("/ionstyle/Settings/embossed", true).toBool();
    ui.embossed->setChecked(oldembossed);

    oldgradients =
        settings.value("/ionstyle/Settings/gradients", true).toBool();
    ui.gradients->setChecked(oldgradients);

    oldhighlights =
        settings.value("/ionstyle/Settings/highlights", true).toInt();
    ui.highlights->setValue(oldhighlights);

    // connections
    connect(ui.hlfocus, SIGNAL(toggled(bool)),
            this, SLOT(updateChanged()));
    connect(ui.embossed, SIGNAL(toggled(bool)),
            this, SLOT(updateChanged()));
    connect(ui.gradients, SIGNAL(toggled(bool)),
            this, SLOT(updateChanged()));
    connect(ui.highlights, SIGNAL(valueChanged(int)),
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

    if ((ui.hlfocus->isChecked() != oldhlfocus) ||
        (ui.embossed->isChecked() != oldembossed) ||
        (ui.gradients->isChecked() != oldgradients) ||
        (ui.highlights->value() != oldhighlights)) {
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
    settings.setValue("/ionstyle/Settings/hlfocuss", ui.hlfocus->isChecked());
    settings.setValue("/ionstyle/Settings/embossed", ui.embossed->isChecked());
    settings.setValue("/ionstyle/Settings/gradients", ui.gradients->isChecked());
    settings.setValue("/ionstyle/Settings/highlights", ui.highlights->value());
}

//////////////////////////////////////////////////////////////////////////////
// defaults()
// ----------
// Set to the defaults

void IonStyleConfig::defaults()
{
    ui.hlfocus->setChecked(true);
    ui.embossed->setChecked(true);
    ui.gradients->setChecked(true);
    ui.highlights->setValue(2);
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
