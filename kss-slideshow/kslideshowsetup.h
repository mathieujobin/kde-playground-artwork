/* Slide Show Screen Saver
 * (C) 1999 Stefan Taferner <taferner@kde.org>
 * (C) 2003 Sven Leiber <s.leiber@web.de>
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

