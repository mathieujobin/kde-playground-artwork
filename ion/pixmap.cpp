/* kate: hl c++; indent-mode cstyle; space-indent on; indent-width 4; replace-tabs on;
  Ion style.
  Copyright (c) 2006 Matthew Woehlke <mw_triad@sourceforge.net>

  Released under the GNU General Public License (GPL) v2.

  Based on the Bluecurve style:
  Copyright (c) 2002 Red Hat, Inc.
  And on the Phase style:
  Copyright (c) 2004 David Johnson

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

#include <math.h>

#include "ionglowstyle.h"

#ifndef MIN
# define MIN(a,b) (a<b?a:b)
#endif

typedef struct {
    double h;
    double l;
    double s;
} cHls;

//////////////////////////////////////////////////////////////////////////////
// normalize()
// ----------
// Clamp value to sane range (0.0 - 1.0 for doubles, 0 - 255 for ints)
static inline double normalize(double a)
{
    return (a < 0.0 ? 0.0 : a > 1.0 ? 1.0 : a);
}

static inline unsigned char normalize(int a)
{
    return (a < 0 ? 0 : a > 255 ? 255 : a);
}

//////////////////////////////////////////////////////////////////////////////
// blend()
// ----------
// Blend between two doubles
static inline double blend(double a, double b, double k)
{
    return a + (b - a) * k;
}

//////////////////////////////////////////////////////////////////////////////
// rgbToHls()
// ----------
// Convert RGB to HLS
static cHls rgbToHls(const QRgb color) {
    double red   = qRed  (color) / 255.0;
    double green = qGreen(color) / 255.0;
    double blue  = qBlue (color) / 255.0;

    double min;
    double max;
    double h, l, s;
    double delta;

    if (red > green) {
        max = (red   > blue ? red   : blue);
        min = (green < blue ? green : blue);
    } else {
        max = (green > blue ? green : blue);
        min = (red   < blue ? red   : blue);
    }

    l = 0.5 * (max + min);
    s = 0.0;
    h = 0.0;

    if (max != min) {
        delta = max - min;

        if (l <= 0.5)
            s = delta / (max + min);
        else
            s = delta / (2.0 - max - min);

        if (red == max)
            h = (green - blue) / delta;
        else if (green == max)
            h = 2.0 + (blue - red) / delta;
        else if (blue == max)
            h = 4.0 + (red - green) / delta;

        h *= 60.0;
        if (h < 0.0)
            h += 360.0;
    }

    cHls result = { h, l, s };
    return result;
}

//////////////////////////////////////////////////////////////////////////////
// hToC()
// ----------
// Get channel from hue
static inline double hToC(double h, double m1, double m2)
{
    while (h > 360.0) h -= 360.0;
    while (h <   0.0) h += 360.0;
    h *= 0.01666666666666666667;

    if (h < 1.0)
        return blend(m1, m2, h);
    if (h < 3.0)
        return m2;
    if (h < 4.0)
        return blend(m1, m2, 4.0 - h);
    return m1;
}


//////////////////////////////////////////////////////////////////////////////
// hlsToRgb()
// ----------
// Convert HLS to RGB
static QRgb hlsToRgb(const cHls &hls) {
    double m1, m2;
    double r, g, b;

    if (hls.l <= 0.5)
        m2 = hls.l * (1.0 + hls.s);
    else
        m2 = hls.l + hls.s * (1.0 - hls.l);
    m1 = 2.0 * hls.l - m2;

    r = hToC(hls.h + 120.0, m1, m2);
    g = hToC(hls.h,         m1, m2);
    b = hToC(hls.h - 120.0, m1, m2);

    return qRgb(normalize((int)(r*255.0)),
                normalize((int)(g*255.0)),
                normalize((int)(b*255.0)));
}

//////////////////////////////////////////////////////////////////////////////
// shade()
// ----------
// Shift color towards black in HLS space (0.0 = black, 1.0 = no change)
QRgb shade(const QRgb color, double lk, double sk)
{
    cHls hls = rgbToHls(color);
    hls.l = normalize(hls.l * lk);
    hls.s = normalize(hls.s * sk);
    return hlsToRgb(hls);
}

QColor shade(const QColor &color, double lk, double sk)
{
    return QColor(shade(color.rgb(), lk, sk));
}

//////////////////////////////////////////////////////////////////////////////
// shade()
// ----------
// Shift color towards white in HLS space (0.0 = white, 1.0 = no change)
QRgb shadei(const QRgb color, double lk, double sk)
{
    cHls hls = rgbToHls(color);
    hls.l = 1.0 - normalize((1.0 - hls.l) * lk);
    hls.s = 1.0 - normalize((1.0 - hls.s) * sk);
    return hlsToRgb(hls);
}

QColor shadei(const QColor &color, double lk, double sk)
{
    return QColor(shadei(color.rgb(), lk, sk));
}

//////////////////////////////////////////////////////////////////////////////
// blend()
// ----------
// Blend between two colors
QRgb blend(const QRgb c1, const QRgb c2, double k)
{
    if (k < 0.0) return c1;
    if (k > 1.0) return c2;

    int r = (int)blend((double)qRed  (c1), (double)qRed  (c2), k);
    int g = (int)blend((double)qGreen(c1), (double)qGreen(c2), k);
    int b = (int)blend((double)qBlue (c1), (double)qBlue (c2), k);
    int a = (int)blend((double)qAlpha(c1), (double)qAlpha(c2), k);

    return qRgba(normalize(r), normalize(g), normalize(b),
                 normalize(a));
}

QColor blend(const QColor &c1, const QColor &c2, double k)
{
    return QColor(blend(c1.rgb(), c2.rgb(), k));
}

//////////////////////////////////////////////////////////////////////////////
// genImage()
// ----------
// Generate alpha+intensity image
QImage *
genImage(
    QRgb color,
    int size,
    unsigned char *alpha,
    unsigned char *intensity,
    QRgb dark,
    QRgb light
        )
{
    int i, j;
    QImage *img = new QImage(size, size, 32);
    img->setAlphaBuffer(true);

    // fill in color
    if (intensity) {
        for (j = 0; j < size; j++) {
            QRgb *line = (QRgb*)img->scanLine(j);
            for (i = 0; i < size; i++) {
                double k = intensity[j*size+i] * 0.0078125;
                if (k > 1.0)
                    line[i] = blend(light, color, 2.0 - k);
                else
                    line[i] = blend(dark, color, k);
            }
        }
    } else {
        img->fill(color);
    }

    // fill in alpha
    for (j = 0; j < size; j++) {
        QRgb *line = (QRgb*)img->scanLine(j);
        for (i = 0; i < size; i++) {
            QRgb c = line[i];
            line[i] = qRgba(qRed(c), qGreen(c), qBlue(c), alpha[j*size+i]);
        }
    }

    // done
    return img;
}

//////////////////////////////////////////////////////////////////////////////
// genGradient()
// ----------
// Generate gradient image
QImage *
genGradient(
    QRgb c1,
    QRgb c2,
    bool horizontal,
    int w, int h,
    double bias
           )
{
    QImage *img = new QImage(w, h, 32);
    img->setAlphaBuffer(true);

    if (horizontal) {
        double k = 1.0 / h;
        int i, j;
        for (i = 0; i < h; i++) {
            QRgb c = blend(c1, c2, ((double)i + bias) * k);
            QRgb *line = (QRgb*)img->scanLine(i);
            for (j = 0; j < w; j++)
                line[j] = c;
        }
    } else {
        double k = 1.0 / w;
        int i, j;
        int n = img->bytesPerLine();
        QRgb *bits = (QRgb*)img->bits();
        // Create first line
        for (j = 0; j < w; j++)
            bits[j] = blend(c1, c2, ((double)j + bias) * k);
        // Copy first line to remaining lines
        for (i = 1; i < h; i++)
            memcpy(img->scanLine(i), bits, n);
    }
    return img;
}
