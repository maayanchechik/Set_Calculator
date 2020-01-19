#define _GNU_SOURCE

#include "set.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Names of commands */
#define READ_SET 1
#define PRINT_SET 2
#define UNION_SET 3
#define INTERSECT_SET 4
#define SUB_SET 5
#define SYMDIFF_SET 6
#define STOP 7

/* Error codes */
#define ILLEGAL_COMMAND_NAME -1
#define ILLEGAL_SET_NAME -2
#define ILLEGAL_COMMA -3
#define DOUBLE_COMMA -4
#define AFTER_COMMAND_LINE -5
#define NOT_ENOUGH_NAMES -6

int comma_before_set_name(char* s);
void remove_blank_from_token(char** token);
int extract_command(char** s);
int extract_set_name(char** s);
int extract_three_sets(char* s, int *index1, int *index2, int *index3);
int process_command(Set** sets, char* s);
void print_uint_bin(unsigned int num_dec);


/* This function goes over an input string and retruns whether 
   there is a comma before the text.*/
int comma_before_set_name(char* s){
  int i = 0;
  /* While the char is an end or a space or a tab or a comma, check comma.*/
  while ((s[i] != '\0') &&
	 ((s[i] == ' ')||(s[i] == '\t')||(s[i] == ','))) {
    if (s[i] == ',')
      return 1;
    i++;
  }
  return 0;
}

/* This function receives a pointer to a token and removes all 
   blanks from the beginning and end of the token.*/
void remove_blank_from_token(char** token){
  int i;
  if ((*token) == NULL || strlen(*token) == 0){
    return;
  }
  /* Remove blanks from end of token*/
  i = strlen(*token)-1;
  while((*token)[i] == ' '||(*token)[i] == '\t'){
    (*token)[i] = '\0';
    i--;
  }
  /* Remove blanks from beggining of token*/
  while((**token) == ' '||(**token) == '\t'){
    (*token)++;
  }
}

/* This function receives a pointer to the command name and returns
   the command's matching number, as defined at the top of the program.
   If the first word in the string does not match any known command name
   the function returns ILLEGAL_COMMAND_NAME.*/
int extract_command(char** s){
  char* command;
  /* The command is separated by the rest of the inut by tabs 
     and blanks only.*/
  command = strtok_r(*s, " \t", s);
  /* Now command includes the first text in s, and s includes the rest.*/
  if (!strcmp(command, "read_set"))
    return READ_SET;
  if (!strcmp(command, "print_set"))
    return PRINT_SET;
  if (!strcmp(command, "union_set"))
    return UNION_SET;
  if (!strcmp(command, "intersect_set"))
    return INTERSECT_SET;
  if (!strcmp(command, "sub_set"))
    return SUB_SET;
  if (!strcmp(command, "symdiff_set"))
    return SYMDIFF_SET;
  if (!strcmp(command, "stop"))
    return STOP;
  return ILLEGAL_COMMAND_NAME;
}

/* This function receives a string and retuns the matchin number of the
   set name, which is the first word in the string*/
int extract_set_name(char** s){
  char* set_name;
  set_name = strsep(s, ",");
  
  /* Check if the rest of the string is empty or has two consecutive commas*/
  if (string_blank_or_empty(set_name)){
    if (string_blank_or_empty(*s)){
      printf("This command requires more set names\n");
      return NOT_ENOUGH_NAMES;
    }else{
      printf("There are two consecutive commas\n");
      return DOUBLE_COMMA;
    }
  }
  remove_blank_from_token(&set_name);
  if (!strcmp(set_name, "SETA"))
    return 0;
  if (!strcmp(set_name, "SETB"))
    return 1;
  if (!strcmp(set_name, "SETC"))
    return 2;
  if (!strcmp(set_name, "SETD"))
    return 3;
  if (!strcmp(set_name, "SETE"))
    return 4;
  if (!strcmp(set_name, "SETF"))
    return 5;

  printf("Incorrect set name [%s]\n", set_name);
  return ILLEGAL_SET_NAME;
}

int extract_three_sets(char* s, int *index1, int *index2, int *index3){

  *index1 = extract_set_name(&s);
  if (*index1 < 0) return *index1;

  *index2 = extract_set_name(&s);
  if (*index2 < 0) return *index2;

  *index3 = extract_set_name(&s);
  if (*index3 < 0) return *index3;

  if (s != NULL){
    printf("Exstrenuos text after the proper input to the command has ended\n");
    return AFTER_COMMAND_LINE;
  }
  return 0;
}

/* This function receives the input command and caries the command
   out, making sure to stop and inform the user of errors if they occur.*/
int process_command(Set** sets, char* s){
  int command;
  int stop = 0;
  int set_index1, set_index2, set_index3, sets_are_correct;

  printf("=======================\n");
  printf("The input line is [%s]\n", s);
  command = extract_command(&s);
  if (command == ILLEGAL_COMMAND_NAME){
    /*If there is a comma before command name, then the program will
      receive it as the command name and print it as the incorrect command.*/
    printf("Incorrect command [%d]\n", command);
    return stop;
  }

  if (comma_before_set_name(s)) {
    printf("Illegal missplaced comma between command and set name\n");
    return stop;
  }

  switch(command){

  case READ_SET:
    set_index1 = extract_set_name(&s);
    if (set_index1 == ILLEGAL_SET_NAME || 
	set_index1 == DOUBLE_COMMA ||
        set_index1 == NOT_ENOUGH_NAMES){
      break;
    }

    read_set(sets[set_index1], s);
    break;

  case PRINT_SET:
    set_index1 = extract_set_name(&s);
    if (set_index1 == ILLEGAL_SET_NAME || 
	set_index1 == DOUBLE_COMMA ||
        set_index1 == NOT_ENOUGH_NAMES){
      break;
    }
    if (s != NULL){
      printf("There is exstrenuous text at the end of the command line\n");
      break;
    }
    print_set(sets[set_index1]);
    break;

  case UNION_SET:
    sets_are_correct = extract_three_sets(s, &set_index1, &set_index2,
					  &set_index3);
    if (sets_are_correct == ILLEGAL_SET_NAME || 
	sets_are_correct == DOUBLE_COMMA ||
        sets_are_correct == AFTER_COMMAND_LINE ||
        sets_are_correct == NOT_ENOUGH_NAMES){
      break;
    }
    union_set(sets[set_index1],sets[set_index2],sets[set_index3]);
    break;

  case INTERSECT_SET:
    sets_are_correct = extract_three_sets(s, &set_index1, &set_index2,
					  &set_index3);
    if (sets_are_correct == ILLEGAL_SET_NAME || 
	sets_are_correct == DOUBLE_COMMA ||
        sets_are_correct == AFTER_COMMAND_LINE ||
        sets_are_correct == NOT_ENOUGH_NAMES){
      break;
    }
    intersect_set(sets[set_index1],sets[set_index2],sets[set_index3]);
    break;

  case SUB_SET:
    sets_are_correct = extract_three_sets(s, &set_index1, &set_index2,
					  &set_index3);
    if (sets_are_correct == ILLEGAL_SET_NAME || 
	sets_are_correct == DOUBLE_COMMA ||
        sets_are_correct == AFTER_COMMAND_LINE ||
        sets_are_correct == NOT_ENOUGH_NAMES){
      break;
    }
    sub_set(sets[set_index1],sets[set_index2],sets[set_index3]);
    break;

  case SYMDIFF_SET:
    sets_are_correct = extract_three_sets(s, &set_index1, &set_index2,
					  &set_index3);
    if (sets_are_correct == ILLEGAL_SET_NAME || 
	sets_are_correct == DOUBLE_COMMA ||
        sets_are_correct == AFTER_COMMAND_LINE ||
        sets_are_correct == NOT_ENOUGH_NAMES){
      break;
    }        
    symdiff_set(sets[set_index1], sets[set_index2], sets[set_index3]);
    break;

  case STOP:
    if(!string_blank_or_empty(s)){
      printf("There is extrenuous text at the end of the command line\n");
      break;
    }
    printf("Program terminated by stop command.\n");
    stop = 1;
    break;
    
  default:
    printf("Programmer error! [%s]\n", s);
  }

  return stop;
}

/* This function receives an unsigned int and returns it in
   binary form, for debuggging purposes.
   based on a function from Maman 11*/
void print_uint_bin(unsigned int num_dec){
  int num_bin[CELL_LENGTH];
  int i;
  if (num_dec == 0){
    for (i = 0; i < CELL_LENGTH; i++){
      printf("0");
    }
    return;
  }
  /* Convert num_dec to a binary number in the array num_bin. Notice
     this is the reversed version of the binary representation of
     num_dec.*/
  for (i = 0; i < CELL_LENGTH; i++){
    num_bin[i] = num_dec%2;
    num_dec = num_dec/2;
  }
  /* Print num_bin in reverse. */
  for (i = CELL_LENGTH-1; i >= 0; i--){
    printf("%d",num_bin[i]);
  }
}

int main(){

  char *line_buf = NULL;
  size_t line_buf_size = 0;
  ssize_t line_size;
  int stop;

  /* We were instructed to explicitely define 6 sets and
     initialize each of them to an empty set*/
  Set SETA = {0,0,0,0};
  Set SETB = {0,0,0,0};
  Set SETC = {0,0,0,0};
  Set SETD = {0,0,0,0};
  Set SETE = {0,0,0,0};
  Set SETF = {0,0,0,0};
  Set* sets[6];
  sets[0] = &SETA;
  sets[1] = &SETB;
  sets[2] = &SETC;
  sets[3] = &SETD;
  sets[4] = &SETE;
  sets[5] = &SETF;
  
  stop = 0;
  line_size = 1;
  /*  process_command(sets, line_buf);    */
  /* Loop through until we are done with the file. */
  while (!stop && line_size >= 0)  {
    line_size = getline(&line_buf, &line_buf_size, stdin);
    /* printf("===========================================\n");
    printf("chars=%06ld, buf size=%06lu, contents: [%s]\n",
    line_size, line_buf_size, line_buf); */
    if (line_size > 0) {
      if (line_buf[line_size-1] == '\n') {
	line_buf[line_size-1] = '\0';
      }
      if(!string_blank_or_empty(line_buf)){
      stop = process_command(sets, line_buf);
      }
    }
  }

  /* Free the allocated line buffer */
  free(line_buf);
  line_buf = NULL;
  return 0;
}
