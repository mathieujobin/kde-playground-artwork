/***************************************************************************
 *   Copyright (C) 2006-2007 by Thomas Lübking                             *
 *   thomas.luebking@web.de                                                *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <QtDebug>
#include "oxrender.h"
// #include <private/qwidget_p.h>

bool TabAnimInfo::eventFilter( QObject* object, QEvent* event )
{
   if (event->type() != QEvent::Paint || !animStep)
      return false;
   QPainter p((QWidget*)object);
   p.drawPixmap(0,0, tabPix[2]);
   p.end();
   return true;
}

// QPixmap::grabWidget(.) currently fails on the background offset,
// so we use our own implementation
QPixmap grabWidget(QWidget * root)
{
    if (!root)
        return QPixmap();
   
   QPixmap pix(root->size());
   QPoint zero(0,0);
   QPainter p(&pix);
   const QBrush bg = root->palette().brush(root->backgroundRole());
   if (bg.style() == Qt::TexturePattern)
      p.drawTiledPixmap(root->rect(), bg.texture(), root->mapTo(root->topLevelWidget(), zero));
   else
      p.fillRect(root->rect(), bg);
   p.end();
   
   QList <QWidget*> widgets = root->findChildren<QWidget*>();
   
   // resizing (in case)
   if (root->testAttribute(Qt::WA_PendingResizeEvent) ||
       !root->testAttribute(Qt::WA_WState_Created))
   {
      QResizeEvent e(root->size(), QSize());
      QApplication::sendEvent(root, &e);
   }
   foreach (QWidget *w, widgets)
   {
      if (root->testAttribute(Qt::WA_PendingResizeEvent) ||
         !root->testAttribute(Qt::WA_WState_Created))
      {
         QResizeEvent e(w->size(), QSize());
         QApplication::sendEvent(w, &e);
      }
   }
   
   // painting
   QPainter::setRedirected( root, &pix );
   QPaintEvent e = QPaintEvent(QRect(zero, root->size()));
   QCoreApplication::sendEvent(root, &e);
   QPainter::restoreRedirected(root);
   
   foreach (QWidget *w, widgets)
   {
      if (w->isVisibleTo(root))
      {
         if (w->autoFillBackground())
         {
            const QBrush bg = w->palette().brush(w->backgroundRole());
            QPainter p(&pix);
            QRect wrect = QRect(zero, w->size()).translated(w->mapTo(root, zero));
            if (bg.style() == Qt::TexturePattern)
               p.drawTiledPixmap(wrect, bg.texture(), w->mapTo(root->topLevelWidget(), zero));
            else
               p.fillRect(wrect, bg);
            p.end();
         }
         QPainter::setRedirected( w, &pix, -w->mapTo(root, zero) );
         e = QPaintEvent(QRect(zero, w->size()));
         QCoreApplication::sendEvent(w, &e);
         QPainter::restoreRedirected(w);
      }
   }
   
   return pix;
}

static QMap<QWidget*, uint> progressbars;
static QMap<QTabWidget*, TabAnimInfo*> tabwidgets;
static int activeTabs = 0;

// --- ProgressBars --------------------
void OxygenStyle::updateProgressbars()
{
#if 1
   //Update the registered progressbars.
   QMap<QWidget*, uint>::iterator iter;
   QPainter p;
   QProgressBar *pb;
   QStyleOptionProgressBarV2 *opt = 0L;
   anmiationUpdate = true;
   for (iter = progressbars.begin(); iter != progressbars.end(); iter++)
   {
      if ( !qobject_cast<QProgressBar*>(iter.key()) )
         continue;
      pb = (QProgressBar*)(iter.key());
      if (pb->paintingActive() || !pb->isVisible() || !(pb->value() > pb->minimum()) || !(pb->value() < pb->maximum()))
         continue;
#ifdef Q_WS_X11
      if (!opt)
         opt = new QStyleOptionProgressBarV2();
      opt->palette = pb->palette();
      opt->minimum = pb->minimum();
      opt->maximum = pb->maximum();
      opt->progress = pb->value();
      opt->textAlignment = pb->alignment();
      opt->textVisible = pb->isTextVisible();
      opt->text = pb->text();
      opt->rect = pb->rect();
      opt->state = 0;
      if (pb->isEnabled()) opt->state |= State_Enabled;
      if (pb->hasFocus()) opt->state |= State_HasFocus;
      if (pb->orientation() == Qt::Horizontal) opt->state |= State_Horizontal;
      opt->bottomToTop = pb->textDirection() == QProgressBar::BottomToTop;
      opt->invertedAppearance = pb->invertedAppearance();
      opt->orientation = pb->orientation();
      opt->direction = pb->layoutDirection();
      opt->rect = visualRect(pb->layoutDirection(), pb->rect(), subElementRect( SE_ProgressBarContents, opt, pb ));
#endif
      // we want custom sized rings, depending on the height of the progress
      if (pb->orientation() == Qt::Vertical)
         progressShift = ++iter.value() % (opt->rect.width()<<1);
      else
         progressShift = ++iter.value() % ((opt->rect.height()<<1)-1);
      // i don't want to paint the whole progressbar, as only the progress and maybe the percentage needs an update - saves cpu especially on lack of HW accelerated XRender alpha blending
      // TODO: this is X11 only, find a better way (maybe just repaint the whole widget)
#ifndef Q_WS_X11
#warning not X11, animated progress may be slow
      pb->repaint();
#else
      pb->setAttribute ( Qt::WA_PaintOutsidePaintEvent );
      p.begin(pb);
      drawControl( CE_ProgressBarContents, opt, &p, pb );
      opt->rect = visualRect(pb->layoutDirection(), pb->rect(), subElementRect( SE_ProgressBarLabel, opt, pb ));
      drawControl( CE_ProgressBarLabel, opt, &p, pb );
      p.end();
      pb->setAttribute ( Qt::WA_PaintOutsidePaintEvent, false );
#endif
   }
   if (opt)
      delete opt;
   anmiationUpdate = false;
#endif
}

void OxygenStyle::progressbarDestroyed(QObject* obj)
{
   progressbars.remove(static_cast<QWidget*>(obj));
   if (!activeTabs && progressbars.count() == 0)
      timer->stop();
}

// --- TabWidgets --------------------
void OxygenStyle::tabChanged(int index)
{
   QTabWidget* tw = (QTabWidget*)sender();
   QMap<QTabWidget*, TabAnimInfo*>::iterator i = tabwidgets.find(tw);
   if (i == tabwidgets.end()) // this tab isn't handled for some reason?
      return;
   
   TabAnimInfo* tai = i.value();
   
   QWidget *ctw = tw->widget(tai->lastTab);
   tai->lastTab = index;
   if (!ctw)
      return;
   tai->tabPix[0] = /*QPixmap::*/grabWidget(ctw);
   
   ctw = tw->currentWidget();
   if (!ctw)
   {
      tai->tabPix[0] = QPixmap();
      return;
   }
   tai->tabPix[1] = /*QPixmap::*/grabWidget(ctw);
   tai->tabPix[2] = tai->tabPix[0];
   
   tai->animStep = 6;
   
   OXRender::blend(tai->tabPix[1], tai->tabPix[2], 0.1666);
   ctw->parentWidget()->installEventFilter(tai);
   _BLOCKEVENTS_(ctw);
   QList<QWidget*> widgets = ctw->findChildren<QWidget*>();
   foreach(QWidget *widget, widgets)
   {
      _BLOCKEVENTS_(widget);
      if (widget->autoFillBackground())
      {
         tai->autofillingWidgets.append(widget);
         widget->setAutoFillBackground(false);
      }
   }
   ctw->repaint();
   if (!timer->isActive()) timer->start(50);
}

void OxygenStyle::updateTabAnimation()
{
   QMap<QTabWidget*, TabAnimInfo*>::iterator i;
   activeTabs = 0;
   TabAnimInfo* tai;
   QWidget *ctw = 0, *widget = 0; QList<QWidget*> widgets;
   int index;
   for (i = tabwidgets.begin(); i != tabwidgets.end(); i++)
   {
      tai = i.value();
      if (!tai->animStep)
         continue;
      ctw = i.key()->currentWidget();
      if (! --(tai->animStep)) // zero, stop animation
      {
         tai->tabPix[2] =
            tai->tabPix[1] =
            tai->tabPix[0] = QPixmap();
         ctw->parentWidget()->removeEventFilter(tai);
         _UNBLOCKEVENTS_(ctw);
         widgets = ctw->findChildren<QWidget*>();
         ctw->update();
         foreach(widget, widgets)
         {
            index = tai->autofillingWidgets.indexOf(widget);
            if (index != -1)
            {
               tai->autofillingWidgets.removeAt(index);
               widget->setAutoFillBackground(true);
            }
            _UNBLOCKEVENTS_(widget);
            widget->update();
         }
         tai->autofillingWidgets.clear();
         continue;
      }
      ++activeTabs;
      tai->tabPix[2] = tai->tabPix[0];
      OXRender::blend(tai->tabPix[1], tai->tabPix[2], 1.1666-0.1666*tai->animStep);
      ctw->parentWidget()->repaint();
   }
   if (!activeTabs && progressbars.count() == 0)
      timer->stop();
}

void OxygenStyle::tabDestroyed(QObject* obj)
{
   tabwidgets.remove(static_cast<QTabWidget*>(obj));
//    delete tai;
   if (tabwidgets.count() == 0 && progressbars.count() == 0)
      timer->stop();
}

