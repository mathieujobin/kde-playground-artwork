//////////////////////////////////////////////////////////////////////////////
// oxygenbutton.h
// -------------------
// Oxygen window decoration for KDE.
// -------------------
// Copyright (c) 2006, 2007 Riccardo Iaconelli <ruphy@fsfe.org>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//////////////////////////////////////////////////////////////////////////////

#include "oxygenclient.h"
#include "oxygen.h"
extern "C"
{
KDE_EXPORT KDecorationFactory* create_factory()
{
    return new Oxygen::OxygenFactory();
}
}
namespace Oxygen
{


//////////////////////////////////////////////////////////////////////////////
// OxygenFactory Class                                                     //
//////////////////////////////////////////////////////////////////////////////

bool OxygenFactory::initialized_ = false;
Qt::Alignment OxygenFactory::titlealign_ = Qt::AlignHCenter;

//////////////////////////////////////////////////////////////////////////////
// OxygenFactory()
// ----------------
// Constructor

OxygenFactory::OxygenFactory()
{
    readConfig();
    initialized_ = true;
}

//////////////////////////////////////////////////////////////////////////////
// ~OxygenFactory()
// -----------------
// Destructor

OxygenFactory::~OxygenFactory() { initialized_ = false; }

//////////////////////////////////////////////////////////////////////////////
// createDecoration()
// -----------------
// Create the decoration

KDecoration* OxygenFactory::createDecoration(KDecorationBridge* b)
{
    return new OxygenClient(b, this);
}

//////////////////////////////////////////////////////////////////////////////
// reset()
// -------
// Reset the handler. Returns true if decorations need to be remade, false if
// only a repaint is necessary

bool OxygenFactory::reset(unsigned long changed)
{
    // read in the configuration
    initialized_ = false;
    bool confchange = readConfig();
    initialized_ = true;

    if (confchange ||
        (changed & (SettingDecoration | SettingButtons | SettingBorder))) {
        return true;
    } else {
        resetDecorations(changed);
        return false;
    }
}

//////////////////////////////////////////////////////////////////////////////
// readConfig()
// ------------
// Read in the configuration file

bool OxygenFactory::readConfig()
{
    // create a config object
    KConfig config("kwinexamplerc");
    config.setGroup("General");

    // grab settings
    Qt::Alignment oldalign = titlealign_;
    QString value = config.readEntry("TitleAlignment", "AlignHCenter");
    if (value == "AlignLeft") titlealign_ = Qt::AlignLeft;
    else if (value == "AlignHCenter") titlealign_ = Qt::AlignHCenter;
    else if (value == "AlignRight") titlealign_ = Qt::AlignRight;

    if (oldalign == titlealign_)
        return false;
    else
        return true;
}



} //namespace Oxygen