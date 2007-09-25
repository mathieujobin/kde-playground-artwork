/* SUSE2 KWin window decoration
  Copyright (C) 2005 Gerd Fleischer <gerdfleischer@web.de>

  based on the window decoration "Plastik"
  Copyright (C) 2003 Sandro Giessl <ceebx@users.sourceforge.net>

  based on the window decoration "Web":
  Copyright (C) 2001 Rik Hemsley (rikkus) <rik@kde.org>

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public
  License as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; see the file COPYING.  If not, write to
  the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
  Boston, MA  02110-1301  USA.
*/

//#include <kdebug.h>

#include <QGroupBox>
#include <QCheckBox>
#include <QRadioButton>
#include <QButtonGroup>
#include <QSpinBox>
#include <QImage>
#include <QLabel>
#include <QComboBox>
#include <QPixmap>

#include <kconfig.h>
#include <klocale.h>
#include <kglobal.h>
#include <kstandarddirs.h>
#include <kurl.h>
#include <kfiledialog.h>
#include <kfileitem.h>
#include <kcolorbutton.h>

#include "config.h"

SUSE2Config::SUSE2Config(KConfig *, QWidget *parent)
    : QObject(parent), m_config(0), m_dialog(0)
{
    m_parent = parent;
    // create the configuration object
    m_config = new KConfig("kwinSUSE2rc");
    KConfigGroup cg(m_config, "General");

//    KGlobal::locale()->insertCatalog("kwin_SUSE2");

    // create and show the configuration dialog
    m_dialog = new ConfigDialog(parent);

    alignGroup = new QButtonGroup();
    QList<QRadioButton *> alignButtons = m_dialog->titleAlign->findChildren<QRadioButton *>();
    foreach (QRadioButton *button, alignButtons) alignGroup->addButton(button);

    roundGroup = new QButtonGroup();
    QList<QRadioButton *> roundButtons = m_dialog->roundedCorners->findChildren<QRadioButton *>();
    foreach (QRadioButton *button, roundButtons) roundGroup->addButton(button);

    m_dialog->show();

    // load the configuration
    load(cg);
    toggleIconSettings(m_dialog->useTitleProps->isChecked());

    // setup the connections
    connect(alignGroup, SIGNAL(buttonClicked(int)), SIGNAL(changed()));
    connect(roundGroup, SIGNAL(buttonClicked(int)), SIGNAL(changed()));
    connect(m_dialog->menuClose, SIGNAL(toggled(bool)), SIGNAL(changed()));
    connect(m_dialog->titleShadow, SIGNAL(toggled(bool)), SIGNAL(changed()));
    connect(m_dialog->addSpace, SIGNAL(valueChanged(int)), SIGNAL(changed()));
    connect(m_dialog->titlebarStyle, SIGNAL(activated(int)), SIGNAL(changed()));

    connect(m_dialog->buttonType, SIGNAL(activated(int)), SIGNAL(changed()));
    connect(m_dialog->customColors, SIGNAL(toggled(bool)), SIGNAL(changed()));
    connect(m_dialog->useTitleProps, SIGNAL(toggled(bool)), SIGNAL(changed()));
    connect(m_dialog->animateButtons, SIGNAL(toggled(bool)), SIGNAL(changed()));
    connect(m_dialog->redCloseButton, SIGNAL(toggled(bool)), SIGNAL(changed()));
    connect(m_dialog->iconSize, SIGNAL(valueChanged(int)), SIGNAL(changed()));
    connect(m_dialog->customIconColors, SIGNAL(toggled(bool)), SIGNAL(changed()));
    connect(m_dialog->activeFgColor, SIGNAL(changed(const QColor &)), SIGNAL(changed()));
    connect(m_dialog->activeBgColor, SIGNAL(changed(const QColor &)), SIGNAL(changed()));
    connect(m_dialog->inactiveFgColor, SIGNAL(changed(const QColor &)), SIGNAL(changed()));
    connect(m_dialog->inactiveBgColor, SIGNAL(changed(const QColor &)), SIGNAL(changed()));
    connect(m_dialog->iconShadow, SIGNAL(toggled(bool)), SIGNAL(changed()));

    connect(m_dialog->titleBarLogo, SIGNAL(toggled(bool)), SIGNAL(changed()));
    connect(m_dialog->titleBarLogoOffset, SIGNAL(valueChanged(int)), SIGNAL(changed()));

    connect(m_dialog->useTitleProps, SIGNAL(toggled(bool)), SLOT(toggleIconSettings(bool)));
    connect(m_dialog->selectButton, SIGNAL(clicked()), SLOT(selectImage()));
}

SUSE2Config::~SUSE2Config()
{
    if (m_dialog) delete m_dialog;
    if (m_config) delete m_config;
}

void SUSE2Config::load(const KConfigGroup &)
{
    KConfigGroup configGroup(m_config, "General");

    QString alignValue = configGroup.readEntry("TitleAlignment", "AlignLeft");
    QRadioButton *alignButton = m_dialog->titleAlign->findChild<QRadioButton *>(alignValue.toLatin1());
    if (alignButton) alignButton->setChecked(true);

    QString roundValue = configGroup.readEntry("RoundedCorners", "NotMaximized");
    QRadioButton *roundButton = m_dialog->roundedCorners->findChild<QRadioButton *>(roundValue.toLatin1());
    if (roundButton) roundButton->setChecked(true);

    bool closeMenu = configGroup.readEntry("CloseOnMenuDoubleClick", false);
    m_dialog->menuClose->setChecked(closeMenu);

    bool titleShadow = configGroup.readEntry("TitleShadow", true);
    m_dialog->titleShadow->setChecked(titleShadow);

    int addSpace = configGroup.readEntry("AddSpace", 4);
    m_dialog->addSpace->setValue(addSpace);

    int titlebarStyle = configGroup.readEntry("TitleBarStyle", 0);
    m_dialog->titlebarStyle->setCurrentIndex(titlebarStyle);

    int titleButtonType = configGroup.readEntry("TitleBarButtonType", 2);
    m_dialog->buttonType->setCurrentIndex(titleButtonType);

    bool customColors = configGroup.readEntry("CustomColors", false);
    m_dialog->customColors->setChecked(customColors);

    bool useTitleProps = configGroup.readEntry("UseTitleProps", true);
    m_dialog->useTitleProps->setChecked(useTitleProps);

    bool animateButtons = configGroup.readEntry("AnimateButtons", true);
    m_dialog->animateButtons->setChecked(animateButtons);

    bool redCloseButton = configGroup.readEntry("RedCloseButton", false);
    m_dialog->redCloseButton->setChecked(redCloseButton);

    int iconSize = configGroup.readEntry("IconSize", 45);
    m_dialog->iconSize->setValue(iconSize);

    bool customIconColors = configGroup.readEntry("CustomIconColors", false);
    m_dialog->customIconColors->setChecked(customIconColors);

    QColor afgcolor = QColor(10, 20, 40);
    QColor aFgColor = configGroup.readEntry("AFgColor", afgcolor);
    m_dialog->activeFgColor->setColor(aFgColor);

    QColor abgcolor = QColor(210, 220, 240);
    QColor aBgColor = configGroup.readEntry("ABgColor", abgcolor);
    m_dialog->activeBgColor->setColor(aBgColor);

    QColor ifgcolor = QColor(40, 40, 40);
    QColor iFgColor = configGroup.readEntry("IFgColor", ifgcolor);
    m_dialog->inactiveFgColor->setColor(iFgColor);

    QColor ibgcolor = QColor(240, 240, 240);
    QColor iBgColor = configGroup.readEntry("AFgColor", ibgcolor);
    m_dialog->inactiveBgColor->setColor(iBgColor);

    bool iconShadow = configGroup.readEntry("IconShadow", true);
    m_dialog->iconShadow->setChecked(iconShadow);

    bool titleBarLogo = configGroup.readEntry("TitleBarLogo", false);
    m_dialog->titleBarLogo->setChecked(titleBarLogo);

    int titleBarLogoOffset = configGroup.readEntry("TitleBarLogoOffset", 3);
    m_dialog->titleBarLogoOffset->setValue(titleBarLogoOffset);

    QString titleBarImage = KStandardDirs::locate("data", "kwin/pics/titlebar_decor.png");
    titlebarLogoURL = configGroup.readEntry("TitleBarLogoURL", titleBarImage);
    QImage tmpLogo = QImage::QImage(titlebarLogoURL);
    m_dialog->logoImage->setPixmap(QPixmap::fromImage(tmpLogo.scaledToHeight(20, Qt::SmoothTransformation)));
}

void SUSE2Config::save(KConfigGroup &) const
{
    KConfigGroup configGroup(m_config, "General");

    QRadioButton *alignButton = (QRadioButton *)alignGroup->checkedButton();
    if (alignButton) configGroup.writeEntry("TitleAlignment", QString(alignButton->objectName()));
    QRadioButton *roundButton = (QRadioButton *)roundGroup->checkedButton();
    if (roundButton) configGroup.writeEntry("RoundedCorners", QString(roundButton->objectName()));
    configGroup.writeEntry("CloseOnMenuDoubleClick", m_dialog->menuClose->isChecked());
    configGroup.writeEntry("TitleShadow", m_dialog->titleShadow->isChecked());
    configGroup.writeEntry("AddSpace", m_dialog->addSpace->value());
    configGroup.writeEntry("TitleBarStyle", m_dialog->titlebarStyle->currentIndex());

    configGroup.writeEntry("TitleBarButtonType", m_dialog->buttonType->currentIndex());
    configGroup.writeEntry("CustomColors", m_dialog->customColors->isChecked());
    configGroup.writeEntry("UseTitleProps", m_dialog->useTitleProps->isChecked());
    configGroup.writeEntry("AnimateButtons", m_dialog->animateButtons->isChecked());
    configGroup.writeEntry("RedCloseButton", m_dialog->redCloseButton->isChecked());
    configGroup.writeEntry("IconSize", m_dialog->iconSize->value());
    configGroup.writeEntry("CustomIconColors", m_dialog->customIconColors->isChecked());
    configGroup.writeEntry("AFgColor", m_dialog->activeFgColor->color());
    configGroup.writeEntry("ABgColor", m_dialog->activeBgColor->color());
    configGroup.writeEntry("IFgColor", m_dialog->inactiveFgColor->color());
    configGroup.writeEntry("IBgColor", m_dialog->inactiveBgColor->color());
    configGroup.writeEntry("IconShadow", m_dialog->iconShadow->isChecked());

    configGroup.writeEntry("TitleBarLogo", m_dialog->titleBarLogo->isChecked());
    configGroup.writeEntry("TitleBarLogoOffset", m_dialog->titleBarLogoOffset->value());
    configGroup.writeEntry("TitleBarLogoURL", titlebarLogoURL);
    m_config->sync();
}

void SUSE2Config::defaults()
{
    QRadioButton *alignButton = m_dialog->titleAlign->findChild<QRadioButton *>("AlignLeft");
    if (alignButton) alignButton->setChecked(true);
    QRadioButton *roundButton = m_dialog->roundedCorners->findChild<QRadioButton *>("NotMaximized");
    if (roundButton) roundButton->setChecked(true);
    m_dialog->menuClose->setChecked(false);
    m_dialog->titleShadow->setChecked(true);
    m_dialog->addSpace->setValue(4);
    m_dialog->titlebarStyle->setCurrentIndex(0);

    m_dialog->buttonType->setCurrentIndex(2);
    m_dialog->customColors->setChecked(false);
    m_dialog->useTitleProps->setChecked(true);
    m_dialog->animateButtons->setChecked(true);
    m_dialog->redCloseButton->setChecked(false);
    m_dialog->iconSize->setValue(45);
    m_dialog->customIconColors->setChecked(false);
    m_dialog->activeFgColor->setColor(QColor(10, 20, 40));
    m_dialog->activeBgColor->setColor(QColor(210, 220, 240));
    m_dialog->inactiveFgColor->setColor(QColor(40, 40, 40));
    m_dialog->inactiveBgColor->setColor(QColor(240, 240, 240));
    m_dialog->iconShadow->setChecked(true);

    m_dialog->titleBarLogo->setChecked(false);
    m_dialog->titleBarLogoOffset->setValue(3);
    titlebarLogoURL = KStandardDirs::locate("data", "kwin/pics/titlebar_decor.png");
    QImage tmpLogo = QImage::QImage(titlebarLogoURL);
    m_dialog->logoImage->setPixmap(QPixmap::fromImage(tmpLogo.scaledToHeight(20, Qt::SmoothTransformation)));
}

void SUSE2Config::toggleIconSettings(bool checked) const
{
    m_dialog->customIconColors->setEnabled(!checked);
    m_dialog->activeLbl->setEnabled(!checked);
    m_dialog->activeDownLbl->setEnabled(!checked);
    m_dialog->inactiveLbl->setEnabled(!checked);
    m_dialog->inactiveDownLbl->setEnabled(!checked);
    m_dialog->activeFgColor->setEnabled(!checked);
    m_dialog->activeBgColor->setEnabled(!checked);
    m_dialog->inactiveFgColor->setEnabled(!checked);
    m_dialog->inactiveBgColor->setEnabled(!checked);
    m_dialog->iconShadow->setEnabled(!checked);
}

void SUSE2Config::selectImage()
{
    KUrl logoURL = KFileDialog::getImageOpenUrl(KUrl::fromPath(titlebarLogoURL), m_parent, i18n("Select Logo Image"));
    KFileItem tmpFileItem = KFileItem(KFileItem::Unknown, KFileItem::Unknown, logoURL);
    if (!logoURL.isEmpty() && tmpFileItem.isFile() && tmpFileItem.isReadable()) {
        titlebarLogoURL = logoURL.path();
        QImage tmpLogo = QImage::QImage(titlebarLogoURL);
        m_dialog->logoImage->setPixmap(QPixmap::fromImage(tmpLogo.scaledToHeight(20, Qt::SmoothTransformation)));
        emit changed();
    }
}

//////////////////////////////////////////////////////////////////////////////
// Plugin Stuff                                                             //
//////////////////////////////////////////////////////////////////////////////

extern "C"
{
    KDE_EXPORT QObject *allocate_config(KConfig *config, QWidget *parent) {
        return (new SUSE2Config(config, parent));
    }
}

#include "config.moc"

// kate: space-indent on; indent-width 4; replace-tabs on;
