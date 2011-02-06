#ifndef oxygentransparencyconfig_h
#define oxygentransparencyconfig_h
/*
Copyright (c) 2010 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
Copyright (C) 2003 Sandro Giessl <ceebx@users.sourceforge.net>

based on the Keramik configuration dialog:
Copyright (c) 2003 Maksim Orlovich <maksim.orlovich@kdemail.net>

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

#include "ui_oxygentransparencyconfig.h"

#include <QtCore/QStringList>

namespace Oxygen
{
    class AnimationConfigWidget;

    class TransparencyConfig: public QWidget, public Ui::OxygenTransparencyConfig
    {

        Q_OBJECT

        public:

        //! constructor
        TransparencyConfig(QWidget* parent);

        //! destructor
        virtual ~TransparencyConfig( void )
        {}

        Q_SIGNALS:

        //! emmited whenever one option is changed.
        void changed(bool);

        public Q_SLOTS:

        //! save current state
        void save( void );

        //! restore all default values
        void defaults( void );

        //Everything below this is internal.

        //! reset to saved configuration
        void reset( void );

        protected Q_SLOTS:

        //! update modified state when option is checked/unchecked
        void updateChanged( void );

        //! edit black list
        virtual void editExceptions( void );

        protected:

        //! load setup from config data
        void load( void );

        private:

        //! grey listed applications
        QStringList _opacityGreyList;

        //! black listed applications
        QStringList _opacityBlackList;

    };

}
#endif
