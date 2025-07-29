#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "assem.h"

/**
 * pro_assembler - Processes an assembly source file, identifies macros, and generates the output file.
 * 
 * @param ifp: Pointer to the input file stream (source assembly file).
 * @param ofp: Pointer to the output file stream (processed output file).
 * @param argv: The name of the source file (used for error reporting and in the first pass function).
 * 
 * @return: Returns 0 if processing is successful, otherwise returns 1 if there is an error.
 * 
 * This function performs the following steps:
 * 1. Initializes variables and sets up the linked list of macros.
 * 2. Reads the source file line by line.
 * 3. Processes each line to:
 *    - Check for macro definitions and end markers.
 *    - Store macro definitions in the linked list.
 *    - Replace macro names with their definitions in the output file.
 * 4. Validates macro names and checks for extraneous text.
 * 5. Allocates and reallocates memory for storing macro commands.
 * 6. Writes the processed content to the output file.
 * 7. Calls `first_pass` to perform additional processing on the output file.
 * 8. Frees allocated memory for macros and returns the result.
 * 
 * Detailed processing includes:
 * - Handling lines starting with "macr" to define a new macro.
 * - Handling lines starting with "endmacr" to mark the end of a macro definition.
 * - Checking for extraneous text before and after macro definitions and end markers.
 * - Replacing macro names with their content in the output file.
 * - Handling lines that are neither macro definitions nor macro names.
 */
int pro_assembler(FILE *ifp, FILE *ofp, char *argv, int length_name_file) /* Processes assembly source file and handles macros */
{
    macro_tabel *head, *curr, *p;
    int i, end = 1, is_macro = 0, counter,length=0;
    int error = 0, countline = 0;
    char line[LEN_LINE];
    char first_word[MAX_LEN_MACRO];
    char second_word[MAX_LEN_MACRO];
    char third_word[MAX_LEN_MACRO];
    
    head = NULL;

    while (end != EOF)
    {
        countline++;/*סופר מספר שורות בקובץ*/
        for (i = 0; i < LEN_LINE - 1; i++)/*קולט שורה בקובץ*/
        {
            end = fscanf(ifp, "%c", &line[i]);
            if (line[i] == '\n' || end == EOF) /* Exit if end of line or end of file */
                break;
        }
        line[i + 1] = '\0';

        length+=strlen(line);

        if (end != EOF)
        {
            counter = sscanf(line, "%s %s %s", first_word, second_word, third_word); /* Read words from the line */
            if (counter == -1)/*בודק שזה לא שורה ריקה*/
                continue;
            if (line[0] == ';') /* Skip comment lines */
                continue;

            if (strcmp(second_word, "macr") == 0) /* Check for extraneous text before macro definition */
            {
                printf("error in line %d: Extraneous text before the macro\n", countline);
                error = 1;
            }
            
            if (strcmp(first_word, "macr") == 0) /* Check for macro definition */
            {
                if (good_name(second_word, &head, countline)) /* Validate macro name */
                    error = 1;

                if (counter > TOW) /* Check for extraneous text after macro definition */
                {
                    printf("error in line %d: Extraneous text after macro,-%s\n", countline, third_word);
                    error = 1;
                }
                
                curr = (macro_tabel *)malloc(sizeof(macro_tabel));
                enter_macro(second_word, &head, &curr); /* Add macro to list */

                is_macro = 1; /* Set flag indicating macro is being defined */
            }


            else if (strcmp(first_word, "endmacr") == 0) /* Check for end of macro definition */
            {
                if (counter > 1) /* Check for extraneous text after end of macro */
                {
                    printf("error in line %d: Extraneous text after end of macro\n", countline);
                    error = 1;
                }

                is_macro = 0; /* Clear flag indicating end of macro definition */
            }


            else if (is_macro) /* Process lines within macro definition */
            {
                curr->commands = (char *)realloc(curr->commands, (length+1) * sizeof(char)); /* Allocate memory for command  */
                if (curr->commands == NULL)
                {
                    printf("Error: Insufficient memory\n\n");
		    free(curr->commands);
                    exit(1);
                }
                else
                    strcat(curr->commands, line); /* Append line to macro commands */
            }

            else if (name_of_macro(first_word, &head, &p) && counter == 1) /* Replace macro name with macro content */
                fprintf(ofp, "%s", p->commands);
            
            else /* Write non-macro lines to output file */
                fprintf(ofp, "%s", line);
        }
    }

    first_pass(ofp, argv, head, error,length_name_file);

    if(head!=NULL)
	free_macro(&head); /* Free memory allocated for macros */

    return 0; /* Return 0 upon successful processing */
}

int enter_macro(char *name_macro,macro_tabel **head,macro_tabel **curr)/*מקבל את השם של המאקרו ואת כתובת הראש של הרשימה ו*/
{

	if ((*curr) == NULL)
	{
   		printf("Error: Insufficient memory\n\n");
		free_macro(head); /* Free memory allocated for macros */
		exit(1);
	}
	strcpy((*curr)->name,name_macro);
	(*curr)->next=*head;
	*head=(*curr);
	return 0;
}
int name_of_macro(char *pointer,macro_tabel **head,macro_tabel **p)/*בודק אם זה שם של מאקרו, וצריך להחליף בשורות קוד*/
{
	
	*p=*head;
	while(*p!=NULL)
	{
		if(strcmp(pointer,(*p)->name)==0)
			return 1;
		else
			(*p)=(*p)->next;
	}
	return 0;
}

int free_macro(macro_tabel **head)/*משחרר את הזיכרון*/
{
	macro_tabel *p2;
	macro_tabel *p;
	p=(*head);
	p2=p->next;
	while(p2!=NULL)
	{
		free(p->commands);
		free(p);
		p=p2;	
		p2=p->next;
	}
	return 0;
}
int good_name(char *name_macro,macro_tabel **head,int countline)/*בודק שהשם של המאקרו טוב*/
{
	int i;
	char *name_of_command[]={"mov","cmp","add","sub","lea","clr","not","inc","dec","jmp","bne","red","prn","jsr","rts","stop"};
	char *name_of_guidance[]={".data",".string",".entry",".extern"};
	macro_tabel *p=*head;
	

	for(i=0;i<strlen(*name_of_command);i++)/*בודק שהשם של המאקרו אינו שם של פקודה*/
	{
		if(strcmp(name_of_command[i],name_macro)==0)
		{
			printf("error in line %d: the macro have name of command/n ",countline);
			return 1;
		}
	}

	for(i=0;i<strlen(*name_of_guidance);i++)/*בודק שהשם של המאקרו אינו שם של הוראה*/
	{
		if(strcmp(name_of_guidance[i],name_macro)==0)
		{
			printf("error in line %d: the macro have name of guidance sentence/n ",countline);
			return 1;
		}
	}

	if(strlen(name_macro)>31)/*בודק שגודל שם המאקרו אינו יותר מ-31*/
	{
		printf("error in line %d: the length of the name of macro is too long /n ",countline);
			return 1;
	}


	while(p!=NULL)/*בודק שהשם של המאקרו אינו קיים כבר ברשימת המאקרואים*/
	{
		if(strcmp(name_macro,p->name)==0)
		{
			printf("error in line %d: the name of the macro is exist /n ",countline);
			return 1;
		}
		else
			p=p->next;
	}
	


	return 0;
}
