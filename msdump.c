/*
	MS-DOS like dump program.
		made by yusuke    function dump()
		     by ohirune   other important parts
		     by shin      some options
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HELP 1
#define ERROR -1
#define MAX_FILES 256

struct {
  int  file_num;
  char* file_name[MAX_FILES];
  int given_adr_high;
} option;


int dump (FILE *fp)
{
  /*dump*/
  int c=0;
  int l[16];
  int count;
  int adr_low=0;
  int adr_high = option.given_adr_high;
  int show_end = 16;
  int i, trash;

  for( i = 0; i < adr_high; i++ ) {
    trash = fgetc( fp );
    if( trash == EOF ) return ERROR;
  }
  while(c!=EOF) {
    c = 0;
    if(adr_low==0) {
      for(count=0;count<16;count++) {
	l[count]=fgetc(fp);
      }
      printf( "%8.8X  ", adr_high );
    }
    c = l[adr_low];
    if( c != EOF ) {
      printf("%2.2X",c);
    } else {
      printf( " " );
      show_end = adr_low;
      for( ; adr_low < 16; adr_low++ ) printf( "   " );
    }
    if( adr_low == 7 )printf("-");
    else printf(" ");
    adr_low++;
    if(adr_low==16) {
      adr_low=0;
      adr_high+=16;
      printf("  ");
    }
    if( adr_low == 0 || c == EOF ) {
      for(count=0;count<show_end;count++) {
#ifndef COLOR
	if( isprint( l[count] ) ) printf("%c",l[count]);
	else printf(".");
#else
	if( iscntrl(l[count]) )
	    printf("\033[32m%c\033[m", l[count] + '@');
	else if( isprint( l[count] ) )
	    printf("\033[33m%c\033[m", l[count]);
	else
	    printf(".");
#endif
      }
      printf("\n");
    }
  }
  return 0;
}

int paramchk (int argv, char **argc)
{
  int i, j;
  
  for (i = 1; i < argv; i++) {
    if (argc[i][0] != '-') {
      break;
    }
    if (!strcasecmp (argc[i],"-h")){
      return HELP;
    } 
    if ( argc[i][1] == 'x' || argc[i][1] == 'X' ) {
      if( sscanf( argc[i] + 2, "%x", &option.given_adr_high ) != 1 ) {
	option.given_adr_high = 0;
      }
    }
    if ( isdigit( argc[i][1] ) ) {
      if( sscanf( argc[i] + 1, "%d", &option.given_adr_high ) != 1 ) {
	option.given_adr_high = 0;
      }
    }
    /* else if ... <<switch check>> */
  }

  for (j = 0; i < argv && j < MAX_FILES; i++, j++)
    option.file_name[j] = argc[i];

  option.file_num = j;

  return 0;
}    

       
int main (int argv, char **argc)
{
  FILE *fp_in;
  int i;

  if (paramchk(argv, argc) == HELP){
    printf( "MS-DOS like dump program. <Presented by KMC>\n" );
    printf( "Usage : msdump [-<offset>|-x<hex-offset>] [file...]\n" );
    return 0;
  }
  if (option.file_num == 0){
    if (dump (stdin) == ERROR){
      fprintf (stderr, "dump error:stdin\n");
      return 1;
    }
  } else {
    for (i = 0; i < option.file_num; i++ ) {
      if ((fp_in = fopen (option.file_name[i], "r")) == NULL){
	fprintf (stderr, "cannot open file:%s\n", option.file_name[i]);
	return 1;
      }
      if (dump (fp_in) == ERROR){
	fprintf (stderr, "dump error:%s\n", option.file_name[i]);
	return 1;
      }
      fclose (fp_in);
    }
  }

  return 0;
}

