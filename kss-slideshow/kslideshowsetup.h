/*
 * Copyright 1999 Stefan Taferner <taferner@kde.org>
 * Copyright 2001 Martin R. Jones <mjones@kde.org>
 * Copyright 2003 Sven Leiber <s.leiber@web.de>
 * Copyright 2009 Alex Merry <alex.merry@kdemail.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301  USA
 */


#ifndef KSLIDESHOWSETUP_H
#define KSLIDESHOWSETUP_H

#include <KDialog>

class kSlideShowSaver;
class KUrl;
class QString;
class SlideShowCfg;
class KAboutData;

class kSlideShowSetup : public KDialog
{
    Q_OBJECT
public:
    kSlideShowSetup ( KAboutData *aboutData, QWidget *parent = 0 );
    ~kSlideShowSetup();

protected:
    void readSettings();

protected slots:
    void slotOk();
    void slotHelp();
    void writeSettings();
    void slotDirSelected ( const QString &where );
    void slotDirSelected ( const KUrl & );
private:
    kSlideShowSaver *mSaver;
    SlideShowCfg *cfg;
    KAboutData *mAboutData;
};

#endif /* KSLIDESHOWSETUP_H */

