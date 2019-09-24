/*
 * =====================================================================================
 *
 *       Filename:  myedit.h
 *
 *    Description:  myedit header file
 *
 *        Version:  1.0
 *        Created:  11/11/2017 02:26:04 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Yimin Gu (), guyimin@mail.ustc.edu.cn
 *        Company:  
 *
 * =====================================================================================
 */

#ifndef MYEDIT_H
#define MYEDIT_H
#include <stdio.h>
#include "hstring.h"

#define AUTHOR "Peter Gu"
#define GITHUB "https://github.com/ustcpetergu/Datastructure/tree/master/string"
#define EMAIL "guyimin@mail.ustc.edu.cn"
#define VERSION "1.0"
//#define DEBUG
#define MAXBUF 10000
typedef struct line{
	HString *s;
	struct line *back;
	struct line *next;
}Line, *File;//this is a double linked list
typedef struct{
	int linenum;
	int col;
	int colshouldbe;//to make moving more friendly
	Line *line;
	int start;
}Linepos;
//this has three vars, 'cursor', 'start', and 'end'; and 'end' may not be used
//linenumber, cursor colume, pointer to line, and from "start" to show on screen
//col and start is needed only by 'cursor', not 'start' and 'end'
typedef struct{
	int x;
	int y;
}Curpos;
typedef struct{
	int isdirty;
	int col;
	int row;//screen col and row
	int readonly;
	int rawmode;
	int linenum;
	File file;
}Fileedit;
//typedef struct{
//	line *front;
//	int linenum;
//}file;//the first line and line number
enum KEY_ACTION{
        KEY_NULL = 0,       /* NULL */
	CTRL_A = 1,
	CTRL_B = 2,
        CTRL_C = 3,         /* Ctrl-c */
        CTRL_D = 4,         /* Ctrl-d */
        CTRL_E = 5,
        CTRL_F = 6,         /* Ctrl-f */
	CTRL_G = 7,
        CTRL_H = 8,         /* Ctrl-h */
        TAB = 9,            /* Tab */
        CTRL_L = 12,        /* Ctrl+l */
	CTRL_N = 14,
        ENTER = 13,         /* Enter */
	CTRL_P = 16,
        CTRL_Q = 17,        /* Ctrl-q */
        CTRL_S = 19,        /* Ctrl-s */
        CTRL_U = 21,        /* Ctrl-u */
        ESC = 27,           /* Escape */
        BACKSPACE =  127,   /* Backspace */
        /* The following are just soft codes, not really reported by the
         * terminal directly. */
        ARROW_LEFT = 1000,
        ARROW_RIGHT,
        ARROW_UP,
        ARROW_DOWN,
        DEL_KEY,
        HOME_KEY,
        END_KEY,
        PAGE_UP,
        PAGE_DOWN,
	ARROW_RIGHT_NO_SHOULDBE
};//copied from kilo.c
typedef struct{
    char *b;
    int len;
}abuf;

#define ABUF_INIT {NULL,0}

void abAppend(abuf *ab, const char *s, int len);
void abFree(abuf *ab);

void mylog(char *s, ...);
void logE();

int getWindowSize(int *rows, int *cols);
void disableRawMode(int fd);
int enableRawMode(int fd);
//int saveCursorPosition(Curpos *pos); 
void setCursorPosition(Curpos *pos); 

void setstatusmsg(int level, const char *msg, ...);


int hellopage();
int endpage();

void fileeditinit(Fileedit *e);
void readfile(FILE **f, char *file);
void do_free();

void clearscreen();
void refreshscreen();

int readkey();
void processkey();

void save();
void quit();
void help();

void movecursor(int arrow);
void inschar(int ch);
void insline();
void delchar();

#endif
