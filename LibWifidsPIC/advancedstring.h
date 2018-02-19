/**
* @file advancedstring.h
* @description Functions to manipulate strings
* @author Felipe Gil
* @date 2018-2-5
* @version 2.0 2018-2-5 18:20
*/
#ifndef _ADVANCEDSTRING_H
#define _ADVANCEDSTRING_H

#include <string.h>
#include <stdint.h>

/**
 * Extract a substring from a string "text", 
 * from integer positions "start" to "end", BOTH INCLUSIVE.
 * 
 * The function returns a confirmation bit in case
 * something wrong is detected 
 * (e.g. end > length of text).
 * 
 * @param *text String from which the substring is selected.
 * @param *substr Return the substring by reference.
 * @param start Integer indicating starting position of substring.
 * @param end Integer indicating ending position of substring.
 * 
 * @return int Confirmation. 1 : correct substring, 0: error
 */
int substring_int(char *text, char *substr, int start, int end);

/**
 * Extract a substring from a string "text", 
 * defined by char pointers "start" and "end":
 * start is included but end isn't.
 *
 * The function returns a confirmation bit in case
 * something wrong is detected 
 * (e.g. "end" > length of text, or "end" before "start").
 * 
 * @param *text String from which the substring is selected.
 * @param *substr Return the substring by reference.
 * @param *start Pointer to start position
 * @param *end Pointer to end position
 * 
 * @return int Confirmation. 1 : correct substring, 0: error
 */
int substring(char *text, char *substr, char *start, char *end);

/**
 * Convert a string into an unsigned integer.
 * 
 * The string must be only a decimal number. 
 * The translation is made by using the ASCII table.
 * Error detection (*ok = 0): 
 * - *number contains non-decimal-number characters
 * - *number > 65535
 * 
 * @param char *number The string to be converted into integer.
 * @param int *ok Confirmation (0 if error)
 * @return uint16_t Unsigned integer equivalent of the string number.
 */
uint16_t stringToUint(char *number, int *ok);


#endif