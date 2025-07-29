#include <stdio.h>       /* Includes standard library for input/output operations */
#include <stdlib.h>      /* Includes standard library for general utility functions */
#include <string.h>      /* Includes standard library for string manipulation functions */
#include <ctype.h>       /* Includes standard library for character handling functions */
#include "fpass.h"       /* Includes custom header file (not defined here) */
#include "assem.h"       /* Includes custom header file for assembler-related functions */
#include "action.h"

/* Global arrays for storing binary code representations */

char databiaryarr[SIZE_OF_MEMORY-START_OF_MEMORY][LEN_BITS]; /* Array for storing the binary encoding of data */

/* 
 * The first_pass function performs the first pass of the assembly process. 
 * It reads an assembly file, processes each line, and generates necessary data structures.
 * 
 * Parameters:
 *  - ifp: File pointer to the input assembly file.
 *  - name_of_file: Name of the output file.
 *  - macro_table: Pointer to the macro table.
 *  - error: Error flag to indicate if there was an error in preprocessing.
 * 
 * Returns:
 *  - 0 on success, 1 on failure.
 */

int first_pass(FILE *ifp, char *name_of_file, macro_tabel *macro_table, int error, int length_name_file)
{
    int DC = 0, IC = 0, type, have_label;

/* have_label - Flag indicating whether there is a label that needs to be encoded, 
   type - Type of the label, 
   IC - Number of rows in the instruction encoding array, 
   DC - Number of rows in the data encoding array */

    int i, j = 0, k = 0, end = 1, dlabel = 0, derror = 0, paction,amount_word,not_save_label=0;
    char line[LEN_LINE];
    char first_word[LEN_LABEL], second_word[LEN_LABEL];
    int countline = 0; /* Line number in the file */
    int entryplace = 0, labelplace = 0, lineplace = 0;
    line_label *lineslabelarr;/* Array where the lines in the encoding array with labels that need to be replaced are stored */

    entry_label *ent_lab; /* Array to store entry label information to be written to file */
    labels *label_table; /* Array to store label information */

    char actionbinaryarr[SIZE_OF_MEMORY-START_OF_MEMORY][LEN_BITS];/* Instruction encoding array */

    memset(actionbinaryarr, ASCII_ZERO, sizeof(actionbinaryarr));/* Zeroes in the data array */


    fseek(ifp, 0, SEEK_SET);

    /* Allocate initial memory for line_label array */
    lineslabelarr = (line_label *)malloc(sizeof(line_label));
    if (lineslabelarr == NULL)
    {
        printf("Error: Insufficient memory\n\n");
        free(lineslabelarr);
        exit(1);
    }

    /* Allocate initial memory for entry_label array */
    ent_lab = (entry_label *)malloc(sizeof(entry_label));
    if (ent_lab == NULL)
    {
        printf("Error: Insufficient memory\n\n");
        free(ent_lab);
        exit(1);
    }

    /* Allocate initial memory for labels array */
    label_table = (labels *)malloc(sizeof(labels));
    if (label_table == NULL)
    {
        printf("Error: Insufficient memory\n\n");
        free(label_table);
        exit(1);
    }

    /* Process the entire file and encode it */
    while (end != EOF)
    {

        countline++;

        for (i = 0; i < LEN_LINE - 1; i++)   /* Read characters from the file */
        {
            end = fscanf(ifp, "%c", line+i);   /* Read a single character */

            if (line[i] == '\n' || end == EOF)   /* If end of line or file */
                break;
            if (i == LEN_LINE - TOW && line[i] != '\n' && end != EOF) /* Check for line length exceeding 80 characters */
                printf("Error in line %d: the length of the line is longer than 80 characters \n", countline);
        }

        if (i==0 || end == EOF)   /* If end of line or file */
            break;

        line[i + 1] = '\0'; 

        /* Remove leading spaces and check if the line is empty */
        without_profits_start(line);

	first_word[0]='\0';
	second_word[0]='\0';

	j=0;
	k=0;
	dlabel=0;
	not_save_label=0;

        sscanf(line, "%s %s ", first_word, second_word); /* Extract the first two words of the line */

        while (line[j] != '\0') /* Iterate through the line */
        {

            if ((line[j] == ' ' || line[j] == '\t' || line[j] == '\n') && (line[j - 1] != ' ' && line[j - 1] != '\t')) /* If a word is detected */
            {


                if (k == 0 && line[j - 1] == ':') /* If label is detected */
                {

		    first_word[j-1]='\0';/* Removes ':' from the end of the label */

		    if(second_word[0]=='\0')
                        printf("Error in line %d: the line after the label is a empty\n", countline);

                    if (!chaking_label(first_word, k, j - 1, countline)) /* Check if the label is valid */
		    {
                        derror = 1;
			not_save_label=1;
		    }
                    if (is_macro(first_word, macro_table)) /* Check if the label is a macro name */
                    {
                        printf("Error in line %d: the label is a macro name \n", countline);
                        derror = 1;
			not_save_label=1;
                    }
                       
		    if(exist_label(label_table,first_word,labelplace,countline))
		    {
                        derror = 1;
			not_save_label=1;
		    }

                    if (second_word == NULL) /* Check if the line is empty after the label */
                    {
                        printf("Error in line %d: no sentence after label \n", countline);
                        derror = 1;
			not_save_label=1;
                    }

                    dlabel = 1; /* Set flag indicating a label is present */
                }


                else if (line[k] == '.') /* If data declaration is detected */
                {
                    type = chaking_data(line, k, j - 1);
                    if (type) /* Check if it's a valid data type */
                    {
                        if (dlabel && not_save_label==0 && type != THREE && type != FORE) /* If a label was detected, save the label */
                        {
                            save_label(first_word, ".data", DC, label_table + labelplace, countline); /* Save the label, type, and location */
                            labelplace++;

                            /* Allocate more space for label_table */
                            label_table = (labels *)realloc(label_table, (labelplace + 1) * sizeof(labels));
                            if (label_table == NULL)
                            {
                                printf("Error: Insufficient memory\n\n");
                                free(label_table);
                                exit(1);
                            }
                        }

                        strcpy(line, line + j); /* Move the line pointer to the data section */
			line[strlen(line)-1]='\0';
        		amount_word=sscanf(line, "%s %s", first_word, second_word); /* Extract the first two words of the line */
			if(amount_word==-1)/* Checks if there are parameters after the definition */

			{
                                printf("Error in line %d: missing parameters \n", countline);
                                derror = 1;
				break;
			}


                        if (type == 1) /* .data */
                        {
                            if (miss_comma(line,countline)) /* Check for missing commas in the data line */
                                derror = 1;
                            without_space(line,strlen(line)); /* Remove spaces from the data line */

                            if (good_comma(line,countline)) /* Check if commas are correctly placed */
                                derror = 1;
                            if (check_string(line, strlen(line), countline)) /* Check if numbers are within range and no invalid characters */
                                derror = 1;

                            DC += coding_data(line, ".data", strlen(line)); /* Encode the data and update DC */
                        }
                        else if (type == TOW) /* .string */
                        {

                            if (without_space(line,strlen(line))) /* Remove spaces from the data line */
                                derror = 1;

                            if (line[0] != '"' || line[strlen(line) - 1] != '"') /* Check if quotes are present at the start and end */
                            {
                                printf("Error in line %d: missing \" \n", countline);
                                derror = 1;
                            }

                            DC+= coding_data(line, ".string", strlen(line)); /* Encode and update the lines */


                        }
                        else if (type == THREE) /* .extern */
                        {
                            if (without_space(line,strlen(line))) /* Remove spaces from the data line */
                                derror = 1;

                            if (amount_word==1) /* Check if there is only one label */
                            {

                    		if (!chaking_label(line, 0, strlen(line), countline)) /* Check if the label is valid */
				{
                       			derror = 1;
					not_save_label=1;
				}
                    		if (is_macro(line, macro_table)) /* Check if the label is a macro name */
                   		{
                        		printf("Error in line %d: the label is a macro name \n", countline);
                       			derror = 1;
					not_save_label=1;
                    		}
                       
		   		if(exist_label(label_table,line,labelplace,countline))
				{
					derror=1;
					not_save_label=1;
				}
				if(not_save_label==0)
				{
                                	save_label(line, ".external", 0, label_table + labelplace, countline); /* Save the label defined in the label table */
                                	labelplace++;

                                	/* Allocate more space for label_table */
                               		 label_table = (labels *)realloc(label_table, (labelplace + 1) * sizeof(labels));
                                	if (label_table == NULL)
                                	{
                                   		printf("Error: Insufficient memory\n\n");
                                    		free(label_table);
                                   		exit(1);
                                	}
				}
                            }
                            else
                            {
                                printf("Error in line %d: The number of arguments in extern is illegal \n", countline);
                                derror = 1;
                            }
                        }
                        else /* .entry */
                        {
                            if (amount_word == 1) /* Check if there is only one label */
                            {
                                if (without_space(line,strlen(line))) /* Remove spaces from the label line */
                                    derror = 1;

                                strcpy((ent_lab + entryplace)->name, line); /* Copy the label name to the entry array */
                                (ent_lab + entryplace)->line_of_file = countline; /* Store the line number in the file */
                                entryplace++;
                                ent_lab = (entry_label *)realloc(ent_lab, (entryplace + 1) * sizeof(entry_label));

                                if (ent_lab == NULL)
                                {
                                    printf("Error: Insufficient memory\n\n");
                                    free(ent_lab);
                                    exit(1);
                                }
                            }
                            else
                            {
                                printf("Error in line %d: The number of arguments in entry is illegal \n", countline);
                                derror = 1;
                            }
                        }
                    }
                    else /* If the word after '.' is not valid */
                    {
                        printf("Error in line %d: illegal word after '.' \n", countline);
                        derror = 1;
                    }
                    break; /* Move to the next line */
                }
                else if ((paction = chaking_action(dlabel?second_word:first_word, k, j - 1, countline))) /* If an action is detected */
                {
                    if (dlabel == 1) /* If a label was detected, save it in the table */
                    {
                        save_label(first_word, ".code", IC + START_OF_MEMORY, label_table + labelplace, countline); /* Save the label, type, and location */
                        labelplace++;
                        label_table = (labels *)realloc(label_table, (labelplace + 1) * sizeof(labels));
                        if (label_table == NULL)
                        {
                            printf("Error: Insufficient memory\n\n");
                            free(label_table);
                            exit(1);
                        }
                    }

                    strcpy(line, line + j); /* Move the line pointer to the registers section */
		    without_profits_start(line);
                    have_label = coding_action(line, paction-1, countline, IC,actionbinaryarr); /* Get the number of lines to update, if negative indicates label in instruction */
                    IC+= abs(have_label); /* Update IC with the absolute value of the line count */

                    if (have_label < 0)
                    {
			if(isalpha(actionbinaryarr[IC-1][0]))
			{

                        	(lineslabelarr + lineplace)->line_in_tabel = IC-1; /* Update the line number in the table for labels */
                       		(lineslabelarr + lineplace)->line_in_file = countline; /* Update the line number in the file for labels */

                        	lineplace++;
                        	lineslabelarr = (line_label *)realloc(lineslabelarr, (lineplace + 1) * sizeof(line_label));
                        	if (lineslabelarr == NULL)
                        	{
                        	    printf("Error: Insufficient memory\n\n");
                        	    free(lineslabelarr);
                        	    exit(1);
                        	}

			}

			if(isalpha(actionbinaryarr[IC-2][0]))
			{

                        	(lineslabelarr + lineplace)->line_in_tabel = IC-2; /* Update the line number in the table for labels */
                       		(lineslabelarr + lineplace)->line_in_file = countline; /* Update the line number in the file for labels */

                        	lineplace++;
                        	lineslabelarr = (line_label *)realloc(lineslabelarr, (lineplace + 1) * sizeof(line_label));
                        	if (lineslabelarr == NULL)
                        	{
                        	    printf("Error: Insufficient memory\n\n");
                        	    free(lineslabelarr);
                        	    exit(1);
                        	}

			}

		
                    }
                    break; /* Move to the next line */
                }
                else  /* If it's not a label, action, or data, it's an error */
                {
                    printf("Error in line %d: illegal word\n", countline);
                    derror = 1;
                    break; /* Move to the next line */
                }
            }

            if (line[j] == ' ' || line[j] == '\t')
                k = j + 1; /* Point to the start of the word */

            j++;


        }
    }


    if ((DC + IC + START_OF_MEMORY) > SIZE_OF_MEMORY) /* Check for memory overflow */
    {
        printf("Error: Memory fault \n");
        derror = 1;
    }
    if (valid_label(ent_lab, label_table, entryplace, labelplace)) /* Check for valid labels in entry */
        derror = 1;

    if (error) /* Check if there was an error in preprocessing */
        derror = 1;

    update_line_data_label(label_table,  labelplace,  IC);

    if (entryplace) /* If there are entry labels */
        if(enter_line_entry(ent_lab, label_table, entryplace, labelplace)) /* Insert line numbers into entry labels table */
             return 1; /* Return 1 to indicate failure to generate the file */

    if (second_pass(databiaryarr, actionbinaryarr, lineslabelarr, label_table, labelplace, lineplace, name_of_file, length_name_file, IC, DC, derror)) /* Perform the second pass */
        return 1; /* Return 1 to indicate failure to generate the file */
    if (entryplace)
	entry_file(ent_lab, entryplace, name_of_file, length_name_file); /* Create the entry file */


    free(lineslabelarr);
    free(ent_lab);
    free(label_table);

    return 0; /* Return 0 to indicate success */
} /* End of first_pass */




/**
 * @brief Writes entry labels and their line numbers to a file.
 * 
 * This function creates an output file with the `.ent` extension and writes the entry labels along with their 
 * corresponding line numbers to the file. Each line in the file contains an entry label and its associated line number.
 * 
 * @param ent_lab Array of entry labels, each containing a name and a line number.
 * @param entryplace The number of entry labels to be written to the file.
 * @param name_of_file The base name of the output file, without extension.
 * 
 * @return int Returns 0 on success, or 1 if there is an error opening the file.
 */
int entry_file(entry_label *ent_lab, int entryplace, char *name_of_file,int length_name_file)
{
    int i;
    char *file_name;
    FILE *ofp;

    file_name = (char *)malloc(sizeof(char)*length_name_file);

    if (file_name == NULL)
    {
        printf("Error: Insufficient memory\n\n");
        exit(1);
    }

    /* Create the output file name by appending ".ent" to the base name */
    strcpy(file_name, name_of_file);
    strcat(file_name, ".ent");

    /* Open the file for writing */
    if ((ofp = fopen(file_name, "w")) == NULL)
    {
        printf("error: Cannot open file %s.\n\n", file_name);
        return 1; /* Return 1 to indicate error opening the file */
    }

    /* Write each entry label and its line number to the file */
    for (i = 0; i < entryplace; i++)
    {
        fprintf(ofp, "%s %d\n", (ent_lab + i)->name, (ent_lab + i)->line_of_label);
    }

    /* Close the file */
    fclose(ofp);
    free(file_name);
    return 0; /* Return 0 indicating successful file write */
}


/**
 * @brief Adds binary code data to the data array.
 * 
 * This function copies the given binary code data from the `arr` parameter into a static data array `databiaryarr`.
 * It stores the data at the current index, then increments the index for the next entry.
 * 
 * @param arr The binary code data to be added.
 * 
 * @return int Always returns 0 (indicating successful addition).
 */
int enter_code_data(char arr[])
{
    static int i = 0; /* Static index to keep track of the current position in the data array */
    strcpy(databiaryarr[i++], arr); /* Copy the binary code data to the data array */

    return 0; /* Return 0 indicating successful operation */
}

/**
 * @brief Updates the entry labels with their corresponding line numbers.
 * 
 * This function updates the `entry_label` array with the line numbers of the labels specified in `entry_label`.
 * It compares each entry label with the labels in `label_tabel`, and if a match is found, it sets the 
 * `line_of_label` field of the entry label to the corresponding line number from the label table.
 * 
 * @param ent_lab Array of entry labels to be updated.
 * @param label_tabel Array of labels with line numbers.
 * @param entryplace The number of entry labels.
 * @param labelplace The number of labels in the label table.
 * 
 * @return int Always returns 0 (indicating successful update).
 */
int enter_line_entry(entry_label *ent_lab, labels *label_tabel, int entryplace, int labelplace)
{
    int i, j;

    if (entryplace == 0) /* No entry labels to process */
        return 0;

    /* Loop through each entry label */
    for (i = 0; i < entryplace; i++)
    {
        /* Compare with each label in the label table */
        for (j = 0; j < labelplace; j++)
        {
            /* If the entry label matches a label in the label table */
            if (strcmp((ent_lab + i)->name, (label_tabel + j)->namelabel) == 0)
            {
                (ent_lab + i)->line_of_label = ((label_tabel + j)->linelabel); /* Update the line number */
                break; /* Exit the inner loop once a match is found */
            }
        }
    }

    return 0; /* Return 0 indicating successful update */
}





/**
 * @brief Removes leading spaces, tabs, and newline characters from a line.
 * 
 * This function trims leading whitespace characters (spaces, tabs, and newlines) from the beginning of a line.
 * It modifies the input line so that the first non-whitespace character becomes the new start of the string.
 * 
 * @param line The string to be trimmed.
 * 
 * @return char* A pointer to the modified string with leading whitespace removed.
 */
int without_profits_start(char line[])
{
    int i = 0;

    /* Skip leading spaces, tabs, and newlines */
    while (line[i] == ' ' || line[i] == '\t' || line[i] == '\n')
        i++;
    memmove(line, line + i, strlen(line + i) + 1);
    /* Move the start of the string to the first non-whitespace character */
    return 0;
}



/* Function to remove spaces and tabs from a string*/
int without_space(char str[],int length)
{
    int i,j = 0; /* Variables declaration and initialization*/
    char *str_copy; /* Pointer to hold the copy of the string*/
    

    /* Allocating memory for the copy of the string without spaces and tabs*/
    str_copy = (char *)calloc(length+1, sizeof(char));
    if (str_copy == NULL)
    {
        printf("no place\n\n"); /* Error message if memory allocation fails*/
        exit(1);
    }

    /* Copying non-space and non-tab characters to the new string*/
    for (i = 0; i < length; i++)
    {
        if (str[i] != ' ' && str[i] != '\t' && str[i] != '\n')
        {
            str_copy[j] = str[i];
            j++;
        }
    }
   str_copy[j]='\0';
   strcpy(str,str_copy);/*מעתיק את המחרוזת בלי הרווחים למחרוזת המקורית*/
   free(str_copy);
    return 0; /* Returning 0 indicating success*/
}


/**
 * Compares a given string with a list of words to find a match.
 *
 * This function checks if the string `arr` matches any of the strings
 * in the `words` array. It performs the comparison using `strncmp` to 
 * account for potential differences in string length and includes a null 
 * terminator check. If a match is found, it returns the 1-based index of 
 * the matching word in the array. If no match is found, it returns 0.
 *
 * @param arr The string to be compared against the list of words.
 * @param words An array of strings to compare against.
 * @param numwords The number of words in the `words` array.
 * @return The 1-based index of the matching word if a match is found, 
 *         otherwise 0.
 */
int compare_with_words(char arr[], char *words[], int numwords)
{
    int i;
    for(i = 0; i < numwords; i++)
    {
        if(strncmp(arr, words[i], strlen(words[i]) + 1) == 0)
            return i + 1;
    }

    return 0; /* Return 0 if no match is found */
}



/**
 * Checks if the current label has already been defined before.
 *
 * This function iterates through the list of existing labels to determine if the
 * given label name (`new_label`) already exists in the label table (`label_table`).
 * If the label is found, it prints an error message indicating the line number
 * where the duplication occurred, and returns 1. If the label is not found,
 * it returns 0.
 *
 * @param label_table Pointer to an array of labels where each label has a name.
 * @param new_label The name of the new label to be checked for existence.
 * @param labelplace The number of labels currently in the label table.
 * @param countline The line number where the check is being performed, used
 *                  for error reporting.
 * @return 1 if the label already exists in the label table, otherwise 0.
 */
int exist_label(labels *label_table, char new_label[], int labelplace, int countline)
{
    int i;
    for(i = 0; i < labelplace; i++)
    {
        if(strcmp(label_table[i].namelabel, new_label) == 0)
        {
            printf("Error in line %d: this label's name already exists\n", countline);
            return 1;
        }
    }
    return 0;
}



