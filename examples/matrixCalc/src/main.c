#include <PDCurses/curses.h>
typedef char * pchar;
#define ARR_TYPE pchar
#include <clib/array.h>
typedef void (*pfn)(clib_arr * vars);
#define ARR_TYPE pfn
#include <clib/array.h>
#include "curses_menu.h"
#include "curses_colors.h"
#include "curses_functions.h"
#include "matrix.h"

int main()
{
    init_stdscr();
    WINDOW * title = init_title(stdscr);

    clib_arr_pchar strs;
    clib_arr_cast_pchar(&strs,6,(pchar[]){
        "Scan matrix",
        "Print matrix",
        "Add matrixies",
        "Mut matrixies",
        "Transpose matrix",
        "Exit"
    });

    clib_arr_pfn fns;
    clib_arr_cast_pfn(&fns,6,(pfn[]){
        menu_scan,
        
    });

    clib_arr vars;
    clib_arr_init(&vars,0,sizeof(mtrx));

    WINDOW * menu = init_menu(
        "Chose action:",
        &strs,
        &fns,
        &vars,
        stdscr,
        COLOR_CYAN_BG,
        "-> ",
        3
    );

    delwin(title);
    delwin(menu);
    endwin();
    return 0;
}