/*
 * =====================================================================================
 *
 *       Filename:  hstring.c
 *
 *    Description:  hstring.c , a practice of datasturcture
 *
 *        Version:  1.0
 *        Created:  10/24/2017 09:11:19 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Yimin Gu (), guyimin@mail.ustc.edu.cn
 *        Company:  
 *
 * =====================================================================================
 */

#include "hstring.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

Status StrInit(HString *s)
{
	s->ch = NULL;
	s->length = 0;
	return OK;
}
Status StrAssign(HString *s, char *chars)
{
	if(s->ch) free(s->ch);
	int length = 0;
	char *c;
	for(length = 0, c = chars; *c; length++, c++);
	if(!length){
		s->ch = NULL;
		s->length = 0;
	}
	else{
		if(!(s->ch = (char *)malloc(length * sizeof(char))))exit(OVERFLOW);
		s->length = length;
		int i;
		for(i = 0; i < length; i++) s->ch[i] = chars[i];
	}
	return OK;
} 
Status DestroyString(HString *s)
{
	if(s->ch) free(s->ch);
	return OK;
}
Status ClearString(HString *s)
{
	if(s->ch) {free(s->ch); s->ch = NULL;}
	s->length = 0;
	return OK;
}
Status StrCopy(HString *dest, HString src)
{
	if(dest->ch) free(dest->ch);
	if(!(dest->ch = (char *)malloc(src.length * sizeof(char)))) exit(OVERFLOW);
	dest->length = src.length;
	memcpy(dest->ch, src.ch, src.length * sizeof(char));
	return OK;
} 
Status ConCat(HString *dest, HString *s1, HString *s2)
{
	if(dest->ch) free(dest->ch);
	if(!(dest->ch = (char *)malloc((s1->length + s2->length) * sizeof(char))))
		exit(OVERFLOW);
	dest->length = s1->length + s2->length;
	memcpy(dest->ch, s1->ch, s1->length * sizeof(char));
	memcpy(dest->ch + s1->length - 1, s2->ch, s2->length * sizeof(char));
	return OK;
}
Status StrInsert(HString *s, int pos, HString t)
{
	if(pos < 0 || pos > s->length) return ERROR;
	if(!(s->ch = (char *)realloc(s->ch, (s->length + t.length) * sizeof(char)))) exit(OVERFLOW);
	int i;
	for(i = s->length + t.length - 1; i >= t.length - 1 + pos; i--)
	s->ch[i] = s->ch[i - t.length];
	memcpy(s->ch + pos, t.ch, t.length * sizeof(char));
	s->length += t.length;
	return OK;
}
Status StrAppend(HString *s, HString *t)
{
	if(!(s->ch = realloc(s->ch, s->length + t->length)))
		exit(OVERFLOW);
	int i;
	for(i = 0; i < t->length; i++)
		s->ch[s->length + i] = t->ch[i];
	s->length += t->length;
	return OK;
}
Status StrInsertChar(HString *s, int pos, char c)
{
	HString sing;
	StrInit(&sing);
	char t[1];
	t[0] = c;
	StrAssign(&sing, t);
	if(!StrInsert(s, pos, sing)){
		DestroyString(&sing);
		return ERROR;
	}
	DestroyString(&sing);
	return OK;
}
Status StrDelete(HString *s, int pos, int len)
{
	if(pos < 1 || pos > s->length || len < 0 || len > s->length - pos + 1)
		return ERROR;
	if(s->length == 1){
		free(s->ch);
		s->ch = NULL;
		s->length = 0;
		return OK;
	}
	pos--;
	int i = 0;
	for(i = 0; i < s->length - len - pos; i++)
		s->ch[pos + i] = s->ch[pos + i + len];
	s->length -= len;
	if(!(s->ch = (char *)realloc(s->ch, s->length * sizeof(int)))) exit(OVERFLOW);
	return OK;
} 
Status SubString(HString s, HString *t, int pos, int len)
{
	if(pos < 1 || pos > s.length || len < 0 || len > s.length - pos + 1) return ERROR;
	t->length = len;
	if(!(t->ch = (char *)malloc(len * sizeof(char)))) exit(OVERFLOW);
	memcpy(t->ch, s.ch, len * sizeof(int));
	return OK;
}
int StrEmpty(HString s)
{
	return s.ch ? 1 : 0;
}
int StrLength(HString s)
{
	return s.length;
}
int StrCompare(HString s1, HString s2)
{
	int i = 0;
	while(i < s1.length && i < s2.length){
		/*
		if(s1.ch[i] < s1.ch[i]) return -1;//less than
		if(s1.ch[i] > s1.ch[i]) return 1;//greater than
		*/
		if(s1.ch[i] != s2.ch[i]) return (s1.ch[i] - s2.ch[i]);
		i++;
	}
	/*
	if(s1.length > s2.length) return 1;//greater than
	if(s1.length < s2.length) return -1;//less than
	if(s1.length == s2.length) return 0;//equal
	*/
	//writen by me, so verbose!
	return s1.length - s2.length;
}
static void get_next(HString s, int *next)
{
	int i = 0, j = -1;
	next[0] = -1;
	while(i < s.length - 1){
		if(j == -1 || s.ch[i] == s.ch[j]){
			i++;
			j++;
			if(s.ch[i] !=s.ch[j]) next[i] = j;
			else next[i] = next[j];
		}
		else j = next[j];
	}
}
/*
static void get_next(HString s, int *next)
{
	int i = 0, j = -1;
	next[0] = -1;
	while(i < s.length - 1){
		if(j == -1 || s.ch[i] == s.ch[j]){
			i++;
			j++;
			next[i] = j;
		}
		else j = next[j];
	}
}
*/
//another get_next (not better than the first)
int KMPIndex(HString s, HString t, int pos)
{
	int i = pos - 1;
	int j = 0;
	int *next;
	if(!(next = (int *)malloc(t.length * sizeof(int)))) exit(OVERFLOW);
	get_next(t, next);
	//int k;
	//for(k = 0; k < t.length; k++) printf("next:%d\n", next[k]);
	while(i < s.length && j < t.length){
		if(j == -1 || s.ch[i] == t.ch[j]){ i++; j++; }
		else j = next[j];
	}
	if(j == t.length) return i - t.length + 1;
	return 0;
}
void StrPrint(HString s)
{
	int i = 0;
	for(i = 0; i < s.length; i++) putchar(s.ch[i]);
}
void StrFPrint(FILE *f, HString s)
{
	int i = 0;
	for(i = 0; i < s.length; i++)
		fprintf(f, "%c", s.ch[i]);
}

