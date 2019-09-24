/*
 * =====================================================================================
 *
 *       Filename:  myeditio.c
 *
 *    Description:  myedit input and output functions
 *
 *        Version:  1.0
 *        Created:  11/11/2017 02:28:40 PM
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
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include "myedit.h"
#include "hstring.h"

extern Fileedit E;
extern FILE *F;
extern FILE *L;
extern char *filename;
extern char *statusmsg;
extern abuf bufmsg;
extern int isediting;
extern Linepos start;
extern Linepos cursor;
extern Linepos end;
extern Curpos saved;
void mylog(char *s, ...)
{
#ifdef DEBUG
//	fputs(s, L);
	char sfinal[MAXBUF];
	va_list ap;
	va_start(ap, s);
	vsnprintf(sfinal, MAXBUF, s, ap);
	fprintf(L, "log: %s\n", sfinal);
#endif
}
void logE()
{
#ifdef DEBUG
	fprintf(L, "logE:\n   isdirty:   %d\ncol:   %d\nrow:   \
%d\nreadonly:   %d\nrawmode:   %d\n \
linenum:   %d\n", E.isdirty, E.col, E.row, E.readonly,
			E.rawmode, E.linenum);
	fprintf(L, "  cursor.linenum:%d\n", cursor.linenum);
	fprintf(L, "  cursor.col:%d\n", cursor.col);
	Line *p = E.file;
	fprintf(L, "--start--\n");
	while(p){
		StrFPrint(L, *(p->s));
		fprintf(L, "\n");
		p = p->next;
	}
	fprintf(L, "--end--\n");
	
#endif
}
void outofmem()
{
	printf("OUT OF MEMORY!!");
	disableRawMode(0);
	exit(2);
}
int hellopage()
{
	enableRawMode(0);
//	clearscreen();
	printf("\033[0;37mWelcome to Myedit!\033[0m\r\n");
	printf("Codes available at  %s\r\n", GITHUB);
	printf("If you find any bug, or have any suggestion, you can mail to %s\r\n", EMAIL);
	printf("But I may not have the ability and time to fix them\r\n");
	printf("\033[0;31mATTENTION!\033[0m\r\n\
I'm not responsible for (more than) these occasions:\r\n\
1> You lost your file (I mean FILE, not only CHANGE) you are editing\r\n\
2> You lost your file simply unmoved on your disk\r\n\
3> You terminal misbehave after running this program\r\n\
4> You pick up wrong words or grammars from these text messages, \r\n\
use them in your own writings, and punished by your teachers\r\n\
5> Any other errors and misfortunes\r\n\
By %s, USTC, 2017\r\n\
", AUTHOR);
	printf("Decide what to do: e to edit, r to open read-only, q to quit\r\n");
	char c;
	while((c = readkey())){
		if(c == 'q') return 0;
		if(c == 'e'){
			E.readonly = 0;
			return 1;
		}
		if(c == 'r'){
			E.readonly = 1;
			return 1;
		}
	}
	disableRawMode(0);
	return 0;
}
void setstatusmsg(int level, const char *msg, ...)
{
	char msgfinal[MAXBUF];
	va_list ap;
	va_start(ap, msg);
	vsnprintf(msgfinal, E.col, msg, ap);
	if(bufmsg.b) abFree(&bufmsg);
	abuf bufmsg = {NULL, 0};
	//saveCursorPosition(&saved);
	switch (level){
		case 0://ok
			//write(1, "\033[36m", 5);
			abAppend(&bufmsg, "\033[36m", 5);
			break;
		case 1://warning
			//write(1, "\033[35m", 6);
			abAppend(&bufmsg, "\033[35m", 5);
			//printf("\033[35m");
			break;
		case 2://error
			//write(1, "\033[31m", 5);
			abAppend(&bufmsg, "\033[31m", 5);
			//printf("\033[33m");
			break;
		case -1://do nothing
		default:
			break;
	}
	if(strlen(msgfinal) > E.col){
//		int i;
//		abAppend(&bufmsg, "\r", 1);
		//putchar('\r');
//		for(i = 0; i < E.col; i++)
//			abAppend(&bufmsg, msgfinal[i], 1);
		abAppend(&bufmsg, msgfinal, E.col);
//		abAppend(&bufmsg, "\r", 1);
		//putchar('\r');
	}
	else{
//		abAppend(&bufmsg, "\r", 1);
		abAppend(&bufmsg, msgfinal, strlen(msgfinal));
//		abAppend(&bufmsg, "\r", 1);
	}
	//printf("%ld", write(1, "hel\x1b[0mlo", 9));
	if(level != -1) abAppend(&bufmsg, "\033[0m", 4);
	abAppend(&bufmsg, "\0", 1);
	//write(1, bufmsg.b, tmp.len);
	statusmsg = bufmsg.b;
//	setCursorPosition(&saved);
}
int endpage()
{
	clearscreen();
//	enableRawMode(0);
	printf("\rThank you for using!\r\n");
	printf("\rPress any key to quit.\r\n");
//	disableRawMode(0);
	return 1;
}
void clearscreen()
{
	printf("\033c");
}
void printaline(int num, HString *s, int start, int max)
{
	//struct abuf ab = ABUF_INIT;
	printf("\r");
	printf("%3d", num);//line number
	if(start == 1) printf("|");
	else printf("<");
	int i;
	for(i = 1; i <= max - 5; i++){
		if(i + start - 1 <= s->length){
			if(s->ch[start + i - 2] == '\t') printf("\033[;32mt\033[0m");
			//'\t' is a green t, to make sure every char only need one position
			else printf("%c", s->ch[start + i - 2]);
		}
		else printf(" ");
	}
	if(s->length - start + 1> max - 5) printf(">");
	else printf("|");
	printf("\r");
}
void middleprint(char *s, int len)
{
	int slen = strlen(s);
	int i;
	printf("\r");
	if(slen > len){
		for(i = 1; i < len - 3; i++)
			putchar(s[i]);
		printf("...");
	}
	else{
		for(i = 0; i < (len - slen) / 2; i++) printf(" ");
		printf("%s", s);
		for(i = 0; i < (len - slen) / 2; i++) printf(" ");
	}
	printf("\r");
}
void leftprint(char *s, int len)
{
	printf("\r");
	int i;
	if(strlen(s) > len){
		for(i = 0; i < len - 3; i++)
			putchar(s[i]);
		printf("...");
	}
	else
		printf("%s", s);
	printf("\r");
}
void refreshscreen()
{
	printf("\033[?25l\033[H");
	//hide cursor and go home
	char tmp[1000];
	//title
	sprintf(tmp, "MyEdit - %s", filename);
	printf("\033[0;37m");//white title
	middleprint(tmp, E.col);
	printf("\033[0m\r\n");
	int i;
	Line *l = start.line;
	for(i = 0; i <= E.row - 3; i++){
		if(l){
//			if(l == cursor.line)//the line editing
//				printaline(start.linenum + i, l->s, cursor.start, E.col);
//			else
			printaline(start.linenum + i, l->s, cursor.start, E.col);
			printf("\r\n");
			l = l->next;
		}
		else{
			HString empty;
			empty.length = 0;
			empty.ch = NULL;
			printaline(start.linenum + i, &empty, 1, E.col);
			printf("\r\n");
		}
	}

	for(i = 0; i < E.col; i++)
		printf(" "); //overwrite old msg
	printf("\r");
	printf("%s", statusmsg); //last status bar
//	mylog("%s", statusmsg);
//	fflush(stdout);
//	mylog(statusmsg);
	//mylog(statusmsg);
	//abFree(&bufmsg);

	//now move the cursor to the pos to edit
	saved.x = cursor.linenum - start.linenum + 2;
	saved.y = cursor.col - cursor.start + 5;
	setCursorPosition(&saved);

	//struct abuf ab = ABUF_INIT;
	//abAppend(&ab,"\x1b[?25l",6); /* Hide cursor. */
	//abAppend(&ab,"\x1b[H",3); /* Go home. */
	//abAppend(&ab,"\x1b[?25h",6); /* Show cursor. */
	//write(1,ab.b,ab.len);
	//abFree(&ab);
	
	printf("\033[?25h"); //show cursor
	fflush(stdout);
}
void readfile(FILE **f, char *file)
{
	if((*f = fopen(file, "r")) == NULL){
		int err = errno;
		printf("Can not open file %s: %d\n\r", file, errno);
		disableRawMode(0);
		exit(err);
	}
	E.linenum = 0;
	char tmp[MAXBUF];
	//if(!(E.file = (File)malloc(sizeof(Line))))
		//outofmem();
	Line *p, *q = NULL;
	while(fgets(tmp, MAXBUF, *f)){
		E.linenum++;
		if(!(p = (Line *)malloc(sizeof(Line))))
			outofmem();
		if(!q) E.file = p;//first time, set E.file
		if(!(p->s = (HString *)malloc(sizeof(HString))))
			outofmem();
		if(tmp[strlen(tmp) - 1] == '\n')
			tmp[strlen(tmp) - 1] = '\0'; //no last '\n'
		p->s->length = strlen(tmp);
		StrAssign(p->s, tmp);
		p->back = q;
		if(q) q->next = p;//first time judge
		q = p;//q = q->next;
	}
	fclose(*f);
	if(q)
		p->next = NULL;
	else
		E.file = NULL;//empty file
	setstatusmsg(0, "file read successfully.");
}
void do_free()
{
	Line *p = E.file, *q;
	while(p){
		q = p->next;
		free(p->s);
		free(p);
		p = q;
	}
}
int readkey()
{
	int nread;
	char c, seq[3];
	while((nread = read(0, &c, 1)) == 0);
	if(nread == -1) exit(1);
	while(1){
		switch(c){
		case ESC:
			if(read(0, seq, 1) == 0) return ESC;
			if(read(0, seq + 1, 1) == 0) return ESC;
			if(seq[0] == '['){
				if(seq[1] >= '0' && seq[1] <= '9'){
					if(read(0, seq + 2, 1) == 0) return ESC;
					if(seq[2] == '~'){
						switch(seq[1]){
						case '3':return DEL_KEY;
						case '5':return PAGE_UP;
						case '6':return PAGE_DOWN;
						}
					}
				}
				else{
					switch(seq[1]){
					case 'A': return ARROW_UP;
					case 'B': return ARROW_DOWN;
					case 'C': return ARROW_RIGHT;
					case 'D': return ARROW_LEFT;
					case 'E': return HOME_KEY;
					case 'F': return END_KEY;
					}
				}
			}
			else if(seq[0] == 'O'){
				switch(seq[1]){
				case 'H': return HOME_KEY;
				case 'F': return END_KEY;
				}
			}
			break;
		
		default: return c;
		}
	}
}//copied and modified from kilo.c
void save()
{
	if(!E.isdirty){
		setstatusmsg(0, "no change since last save");
		return;
	}
	if((F = fopen(filename, "w")) == NULL){
		setstatusmsg(2, "Unable to save file: %d", errno);
		char filebarename[1000] = "";
		strcpy(filebarename, "/tmp/");
		int i, istart;
		for(i = strlen(filename) - 1; i >= 0; i--)
			if(filename[i] == '/')
				break;
		istart = i;
//		setstatusmsg(2, "%d", i);
		for(i = istart + 1; i < strlen(filename); i++)
			filebarename[5 + i - istart - 1] = filename[i];
//		setstatusmsg(2, "%s", filebarename);
		if((F = fopen(filebarename, "w")) == NULL){
			setstatusmsg(2, "unable to save to %s:%d, Deal with it yourself", filebarename, errno);
			return;
		}
		Line *p = E.file;
		while(p){
			for(i = 0; i < p->s->length; i++)
				fputc(p->s->ch[i], F);
			p = p->next;
			if(p) fputc('\n', F);
		}
		fclose(F);
		E.isdirty = 0;
		setstatusmsg(1, "file saved to %s(unable to save origin file)", filebarename);
		return;
	}
	int i;
	Line *p = E.file;
	while(p){
		for(i = 0; i < p->s->length; i++)
			fputc(p->s->ch[i], F);
		if(p->next) fputc('\n', F);
		p = p->next;
	}
	fclose(F);
	E.isdirty = 0;
	setstatusmsg(0, "file saved");
}
void quit()
{
	if(E.isdirty){
		setstatusmsg(0, "File dirty!ctrl-Q 2 more times to quit!");
		refreshscreen();
		if(readkey() == CTRL_Q){
			setstatusmsg(0, "File dirty!ctrl-Q 1 more times to quit!");
			refreshscreen();
			if(readkey() == CTRL_Q){
				isediting = 0;
				return;
				do_free();
			}
			else return;
		}
		else
			return;
	}
	else
		do_free();
		isediting = 0;
	return;
}
void help()
{
	clearscreen();
	printf("\033[0;37mMyEdit help -- version %s\033[0m\r\n", VERSION);
	printf("^S to Save\r\n\
^C, ^D, or ^Q to quit, ^Q three times to abandon changes and force quit\r\n\
^H to view this help message\r\n\
Use Arrow heads or Emacs-like ^F, ^B, ^N, ^P to move\r\n\
If your terminal misbehave after this program crashed, just type\r\n\
$reset\r\n\
and every thing will be OK.\r\n\
");
	printf("Read source code yourself to get more help at %s.\r\n", GITHUB);
	printf("Any key to return ...\r\n");
	readkey();
	return;
}

