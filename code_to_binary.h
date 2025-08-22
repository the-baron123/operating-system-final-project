#ifndef CODE_TO_BINARY_H
#define CODE_TO_BINARY_H

/*
 * These functions get arguments for a word in the assembly language,
 * convert them into their binary representation, and return a binary list.
 */

/* source: 1,2,3 | dest: 0,1,2,3 */
binary* code_for_zero(char* word, int line, char* file_name, int opcode, labels* labels_table, int phase);

/* source: 0,1,2,3 | dest: 0,1,2,3 */
binary* code_for_one(char* word, int line, char* file_name, int opcode, labels* labels_table, int phase);

/* source: 1,2 | dest: 1,2,3 */
binary* code_for_four(char* word, int line, char* file_name, int opcode, labels* labels_table, int phase);

/* source: none | dest: 1,2,3 */
binary* code_for_five(char* word, int line, char* file_name, int opcode, labels* labels_table, int phase);

/* source: none | dest: none */
binary* code_for_thirteen(char* word, int line, char* file_name, int opcode, labels* labels_table, int phase);

/* source: none | dest: none */
binary* code_for_fourteen(int opcode);

#endif /* CODE_TO_BINARY_H */
