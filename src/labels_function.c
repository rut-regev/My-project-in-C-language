#include <stdio.h>       /* Includes standard library for input/output operations */
#include <stdlib.h>      /* Includes standard library for general utility functions */
#include <string.h>      /* Includes standard library for string manipulation functions */
#include <ctype.h>       /* Includes standard library for character handling functions */
#include "fpass.h"       /* Includes custom header file (not defined here) */
#include "assem.h"       /* Includes custom header file for assembler-related functions */
#include "labels.h"
/* 
 * The update_line_data_label function updates the memory addresses of data labels.
 * This function adjusts the location of data labels by adding the current Instruction Counter (IC) plus 100.
 * This is necessary to account for the space reserved for code instructions and ensure correct address calculation.
 * 
 * Parameters:
 *  - label_tabel: Pointer to an array of label structures. Each label contains information about its type and address.
 *  - labelplace: Number of labels in the array. Determines the range to iterate through.
 *  - IC: Instruction Counter value. Represents the current position in the code memory.
 * 
 * Returns:
 *  - 0 on success. This indicates that the function completed its task without encountering any errors.
 */
int update_line_data_label(labels *label_tabel, int labelplace, int IC)
{
    int j;  /* Loop counter for iterating through the labels */

    /* Check if there are no labels to process */
    if (labelplace == 0)
        return 0;

    /* Iterate through each label in the label table */
    for (j = 0; j < labelplace; j++)
    {
        /* Check if the label type is '.data' */
        if (strcmp(".data", (label_tabel + j)->typelabel) == 0)
        {
            /* Update the address of data labels by adding IC + 100 */
            (label_tabel + j)->linelabel += IC + START_OF_MEMORY;
        }
    }

    return 0;  /* Indicate that the function completed successfully */
}

/* 
 * The valid_label function validates entry labels in the code.
 * It ensures that all entry labels are defined within the label table and do not conflict with external labels.
 * This function checks for proper definition and avoids duplication errors between entry and external labels.
 * 
 * Parameters:
 *  - ent_lab: Pointer to an array of entry label structures. Each entry label contains information about its name and the line number where it appears.
 *  - label_tabel: Pointer to an array of label structures. Each label contains information about its name, type, and address.
 *  - entryplace: Number of entry labels. Determines the range of entry labels to validate.
 *  - labelplace: Number of labels in the label table. Determines the range of labels to check against.
 * 
 * Returns:
 *  - 0 if all entry labels are valid. This means that all entry labels are correctly defined and do not conflict with external labels.
 *  - 1 if there is an error. This indicates that one or more entry labels are either not defined or conflict with external labels.
 */
int valid_label(entry_label *ent_lab, labels *label_tabel, int entryplace, int labelplace)
{
    int i, j;  /* Loop counters for iterating through entry labels and label table */
    int flag = 0;  /* Flag to track if an entry label is found in the label table */

    /* If there are no entry labels, no need to validate */
    if (entryplace == 0)
        return 0;

    /* If there are entry labels but no labels in the label table, it is an error */
    if (entryplace && labelplace == 0)
        return 1;

    /* Validate each entry label */
    for (i = 0; i < entryplace; i++)
    {
        flag = 0;

        /* Check if the entry label is defined in the label table */
        for (j = 0; j < labelplace; j++)
        {
            /* If the entry label is defined as both entry and external, report an error */
            if (strcmp((ent_lab + i)->name, (label_tabel + j)->namelabel) == 0 &&
                strcmp((label_tabel + j)->typelabel, ".external") == 0)
            {
                printf("Error in line %d: the same label defined in entry and in extern.\n", (ent_lab + i)->line_of_file);
                printf("Error in line %d: the same label defined in entry and in extern.\n", (label_tabel + j)->line_of_file);
                return 1;
            }

            /* If the label is found in the label table, set flag to indicate it is defined */
            if (strcmp((ent_lab + i)->name, (label_tabel + j)->namelabel) == 0)
            {
                flag = 1;
                continue;
            }
        }

        /* If the entry label is not found in the label table, report an error */
        if (flag == 0)
        {
            printf("Error in line %d: the label in the entry not defined \n", (ent_lab + i)->line_of_file);
            return 1;
        }
    }

    return 0;  /* Indicate that all entry labels are valid */
}




/* 
 * The save_label function saves label information to a label structure.
 * This function populates the label structure with the label's name, type, address, and line number from the source file.
 * 
 * Parameters:
 *  - name: A character array (string) containing the label's name.
 *  - type: A character array (string) indicating the type of the label (e.g., ".data", ".code").
 *  - line: An integer representing the address (or line number) of the label in memory.
 *  - place: A pointer to the `labels` structure where the label information will be saved.
 *  - countline: An integer representing the line number in the source file where the label was found.
 * 
 * Returns:
 *  - 0 on success. This indicates that the label information was successfully saved in the structure.
 */
int save_label(char name[], char type[], int line, labels *place, int countline)
{
    strcpy(place->namelabel, name);      /* Copy the label's name into the structure */
    strcpy(place->typelabel, type);      /* Copy the label's type into the structure */
    place->linelabel = line;             /* Set the label's address in memory */
    place->line_of_file = countline;     /* Set the line number from the source file */

    return 0;  /* Indicate that the function completed successfully */
}

/* 
 * The chaking_label function validates a label according to several criteria.
 * This function checks if the label meets naming conventions, is not too long, and does not conflict with reserved words or commands.
 * 
 * Parameters:
 *  - line: A character array (string) representing the entire line of text being processed.
 *  - start: The starting index of the label within the line.
 *  - end: The ending index of the label within the line.
 *  - countline: An integer representing the line number in the source file where the label was found.
 * 
 * Returns:
 *  - 1 if the label is valid. This indicates that the label meets all criteria.
 *  - 0 if the label is invalid. This indicates that the label does not meet the required criteria or conflicts with reserved words.
 */
int chaking_label(char line[], int start, int end, int countline)
{
    int i;  /* Loop counter for iterating through characters in the label */
    
    /* Arrays of reserved words and guidance directives */
    char *name_of_command[AMONT_COMMAND] = {"mov", "cmp", "add", "sub", "lea", "clr", "not", "inc", "dec", "jmp", "bne", "red", "prn", "jsr", "rts", "stop"};
    char *name_of_guidance[AMONT_GUIDANCE] = {".data", ".string", ".entry", ".extern"};
    char *name_of_register[AMONT_REGISTERS] = {"r1", "r2", "r3", "r4","r5", "r6", "r7", "r0"};
    
    /* Check if the length of the label exceeds the maximum allowed length */
    if ((end - start) > LEN_LABEL)
    {
        printf("Error in line %d: The label name is too long\n", countline);
        return 0;
    }

    /* Check if the label starts with an alphabetic character */
    if (!isalpha(line[start]))
    {
        printf("Error: The label starts with an illegal character in line %d\n", countline);
        return 0;
    }

    /* Check if all characters in the label are either alphabetic or digits */
    for (i = start + 1; i < end; i++)
    {
        if (!(isalpha(line[i]) || isdigit(line[i])))
        {
            printf("Error: The label contains illegal characters in line %d\n", countline);
            return 0;
        }
    }

    /* Check if the label matches any reserved commands */
    if (compare_with_words(line, name_of_command, AMONT_COMMAND))
    {
        printf("Error in line %d: the label have name of command\n", countline);
        return 0;
    }

    /* Check if the label matches any reserved guidance directives */
    if (compare_with_words(line, name_of_guidance, AMONT_GUIDANCE))
    {
        printf("Error in line %d: the label have name of guidance\n", countline);
        return 0;
    }

    /* Check if the label matches any reserved egister */
    if (compare_with_words(line, name_of_register, AMONT_REGISTERS))
    {
        printf("Error in line %d: the label have name of egister\n", countline);
        return 0;
    }

    /* Check if the label matches any reserved egister */
    if (strcmp(line,"macr")==0 || strcmp(line,"endmacr")==0)
    {
        printf("Error in line %d: the label have name of macro definition\n", countline);
        return 0;
    }

    return 1;  /* Indicate that the label is valid */
}


/* 
 * The `is_macro` function checks if a given label name matches any of the macro names 
 * in a linked list of macros. The label is considered a macro if its name (minus 
 * a trailing colon) matches any macro name in the list.
 * 
 * Parameters:
 *  - `name_label` (char[]): The string containing the label name to be checked.
 *  - `j` (int): The index in the `name_label` array where the trailing colon (':') is located.
 *  - `p` (macro_tabel *): A pointer to the head of a linked list of macros.
 * 
 * Description:
 *  - The function copies the label name into a temporary array and removes the trailing 
 *    colon by setting the character at index `j` to null terminator (`\0`).
 *  - It then traverses the linked list of macros, comparing the label name with each macro's name.
 *  - If a match is found, the function returns `1` (true), indicating that the label name 
 *    corresponds to a macro.
 *  - If no match is found after traversing the entire list, the function returns `0` (false).
 * 
 * Returns:
 *  - `1` if the label name matches a macro name in the linked list.
 *  - `0` if the label name does not match any macro names in the linked list.
 */
int is_macro(char name_label[], macro_tabel *p)
{
    char arr_copy[LEN_LABEL];  /* Temporary array to hold the label name without trailing colon */
	if(p==NULL)
		return 0;
    /* Copy the label name into the temporary array */
    strcpy(arr_copy, name_label);
    

    /* Traverse the linked list of macros */
    while (p->next != NULL)
    {
        /* Compare the current macro's name with the label name */
        if (strcmp(p->name, arr_copy) == 0)
        {
            return 1;  /* The label name matches a macro name */
        }
        p = p->next;  /* Move to the next macro in the list */
    }

    return 0;  /* No match found; the label name does not correspond to any macro */
}




