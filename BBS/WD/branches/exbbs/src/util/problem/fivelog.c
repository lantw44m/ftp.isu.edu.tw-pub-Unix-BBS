//#include "bbs.h"
#include<stdio.h>
#include<stdlib.h>
#include<conio.h>
#include<string.h>
#define MAXUSERS 300

struct fivelog
{
	char username[20];
	int  win;
	int  lose;
	int  equ;
	int person;
};

typedef struct fivelog fivelog;
struct fivelog allman[MAXUSERS];
int strfind(char *, char *, int);
void strget(char *, int, int, char *);
void userlog(char *, int);
void sortlog(void);
void xchglog(fivelog *, fivelog *);
int top=0;

void main(int argc, char **argv)
{
  FILE *inf,*inf2;
  char s1[100],s2[20],s3[20],s4[20];
  int i,j,k,w,row;

  if (argc != 4)
  {
	  printf("usage error!\n");
	  exit(-1);
  }

//  inf = fopen(BBSHOME "/log/f.log", "r");
  inf = fopen(argv[1],"r");
//  inf2 = fopen(BBSHOME "/log/f.log2", "w");
  inf2 = fopen(argv[2],"w");

  if (inf == NULL || inf2 == NULL)
  {
	 printf("Sorry, the data is not ready.\n");
	 exit(-1);
  }

  row=atoi(argv[3]);

  for (i=0; fgets(s1,sizeof(s1),inf) ;i++)
  {
	  j = strfind(s1,"[32m",0)+1;
	  k = strfind(s1," ",j)-1;
	  strget(s1,j,k,s2);
	  j = strfind(s1,"[36m",j)+1;
	  j = strfind(s1," ",j)+1 ;
	  k = strfind(s1," ",j)-1;
	  strget(s1,j,k,s3);
	  j = strfind(s1," ",k)+1;
	  strget (s1,j,j+3,s4);

	  if (strcmp(s4,"戰和")==0)
		  w=0;
	  else
		  w=1;
	  userlog(s2,w);
	  userlog(s3,-w);
  }

  sortlog();

  fprintf(inf2,"----五子棋對戰排行榜-----------------------\n");
  fprintf(inf2,"    名次  USERID     勝場  敗場  和棋  勝率\n");
  for (i=0; i<row; i++)
  {
	  fprintf(inf2,"     %2d %-13s %3d  %4d  %4d  %3d\%\n",i+1,allman[i].username,
		  allman[i].win,allman[i].lose,allman[i].equ,allman[i].person);
  }

  fclose(inf);
  fclose(inf2);
}

int strfind(char *a, char *b, int st)
{
	int i, j, k=strlen(a) , l=strlen(b);

	for (i=st,j=0;i<k;i++)
	{
		if (a[i]==b[j])
			j++;
		if (j==l)
			return i;
	}
	return -1;
}

void strget(char *s, int a, int b,char t[20])
{
	for(int i=a ; i<=b ; i++)
		t[i-a]=s[i];
	t[i-a]='\0';
}

void userlog(char name[20], int w)
{
	int i;
	for (i=0; i<top ; i++)
		if (strcmp(allman[i].username,name)==0)
			break;

	if (i==top)
	{
		top++;
		strcpy(allman[i].username,name);
		allman[i].win=0;
		allman[i].lose=0;
		allman[i].equ=0;
		allman[i].person=0;
	}

	if (w == 1)
		allman[i].win++;
	else if (w == -1)
		allman[i].lose++;
	else if (w == 0)
		allman[i].equ++;

	allman[i].person = (float) allman[i].win / (allman[i].win+allman[i].lose+allman[i].equ) * 100;
}

void sortlog(void)
{
	int i,j;

	for (i=0;i<top-1;i++)
		for (j=i+1;j<top;j++)
		{
			if (allman[i].win < allman[j].win)
				xchglog(&allman[i], &allman[j]);

			else if (allman[i].win == allman[j].win)
				  if (allman[i].person < allman[j].person)
					  xchglog(&allman[i], &allman[j]);

				  else if (allman[i].person == allman[j].person)
						 if (allman[i].lose > allman[j].lose)
							  xchglog(&allman[i], &allman[j]);
		}
}

void xchglog(fivelog *a, fivelog *b)
{
		struct fivelog tmpman;
		strcpy(tmpman.username,a->username);
		tmpman.win=a->win;
		tmpman.lose=a->lose;
		tmpman.equ=a->equ;
		tmpman.person=a->person;
		strcpy(a->username,b->username);
		a->win=b->win;
		a->lose=b->lose;
		a->equ=b->equ;
		a->person=b->person;
		strcpy(b->username,tmpman.username);
		b->win=tmpman.win;
		b->lose=tmpman.lose;
		b->equ=tmpman.equ;
		b->person=tmpman.person;
}