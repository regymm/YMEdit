/*
 * =====================================================================================
 *
 *       Filename:  hstring.h
 *
 *    Description:  hstring.h
 *
 *        Version:  1.0
 *        Created:  10/24/2017 09:11:43 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Yimin Gu (), guyimin@mail.ustc.edu.cn
 *        Company:  
 *
 * =====================================================================================
 */

#ifndef HSTRING_H
#define HSTRING_H
#define TRUE 1
#define FALSE 0
#define OK 1
#define ERROR 0
#define INFEASIBLE -1
#define OVERFLOW -2

#define MAXSIZE 10000
#include <stdio.h>
typedef int Status;

typedef struct{
	char *ch;//no '\0'
	int length;
}HString;

Status StrInit(HString *s);
Status StrAssign(HString *s, char *chars);
Status DestroyString(HString *s);
Status ClearString(HString *s);
Status StrCopy(HString *dest, HString src);
Status ConCat(HString *dest, HString *s1, HString *s2);
Status StrAppend(HString *s, HString *t);
Status StrInsert(HString *s, int pos, HString t);//insert after pos, pass
Status StrInsertChar(HString *s, int pos, char c);//insert after pos, pass
Status StrDelete(HString *s, int pos, int len);//delete len chars, start from pos
Status SubString(HString s, HString *t, int pos, int len);
int StrEmpty(HString s);
int StrLength(HString s);
int StrCompare(HString s1, HString s2);
int KMPIndex(HString s, HString t, int pos);
void StrPrint(HString s);//no '\n'
void StrFPrint(FILE *f, HString s);
#endif
