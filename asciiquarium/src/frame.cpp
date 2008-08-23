/*
 * Asciiquarium - Native KDE Screensaver based on the Asciiquarium program
 *    (c) Kirk Baucom <kbaucom@schizoid.com>, which you can find at
 *    http://www.robobunny.com/projects/asciiquarium/
 *
 * Ported to KDE by Maksim Orlovich <maksim@kde.org> and
 * Michael Pyne <michael.pyne@kdemail.net>.
 *
 * Copyright (c) 2003 Kirk Baucom     <kbaucom@schizoid.com>
 * Copyright (c) 2005 Maksim Orlovich <maksim@kde.org>
 * Copyright (c) 2005, 2008 Michael Pyne <michael.pyne@kdemail.net>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 51
 * Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
 */

#include "frame.h"

#include <kglobalsettings.h>
#include <kdebug.h>

#include <QtCore/QStringList>
#include <QtGui/QImage>
#include <QtGui/QFontMetrics>
#include <QtGui/QPainter>
#include <QtGui/QBitmap>

void Frame::convertDataToPixmap(const Screen *screen)
{
    if(!height() || !width()) {
        // Assume we're not ready to go.
        return;
    }

    int w = screen->cellWidth(), h = screen->cellHeight();
    QPixmap pix(width() * w, height() * h);
    pix.fill(Qt::transparent);

    QPainter p;

    p.begin(&pix);

    p.setFont(KGlobalSettings::fixedFont());
    QFontMetrics fm(p.font());
    int leadHeight = fm.leading() + fm.descent();

    for(unsigned j = 0; j < m_data.count(); ++j) {
        QVector<Screen::Pixel> row = m_data[j];
        if(row.isEmpty())
            continue;

        unsigned first, last;
        for (first = 0; first < row.count() && row[first].letter == ' '; ++first)
            ;

        last = row.count() - 1; // Assume the end is already stripped.

        for(unsigned i = first; i <= last; ++i) {
            if(row[i].letter == m_transparentChar)
                continue;

            p.setPen(row[i].color);
            p.fillRect(i * w, j * h, w, h, Qt::black);
            p.drawText(i * w, j * h + (h - 1 - leadHeight), QChar(row[i].letter));
        }
    }

    p.end();

    QPixmap erase(pix);
    erase.fill(Qt::black);

    m_pixmap = pix;
    m_erasePixmap = erase;

    // Clear m_data to save a wee bit of memory.
    m_data.clear();
}

Frame::Frame (QString text, QString mask, QRgb defaultColor, QChar transparent)
{
    //First, process the pixels.

    QStringList rows = text.split('\n');
    m_height = rows.size();
    m_width  = 0;
    m_transparentChar = transparent;

    foreach(QString curRow, rows) {
        QVector<Screen::Pixel> row;
        int strLen = curRow.length(); // make sure this is cached.

        // ?? is changed to {} for trigraph purposes, change it back.
        curRow.replace("{}", "??");

        for (int pos = 0; pos < strLen; ++pos)
        {
            Screen::Pixel p;
            p.letter = curRow.at(pos).unicode();
            p.color  = defaultColor;
            row.append(p);
        }

        m_width = qMax(m_width, row.size());
        m_data.append(row);
    }

    // Now, the colors.
    QStringList cols = mask.split('\n');
    int y = 0;

    foreach(const QString &curCol, cols) {
        if (y >= m_data.size())
            break;

        for (int pos = 0; pos < curCol.length() && pos < m_data[y].size(); ++pos)
        {
            switch (curCol.at(pos).unicode())
            {
                //Colors stolen from konsole, TEWidget.cpp
                case 'R':
                    m_data[y][pos].color = 0xFF5454;
                    break;
                case 'r':
                    m_data[y][pos].color = 0xB21818;
                    break;
                case 'C':
                    m_data[y][pos].color = 0x54FFFF;
                    break;
                case 'c':
                    m_data[y][pos].color = 0x18B2B2;
                    break;
                case 'Y':
                    m_data[y][pos].color = 0xFFFF54;
                    break;
                case 'y':
                    m_data[y][pos].color = 0xB26818;
                    break;
                case 'G':
                    m_data[y][pos].color = 0x54FF54;
                    break;
                case 'g':
                    m_data[y][pos].color = 0x18B218;
                    break;
                case 'B':
                    m_data[y][pos].color = 0x5454FF;
                    break;
                case 'b':
                    m_data[y][pos].color = 0x1818B2;
                    break;
                case 'M':
                    m_data[y][pos].color = 0xFF54FF;
                    break;
                case 'm':
                    m_data[y][pos].color = 0xB218B2;
                    break;
                case 'W':
                    m_data[y][pos].color = 0xFFFFFF;
                    break;
                case 'w':
                    m_data[y][pos].color = 0xB2B2B2;
                    break;
                case ' ':
                    break;
                default:
                    kDebug() << "dunno about color code:" << curCol.at(pos);
                    m_data[y][pos].color = 0xFFFFFF;
            }
        }

        ++y;
    } // foreach
}

void Frame::paint(Screen* scr, int x, int y)
{
    if(m_pixmap.isNull())
        convertDataToPixmap(scr);

    scr->updateSpan(x, y, m_pixmap);
}

void Frame::erase(Screen* scr, int x, int y)
{
    if(m_erasePixmap.isNull())
        convertDataToPixmap(scr);

    scr->clearSpan(x, y, m_erasePixmap);
}

// vim: set et ts=8 sw=4:
