#ifndef __SET_H__
#define __SET_H__

#define LENGTH 4
#define CELL_LENGTH 32

/* Assuming int is 4 bytes = 4*8 bit = 32 bits
   32 bit * 4 cells in the array = 128 bit.*/
typedef unsigned int Set[LENGTH];

/* Read the set from the input stream.*/
void read_set(Set* A, char* s);

/* Print the members of a set.*/
void print_set(Set* A);

/* Compute the union of two sets.*/
void union_set(Set* A, Set* B, Set *C);

/* Compute the intersection of two sets.*/
void intersect_set(Set* A, Set* B, Set *C);

/* Compute the set difference (subtraction) of two sets.*/
void sub_set(Set* A, Set* B, Set *C);

/* Compute exclusive or of two sets.*/
void symdiff_set(Set* A, Set* B, Set *C);


/* Internal functions that help the other functions:*/

/* Receives string and regturns whether it is only blanks*/
int string_blank_or_empty(char* s);


#endif
