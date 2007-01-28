/*
    Copyright (C) 2007 Fredrik Höglund <fredrik@kde.org>

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions
    are met:

    1. Redistributions of source code must retain the above copyright
       notice, this list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright
       notice, this list of conditions and the following disclaimer in the
       documentation and/or other materials provided with the distribution.

    THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
    IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
    OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
    IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
    INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
    NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
    THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
    THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include <QImage>
#include <QSvgRenderer>
#include <QPainter>
#include <QCursor>
#include <QFile>
#include <QDebug>

#include <X11/Xlib.h>
#include <X11/Xcursor/Xcursor.h>


static Cursor createImageCursor(Display *dpy, QImage &image, int size = 24)
{
    if (image.format() != QImage::Format_ARGB32_Premultiplied)
        image = image.convertToFormat(QImage::Format_ARGB32_Premultiplied);

    XcursorImage cursor;
    cursor.version = XCURSOR_IMAGE_VERSION;
    cursor.size    = size;
    cursor.width   = image.width();
    cursor.height  = image.height();
    cursor.xhot    = 0;
    cursor.yhot    = 0;
    cursor.delay   = 0;
    cursor.pixels  = reinterpret_cast<XcursorPixel*>(image.bits());

    return XcursorImageLoadCursor(dpy, &cursor);
}


static Cursor createImageCursor(Display *dpy, const QString &fileName)
{
    QImage image(fileName);

    return createImageCursor(dpy, image);
}


static Cursor createSvgCursor(Display *dpy, const QString &fileName, int size)
{
    QSvgRenderer renderer(fileName);

    QSize imageSize = (size == -1 ?
        renderer.defaultSize() : QSize(size, size));

    QImage image(imageSize, QImage::Format_ARGB32_Premultiplied);

    QPainter painter;
    painter.begin(&image);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setCompositionMode(QPainter::CompositionMode_Source);
    painter.fillRect(image.rect(), Qt::transparent);
    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
    renderer.render(&painter);
    painter.end();

    return createImageCursor(dpy, image);
}


static Cursor createX11Cursor(Display *dpy, const QString &fileName, int size)
{
    if (size == -1)
        size = XcursorGetDefaultSize(dpy);

    XcursorImages *images = XcursorFilenameLoadImages(QFile::encodeName(fileName), size);
    Cursor cursor = XcursorImagesLoadCursor(dpy, images);
    XcursorImagesDestroy(images);

    return cursor;
}


static void previewCursor(Display *dpy, Cursor cursor)
{
    XSetWindowAttributes attr;
    attr.override_redirect = true;
    attr.cursor = cursor;

    int screen = DefaultScreen(dpy);
    Window window = XCreateWindow(dpy, RootWindow(dpy, screen), 0, 0,
                                  DisplayWidth(dpy, screen), DisplayHeight(dpy, screen),
                                  0, CopyFromParent, InputOnly, CopyFromParent,
                                  CWOverrideRedirect | CWCursor, &attr);

    XSelectInput(dpy, window, ButtonPressMask | ButtonReleaseMask);
    XMapWindow(dpy, window);

    XEvent event;
    while (XNextEvent(dpy, &event))
    {
        switch (event.type)
        {
            case ButtonPress:
            case ButtonRelease:
                return;
        }
    }
}


int main(int argc, char **argv)
{
    QString fileName = (argc > 1) ? argv[1] : QString();
    int size = (argc > 2) ? QString(argv[2]).toInt() : -1;

    if (fileName.isEmpty())
    {
        qDebug("Usage: testcursorfile <svg|png|xcur> [size]");
        return 0;
    }

    Display *dpy = XOpenDisplay(NULL);
    Cursor cursor;

    QString extension = fileName.right(4).toLower();

    if (extension == QLatin1String(".png"))
        cursor = createImageCursor(dpy, fileName);
    else if (extension == QLatin1String(".svg"))
        cursor = createSvgCursor(dpy, fileName, size);
    else
        cursor = createX11Cursor(dpy, fileName, size);

    if (!cursor)
        qFatal("Failed to load cursor.");

    previewCursor(dpy, cursor);

    return 0;
}
