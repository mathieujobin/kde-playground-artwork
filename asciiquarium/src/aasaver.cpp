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
 * ASCII Art was mostly created by Joan Stark:
 *    http://www.geocities.com/SoHo/7373/
 *
 * the rest was crafted by Kirk Baucom, and all of it was edited by
 * Maksim Orlovich and Michael Pyne to adopt to C++ formatting.
 *
 * Ported to KDE 4 by Michael Pyne.
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

#include "aasaver.h"

#include <QtGui/QDesktopWidget>
#include <QtGui/QApplication>
#include <QtGui/QPainter>
#include <QtGui/QBrush>
#include <QtGui/QPaintEvent>
#include <QtCore/QList>

#include <kdebug.h>
#include <klocale.h>
#include <kconfigdialog.h>

#include <cstdlib>

#include "screen.h"
#include "frame.h"
#include "sprite.h"

#include "AASaverConfig.h"
#include "ui_settingswidget.h"

#define ARRAY_SIZE(arr) sizeof(arr)/sizeof(arr[0])

AASaver *AASaver::m_instance = 0;

AASaver::AASaver(WId id): KScreenSaver(id)
{
    if(m_instance)
        std::abort();

    m_instance = this;

    screen = new Screen(this);

    addEnvironment();
    addCastle();
    addAllSeaweed();
    addAllFish();
    addRandom(screen);

    setAttribute(Qt::WA_OpaquePaintEvent);
    update(rect());
}

AASaver *AASaver::instance()
{
    if(!m_instance)
        std::abort();

    return m_instance;
}

QString AASaver::randColor(QString color_mask)
{
    char colors[] = {'c','C','r','R','y','Y','b','B','g','G','m','M'};
    for (int i = 1; i <= 9; ++i)
    {
        char color = colors[m_randomSequence.getLong(ARRAY_SIZE(colors))];
        color_mask.replace('0' + i, color);
    }
    return color_mask;
}

double AASaver::doubleRand(int max)
{
    return m_randomSequence.getDouble() * max;
}

void AASaver::addCastle()
{
    QString castle_image =
            "               T~~\n"
            "               |\n"
            "              /^\\\n"
            "             /   \\\n"
            " _   _   _  /     \\  _   _   _\n"
            "[ ]_[ ]_[ ]/ _   _ \\[ ]_[ ]_[ ]\n"
            "|_=__-_ =_|_[ ]_[ ]_|_=-___-__|\n"
            " | _- =  | =_ = _    |= _=   |\n"
            " |= -[]  |- = _ =    |_-=_[] |\n"
            " | =_    |= - ___    | =_ =  |\n"
            " |=  []- |-  /| |\\   |=_ =[] |\n"
            " |- =_   | =| | | |  |- = -  |\n"
            " |_______|__|_|_|_|__|_______|\n";


    QString castle_mask =
        "                RR\n"
        "\n"
        "              yyy\n"
        "             y   y\n"
        "            y     y\n"
        "           y       y\n"
        "\n"
        "\n"
        "\n"
        "              yyy\n"
        "             yy yy\n"
        "            y y y y\n"
        "            yyyyyyy\n";

    Frame f(castle_image, castle_mask, 0x686868/* XXX: why grey? */ );

    Sprite* castle = new Sprite(screen,
        screen->width() - 32, screen->height() - 13, 22);
    castle->addFrame(f);
    screen->addSprite(castle);
}

void AASaver::addEnvironment()
{
    QString water_line_segment[] = {
        "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~",
        "^^^^ ^^^  ^^^   ^^^    ^^^^      ",
        "^^^^      ^^^^     ^^^    ^^     ",
        "^^      ^^^^      ^^^    ^^^^^^  "
    };

    // tile the segments so they stretch across the screen
    int segment_size   = water_line_segment[0].length();
    int segment_repeat = int(screen->width()/segment_size) + 1;

    for (unsigned i = 0; i < ARRAY_SIZE(water_line_segment); ++i) {
        //do the tiling
        QString out;
        for (int r = 0; r < segment_repeat; ++r)
            out += water_line_segment[i];

        //create a sprite.
        Sprite* s = new Sprite(screen, 0, i + 5, 22);
        s->addFrame(Frame(out, QString::null, 0x149494));
        screen->addSprite(s);
    }
}

void AASaver::addAllSeaweed()
{
    // figure out how many seaweed to add by the width of the screen
    int seaweed_count = int(screen->width() / 15.0);
    for (int i = 1; i <= seaweed_count; ++i)
        addSeaweed(screen);
}

/**
 * Special class to represent seaweed.  Seaweed can disappear over time, and
 * when this happens, this class will automatically spawn another one.
 */
class Seaweed: public Sprite
{
    int m_ticks; ///< Number of animation ticks elapsed.
    int m_lifeTimeMS; ///< Life time of seaweed in milliseconds.
    static QList<int> m_positions;

public:
    /**
     * Constructor.  The \p x, \p y, and \p z coordinates are all in logical
     * coordinates.
     *
     * @param s The Screen to be created in.
     * @param x The x coordinate to place the seaweed at.
     * @param y The y coordinate to place the seaweed at.
     * @param life The length of time in milliseconds the seaweed will live for.
     */
    Seaweed(Screen* s, int x, int y, int life): Sprite(s, x, y, 21),
        m_ticks(0), m_lifeTimeMS(life)
    {
        m_positions.append(x);
    }

    ~Seaweed()
    {
        m_positions.removeAll(m_x);
    }

    static bool isTooClose(int newX)
    {
        foreach(int i, m_positions) {
            if(qAbs(i - newX) < 3)
                return true;
        }

        return false;
    }

    /**
     * Reimplemented from Sprite::tickUpdate() to handle keeping track of
     * Seaweed lifetime.  Calls the inherited tickUpdate() as well.
     */
    virtual bool tickUpdate()
    {
        ++m_ticks;
        if (m_ticks * m_screen->msPerTick() > m_lifeTimeMS)
        {
            erase();
            kill();
            AASaver::instance()->addSeaweed(m_screen);
        }

        return Sprite::tickUpdate();
    }
};

QList<int> Seaweed::m_positions;

void AASaver::addSeaweed(Screen* screen)
{
    QString seaweed_image[] = {"", ""};
    int height = m_randomSequence.getLong(5) + 3;
    for (int i = 1; i <= height; ++i)
    {
        int left_side  = i % 2;
        int right_side = !left_side;
        seaweed_image[left_side]  += "(\n";
        seaweed_image[right_side] += " )\n";
    }

    int x = m_randomSequence.getLong(screen->width() - 2) + 1;
    int y = screen->height() - height;

    // Keep guessing random numbers until the seaweed isn't too
    // bunched up.
    while(Seaweed::isTooClose(x))
        x = m_randomSequence.getLong(screen->width() - 2) + 1;

    Seaweed* s = new Seaweed(screen, x, y,
        m_randomSequence.getLong(4*60000) + (8*60000)); // seaweed lives for 8 to 12 minutes
    s->addFrame(Frame(seaweed_image[0], QString::null, 0x18AF18));
    s->addFrame(Frame(seaweed_image[1], QString::null, 0x18AF18));
    s->setFrameDelay(m_randomSequence.getLong(50) + 250);
    screen->addSprite(s);
}

/**
 * Class to represent an AirBubble.  The bubble will automatically float up,
 * even though it is not descended from MovingSprite.
 */
class AirBubble : public Sprite
{
    const int m_startY; ///< Y coordinate we started at, needed to choose a frame.

public:
    /**
     * Constructor.  The \p x, \p y, and \p z coordinates are all in logical
     * coordinates.
     *
     * @param screen The Screen to be created in.
     * @param x The x coordinate to start at.
     * @param y The y coordinate to start at.
     * @param z The depth to start at.
     */
    AirBubble(Screen *screen, int x, int y, int z) :
        Sprite(screen, x, y, z), m_startY(y)
    {
        addFrame(Frame(".", QString(), 0x18B2B2));
        addFrame(Frame("o", QString(), 0x18B2B2));
        addFrame(Frame("O", QString(), 0x18B2B2));

        setFrameDelay(100);
    }

    /**
     * Reimplemented from Sprite::tickUpdate() to handle moving the sprite and
     * updating the current frame.  The inherited tickUpdate() is not called.
     */
    virtual bool tickUpdate()
    {
        if (!timerTick())
            return false;

        erase();

        m_currentFrame = 0;
        if(m_startY - m_y > 5)
            m_currentFrame = 1;
        if(m_startY - m_y > 11)
            m_currentFrame = 2;

        m_y--;
        if(m_y < 9)
            kill();

        return true;
    }
};

/**
 * Moving sprite, will be killed when it moves off of the screen.
 */
class MovingSprite: public Sprite
{
protected:
    int m_direct; ///< Direction to move in, -1 == left, 1 == right.
    double m_speed; ///< Speed to move at (Currently m_speed per tick).
    double m_realX; ///< Used for accuracy, holds fractional x position.
    int m_ticksSinceLastChange; ///< Number of timer ticks since last frame change.
    int m_frameTime;            ///< Amount of time in milliseconds to show each frame.

public:
    /**
     * Constructor.  The \p x, \p y, and \p z coordinates are all in logical
     * coordinates.
     *
     * @param screen The Screen to be created in.
     * @param direct The direction to move the sprite in along the X axis, either
     *               -1 for the left direction, or 1 for the right direction.
     * @param speed The speed to move the sprite in along the X axis, in
     *              character cells per tick.  Use Screen::msPerTick() to find
     *              out how long a tick takes.  The speed can be fractional
     *              (e.g. 1.5 cells per tick).
     * @param x The x coordinate to start at.
     * @param y The y coordinate to start at.
     * @param z The depth to start at.
     */
    MovingSprite(Screen* screen, int direct, double speed, int x, int y, int z):
        Sprite(screen, x, y, z), m_direct(direct), m_speed(speed), m_realX(x),
        m_ticksSinceLastChange(0), m_frameTime(250)
    {
    }

    /**
     * Sets the amount of time a frame is shown.  Use this function for
     * MovingSprites that are also animated.
     *
     * @param milliseconds Amount of time to show a frame for in milliseconds.
     */
    void setFrameTime(int milliseconds)
    {
        m_frameTime = milliseconds;
    }

    /// Returns the amount of time a frame lasts in milliseconds.
    int frameTime() const { return m_frameTime; }

    /// Returns the direction the sprite travels in.
    int direction() const
    {
        return m_direct;
    }

    /// Returns the fractional speed of the sprite.
    double realSpeed() const
    {
        return m_speed;
    }

    /// Returns the real (fractional) X position of the sprite.
    double realX() const
    {
        return m_realX;
    }

    /**
     * Reimplemented from Sprite::tickUpdate() to handle motion and frame
     * animation.  This function will automatically kill() this sprite when
     * it moves off screen.  The inherited tickUpdate() is not called.
     */
    virtual bool tickUpdate()
    {
        if (!timerTick())
            return false;

        erase();
        m_realX += (m_direct * m_speed);
        m_x = (int) m_realX;

        ++m_ticksSinceLastChange;
        if(m_ticksSinceLastChange * m_screen->msPerTick() > m_frameTime)
        {
            m_ticksSinceLastChange = 0;

            ++m_currentFrame;
            if(m_currentFrame == m_frames.size())
                m_currentFrame = 0;
        }

        if((m_x + m_frames[m_currentFrame].width() < 0) || (m_x > m_screen->width()))
            kill();

        return true;
    }
};

/**
 * Will spawn a random sprite when killed, otherwise behaves just like
 * MovingSprite.
 */
class RandomMovingSprite : public MovingSprite
{
public:
    RandomMovingSprite(Screen *screen, int direct, double speed, int x, int y, int z):
        MovingSprite(screen, direct, speed, x, y, z)
    {
    }

    /// Spawns another RandomMovingSprite before dying.
    virtual void kill()
    {
        MovingSprite::kill();
        AASaver::instance()->addRandom(m_screen);
    }
};

/**
 * Special subclass that represents a fish.  Used so TeethSprite knows when it
 * has caused a collision, and also to handle air bubble generation.
 */
class FishSprite : public MovingSprite
{
    double m_spacesPerBubble;   ///< Amount of spaces a fish moves for each bubble.
    double m_lastBubbleRelease; ///< Amount of space traveled since the last bubble.

public:
    FishSprite(Screen* screen, int direct, double speed, int x, int y, int z):
        MovingSprite(screen, direct, speed, x, y, z), m_lastBubbleRelease(x)
    {
        m_spacesPerBubble = AASaver::instance()->doubleRand(screen->width()) + 12.0;
    }

    /// Spawns another fish before dying.
    virtual void kill()
    {
        MovingSprite::kill();
        AASaver::instance()->addFish(m_screen);
    }

    /**
     * Reimplemented from MovingSprite::tickUpdate() to handle creating air
     * bubbles.  Inherited tickUpdate() is still called.
     */
    virtual bool tickUpdate()
    {
        if(!MovingSprite::tickUpdate())
            return false;

        if(isKilled())
            return true;

        if(qAbs(realX() - m_lastBubbleRelease) >= m_spacesPerBubble)
        {
            m_lastBubbleRelease = realX();

            int bubbleX = m_x;
            QRect geometry = geom();

            if(m_direct > 0) // Moving right
                bubbleX += geometry.width();

            AASaver::instance()->addBubble(m_screen, bubbleX,
                m_y + geometry.height() / 2 - 1, m_z - 1);
        }

        return true;
    }
};

void AASaver::addAllFish()
{
    // Determine how many logical pixels we are dealing with, and find out how
    // many we'd be dealing with in full screen, and then scale the user's
    // number down to adjust so that we look about the same in a window as we
    // do fullscreen. TODO: Xinerama issues?
    QRect fullScreenGeometry(QApplication::desktop()->screenGeometry());

    int full_width = fullScreenGeometry.width() / screen->cellWidth();
    int full_height = fullScreenGeometry.height() / screen->cellHeight() - 9;
    int full_size = full_width * full_height;
    int screen_size = (screen->height() - 9) * screen->width();

    int fish_count = AASaverConfig::fishCount() * screen_size / full_size;
    if(fish_count < 5)
        fish_count = 5;

    for (int i = 1; i <= fish_count; ++ i)
        addFish(screen);
}

Sprite *AASaver::newFish(Screen *screen)
{
    QString fish_image[] = {
"       \\\n"
"     ...\\..,\n"
"\\" "{}" "/'       \\\n" // trigraphs suck
" >=     (  ' >\n"
"/{}\\      / /\n"
"    `\"'\"'/''\n",

"       2\n"
"     1112111\n"
"6  11       1\n"
" 66     7  4 5\n"
"6  1      3 1\n"
"    11111311\n",

//////////////////////////////
"      /\n"
"  ,../...\n"
" /       '\\" "{}" "/\n" // trigraphs suck
"< '  )     =<\n"
" \\ \\      /{}\\\n"
"  `'\\'\"'\"'\n",

"      2\n"
"  1112111\n"
" 1       11  6\n"
"5 4  7     66\n"
" 1 3      1  6\n"
"  11311111\n",
//////////////////////////////
"    \\\n"
"\\?/--\\\n"
">=  (o>\n"
"/?\\__/\n"
"    /\n",

"    2\n"
"6 1111\n"
"66  745\n"
"6 1111\n"
"    3\n",

//////////////////////////////
"  /\n"
" /--\\?/\n"
"<o)  =<\n"
" \\__/?\\\n"
"  \\\n",

"  2\n"
" 1111 6\n"
"547  66\n"
" 1111 6\n"
"  3\n",

//////////////////////////////
"       \\:.\n"
"\\;,{}?,;\\\\\\\\\\,,\n"
"  \\\\\\\\\\;;:::::o\n"
"  ///;;::::::::<\n"
" /;`?``/////``\n",

"       222\n"
"666   1122211\n"
"  6661111111114\n"
"  66611111111115\n"
" 666 113333311\n",

//////////////////////////////
"      .:/\n"
"   ,,///;,{}?,;/\n"
" o:::::::;;///\n"
">::::::::;;\\\\\\\n"
"  ''\\\\\\\\\\''?';\\\n",

"      222\n"
"   1122211   666\n"
" 4111111111666\n"
"51111111111666\n"
"  113333311 666\n",

//////////////////////////////
"  __\n"
"><_'>\n"
"   '\n",

"  11\n"
"61145\n"
"   3\n",

//////////////////////////////
" __\n"
"<'_><\n"
" `\n",

" 11\n"
"54116\n"
" 3\n",

//////////////////////////////
"   ..\\,\n"
">='   ('>\n"
"  '''/''\n",

"   1121\n"
"661   745\n"
"  111311\n",

//////////////////////////////
"  ,/..\n"
"<')   `=<\n"
" ``\\```\n",

"  1211\n"
"547   166\n"
" 113111\n",

//////////////////////////////
"   \\\n"
"  / \\\n"
">=_('>\n"
"  \\_/\n"
"   /\n",

"   2\n"
"  1 1\n"
"661745\n"
"  111\n"
"   3\n",

//////////////////////////////
"  /\n"
" / \\\n"
"<')_=<\n"
" \\_/\n"
"  \\\n",

"  2\n"
" 1 1\n"
"547166\n"
" 111\n"
"  3\n",

//////////////////////////////
"  ,\\\n"
">=('>\n"
"  '/\n",

"  12\n"
"66745\n"
"  13\n",

//////////////////////////////
" /,\n"
"<')=<\n"
" \\`\n",

" 21\n"
"54766\n"
" 31\n",

//////////////////////////////
"  __\n"
"\\/ o\\\n"
"/\\__/\n",

"  11\n"
"61 41\n"
"61111\n",

//////////////////////////////
" __\n"
"/o \\/\n"
"\\__/\\\n",

" 11\n"
"14 16\n"
"11116\n"
};

    // # 1: body
    // # 2: dorsal fin
    // # 3: flippers
    // # 4: eye
    // # 5: mouth
    // # 6: tailfin
    // # 7: gills*
    int fish_num   = m_randomSequence.getLong(ARRAY_SIZE(fish_image)/2);
    int fish_index = fish_num * 2;

    double speed = doubleRand(2) + 0.25;
    int depth = 3 + m_randomSequence.getLong(18);

    QString color_mask = fish_image[fish_index+1];
    color_mask.replace('4', 'W');

    color_mask = randColor(color_mask);

    Frame fishFrame(fish_image[fish_index], color_mask, 0);
    int max_height = 9;
    int min_height = screen->height() - fishFrame.height();

    int x, y, dir;
    y = max_height + m_randomSequence.getLong(min_height - max_height);
    if (fish_num % 2)
    {
        x   = screen->width() - 2;
        dir = -1;
    }
    else
    {
        x   = 1 - fishFrame.width();
        dir = 1;
    }

    Sprite* fish = new FishSprite(screen, dir, speed, x, y, depth);
    fish->addFrame(fishFrame);

    return fish;
}

void AASaver::addFish(Screen* screen)
{
    screen->addSprite(newFish(screen));
}

/**
 * Sprite that represents a blood "splat" in the water.
 */
class Splat : public Sprite
{
public:
    /**
     * Constructor.
     *
     * @param screen The Screen to create the splat in.
     * @param center The point to center the splat around.
     * @param depth  The depth to create the splat at.
     */
    Splat(Screen *screen, QPoint center, int depth) :
        Sprite(screen, 0, 0, depth, 450 /* frame Delay */)
    {
        QString splats[] = {
"\n"
"   .\n"
"  ***\n"
"   '\n"
""
,

"\n"
" \",*;`\n"
" \"*,**\n"
" *\"'~'\n"
""
,
"  , ,\n"
" \" \",\"'\n"
" *\" *'\"\n"
"  \" ; .\n"
""
,
"* ' , ' `\n"
"' ` * . '\n"
" ' `' \",'\n"
"* ' \" * .\n"
"\" * ', '"
        };

        for(unsigned i = 0; i < ARRAY_SIZE(splats); ++i)
            addFrame(Frame(splats[i], QString(), 0xB21818, ' '));

        QRect r(center, QSize(9, 5));
        r.moveCenter(center);
        m_x = r.x();
        m_y = r.y();

        setDieAfterLastFrame(true);
    }
};

/**
 * Invisible sprite which are created on a shark's teeth, to handle collisions
 * with fish, creating splats and kill()'ing the fish.
 */
class TeethSprite : public MovingSprite
{
public:
    /**
     * Constructor.  Copied parameters as appropriate from \p shark.
     *
     * @param shark The shark to create the teeth over.
     */
    TeethSprite(MovingSprite *shark) : MovingSprite(shark->screen(), shark->direction(),
            shark->realSpeed(), 2 + shark->geom().left(), shark->geom().top(), shark->depth())
    {
        m_y += 7;
        m_z -= 1;
        m_realX = 2 + shark->realX();

        if(m_direct > 0) // Moving to right.
            m_realX = -10;

        addFrame(Frame("{}{}{}{}", QString(), 0));
    }

    /// Returns true since we can collide.
    bool canCollide() const { return true; }

    /**
     * Reimplemented in order to handle collisions.  When colliding with a
     * FishSprite, the fish is kill()'ed and a splat is created in its place.
     * Otherwise, nothing is done.
     *
     * @param sprite The Sprite we collided with.
     */
    void collision(Sprite *sprite)
    {
        if(dynamic_cast<FishSprite *>(sprite)) {
            sprite->erase();
            sprite->kill();

            screen()->addSprite(new Splat(screen(), sprite->geom().center(), depth() - 1));
        }
    }
};

void AASaver::addShark(Screen* screen)
{
    QString shark_image[] = {
"                              __\n"
"                             ( `\\\n"
"  ,{}{}{}{}{}{}{}{}{}{}{}{}{}" ")   `\\\n" // trigraphs suck
";' `.{}{}{}{}{}{}{}{}{}{}{}{}" "(     `\\__\n" // trigraphs suck
" ;   `.{}{}{}{}{}{}?__..---''          `~~~~-._\n"
"  `.   `.____...--''                       (b  `--._\n"
"    >                     _.-'      .((      ._     )\n"
"  .`.-`--...__         .-'     -.___.....-(|/|/|/|/'\n"
" ;.'{}{}{}{}?`. ...----`.___.',,,_______......---'\n"
" '{}{}{}{}{}?" "'-'\n", // trigraphs suck

"                                                     \n"
"                                                     \n"
"                                                     \n"
"                                                     \n"
"                                                     \n"
"                                           cR        \n"
"                                                     \n"
"                                          cWWWWWWWW  \n"
"                                                     \n"
"                                                     \n",

"                     __\n"
"                    /' )\n"
"                  /'   ({}{}{}{}{}{}{}{}{}{}{}{}{},\n"
"              __/'     ){}{}{}{}{}{}{}{}{}{}{}{}.' `;\n"
"      _.-~~~~'          ``---..__{}{}{}{}{}{}?.'   ;\n"
" _.--'  b)                       ``--...____.'   .'\n"
"(     _.      )).      `-._                     <\n"
" `\\|\\|\\|\\|)-.....___.-     `-.         __...--'-.'.\n"
"   `---......_______,,,`.___.'----... .'{}{}{}{}?`.;\n"
"                                     `-`{}{}{}{}{}?`\n",

"                                                     \n"
"                                                     \n"
"                                                     \n"
"                                                     \n"
"                                                     \n"
"        Rc                                           \n"
"                                                     \n"
"  WWWWWWWWc                                          \n"
"                                                     \n"
"                                                     \n"
    };

    int shark_num   = m_randomSequence.getLong(ARRAY_SIZE(shark_image)/2);
    int shark_index = shark_num * 2;
    QString color_mask = randColor(shark_image[shark_index+1]);
    Frame sharkFrame(shark_image[shark_index], color_mask, 0x18B2B2);

    int x = -53;
    int y = 9 + m_randomSequence.getLong(screen->height() - (10 + 9));
    int dir = (shark_num % 2) ? -1 : 1;

    if(dir < 0)
        x = screen->width() - 2;

    RandomMovingSprite* shark = new RandomMovingSprite(screen, dir, 2, x, y, 2 /* Always at 2 */);
    shark->addFrame(sharkFrame);
    screen->addSprite(shark);

    TeethSprite *teeth = new TeethSprite(shark);
    screen->addSprite(teeth);
}

void AASaver::addBubble(Screen *screen, int x, int y, int z)
{
    screen->addSprite(new AirBubble(screen, x, y, z));
}

void AASaver::addShip(Screen* screen)
{
    QString ship_image[] = {
"     |    |    |\n"
"    )_)  )_)  )_)\n"
"   )___))___))___)\\\n"
"  )____)____)_____)\\\\\n"
"_____|____|____|____\\\\\\__\n"
"\\                   /",

"     y    y    y\n"
"                 \n"
"                  w\n"
"                   ww\n"
"yyyyyyyyyyyyyyyyyyyywwwyy\n"
"y                   y",

"         |    |    |\n"
"        (_(  (_(  (_(\n"
"      /(___((___((___(\n"
"    //(_____(____(____(\n"
"__///____|____|____|_____\n"
"    \\                   /",

"         y    y    y\n"
"                 \n"
"      w            \n"
"    ww               \n"
"yywwwyyyyyyyyyyyyyyyyyyyy\n"
"    y                   y"
    };

    int ship_num = m_randomSequence.getLong(17) % 2; // right == 0, left == 1
    int x = -24, dir = 1;

    if(ship_num == 1) {
        x = screen->width() - 2;
        dir = -1;
    }

    RandomMovingSprite *ship = new RandomMovingSprite(screen, dir, 1.0, x, 0, 2);
    ship->addFrame(Frame(ship_image[2 * ship_num], ship_image[2 * ship_num + 1], 0xFFFFFF));
    screen->addSprite(ship);
}

void AASaver::addWhale(Screen* screen)
{
    QString whale_image[] = {
"        .-----:\n"
"      .'       `.\n"
",{}{}/       (o) \\\n"
"\\`._/          ,__)",

"             C C\n"
"           CCCCCCC\n"
"           C  C  C\n"
"        BBBBBBB\n"
"      BB       BB\n"
"B    B       BWB B\n"
"BBBBB          BBBB",

"    :-----.\n"
"  .'       `.\n"
" / (o)       \\{}{},\n"
"(__,          \\_.'/",

"   C C\n"
" CCCCCCC\n"
" C  C  C\n"
"    BBBBBBB\n"
"  BB       BB\n"
" B BWB       B    B\n"
"BBBB          BBBBB"
    };

    QString spouty[] = {
"\n"
"\n"
"   :",

"\n"
"   :\n"
"   :",

"  . .\n"
"  -:-\n"
"   :",

"  . .\n"
" .-:-.\n"
"   :",

"  . .\n"
"'.-:-.`\n"
"'  :  '",

"\n"
" .- -.\n"
";  :  ;",

"\n"
"\n"
";     ;"
    };

    int whale_num = m_randomSequence.getLong(2); // 0 = right, 1 = left
    int x = -18, spout_align = 11, dir = 1;

    if (whale_num == 1)
    {
        x = screen->width() - 2;
        spout_align = 1; // Waterspout closer to left side now.
        dir = -1;
    }

    QString mask = whale_image[2 * whale_num + 1];

    RandomMovingSprite *whale = new RandomMovingSprite(screen, dir, 1.0, x, 0, 2);
    whale->setFrameDelay(80);
    whale->setFrameTime(40);

    // We have to add some frames now.  The first five will have no water spout.
    QString blankWhaleFrame = QString("\n\n\n") + whale_image[2 * whale_num];

    for(unsigned i = 0; i < 5; ++i)
        whale->addFrame(Frame(blankWhaleFrame, mask, 0xFFFFFF));

    // Now add frames for the animated water spout.
    QString whaleFrame = whale_image[2 * whale_num];
    for (unsigned i = 0; i < ARRAY_SIZE(spouty); ++i)
    {
        QStringList spoutLines = spouty[i].split("\n");
        QString spout;
        QString padding;

        padding.fill(' ', spout_align);

        // Move spout over an appropriate distance to line up right.
        foreach(const QString &spoutLine, spoutLines) {
            spout += padding;
            spout += spoutLine;
            spout += "\n";
        }

        // Add spout to whale frame.
        whale->addFrame(Frame(spout + whaleFrame, mask, 0xFFFFFF));
    }

    screen->addSprite(whale);
}

void AASaver::addBigFish(Screen* screen)
{
    QString big_fish_image[] = {
" ______\n"
"`\"\"-.  `````-----.....__\n"
"     `.  .      .       `-.\n"
"       :     .     .       `.\n"
" ,{}{}?:   .    .          _ :\n"
": `.{}?:                  (@) `._\n"
" `. `..'     .     =`-.       .__)\n"
"   ;     .        =  ~  :     .-\"\n"
" .' .'`.   .    .  =.-'  `._ .'\n"
": .'{}?:               .   .'\n"
" '{}?.'  .    .     .   .-'\n"
"   .'____....----''.'=.'\n"
"   \"\"{}{}{}{}{}{}?.'.'\n"
"               ''\"'`",

" 111111\n"
"11111  11111111111111111\n"
"     11  2      2       111\n"
"       1     2     2       11\n"
" 1     1   2    2          1 1\n"
"1 11   1                  1W1 111\n"
" 11 1111     2     1111       1111\n"
"   1     2        1  1  1     111\n"
" 11 1111   2    2  1111  111 11\n"
"1 11   1               2   11\n"
" 1   11  2    2     2   111\n"
"   111111111111111111111\n"
"   11             1111\n"
"               11111",

"                           ______\n"
"          __.....-----'''''  .-\"\"'\n"
"       .-'       .      .  .'\n"
"     .'       .     .     :\n"
"    : _          .    .   :{}{}?,\n"
" _.' (@)                  :{}?.' :\n"
"(__.       .-'=     .     `..' .'\n"
" \"-.     :  ~  =        .     ;\n"
"   `. _.'  `-.=  .    .   .'`. `.\n"
"     `.   .               :{}?`. :\n"
"       `-.   .     .    .  `.{}?`\n"
"          `.=`.``----....____`.\n"
"            `.`.{}{}{}{}{}{}?\"\"\n"
"              '`\"``",

"                           111111\n"
"          11111111111111111  11111\n"
"       111       2      2  11\n"
"     11       2     2     1\n"
"    1 1          2    2   1     1\n"
" 111 1W1                  1   11 1\n"
"1111       1111     2     1111 11\n"
" 111     1  1  1        2     1\n"
"   11 111  1111  2    2   1111 11\n"
"     11   2               1   11 1\n"
"       111   2     2    2  11   1\n"
"          111111111111111111111\n"
"            1111             11\n"
"              11111"
    };

    int big_fish_num = m_randomSequence.getLong(2); // right = 0, left = 1

    int maxHeight = 9, minHeight = screen->height() - 15;
    int y = m_randomSequence.getLong(minHeight - maxHeight) + maxHeight;
    int x = -34, dir = 1;

    if(big_fish_num == 1)
    {
        x = screen->width() - 1;
        dir = -1;
    }

    QString colors = randColor(big_fish_image[2 * big_fish_num + 1]);
    RandomMovingSprite *bigFish = new RandomMovingSprite(screen, dir, 3.0, x, y, 2);
    bigFish->addFrame(Frame(big_fish_image[2 * big_fish_num], colors, 0xFFFF54));

    screen->addSprite(bigFish);
}

void AASaver::addNessie(Screen* screen)
{
    QString nessie_image[] = {
"                                                          ____\n"
"            __{}{}{}{}{}{}{}{}{}{}{}{}{}{}{}{}{}{}{}{}{}/   o  \\\n"
"          /    \\{}{}{}{}_{}{}{}{}{}{}{}{}{}{}?_{}{}{}?/     ____ >\n"
"  _{}{}{}|  __  |{}{}?/   \\{}{}{}{}_{}{}{}{}/   \\{}{}|     |\n"
" | \\{}{}?|  ||  |{}{}|     |{}{}?/   \\{}{}?|     |{}?|     |",

"                                                          ____\n"
"                                             __{}{}{}{}?/   o  \\\n"
"             _{}{}{}{}{}{}{}{}{}{}?_{}{}{}?/    \\{}{}?/     ____ >\n"
"   _{}{}{}?/   \\{}{}{}{}_{}{}{}{}/   \\{}{}|  __  |{}?|     |\n"
"  | \\{}{}?|     |{}{}?/   \\{}{}?|     |{}?|  ||  |{}?|     |\n",

"                                                          ____\n"
"                                  __{}{}{}{}{}{}{}{}{}{}/   o  \\\n"
" _{}{}{}{}{}{}{}{}{}{}{}_{}{}{}?/    \\{}{}{}{}_{}{}{}?/     ____ >\n"
"| \\{}{}{}{}{}_{}{}{}{}/   \\{}{}|  __  |{}{}?/   \\{}{}|     |\n"
" \\ \\{}{}{}?/   \\{}{}?|     |{}?|  ||  |{}{}|     |{}?|     |",

"                                                          ____\n"
"                       __{}{}{}{}{}{}{}{}{}{}{}{}{}{}{}?/   o  \\\n"
"  _{}{}{}{}{}_{}{}{}?/    \\{}{}{}{}_{}{}{}{}{}{}{}{}{}/     ____ >\n"
" | \\{}{}{}?/   \\{}{}|  __  |{}{}?/   \\{}{}{}{}_{}{}{}|     |\n"
"  \\ \\{}{}?|     |{}?|  ||  |{}{}|     |{}{}?/   \\{}{}|     |",

"    ____\n"
"  /  o   \\{}{}{}{}{}{}{}{}{}{}{}{}{}{}{}{}{}{}{}{}{}__\n"
"< ____     \\{}{}{}?_{}{}{}{}{}{}{}{}{}{}?_{}{}{}{}/    \\\n"
"      |     |{}{}/   \\{}{}{}{}_{}{}{}{}/   \\{}{}?|  __  |{}{}{}_\n"
"      |     |{}?|     |{}{}?/   \\{}{}?|     |{}{}|  ||  |{}{}?/ |",

"    ____\n"
"  /  o   \\{}{}{}{}?__\n"
"< ____     \\{}{}?/    \\{}{}{}?_{}{}{}{}{}{}{}{}{}{}?_\n"
"      |     |{}?|  __  |{}{}/   \\{}{}{}{}_{}{}{}{}/   \\{}{}{}?_\n"
"      |     |{}?|  ||  |{}?|     |{}{}?/   \\{}{}?|     |{}{}?/ |",

"    ____\n"
"  /  o   \\{}{}{}{}{}{}{}{}{}{}__\n"
"< ____     \\{}{}{}?_{}{}{}{}/    \\{}{}{}?_{}{}{}{}{}{}{}{}{}{}{}_\n"
"      |     |{}{}/   \\{}{}?|  __  |{}{}/   \\{}{}{}{}_{}{}{}{}{}/ |\n"
"      |     |{}?|     |{}{}|  ||  |{}?|     |{}{}?/   \\{}{}{}?/ /",

"    ____\n"
"  /  o   \\{}{}{}{}{}{}{}{}{}{}{}{}{}{}{}?__\n"
"< ____     \\{}{}{}{}{}{}{}{}{}_{}{}{}{}/    \\{}{}{}?_{}{}{}{}{}_\n"
"      |     |{}{}{}_{}{}{}{}/   \\{}{}?|  __  |{}{}/   \\{}{}{}?/ |\n"
"      |     |{}{}/   \\{}{}?|     |{}{}|  ||  |{}?|     |{}{}?/ /"
    };

    QString nessie_mask[] = {
"\n"
"                                                            W\n"
"\n"
"\n"
"\n"
"",

"\n"
"     W\n"
"\n"
"\n"
"\n"
""
    };

    int nessie_num = m_randomSequence.getLong(2); // 0 = right, 1 = left.
    int x = -64, dir = 1;

    if(nessie_num == 1) {
        x = screen->width() - 2;
        dir = -1;
    }

    RandomMovingSprite *nessie = new RandomMovingSprite(screen, dir, 1.4, x, 2, 2);
    nessie->setFrameDelay(75);
    nessie->setFrameTime(400);

    for(unsigned i = 0; i < 4; ++i)
        nessie->addFrame(Frame(nessie_image[nessie_num * 4 + i], nessie_mask[nessie_num], 0x18B218));

    screen->addSprite(nessie);
}

void AASaver::addRandom(Screen* screen)
{
    int choice = m_randomSequence.getLong(5);

    switch(choice)
    {
        case 0:
            addShark(screen);
            break;
        case 1:
            addShip(screen);
            break;
        case 2:
            addBigFish(screen);
            break;
        case 3:
            addNessie(screen);
            break;
        case 4:
            addWhale(screen);
            break;
    }
}

void AASaver::paintEvent(QPaintEvent* pe)
{
    // Dirty region is passed instead of the entire viewport for efficiency.
    screen->paint(pe->region());
}

//
// Implementation of the AASaverInterface
//

AASaverInterface::~AASaverInterface()
{
}

KAboutData *AASaverInterface::aboutData()
{
    return new KAboutData("kdeasciiquarium.kss", 0, ki18n("KDE Asciiquarium"), "0.4.1",
        ki18n("KDE Asciiquarium"));
}

KScreenSaver *AASaverInterface::create(WId id)
{
    return new AASaver(id);
}

QDialog *AASaverInterface::setup()
{
    KConfigDialog *dialog = KConfigDialog::exists("settings");
    if(dialog)
        return dialog;

    dialog = new KConfigDialog(0, "settings", AASaverConfig::self());

    QWidget *settings = new QWidget(dialog);
    Ui::SettingsWidget ui;
    ui.setupUi(settings);

    dialog->addPage(settings, i18n("Asciiquarium Settings"), "preferences-desktop-screensaver");

    return dialog;
}

int main(int argc, char **argv)
{
    AASaverInterface kss;
    return kScreenSaverMain(argc, argv, kss);
}

// vim: set et ts=8 sw=4:
