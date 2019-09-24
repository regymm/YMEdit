/*
 * =====================================================================================
 *
 *       Filename:  myeditvar.c
 *
 *    Description:  myedir global variables
 *
 *        Version:  1.0
 *        Created:  11/11/2017 02:32:02 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Yimin Gu (), guyimin@mail.ustc.edu.cn
 *        Company:  
 *
 * =====================================================================================
 */

#include "myedit.h"
#include "hstring.h"
#include <stdio.h>
#include <termios.h>

Fileedit E;
FILE *F;
FILE *L;
int isediting = 0;
char *filename;
char *statusmsg;
abuf bufmsg;
//copied from kilo.c
struct termios orig_termios; /* In order to restore at exit.*/
Linepos start;
Linepos cursor;
//point to cursor position, and start showing of screen
Curpos saved;
//saved cursor positon on screen

