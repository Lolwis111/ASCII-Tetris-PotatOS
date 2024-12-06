#include "tetris.h"
#include "printk.h"
#include "malloc.h"
#include "keyboard.h"
#include "asm.h"


struct tetris_level {
    int score;
    int nsec;
};

struct tetris {
    char **game;
    int w;
    int h;
    int level;
    int gameover;
    int score;
    struct tetris_block {
        char data[5][5];
        int w;
        int h;
    } current;
    int x;
    int y;
};

// --------------------------------------------
// PotatOS has no rand() yet :)
// --------------------------------------------
#define A 1664525
#define C 1013904223
#define M 4294967296

unsigned int _seed = 12345;

unsigned int random()
{
    _seed = (A * _seed + C) % M;
    return _seed;
}

// also no nanosleep, so we do this trickery (io_wait is about 4ms)
void micro_sleep()
{
    for(int i = 0; i < 10000; i++)
    {
        io_wait();
    }
}

// --------------------------------------------

struct tetris_block blocks[] = {
        {{"##", "##"}, 2, 2},             {{" X ", "XXX"}, 3, 2},
        {{"@@@@"}, 4, 1},                     {{"OO", "O ", "O "}, 2, 3},
        {{"&&", " &", " &"}, 2, 3}, {{"ZZ ", " ZZ"}, 3, 2}};

struct tetris_level levels[] = {
        {0, 1200000},        {1500, 900000},    {8000, 700000},    {20000, 500000},
        {40000, 400000}, {75000, 300000}, {100000, 200000}};

#define TETRIS_PIECES (sizeof(blocks) / sizeof(struct tetris_block))
#define TETRIS_LEVELS (sizeof(levels) / sizeof(struct tetris_level))

void tetris_init(struct tetris *t, int w, int h)
{
    int x, y;
    t->level = 1;
    t->score = 0;
    t->gameover = 0;
    t->w = w;
    t->h = h;
    t->game = malloc(sizeof(char *) * w);

    for (x = 0; x < w; x++)
    {
        t->game[x] = malloc(sizeof(char) * h);

        for (y = 0; y < h; y++) 
        {
            t->game[x][y] = ' ';
        }
    }
}

void tetris_clean(struct tetris *t) 
{
    int x;
    for (x = 0; x < t->w; x++) 
    {
        free(t->game[x]);
    }

    free(t->game);
}

void tetris_print(struct tetris *t) 
{
    int x, y;
    for (x = 0; x < 30; x++) 
    {
        printk("\r\n");
    }

    printk("[LEVEL: %d | SCORE: %d]\r\n", t->level, t->score);

    for (x = 0; x < 2 * t->w + 2; x++) 
    {
        printk("~");
    }

    printk("\r\n");

    for (y = 0; y < t->h; y++) 
    {
        printk("!");

        for (x = 0; x < t->w; x++) 
        {
            if (x >= t->x && y >= t->y && x < (t->x + t->current.w) &&
                    y < (t->y + t->current.h) &&
                    t->current.data[y - t->y][x - t->x] != ' ')
                {
                printk("%c ", t->current.data[y - t->y][x - t->x]);
            } else {
                printk("%c ", t->game[x][y]);
            }
        }

        printk("!\r\n");
    }

    for (x = 0; x < 2 * t->w + 2; x++) 
    {
        printk("~");
    }

    printk("\r\n");
}

int tetris_hittest(struct tetris *t) 
{
    int x, y, X, Y;
    struct tetris_block b = t->current;

    for (x = 0; x < b.w; x++) 
    {
        for (y = 0; y < b.h; y++) 
        {
            X = t->x + x;
            Y = t->y + y;
            if (X < 0 || X >= t->w) 
            {
                return 1;
            }
            if (b.data[y][x] != ' ') 
            {
                if ((Y >= t->h) || (X >= 0 && X < t->w && Y >= 0 && t->game[X][Y] != ' '))
                {
                    return 1;
                }
            }
        }
    }

    return 0;
}

void tetris_new_block(struct tetris *t) 
{
    t->current = blocks[random() % TETRIS_PIECES];

    t->x = (t->w / 2) - (t->current.w / 2);
    t->y = 0;

    if (tetris_hittest(t)) 
    {
        t->gameover = 1;
    }
}

void tetris_print_block(struct tetris *t) 
{
    int x, y;
    struct tetris_block b = t->current;

    for (x = 0; x < b.w; x++) 
    {
        for (y = 0; y < b.h; y++) 
        {
            if (b.data[y][x] != ' ') 
            {
                t->game[t->x + x][t->y + y] = b.data[y][x];
            }
        }
    }
}

void tetris_rotate(struct tetris *t) 
{
    struct tetris_block b = t->current;
    struct tetris_block s = b;
    int x, y;
    b.w = s.h;
    b.h = s.w;

    for (x = 0; x < s.w; x++)
    {
        for (y = 0; y < s.h; y++) 
        {
            b.data[x][y] = s.data[s.h - y - 1][x];
        }
    }

    x = t->x;
    y = t->y;
    t->x -= (b.w - s.w) / 2;
    t->y -= (b.h - s.h) / 2;
    t->current = b;

    if (tetris_hittest(t)) 
    {
        t->current = s;
        t->x = x;
        t->y = y;
    }
}

void tetris_gravity(struct tetris *t) 
{
    t->y++;

    if (tetris_hittest(t)) 
    {
        t->y--;
        tetris_print_block(t);
        tetris_new_block(t);
    }
}

void tetris_fall(struct tetris *t, int l) 
{
    int x, y;
    for (y = l; y > 0; y--) 
    {
        for (x = 0; x < t->w; x++)
        {
            t->game[x][y] = t->game[x][y - 1];
        }
    }
    for (x = 0; x < t->w; x++)
    {
        t->game[x][0] = ' ';
    }
}

void tetris_check_lines(struct tetris *t) 
{
    int x, y, l;
    int p = 100;
    for (y = t->h - 1; y >= 0; y--) 
    {
        l = 1;
        for (x = 0; x < t->w && l; x++) 
        {
            if (t->game[x][y] == ' ') 
            {
                l = 0;
            }
        }
        if (l) 
        {
            t->score += p;
            p *= 2;
            tetris_fall(t, y);
            y++;
        }
    }
}

int tetris_level(struct tetris *t) 
{
    int i;
    for (i = 0; i < TETRIS_LEVELS; i++) 
    {
        if (t->score >= levels[i].score) 
        {
            t->level = i + 1;
        } 
        else
        {
            break;
        }
    }
    return levels[t->level - 1].nsec;
}

void tetris_run(int w, int h) 
{
    struct tetris t;
    char cmd;
    int count = 0;
    tetris_init(&t, w, h);

    tetris_new_block(&t);

    while (!t.gameover) 
    {
        micro_sleep();

        count++;

        if (count % 50 == 0) 
        {
            tetris_print(&t);
        }
        if (count % 350 == 0) 
        {
            tetris_gravity(&t);
            tetris_check_lines(&t);
        }
        
        
        while ((cmd = getch2()) > 0) 
        {
            switch (cmd) 
            {
                case 'q':
                    t.x--;
                    if (tetris_hittest(&t))
                        t.x++;
                    break;
                case 'd':
                    t.x++;
                    if (tetris_hittest(&t))
                        t.x--;
                    break;
                case 's':
                    tetris_gravity(&t);
                    break;
                case ' ':
                    tetris_rotate(&t);
                    break;
            }
        }
        tetris_level(&t);
    }

    tetris_print(&t);
    printk("*** GAME OVER ***\n");

    tetris_clean(&t);
}
