/*
 * =====================================================================================
 *
 *       Filename:  myeditterm.c
 *
 *    Description:  myedit terminal funcs, nearly all is copied from kilo.c
 *
 *        Version:  1.0
 *        Created:  11/11/2017 03:33:49 PM
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
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <errno.h>
#include "myedit.h"

//copied from kilo.c
extern struct termios orig_termios;
extern Fileedit E;
extern FILE *F;

void abAppend(abuf *ab, const char *s, int len) {
    char *new = realloc(ab->b,ab->len+len);

    if (new == NULL) return;
    memcpy(new+ab->len,s,len);
    ab->b = new;
    ab->len += len;
}

void abFree(abuf *ab) {
    free(ab->b);
}

void editorAtExit(void) {
    disableRawMode(0);
}

void disableRawMode(int fd) {
    /* Don't even check the return value as it's too late. */
    if (E.rawmode) {
        tcsetattr(fd,TCSAFLUSH,&orig_termios);
        E.rawmode = 0;
    }
}

/* Raw mode: 1960 magic shit. */
int enableRawMode(int fd) {
    struct termios raw;

    if (E.rawmode) return 0; /* Already enabled. */
    if (!isatty(STDIN_FILENO)) goto fatal;
    atexit(editorAtExit);
    if (tcgetattr(fd,&orig_termios) == -1) goto fatal;

    raw = orig_termios;  /* modify the original mode */
    /* input modes: no break, no CR to NL, no parity check, no strip char,
     * no start/stop output control. */
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    /* output modes - disable post processing */
    raw.c_oflag &= ~(OPOST);
    /* control modes - set 8 bit chars */
    raw.c_cflag |= (CS8);
    /* local modes - choing off, canonical off, no extended functions,
     * no signal chars (^Z,^C) */
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    /* control chars - set return condition: min number of bytes and timer. */
    raw.c_cc[VMIN] = 0; /* Return each byte, or zero for timeout. */
    raw.c_cc[VTIME] = 1; /* 100 ms timeout (unit is tens of second). */

    /* put terminal in raw mode after flushing */
    if (tcsetattr(fd,TCSAFLUSH,&raw) < 0) goto fatal;
    E.rawmode = 1;
    return 0;

fatal:
    errno = ENOTTY;
    return -1;
}

int getWindowSize(int *rows, int *cols) {
    struct winsize ws;
    if (ioctl(1, TIOCGWINSZ, &ws) >= 0) {
        //printf("ioctl: %d, %d\r\n", ws.ws_col, ws.ws_row);
        *cols = ws.ws_col;
        *rows = ws.ws_row;
        return 1;
    }
    return 0;
}//selectively copied from kilo.c

/* Use the ESC [6n escape sequence to query the horizontal cursor position
 * and return it. On error -1 is returned, on success the position of the
 * cursor is stored at *rows and *cols and 0 is returned. */
//int saveCursorPosition(Curpos *pos) {
//	//may be no longer needed?
//    int ifd = 0;
//    int ofd = 1;
//    char buf[32];
//    unsigned int i = 0;
//
//    /* Report cursor location */
//    if (write(ofd, "\x1b[6n", 4) != 4) return -1;
//
//    /* Read the response: ESC [ rows ; cols R */
//    while (i < sizeof(buf)-1) {
//        if (read(ifd,buf+i,1) != 1) break;
//        if (buf[i] == 'R') break;
//        i++;
//    }
//    buf[i] = '\0';
//
//    /* Parse it. */
//    if (buf[0] != ESC || buf[1] != '[') return -1;
//    if (sscanf(buf+2,"%d;%d",&pos->x,&pos->y) != 2) return -1;
//    return 0;
//}
void setCursorOrigin()
{
	printf("\033[1;1H");
}
void setCursorPosition(Curpos *pos)
{
	//char tmp[10];
	//sprintf(tmp, "\033[%d;%dH", pos->x, pos->y);
//	fflush(stdout);
	printf("\033[%d;%dH", pos->x, pos->y);
//	fflush(stdout);
	//printf("%s", tmp);
	//write(1, tmp, sizeof(tmp));
}
