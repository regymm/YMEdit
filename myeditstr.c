/*
 * =====================================================================================
 *
 *       Filename:  myeditstr.c
 *
 *    Description:  myedit string processing funcs
 *
 *        Version:  1.0
 *        Created:  11/11/2017 02:39:36 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Yimin Gu (), guyimin@mail.ustc.edu.cn
 *        Company:  
 *
 * =====================================================================================
 */

#include "hstring.h"
#include "myedit.h"
#include <stdlib.h>

extern Fileedit E;
extern Linepos start;
extern Linepos cursor;
extern Linepos end;

static int mymin(int a, int b)
{
	return a < b ? a : b;
}
void movecursor(int arrow)
{
	//cursor.col
	//cursor.linenum
	int i;
	switch (arrow){
	case ARROW_RIGHT:
	case CTRL_F:
		if(cursor.col == cursor.line->s->length + 1) return;
		if(cursor.col == cursor.start + E.col - 6) cursor.start++;
		cursor.col++;
		cursor.colshouldbe = cursor.col;
		break;
	case ARROW_RIGHT_NO_SHOULDBE:
		if(cursor.col == cursor.line->s->length + 1) return;
		if(cursor.col == cursor.start + E.col - 6) cursor.start++;
		cursor.col++;
		break;
	case HOME_KEY:
	case CTRL_A:
		cursor.start = cursor.col = cursor.colshouldbe = 1;
		break;
	case END_KEY:
	case CTRL_E:
		for(i = cursor.col; i <= cursor.line->s->length; i++)
			movecursor(ARROW_RIGHT);
		break;
	case ARROW_LEFT:
	case CTRL_B:
		if(cursor.col == 1) return;
		if(cursor.col == cursor.start) cursor.start--;
		cursor.col--;
		cursor.colshouldbe = cursor.col;
		break;
	case ARROW_DOWN:
	case CTRL_N:
		if(cursor.linenum == E.linenum) return;
		if(cursor.linenum == start.linenum + E.row - 3){
			start.linenum++;
			start.line = start.line->next;
		} 
		cursor.linenum++;
		cursor.line = cursor.line->next;
		if(cursor.col > cursor.line->s->length){
			cursor.start += cursor.line->s->length + 1 - cursor.col;
			if(cursor.start <= 0) cursor.start = 1;
			cursor.col = cursor.line->s->length + 1;
//			int delta = cursor.line->s->length + 1 - cursor.col;
//			cursor.start += delta;
//			if(cursor.start <= 0) cursor.start = 1;
//			cursor.col += delta;
		}
//		if(cursor.colshouldbe <= cursor.line->s->length){
//			int i;
//			for(i = cursor.col; i < cursor.colshouldbe; i++)
//				movecursor(ARROW_RIGHT);
//		}
			for(i = cursor.col; i < cursor.colshouldbe; i++)
				movecursor(ARROW_RIGHT_NO_SHOULDBE);
		break;
	case ARROW_UP:
	case CTRL_P:
		if(cursor.linenum == 1) return;
		if(cursor.linenum == start.linenum) {
			start.linenum--;
			start.line = start.line->back;
		}
		cursor.linenum--;
		cursor.line = cursor.line->back;
		if(cursor.col > cursor.line->s->length){
			cursor.start += cursor.line->s->length + 1 - cursor.col;
			if(cursor.start <= 0) cursor.start = 1;
			cursor.col = cursor.line->s->length + 1;
		}
		//if(cursor.colshouldbe <= cursor.line->s->length){
			for(i = cursor.col; i < mymin(cursor.colshouldbe, cursor.line->s->length + 1); i++)
				movecursor(ARROW_RIGHT_NO_SHOULDBE);
		//}
		break;
	}
//	char t[25];
//	//sprintf(t, "line %d, col %d, start %d", cursor.linenum, cursor.col, cursor.start);
//	//setstatusmsg(t, 2);
//	sprintf(t, "shouldbe: %d", cursor.colshouldbe);
//	setstatusmsg(2, "shouldbe: %d", cursor.colshouldbe);
}
void inschar(int ch)
{
	if(E.readonly){
		setstatusmsg(2, "read only file!");
		return;
	}
	if(ch != '\t' && (ch < 32 || ch > 126)){
		setstatusmsg(1, "not recognizable char");
		return;
	}
	StrInsertChar(cursor.line->s, cursor.col - 1, ch);
	movecursor(ARROW_RIGHT);
	E.isdirty = 1;
}
void insline()//ENTER
{
	if(E.readonly){
		setstatusmsg(2, "read only file!");
		return;
	}
	int i;
	Line *newline = (Line *)malloc(sizeof(Line));
	newline->s = (HString *)malloc(sizeof(HString));
	newline->s->ch = (char *)malloc((cursor.line->s->length - cursor.col + 1)
			* sizeof(char));
	for(i = cursor.col - 1; i < cursor.line->s->length; i++)
		newline->s->ch[i - cursor.col + 1] = cursor.line->s->ch[i];
	newline->s->length = cursor.line->s->length - cursor.col + 1;
	cursor.line->s->ch = (char *)realloc(cursor.line->s->ch, (cursor.col - 1)
			* sizeof(char));
	cursor.line->s->length = cursor.col - 1;
	newline->next = cursor.line->next;
	newline->back = cursor.line;
	if(cursor.line->next) cursor.line->next->back = newline;
	cursor.line->next = newline;
	E.linenum++;
	movecursor(HOME_KEY);
	movecursor(ARROW_DOWN);
	E.isdirty = 1;
}
void delchar()//BACKSPACE, DELETE
{
	if(E.readonly){
		setstatusmsg(2, "read only file!");
		return;
	}
	if(!StrDelete(cursor.line->s, cursor.col - 1, 1)){
		if(cursor.line->back == NULL) return;//first line
		//else: need to del a line
		int oldlen = cursor.line->back->s->length;
		StrAppend(cursor.line->back->s, cursor.line->s);
		DestroyString(cursor.line->s);
		Line *p = cursor.line;
		cursor.line->back->next = cursor.line->next;
		if(cursor.line->next != NULL) cursor.line->next->back = cursor.line->back;
		cursor.line = cursor.line->back;
		cursor.linenum--;
		if(start.linenum > cursor.linenum){
			start.linenum--;
			start.line = start.line->back;
		} 
		free(p);
		E.linenum--;
		movecursor(HOME_KEY);
		int i;
		for(i = 0; i < oldlen; i++)
			movecursor(ARROW_RIGHT);
	}
	else
		movecursor(ARROW_LEFT);
	E.isdirty = 1;
}




