#include <curses.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void voidendwin() { endwin(); }

int height = 20;
int width = 24;

#include "key.h"
#include "window.h"

int main() {
    srand(time(NULL));
    MainWindow w;
    // KeyHandler::GetInstance()->Init();
    w.Play();
    return 1;

    return 0;
}
