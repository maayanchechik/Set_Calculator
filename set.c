#define _GNU_SOURCE

#include "set.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int string_ends_with_comma_and_blanks(char* s);
int set_is_empty(Set* A);
int string_blank_or_empty(char* s);
int token_has_bad_chars(char* str);
int token_has_bad_blanks(char* str);
void add_num_to_set(Set* A, int num);
void erase_set(Set *A);
void read_set(Set* A, char* s);
void print_set(Set* A);
void union_set(Set* A, Set* B, Set *C);
void intersect_set(Set* A, Set* B, Set *C);
void sub_set(Set* A, Set* B, Set *C);
void symdiff_set(Set* A, Set* B, Set *C);


/* This function goes over the end of an input string and retruns
   whether there is a comma after the text.*/
int string_ends_with_comma_and_blanks(char* s){
  int i;
  /* If the string is empty or null it cannot end with a comma.*/
  if(s == NULL|| strlen(s) == 0){
    return 0;
  }
  i = strlen(s) -1;
  /* While the char is an end or a space or a tab or a comma, check comma.*/
  while ((i>=0) &&
	 ((s[i] == ' ')||(s[i] == '\t')||(s[i] == ','))) {
    if (s[i] == ',')
      return 1;
    i--;
  }
  return 0;
}

/* This function receives a pointer to a set and returns 1 if the set
   is empty.*/
int set_is_empty(Set* A){
  int i;
  int  empty = 1;
  for(i = 0; i<LENGTH; i++){
    if((*A)[i] != 0){
      empty = 0;
    }
  }
  return empty;
}

/* This function receives a string and goes through the string
   returning 0 if the string has somthing other than blanks.*/
int string_blank_or_empty(char* s){
  int i;
  if (s == NULL) {
    return 1;
  }
  for (i = 0; s[i] != '\0'; i++) {
    if (s[i] != ' ' && s[i]!= '\t') {
      return 0;
    }
  }
  return 1;
}


/* Check if the token contains illegal characters */
int token_has_bad_chars(char* str){
  int i;
  for (i = 0; str[i] != '\0'; i++){
    if ((str[i]-'0') < 0 ||  (str[i]-'0') > 9) {
      if (str[i] != ' ' && str[i] != '\t' && str[i] != '-') {
	return 1;
      }
    }
  }
  return 0;
}

  
/* Check if the token has blanks in middle of number */
int token_has_bad_blanks(char* str){
  int was_digit = 0;
  int is_digit = 0;
  int counter = 0;
  int i;
  for (i = 0; str[i] != '\0'; i++){
    if ((str[i]-'0') <= 9 && (str[i]-'0') >=0) {
      is_digit = 1;
    } else {
      is_digit = 0;
    }    
    if (was_digit == 0 && is_digit == 1) {
      counter++;
    }
    was_digit = is_digit;
  }
  if (counter>1) {
    return 1;
  }
  return 0;
}

/* Read the set from the input stream.*/
void add_num_to_set(Set* A, int num){
  int icell = num/CELL_LENGTH;
  int ibit = num - (icell*CELL_LENGTH);
  /* A[icell] =| pow(2, ibit)*/
  (*A)[icell] = (*A)[icell] | 1<<ibit;
}


void erase_set(Set *A) {
  int i;
  for (i = 0; i<LENGTH;i++) {
    (*A)[i] = 0;
  }
}

void read_set(Set* A, char* s){
  char* token; 
  char* rest = s; 
  int num;
  char *non_digit;
  erase_set(A);

  /* If the string is empty then it is not terminated correctly with a -1*/
  if(string_blank_or_empty(s)){
    printf("List of set members is not terminated correctly\n");
    return;
  }
  /* If the string ends with a comma it is an error that the following
     loop will not catch it becuase it seperates with the commas.*/
  if(string_ends_with_comma_and_blanks(s)){
    printf("There is an illegal comma at the end of the command line\n");
    return;
  }
  
  while ((token = strsep(&rest, ","))){
    /* Handle missing commas, e.g. input="5,4 3,2" */
    if (token_has_bad_blanks(token) ) {
      printf("Missing comma in token [%s]\n", token);
      erase_set(A);
      return;
    }
    /* Handle illegal characters, e.g. input="3,2 j" */
    if (token_has_bad_chars(token) ) {
      printf("Invalid set memeber - not an integer in token [%s]\n",token);
      erase_set(A);
      return;
    }
    /* Handle double commas, e.g. input="5,4,,3" */
    if (string_blank_or_empty(token)){
      printf("more than one comma\n");
      erase_set(A);
      return;
    }

    num = strtol(token, &non_digit, 10);

    /* Handle numbers out of range */
    if (num < -1 || num > 127) {
      printf("Invalid set memeber, number out of range\n");
      erase_set(A);
      return;
    }
    /* Handle token containing a non-blank, e.g. input="3,4x,5" */
    if (*non_digit != '\0' && *non_digit!= ' ' && *non_digit != '\t') {
      printf("Invalid set memeber - not an integer\n");
      erase_set(A);
      return;
    }

    if (num == -1){
      if (!string_blank_or_empty(rest)){
	printf("Extrenuous text after end of command\n");
	erase_set(A);
	return;
      }
      return;
    }
    add_num_to_set(A, num);
  }
  
  /* If we reached here, the list was not terminated with a -1 */
  printf("List of set members is not terminated correctly\n");
  erase_set(A);
}

/* Print the members of a set.*/
void print_set(Set* A){
  int i, ibit;
  int counter = 0;
  /* If the set is empty print so.*/
  if(set_is_empty(A)){
    printf("The set is empty\n");
    return;
  }
  printf("{");
  /* Go through the Set.*/
  for (i=0; i < LENGTH; i++) {
    unsigned int value = (*A)[i];
    /* For each cell, go through the bits and print the
       positions of bits that are 1.*/
    for (ibit = i*CELL_LENGTH; ibit < (i+1)*CELL_LENGTH; ibit++) {
      if (value % 2 == 1) {
	/* Print "," between each member of the set but not in the beggining.*/
	if (counter!=0){
	  printf(", ");
	}
	if(counter == 16){
	  printf("\n");
	}
	counter++;
	printf("%d", ibit);
      }
      value = value >> 1;
    }
  }
  printf("}\n");
  
}

/* Compute the union of two sets.*/
void union_set(Set* A, Set* B, Set *C){
  int i;
  for (i=0; i<LENGTH; i++) {
    (*C)[i] = (*A)[i] | (*B)[i];
  }
}

/* Compute the intersection of two sets.*/
void intersect_set(Set* A, Set* B, Set *C){
  int i;
  for (i=0; i<LENGTH; i++) {
    (*C)[i] = (*A)[i] & (*B)[i];
  }
}

/* Compute the set difference (subtraction) of two sets.*/
void sub_set(Set* A, Set* B, Set *C){
  int i;
  for (i=0; i<LENGTH; i++) {
    (*C)[i] = (*A)[i] & ~(*B)[i];
  }
}

/* Compute exclusive or of two sets.*/
void symdiff_set(Set* A, Set* B, Set *C){
  int i;
  for (i=0; i<LENGTH; i++) {
    (*C)[i] = (*A)[i] ^ (*B)[i];
  }
}

