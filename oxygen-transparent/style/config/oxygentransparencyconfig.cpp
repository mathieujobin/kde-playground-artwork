/*
Copyright 2010 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
Copyright 2009 Matthew Woehlke <mw.triad@users.sourceforge.net>
Copyright 2009 Long Huynh Huu <long.upcase@googlemail.com>
Copyright 2003 Sandro Giessl <ceebx@users.sourceforge.net>

originally based on the Keramik configuration dialog:
Copyright 2003 Maksim Orlovich <maksim.orlovich@kdemail.net>

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.

*/

#include "oxygentransparencyconfig.h"
#include "oxygentransparencyconfig.moc"
#include "oxygenblacklistdialog.h"
#include "oxygentransparencyconfigdata.h"

#include <QtCore/QSharedPointer>
#include <QtDBus/QDBusMessage>
#include <QtDBus/QDBusConnection>

#include <KGlobal>
#include <KLocale>
#include <KSharedConfig>
#include <KConfigGroup>
#include <kdemacros.h>
#include <KDialog>

extern "C"
{
    KDE_EXPORT QWidget* allocate_kstyle_config(QWidget* parent)
    {
        KGlobal::locale()->insertCatalog("kstyle_config");
        return new Oxygen::TransparencyConfig(parent);
    }
}

namespace Oxygen
{

    //__________________________________________________________________
    TransparencyConfig::TransparencyConfig(QWidget* parent):
        QWidget(parent)
    {
        KGlobal::locale()->insertCatalog("kstyle_config");

        setupUi(this);

        // exceptions
        connect( _exceptionsButton, SIGNAL( clicked( void ) ), SLOT( editExceptions( void ) ) );
        connect( _opacityFromStyle, SIGNAL( toggled( bool ) ), _decoBackgroundOpacityWidget, SLOT( setDisabled( bool ) ) );

        // load setup from configData
        load();

        connect( _backgroundOpacity, SIGNAL( valueChanged( int ) ), SLOT( updateChanged( void ) ) );
        connect( _decoBackgroundOpacity, SIGNAL( valueChanged( int ) ), SLOT( updateChanged( void ) ) );
        connect( _opacityFromStyle, SIGNAL( toggled(bool) ), SLOT( updateChanged() ) );

    }

    //__________________________________________________________________
    void TransparencyConfig::save( void )
    {
        TransparencyConfigData::setBackgroundOpacity( (_backgroundOpacity->value()*255)/100 );
        TransparencyConfigData::setDecoBackgroundOpacity( (_decoBackgroundOpacity->value()*255)/100 );
        TransparencyConfigData::setOpacityFromStyle( _opacityFromStyle->isChecked() );

        TransparencyConfigData::setOpacityGreyList( _opacityGreyList );
        TransparencyConfigData::setOpacityBlackList( _opacityBlackList );

        TransparencyConfigData::self()->writeConfig();

        // emit dbus signal
        QDBusMessage message( QDBusMessage::createSignal("/OxygenStyle",  "org.kde.Oxygen.Style", "reparseConfiguration") );
        QDBusConnection::sessionBus().send(message);

    }

    //__________________________________________________________________
    void TransparencyConfig::defaults( void )
    {
        TransparencyConfigData::self()->setDefaults();
        load();
    }

    //__________________________________________________________________
    void TransparencyConfig::reset( void )
    {
        TransparencyConfigData::self()->readConfig();
        load();
    }

    //__________________________________________________________________
    void TransparencyConfig::updateChanged()
    {

        bool modified( false );

        // check if any value was modified
        if( _backgroundOpacity->value() != TransparencyConfigData::backgroundOpacity()*100/255 ) modified = true;
        else if( _decoBackgroundOpacity->value() != TransparencyConfigData::decoBackgroundOpacity()*100/255 ) modified = true;
        else if( _opacityFromStyle->isChecked() != TransparencyConfigData::opacityFromStyle() ) modified = true;
        else if( _opacityBlackList != TransparencyConfigData::opacityBlackList() ) modified = true;
        else if( _opacityGreyList != TransparencyConfigData::opacityGreyList() ) modified = true;

        emit changed(modified);

    }

    //__________________________________________________________________
    void TransparencyConfig::load( void )
    {

        _backgroundOpacity->setValue( TransparencyConfigData::backgroundOpacity()*100/255 );
        _decoBackgroundOpacity->setValue( TransparencyConfigData::decoBackgroundOpacity()*100/255 );
        _opacityFromStyle->setChecked( TransparencyConfigData::opacityFromStyle() );

        // black listing
        _opacityGreyList = TransparencyConfigData::opacityGreyList();
        _opacityBlackList = TransparencyConfigData::opacityBlackList();

    }

    //__________________________________________________________________
    void TransparencyConfig::editExceptions( void )
    {
        QSharedPointer<BlackListDialog> dialog( new BlackListDialog( this ) );
        dialog->setLists( _opacityGreyList, _opacityBlackList );
        if( dialog->exec() == QDialog::Rejected ) return;

        _opacityGreyList = dialog->greyList();
        _opacityBlackList = dialog->blackList();
        updateChanged();

    }

}
