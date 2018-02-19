/**
* @file advancedstring.c
* @description Functions to manipulate strings
* @author Felipe Gil
* @date 2018-2-5
* @version 2.0 2018-2-5 18:20 (separate substring with char pointers and with integer positions)
*/

#include "advancedstring.h"

int substring_int(char *text, char *substr, int start, int end)
{
	int correct = 1;	//if error, turn to 0
	int i; 	//iterator of the string
    int text_len;   //lenght of complete string
    int substr_len; //length of substring

	text_len = strlen(text);
	//check that end is smaller than length of text
	if((end+1) > text_len) correct = 0;
	//check that start is before end
	if(start >= end) correct = 0;
	if(correct){
		substr_len = end - start;
		for(i=0;i<=substr_len;i++){
			substr[i] = text[start+i];
		}
		substr[i] = '\0';
	}else{
		substr[0] = '\0';
	}
	return correct;
}

int substring(char *text, char *substr, char *start, char *end){
    int correct = 1;	//if error, turn to 0
	int i=0; 	//iterator of the string
    int text_len;   //lenght of complete string
    char *pointertotext;
    
    pointertotext = text;
    text_len = strlen(text);    //length of text
    
    //First, check that pointers are correct.
    if(text != NULL && substr!= NULL && start != NULL && end != NULL){
        if(start < end){
            if(start >= pointertotext && start < pointertotext + text_len){
                if(end >= pointertotext && end < pointertotext + text_len){
                    while(start != end){
                        substr[i] = *start;
                        i++;
                        start++;
                    }
                    substr[i] = '\0';
                }else{
                    //"Error 3! End is outside the text."
                    correct = 0;
                }
            }else{
                //"Error 2! Start is outside the text."
                correct = 0;
            }
        }else{
            //"Error 1! End is before start."
            correct = 0;
        }
    }else{
        //"Error 0! One or more pointers are NULL.
        correct = 0;
    }
    if(correct == 0){
        substr[0] = '\0';
    }
    return correct;
}
uint16_t stringToUint(char *number, int *ok){
	int num_len;
	int i;	//iterator
	int digit;
	uint16_t result = 0;
	//power of ten to convert individual digits to
	//decimal number (*10 each loop iteration)
	uint16_t ten_power = 1;	
	
	*ok = 1;
	num_len = strlen(number);
	if(num_len > 5){
		*ok = 0;
	}else{
		for(i=(num_len - 1);i>=0;i--){
			//extract ASCII value
			digit = (int) (number[i] - '0');

			//check that it is a number
			if(digit < 0 || digit > 9){
				*ok = 0;
			}else{
				if(i == 0){
					if(digit > 6 || ((digit == 6) && (result > 5535))){
						*ok = 0;
					}else{
						result += digit * ten_power;
						ten_power *= 10;	//for next iteration
					}
				}else{
					result += digit * ten_power;
					ten_power *= 10;	//for next iteration
				}
			}
		}
	}
	return result;
}