//////////////////////////////////////////////////////////////////////////////
// ionstyleconfig.h
// -------------------
// Config dialog for Ion widget style
// -------------------
// Copyright (c) 2007 Matthew Woehlke <mw_triad@sourceforge.net>
//
//   Based on the Phase style:
//     Copyright (c) 2004 David Johnson <david@usermode.org>
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

#ifndef IONSTYLECONFIG_H
#define IONSTYLECONFIG_H

#include "ui_styledialog.h"

class KConfig;

class IonStyleConfig : public Ui::StyleDialog
{
    Q_OBJECT
public:
    IonStyleConfig(QWidget* parent);
    virtual ~IonStyleConfig();

signals:
    void changed(bool);

public slots:
    void save();
    void defaults();

protected slots:
    void updateChanged();

private:
    bool oldhlfocus;
    bool oldembossed;
    bool oldgradients;
    int  oldhighlights;
};

#endif // IONSTYLECONFIG_H
