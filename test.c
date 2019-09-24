/*
 * =====================================================================================
 *
 *       Filename:  test.c
 *
 *    Description:  hstring lib test
 *
 *        Version:  1.0
 *        Created:  10/31/2017 08:35:59 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Yimin Gu (), guyimin@mail.ustc.edu.cn
 *        Company:  
 *
 * =====================================================================================
 */

#include <stdio.h>
#include "hstring.h"

int main(void)
{
	HString s, s2, s3;
	StrInit(&s);
	StrInit(&s2);
	StrInit(&s3);
	StrAssign(&s, "Hello, world!");
	StrAssign(&s2, "!");
	StrAssign(&s3, "lll");

	int pos;
	pos = KMPIndex(s, s2, 1);
	printf("pos %d\n", pos);
	pos = KMPIndex(s, s3, 1);
	printf("pos %d\n", pos);
	printf("check\n");


	DestroyString(&s);//pass
	DestroyString(&s2);//pass
	DestroyString(&s3);//pass
	return 0;
}
