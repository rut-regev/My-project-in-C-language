#include <stdio.h>       /* Includes the standard library for input/output handling */
#include <stdlib.h>      /* Includes the standard library for general-purpose functions */
#include <string.h>      /* Includes the standard library for string handling functions */
#include <ctype.h>       /* Includes the standard library for character handling functions */
#include "fpass.h"       /* Includes a private header file (not defined here) */
#include "spass.h"

/**
 * @brief Performs the second pass of the assembler process.
 * 
 * This function processes data and instruction encoding arrays, replaces label lines with 
 * their addresses, and generates output files including external labels and object code.
 * 
 * @param databiaryarr 2D array containing binary-encoded data lines.
 * @param actionbinaryarr 2D array containing binary-encoded instruction lines.
 * @param lineslabelarr Array of line labels with their associated line numbers.
 * @param label_tabel Array of labels with their names, types, and line numbers.
 * @param labelplace Number of labels in the label table.
 * @param lineplace Number of lines in the lines label array.
 * @param name_of_file Name of the base file for output file creation.
 * @param IC Instruction count (number of instruction lines).
 * @param DC Data count (number of data lines).
 * @param derror Data error flag indicating if there was an error during data encoding.
 * 
 * @return int Returns 0 on successful completion, or 1 if an error occurs.
 */
int second_pass(char databiaryarr[][LEN_BITS], char actionbinaryarr[][LEN_BITS], line_label *lineslabelarr, labels *label_tabel, int labelplace, int lineplace, char *name_of_file, int length_name_file, int IC, int DC, int derror)
{
    int i = 0, j = 0, num_line = START_OF_MEMORY, error = 0, place,flag_ext=1;
    char binary_line[LEN_BITS];/*מערך שבו ישמר קידוד התוית*/
    char *file_name, *file_name2; /* Check if this size is adequate */
    char octal[LEN_LINE_OCTAL];/*מערך קידוד השורה באוקטלי*/
    FILE *ofp, *ofp2;

/* Allocating space for file names */

    file_name = (char *)malloc(sizeof(char)*length_name_file);
    file_name2 = (char *)malloc(sizeof(char)*length_name_file);
    if (file_name == NULL)
    {
        printf("Error: Insufficient memory\n\n");
        exit(1);
    }

    if (file_name2 == NULL)
    {
        printf("Error: Insufficient memory\n\n");
	free(file_name);
        exit(1);
    }

    memset(binary_line, ASCII_ZERO, sizeof(binary_line));/* Fills the array with zeros */

    binary_line[LEN_BITS-1] = '\0';

    strcpy(file_name2, name_of_file);
    strcat(file_name2, ".ext"); /* Append the ".ext" extension to the file name */

    if((ofp2 = fopen(file_name2, "w")) == NULL) /* Open the file extern for writing */
    {
        printf("error: Cannot open file %s\n\n", file_name2);
	free(file_name);
	free(file_name2);
        return 1;
    }

    for(i = 0; i < lineplace; i++) /* Loop through each line label */
    {
        place = (lineslabelarr + i)->line_in_tabel;

        for(j = 0; j < labelplace; j++) /* Loop through each label to match with the action binary array */
        {


            if(strcmp((label_tabel + j)->namelabel, actionbinaryarr[place]) == 0) /* Check if label matches */
            {
                codeing_label(label_tabel, j, binary_line);
                strcpy(actionbinaryarr[place], binary_line); /* Encode and replace label in the array */

                if(strcmp((label_tabel + j)->typelabel, ".external") == 0) /* If external label, write to external file */
		{
                    fprintf(ofp2, "%s %0*d\n", (label_tabel + j)->namelabel, LEN_PRINT_NUM, place+START_OF_MEMORY);
		    flag_ext=0;
		}
                break;
            }

        }
        if(j == labelplace) /* If no matching label found, print an error */
        {
            printf("Error: in line %d: invalid word \n", (lineslabelarr + i)->line_in_file);
            error = 1;
        }
    }

    fclose(ofp2);

    if(flag_ext || error || derror) /* If there is no label, delete the file */

    {
	
        if(remove(file_name2)) /* Remove the external file */
            printf("error: Cannot remove file %s\n\n", file_name2);
    }

    if(error || derror) /* If there were errors, do not create output files and remove existing external file */
    {

	free(file_name);
	free(file_name2);
    
        return 1;
    }



/*/////////////////// Convert all encoded lines to octal format and write them to a file ///////////////////////////////////*/



    strcpy(file_name, name_of_file);
    strcat(file_name, ".ob"); /* Append the ".ob" extension to the file name */

    if((ofp = fopen(file_name, "w")) == NULL)
    {
        printf("error: Cannot open file %s.\n\n", file_name);

	free(file_name);
	free(file_name2);

        return 1;
    }

    fprintf(ofp, "   %d %d\n", IC, DC); /* Print the instruction and data counts */

    for(i = 0; i < IC; i++) /* Write binary code of instructions in octal format along with line number */
    {
        strcpy(binary_line, actionbinaryarr[i]);
        aoctaly_num(binary_line, octal);
        fprintf(ofp, "%0*d %s\n", LEN_PRINT_NUM, num_line++, octal);
    }

    for(i = 0; i < DC; i++) /* Write binary code of data in octal format along with line number */
    {
        strcpy(binary_line, databiaryarr[i]);
        aoctaly_num(binary_line, octal);
        fprintf(ofp, "%0*d %s\n", LEN_PRINT_NUM, num_line++, octal);
    }

    fclose(ofp);


    free(file_name);
    free(file_name2);

    return 0;
} /* End of second_pass */


/**
 * @brief Converts a binary number (represented as a string) to its octal representation.
 * 
 * This function takes a binary number stored in a character array and converts it to its 
 * octal representation. The octal digits are stored in another character array. The binary 
 * number is assumed to be in a format where each group of 3 binary digits is converted to 
 * one octal digit.
 * 
 * @param arr A character array containing the binary number as a string.
 * @param octal A character array where the resulting octal number will be stored.
 * 
 * @return int Returns 0 upon successful conversion.
 */
int aoctaly_num(char arr[], char octal[]) /* Converts a binary number to octal and returns an array with the octal number */
{
    int g, i, number, a, b, c;

    for(g = FORE, i = FOURTEEN; i-TOW >= 0; g--, i -= THREE)
    {
        a = (int)(arr[i]) - ASCII_ZERO;           /* Extracts the binary digit at position i */
        b = (int)(arr[i-1]) - ASCII_ZERO;       /* Extracts the binary digit at position i + 1 */
        c = (int)(arr[i-TOW]) - ASCII_ZERO;       /* Extracts the binary digit at position i + 2 */
        number = a + (b << 1) + (c << TOW); /* Converts the three binary digits to a decimal number */
        octal[g] = (char)number + ASCII_ZERO;     /* Converts the decimal number to an ASCII character for octal digit */

    }
	octal[LEN_LINE_OCTAL-1]='\0';
    return 0;
}

/**
 * codeing_label - Encodes a label into a 15-bit binary string representation.
 * 
 * @param label_tabel: A pointer to an array of `labels` structures, which contain the label values.
 * @param j: The index of the label in the array.
 * @param binary_line: An array where the 15-bit binary representation of the label will be stored.
 * 
 * @return: Returns 0 (though the function does not perform any error handling).
 * 
 * This function checks the value of `label_tabel[j].linelabel` and encodes it into a 15-bit binary string.
 * If the label value is 0, it sets the 15th bit (index 14) to '1'. 
 * If the label value is not 0, it converts the label value to a 12-bit binary representation and
 * stores it in `binary_line`, then sets the 13th and 14th bits to '0' and '1', respectively.
 */
int codeing_label(labels *label_tabel, int j, char binary_line[]) /* Encodes a label into a 15-bit binary string */
{
    /* If the label value is 0, set the 15th bit (index 14) to '1' */
    if ((label_tabel + j)->linelabel == 0)
    {
        memset(binary_line, ASCII_ZERO, LEN_BITS*sizeof(char));/*ממלא את המערך באפסים*/
        binary_line[FOURTEEN] = '1'; 
    }
    else
    {
        /* Convert the label value to a 12-bit binary representation and place it in the array */
        binary_num((label_tabel + j)->linelabel, binary_line, 12);
        binary_line[TWELVE] = '0'; /* Set the 13th bit to '0' */
        binary_line[THIRTEEN] = '1'; /* Set the 14th bit to '1' */
	binary_line[FOURTEEN] = '0';/* Set the 15th bit to '1' */
    }

    binary_line[LEN_BITS-1] = '\0';
    return 0; /* Returns 0; the function does not perform error handling */
}


