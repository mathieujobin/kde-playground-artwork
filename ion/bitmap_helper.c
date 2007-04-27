/* kate: hl c; indent-mode cstyle; space-indent on; indent-width 4; replace-tabs on;
  Ion style.
  Copyright (c) 2006 Matthew Woehlke <mw_triad@sourceforge.net>

  Released under the GNU General Public License (GPL) v2.

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

/*
 * This file is a helper used to create the circle pixmap data for bitmaps.h.
 * It is not built by 'cmake/make', but there is no need to build it unless you
 * want to play around with making bitmaps like those in bitmap.h.
 */

#include <stdio.h>
#include <math.h>

#define SIZE 15

#define TRAV_DATA \
         int i, j; \
         for (j = 0; j < SIZE; j++) \
         for (i = 0; i < SIZE; i++)
#define TINDEX ((j*SIZE)+i)

#define PI 3.1415926535897932384626433832795
#define PI_INV 0.31830988618379067153776752674503

unsigned char d1[SIZE*SIZE];
unsigned char d2[SIZE*SIZE];

inline float _f(unsigned char c) {
    return (float)c / 255.0;
}

inline unsigned char _i(float f) {
    int n = (int)(f * 255.0);
    return (0 > n ? 0 : n > 255 ? 255 : n);
}

/* Multiply two data buffers */
void mult_data(double a, double b) {
    TRAV_DATA {
        float a1 = (1.0 - a) + (a * _f(d1[TINDEX]));
        float a2 = (1.0 - b) + (b * _f(d2[TINDEX]));
        d1[TINDEX] = _i(a1 * a2);
    }
}

/* Print data */
void print_data() {
    int i, j;
    for (j = 0; j < SIZE; j++) {
        for (i = 0; i < SIZE; i++)
            printf("0x%02x,", d1[TINDEX]);
        printf("\n");
    }
}

/* Make basic circle; AA'd edge, solid inside */
void make_data1(unsigned char* data, int x, int y, double r) {
    TRAV_DATA {
        int tx = i - x, ty = j - y;
        double tr = sqrt(tx*tx + ty*ty);
        data[TINDEX] = _i(r - tr);
    }
}

/* Make radial blend */
void make_data2(unsigned char* data, int x, int y,
                double l, double u, double b) {
    double m = PI / (u - l);
    TRAV_DATA {
        int tx = i - x, ty = j - y;
        double tr = sqrt(tx*tx + ty*ty);
        double ta = acos((double)tx/tr) * PI_INV * (j > y ? -1.0 : 1.0) - 0.5;
        while (ta < b) ta += 2.0;
        while (ta > b+2.0) ta -= 2.0;
        data[TINDEX] = _i(cos((ta - l) * m));
    }
}

/* Entry */
int main() {
    int cx = SIZE/2, cy = cx;
    double mr = ceil(SIZE * 0.5);

#if 0
    printf("\n// outline\n");
    make_data1(d1, cx, cy, mr - 1.0);
    print_data();
#endif

#if 0
    printf("\n// non-embossed fill\n");
    make_data1(d1, cx, cy, mr - 2.2);
    make_data1(d2, cx+1, cx+1, mr - 2.2);
    mult_data(1.0, 0.5);
    print_data();
#endif

#if 1
    printf("\n// embossed mid alpha\n");
    make_data1(d1, cx, cy, mr);
    make_data2(d2, cx, cy, 0.25, 1.25, 0.0);
    mult_data(1.0, 1.0);
    print_data();
    printf("\n// embossed light alpha\n");
    make_data1(d1, cx, cy, mr);
    make_data2(d2, cx, cy, -0.75, 0.25, -1.0);
    mult_data(1.0, 1.0);
    print_data();
#endif

#if 0
    printf("\n// embossed fill\n");
    make_data1(d1, cx, cy, mr - 2.2);
    print_data();
#endif

    return 0;
}
