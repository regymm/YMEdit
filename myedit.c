/*
 * =====================================================================================
 *
 *       Filename:  myedit.c
 *
 *    Description:  my simply wheel -- a small (may be the worst you 
 *    have ever seen) editor, a practice of datastructure string and others
 *
 *        Version:  1.0
 *        Created:  10/24/2017 09:11:53 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Yimin Gu (), guyimin@mail.ustc.edu.cn
 *        Company:  
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include "myedit.h"

//extern from myeditvar.c
extern Fileedit E;
extern FILE *F;
extern FILE *L;
extern char *filename;
extern Linepos start;
extern Linepos cursor;
extern int isediting;
extern struct termios orig_termios;

static void Fileeditinit()
{
	E.isdirty = 0;
	isediting = 1;
	if (getWindowSize(&(E.row), &(E.col)) == -1){
		//fail to get winsize, set to 80 * 24
		setstatusmsg(2, "Unable to get size, it may misbehave!");
	}
#ifdef VT100_LAMED
	E.row = 24;
	E.col = 80;
#endif
	readfile(&F, filename);
	start.linenum = 1;
	start.line = E.file;
	cursor.linenum = 1;
	cursor.line = E.file;
	cursor.col = 1;
	cursor.colshouldbe = 1;
	cursor.start = 1;
}
void processkey()
{
	int c = readkey();
	switch(c){
		case CTRL_S:
			save();
			break;
		case CTRL_C:
		case CTRL_D:
		case CTRL_Q:
			quit();
			break;
		case CTRL_G:
			//removed feature
//			find();
			break;
		case CTRL_L:
			setstatusmsg(0, "");
			clearscreen();
			refreshscreen();
			break;
		case CTRL_H:
			help();
		case ARROW_UP:
		case ARROW_DOWN:
		case ARROW_LEFT:
		case ARROW_RIGHT:
		case PAGE_UP:
		case PAGE_DOWN:
		case HOME_KEY:
		case END_KEY:
		case CTRL_A:
		case CTRL_E:
		case CTRL_B:
		case CTRL_F:
		case CTRL_N:
		case CTRL_P:
			movecursor(c);
			break;
		case ENTER:
			insline();
			break;
		case BACKSPACE:
		case DEL_KEY:
			delchar();
			break;
		default:
			inschar(c);
			break;
	}
}
int main(int argc, char *argv[])
{
#ifdef DEBUG
	if(!(L = fopen("log.txt", "w+")))
		exit(1);
	fprintf(L, "start\n");
#endif
	printf("%s, PID %d.\r\n", argv[0], getpid());
#ifdef DEBUG
	filename = "victim.txt";
#else
	if(argc != 2){
		printf("Usage: %s <filename>\r\n", argv[0]);
		exit(1);
	}
	filename = argv[1];
#endif
	E.rawmode = 0;
	if(!hellopage()){
		printf("This is a wise choice.\r\nGoodbye!\r\n");
		return 0;
	}
	Fileeditinit();
	//log==========
//	fprintf(L, "%d", E.linenum);
//	Line *l = E.file;
//	while(l){
//		fprintf(L, "linenum:%d\n", l->s->length);
//		l = l->next;
//	}
	//log==========
//	logE();
//	clearscreen();
//raw mode begin
	enableRawMode(0);
//	mylog("begin main loop...");
	while(isediting){
		logE();
#ifndef NOOUT
		refreshscreen();
		setstatusmsg(0, "^S to save, ^Q to quit, ^H to view help");
#endif
		processkey();
	}
	endpage();
	disableRawMode(0);
#ifdef DEBUG
	fclose(L);
#endif
	return 0;
}

