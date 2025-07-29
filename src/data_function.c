#include <stdio.h>       /* Includes standard library for input/output operations */
#include <stdlib.h>      /* Includes standard library for general utility functions */
#include <string.h>      /* Includes standard library for string manipulation functions */
#include <ctype.h>       /* Includes standard library for character handling functions */
#include "fpass.h"       /* Includes custom header file (not defined here) */
#include "data.h"
/**
 * @brief Checks if a substring matches any known data directive.
 * 
 * This function checks if a substring of the given `word` matches one of the predefined data directives.
 * It compares a portion of the string, starting from `start` and ending at `end`, with known data directives:
 * ".data", ".string", ".extern", and ".entry". If a match is found, it returns an index (1-based) corresponding
 * to the matched directive. If no match is found, it returns 0.
 * 
 * @param word The string to be checked.
 * @param start The starting index of the substring to compare.
 * @param end The ending index of the substring to compare.
 * 
 * @return int Returns the index of the matching directive (1-based) or 0 if no match is found.
 */
int chaking_data(char word[], int start, int end) 
{
    int i;
    char *data_directives[FORE] = {".data", ".string", ".extern", ".entry"};

    /* Loop through known data directives to find a match */
    for (i = 0; i < FORE; i++)
    {
        /* Compare the substring with the current directive */
        if (strncmp(&word[start], data_directives[i], strlen(data_directives[i])) == 0)
        {
            return i + 1; /* Return the index of the directive (1-based) */
        }
    }
    return 0; /* Return 0 if no match is found */
}



/**
 * @brief Checks if a string contains only integers and if the integers are within a valid range.
 * 
 * This function processes a string to ensure that each element is an integer and that each integer
 * is within the specified range. It checks for non-integer characters and verifies that integers are 
 * within the range of -16384 to 16383. The function handles the input string by splitting it with commas.
 * 
 * @param line The string to be checked.
 * @param size The size of the string buffer (not used in this function, but could be relevant for larger implementations).
 * @param countline The line number for error reporting.
 * 
 * @return int Returns 1 if an error is found (non-integer or out-of-range number), otherwise returns 0.
 */
int check_string(char *line, int size, int countline) 
{
    char *p, *comma = ",";
    int num;
    char *pointer = (char *)malloc(sizeof(char) * size);

    if(pointer==NULL)
    {
	printf("Error: Insufficient memory\n\n");
        exit(1);
    }

    strcpy(pointer, line);
    pointer = (char *)strtok(pointer, comma); /* Move to the next token separated by comma */

    /* Loop until the pointer is null (i.e., until the end of the string) */
    while (pointer != 0)
    {

        num = strtol(pointer, &p, TEN); /* Convert the string to an integer */
        
        if (*p != '\0') /* Check if there are non-integer characters */
        {
            printf("Error in line %d: Invalid set member – not an integer \n", countline);
            free(pointer); /* Free allocated memory */
            return 1;
        }
        
        if (num < -MIN_NUM || num > MAX_NUM) /* Check if the number is within the valid range */
        {
            printf("Error in line %d: The data number is out of the range \n", countline);
            free(pointer); /* Free allocated memory */
            return 1;
        }

        pointer = (char *)strtok(NULL, comma); /* Move to the next token separated by comma */
    }

    free(pointer); /* Free allocated memory */
    return 0; /* Return 0 if all checks pass */
}




/* Function to check for a missing comma in a string*/
int miss_comma(char str[],int countline)
{
    int i, j, flag, is_char, comma = 0, length = strlen(str); /* Definition of variables and initial assignment*/

    for (j = 0; j < length; j++) /* For loop iterating over the string*/
    {

        i = j; /* Assigning j to i*/
        flag = 0; /* Resetting the flag*/
        is_char = 1; /* Assigning 1 to is_char*/
        while (str[i] == ' ' || str[i] == '\t' || str[i] == ',') /* Inner while loop*/
        {
            is_char = 0; /* Changing is_char to 0*/
            if (i == 0) /* Checking if i is 0*/
                flag = 1; /* Changing flag to 1*/
            if (str[i] == ',') /* Checking if the character is a comma*/
                comma = 1; /* Changing comma to 1*/
            i++; /* Moving to the next index*/
        }
        if (comma == 0 && str[i] != '\0' && flag == 0 && is_char == 0) /* Checking for a missing comma*/
	{
	    printf("Error in line %d: Missing comma\n\n",countline);
            return 1; 
	}

        j = i; /* Assigning i to j*/
        comma = 0; /* Resetting the comma*/
    }
    return 0; /* Returning 0 in case of no error*/
}


/**
 * @brief Checks for illegal comma placements in a string.
 * 
 * This function examines a string to ensure that commas are correctly placed. It checks for:
 * 1. A comma at the beginning of the string.
 * 2. A comma at the end of the string.
 * 3. Multiple consecutive commas.
 * 
 * If any of these conditions are detected, an error message is printed and the function returns 1.
 * If all checks pass, the function returns 0.
 * 
 * @param str The string to check for comma placements.
 * 
 * @return int Returns 1 if an illegal comma placement is found, otherwise returns 0.
 */
int good_comma(char *str,int countline) 
{
    int i;

    /* Check for a comma at the beginning of the string */
    if (str[0] == ',')
    {
        printf("Error in line %d: Illegal comma\n\n",countline);
        return 1;
    }

    /* Check for a comma at the end of the string */
    if (str[strlen(str) - 1] == ',')
    {
        printf("Error in line %d: Extraneous text after end of command\n\n",countline);
        return 1;
    }

    /* Check for multiple consecutive commas */
    for (i = 1; i < strlen(str) - 1; i++)
    {
        if (str[i] == ',' && str[i + 1] == ',')
        {
            printf("Error in line %d: Multiple consecutive commas\n\n",countline);
            return 1;
        }
    }

    return 0; /* Return 0 if all checks pass */
}



/**
 * @brief Encodes a line of data based on its type and stores it in a larger array.
 * 
 * This function processes a line of data depending on its type. For ".data" type,
 * it splits the line by commas, converts each value to binary, and stores it in the 
 * array through the `enter_code_data` function. For other types, it converts each 
 * character to binary and stores it, appending a terminating zero to signify the end 
 * of the string.
 * 
 * @param line The line of data to encode.
 * @param type The type of data (e.g., ".data" or other types).
 * @param length_line The number of items or characters in the line.
 * 
 * @return int Returns the number of data items encoded.
 */
int coding_data(char *line, char type[], int length_line)
{   
    int length =LEN_BITS-1; /* Length of the binary array */
    char arr[LEN_BITS];    /* Array to hold binary representation */
    int num, i,counter=0;
    char *comma = ","; /* Delimiter for splitting the line */


    if(strcmp(type, ".data") == 0) /* If the type is ".data" */
    {

    	for (i =0; i < length_line; i++)
    	{
        	if (line[i] == ',')
        	{
         		counter++;/*סופר כמות פסיקים*/
       		}
   	}

        line = (char *)strtok(line, comma); /* Tokenize the line by commas */
        num = atoi(line); /* Convert the first token to an integer */
        enter_code_data(binary_num(num, arr, length)); /* Convert to binary and store */

	i=0;
	if(counter!=0)
	{
        	for (i = 0; i < counter; i++)
        	{
            		line = (char *)strtok(NULL, comma); /* Get the next token */
            		num = atoi(line); /* Convert token to integer */
            		enter_code_data(binary_num(num, arr, length)); /* Convert to binary and store */
        	}
	}
	return i+1;
    }
    else
    {
        for (i = 1; i < length_line-1; i++)
        {
            num = (int)line[i]; /* Convert each character to its integer value */
            enter_code_data(binary_num(num, arr, length)); /* Convert to binary and store */
        }

        enter_code_data(binary_num(0, arr, length)); /* Append 0 to signify end of string */
    }   

    return i; /* Return the number of data items processed */
}

/* 
thuse function get unsigned number and print it in bunary.
*/


/**
 * binary_num - Converts an integer to a binary string representation of a specified length.
 * 
 * @param c: The integer to be converted to binary.
 * @param arr: The array where the binary representation of the number will be stored.
 * @param len: The length of the binary string (and the array).
 * 
 * @return: Returns the array containing the binary representation of the number.
 * 
 * This function converts the integer `c` into a binary string representation and stores it in
 * the array `arr` of length `len`. The binary representation is stored from the most significant
 * bit to the least significant bit.
 */
char *binary_num(int c, char arr[], int len) /* Converts a number to a binary string representation of a given length */
{
    int length = len;
    int j;

    for (j = 0; j < length; j++) {
        /* Check if the bit at the current position is 1 */
        if (c & (1 << j)) 
            arr[length - j - 1] = '1'; /* If yes, set the character to '1' */
        else 
            arr[length - j - 1] = '0'; /* Otherwise, set the character to '0' */
    }
    arr[length] = '\0'; /* Add null terminator */
    return arr; /* Return the array with the binary representation */
}
