#include "platform.h"

#ifdef _WIN32
#include <stdlib.h>
#include <stdio.h>

#include <conio.h>
#include <windows.h>

char read_character() {
    auto c = _getch();
    if (c == 3) // ^C
        exit(0);
    return c;
}

void clear_screen() {
    system("cls");
}

void init_locale() {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
}

#endif
