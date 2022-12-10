#include <curses.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void voidendwin() { endwin(); }

int height = 20;
int width = 24;

#include "window.h"

int main() {
    srand(time(NULL));
    MainWindow w;
    w.Show();
    return 1;

#if 0
    WINDOW *win = initscr();  //初始化curses模式

    attron(A_UNDERLINE | A_BOLD);  //设置显示属性

    printw("hello world!\n");
refresh();  //刷新到物理显示器

    getch();  //获取用户输入

    endwin();  //退出curses模式

    return 0;
#endif

#if 0
    initscr();  //初始化curses模式
    int screen_h, screen_w;
    getmaxyx(stdscr, screen_h, screen_w);
    // WINDOW* win = newwin(5, 20, 80, 40);
    WINDOW* win = newwin(screen_h, screen_w, 0, 0);
    box(win, 0, 0);  //绘制边框
    start_color();
    /* 1 - 16 is for blocks */
    init_pair(1, COLOR_BLACK, COLOR_RED);
    init_pair(2, COLOR_BLACK, COLOR_YELLOW);
    init_pair(3, COLOR_BLACK, COLOR_GREEN);
    init_pair(4, COLOR_BLACK, COLOR_CYAN);
    init_pair(5, COLOR_BLACK, COLOR_MAGENTA);
    init_pair(6, COLOR_BLACK, COLOR_BLUE);
    init_pair(7, COLOR_BLACK, COLOR_WHITE);

    wattrset(win, COLOR_PAIR(7));
    mvwaddch(win, 0, 2 * 0, ' ');
    //    mvwaddch(win, 0, 2 * 0 + 1, ' ');

    // wprintw(win,"hello world!\n");

    wrefresh(win);  //刷新到物理显示器

    wgetch(win);  //获取用户输入

    wclear(win);  //清空内容

    delwin(win);  //删除窗口

    endwin();  //退出curses模式

    return 0;

#else
    initscr();

    curs_set(0);
    atexit(voidendwin);
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);
    nonl();
    noecho();
    cbreak();

    start_color();
    /* 1 - 16 is for blocks */
    init_pair(1, COLOR_BLACK, COLOR_RED);
    init_pair(2, COLOR_BLACK, COLOR_YELLOW);
    init_pair(3, COLOR_BLACK, COLOR_GREEN);
    init_pair(4, COLOR_BLACK, COLOR_CYAN);
    init_pair(5, COLOR_BLACK, COLOR_MAGENTA);
    init_pair(6, COLOR_BLACK, COLOR_BLUE);
    init_pair(7, COLOR_BLACK, COLOR_WHITE);

    /* 17 - ? is for other things */
    init_pair(17, COLOR_RED, COLOR_BLACK);     // points
    init_pair(18, COLOR_YELLOW, COLOR_BLACK);  // number of lines
    init_pair(19, COLOR_GREEN, COLOR_BLACK);   // level
    init_pair(20, COLOR_YELLOW, COLOR_BLACK);  // messages
    init_pair(21, COLOR_WHITE, COLOR_BLACK);   // window borders
    init_pair(22, COLOR_WHITE, COLOR_BLACK);   // end of line animation

    //    int y = -1;
    //    int x = -1;
    //    int screen_h, screen_w;
    //    if (y == -1 || x == -1) {
    //        getmaxyx(stdscr, screen_h, screen_w);
    //        if (y == -1) y = (screen_h - height - 2) / 2 - 1;
    //        if (x == -1) x = (screen_w - width - 2) / 2 - 1;
    //    }

    WINDOW* border = newwin(height + 2, width + 2, 0, 0);
    WINDOW* win = derwin(border, height, width, 1, 1);
    wattrset(win, COLOR_PAIR(3));
    wattrset(win, COLOR_PAIR(7));
    mvwaddch(win, 3, 4 * 1, ' ');
    mvwaddch(win, 3, 4 * 1 + 1, ' ');
    mvwaddch(win, 4, 4 * 1, ' ');
    mvwaddch(win, 4, 4 * 1 + 1, ' ');

    mvwaddch(win, 4, 4 * 1 - 1, ' ');
    mvwaddch(win, 4, 4 * 1 - 2, ' ');

    mvwaddch(win, 5, 4 * 1, ' ');
    mvwaddch(win, 5, 4 * 1 + 1, ' ');
    box(win, 0, 0);  //绘制边框

    wrefresh(win);  //刷新到物理显示器
    wgetch(win);    //获取用户输入
    wclear(win);    //清空内容

    delwin(win);  //删除窗口

    endwin();  //退出curses模式
               //    printf("y %d %d\n", y, x);
               //    printf("y %d %d\n", screen_w, screen_h);

    while (1) {
    }
#endif

    return 0;
}
