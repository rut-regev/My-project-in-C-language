#include <stdio.h>       /* Standard library for input/output functions */
#include <stdlib.h>      /*Standard library for general utility functions */
#include <string.h>      /* Standard library for string handling functions */
#include <ctype.h>       /* Standard library for character handling functions */
#include "fpass.h"       /* Includes custom header file (not defined here) */
#include "action.h"	/* Includes custom header file (not defined here) */

/*
 * Function: coding_action
 * Purpose: Codes an action and additional parameters based on a given word.
 * Inputs:
 *  - word[]: the string containing the operands
 *  - paction: The action code 
 *  - countline: The line number in the source code
 *  - ic: Instruction counter
 *  - binaryarr[][16]: Array to store the binary representation 
 * Returns:
 * - an integer with which the IC or specific error codes are updated
 */
int coding_action(char word[],int paction, int countline, int ic, char binaryarr[][CELL_SIZE_MEMORY])
{
	char b[MAX_WORD_SIZE],c[MAX_WORD_SIZE],d[MAX_WORD_SIZE];
	int count=0,i,j=0,numopernd,dlabel=0,doper=0;
	char *token;
	int len = strlen(word);

	/* Binary opcodes for actions */
	char binary_opcode[AMOUNT_ACTION][SIZE_OPCODE]={"0000","0001","0010","0011","0100","0101","0110","0111","1000","1001","1010","1011","1100","1101","1110","1111"};
	
	strncpy(binaryarr[ic],binary_opcode[paction],SIZE_OPCODE); /* Set the action code in the first 4 bits from the left */
	binaryarr[ic][FOUR] = '0';/* Clear \0 */
	binaryarr[ic][A] = '1';/* Set A bit */
	binaryarr[ic][B] = '0'; /* Clear R bit */
	binaryarr[ic][E] = '0'; /* Clear E bit */
	binaryarr[ic][END_LINE_CODING] = '\0';/* Null-terminate the string */
	binaryarr[ic+1][END_LINE_CODING] = '\0';/* Null-terminate the string */
	binaryarr[ic+TWO][END_LINE_CODING] = '\0';/* Null-terminate the string */
	/* Decomposing the command into variables*/
	b[0]='\0';
	c[0]='\0';
	d[0]='\0';
	for(i=0;i<strlen(word);i++)/*chaking ',' */
	{
		if(word[i]==',')
			j=1;
	}
	/* Remove newline character from the end of the word, if present */
	if (len > 0 && word[len-1] == '\n')
	{
            	word[len-1] = '\0';
        }

	/* Tokenize the input word into operands using spaces and commas as delimiters */
	token = strtok(word, " ,");
    	if (token != NULL) {
		strcpy(b, token);
		if(b[0]!='\n' || b[0]!='\0')
			count++;  /* Increment count for each valid operand */

	}


	token = strtok(NULL, " ,");
    	if (token != NULL) 
	{
		strcpy(c, token);
		if(c[0]!='\n' || c[0]!='\0')
			count++;  /* Increment count for each valid operand */

	}


	token = strtok(NULL, " ,");
    	if (token != NULL) 
	{	
		strcpy(d, token);
		if(d[0]!='\n' || d[0]!='\0')
			count++;  /* Increment count for each valid operand */

	}
	/*Checking if there is no missing comma between operands*/
	if(count==2 && j!=1)
	{	
		printf("Error in line %d:Missing comma between operands\n",countline);
		return 0;	
	}
	 /* Check if the number of operands is correct for the given action */
	if(!chaking_operands(count,paction))
	{
		printf("Error in line %d:Incorrect amount of operands\n",countline);
		return 0;
	}
	
	/* handle operations that only accept a single operand when it is a target operand by moving b to c */
	if(paction>=FIVE && paction<=THIRTEEN)
	{	
		strcpy(c,b);
		b[0]='\0';
	}
	 /* If there are no operands, return 1 */		
	if(count==0)
		return 1;
	else if(b[0]!='\0')/*source operand*/
	{
		if(b[0]=='#') /*Immediate addressing*/
		{
			if((isdigit(b[1])) || (((b[1]== '-')|| (b[1]== '+')) && (isdigit(b[TWO]))))
			{	
				token=b+1;
				numopernd=atoi(token);
				if (!is_integer(b))
				{
        				printf("Erorr in line %d: The number is invalid, no real number support\n", countline);
					return 0;
				}
				Immediate_response(numopernd, paction,1,countline,binaryarr,ic);
			}
		}
		else if(((b[0]=='*') && (b[1]=='r') &&(isdigit(b[TWO]))) || ((b[0]=='r') && (isdigit(b[1]))))/*Indirect/direct register addressing*/
		{
			if (!is_integer(b))
			{
        			printf("Erorr in line %d: The number is invalid, no real number support\n", countline);
				return 0;
			}
			doper=1;/* Set flag indicating an operand of this type exists */
			if(isdigit(b[TWO]))
			{
				numopernd=b[TWO]-'0';
				binaryarr[ic][ADDRESS_2_SOURCE]='1';/*Turning on address 2(source operand)*/
			}
			else if(isdigit(b[1]))
			{
				numopernd=b[1]-'0';
				binaryarr[ic][ADDRESS_3_SOURCE]='1';/*Turning on address 3(source operand)*/
			}
			if (!(numopernd>=0 && numopernd<=SEVEN))
			{
        			printf("Erorr in line %d: Register does not exist\n", countline);
				return 0;
			}
			/* Encode the register number into binary format */
			j=INDEX_SIX;
			for(i=1;j>INDEX_NINE;i<<=1)
			{
				binaryarr[ic+1][j]= (numopernd & i)? '1':'0';
				j--;
			}
			binaryarr[ic+1][A]='1';/* Set A bit */
		}
		else if(isalpha(b[0]))/* Label */
		{
			if(chaking_label(b,0,strlen(b),countline))
			{
				strcpy(binaryarr[ic+1],b);
				dlabel=1;/*Lighting a flag that has a label*/
				binaryarr[ic][ADDRESS_1_SOURCE]='1';/*Turning on address 1(source operand */
			}
		}
		else
		{
			printf("Error in line %d: writing an operand, the system does not recognize the type of the operand\n",countline);
			return 0;
		}
	}
	if(count==TWO || b[0]=='\0')/* Handle the destination operand (c) if present or if b is empty */
	{
		if(c[0]=='#') /*Immediate addressing*/
		{
			if((isdigit(c[1]) )||(((c[1]== '-')|| (c[1]=='+'))&& (isdigit(c[TWO]))))
			{
				token=c+1;
				numopernd=atoi(token);
				if (!is_integer(c))
				{
        				printf("Erorr in line %d: The number is invalid, no real number support\n", countline);
					return 0;
				}
				if(b[0]=='\0')
					Immediate_response(numopernd, paction,1,countline,binaryarr,ic);
				else
					Immediate_response(numopernd, paction,TWO,countline,binaryarr,ic);
			}
		}
		else if((c[0]=='*' && c[1]=='r' && isdigit(c[TWO])) || (c[0]=='r' && isdigit(c[1])))/*Indirect/direct register addressing*/
		{
			if (!is_integer(c))
			{
        			printf("Erorr in line %d: The number is invalid, no real number support\n", countline);
				return 0;
			}
			if((c[0]=='r') && (paction==NINE || paction==TEN || paction==THIRTEEN))
			{
				printf("Error in line %d: This operation is not optimized to receive a target operand of this type\n",countline);
				return 0;	
			}
			
			if(isdigit(c[TWO]))
			{	
				numopernd=c[TWO]-'0';
				binaryarr[ic][ADDRESS_2_TARGET]='1';/*Turning on address 2(destination operand)*/
			}
			else if(isdigit(c[1]))
			{
				numopernd=c[1]-'0';
				binaryarr[ic][ADDRESS_3_TARGET]='1';/*Turning on address 3(destination operand)*/
			}
			if (!(numopernd>=0 && numopernd<=SEVEN))
			{
        			printf("Erorr in line %d: Register does not exist\n", countline);
				return 0;
			}
			if(doper==1 || b[0]=='\0')/*Checking if the 2 operands should be coded in one information word*/
			{	
				if(doper==1)
					count=count-1;
				j=INDEX_THREE;
				for(i=1;j>INDEX_SIX;i<<=1)
				{
					binaryarr[ic+1][j]= (numopernd & i)? '1':'0';
					j--;
				}
				binaryarr[ic+1][A]='1';/* Set A bit */
			}
			else
			{
				j=INDEX_THREE;
				for(i=1;j>INDEX_SIX;i<<=1)
				{
					binaryarr[ic+TWO][j]= (numopernd & i)? '1':'0';
					j--;
				}
				binaryarr[ic+TWO][A]='1';/* Set A bit */
			}
		}
		else if(isalpha(c[0]))/* Label */
		{
			if(chaking_label(c,0,strlen(c),countline))
			{	if(b[0]=='\0')
					strcpy(binaryarr[ic+1],c);
				else
					strcpy(binaryarr[ic+TWO],c);
				dlabel=1;/*Lighting a flag that has a label*/
				binaryarr[ic][ADDRESS_1_TARGET]='1';/* Turning on address 1(destination operand)*/
			}
		}
		else
		{
			printf("Error in line %d: writing an operand, the system does not recognize the type of the operand\n",countline);
			return 0;
		}
				
	}
	/* returns the number of rows added to increase the IC in case there is a label returns the negation of the number*/
	if(dlabel==1)/*Checking if a label exists*/
		return (count+1)*(-1);
	return count+1;
}
/*
 * Function: chaking_operands
 * Purpose: Checks if the number of operands is correct for the given action.
 * Inputs:
 *  - amountoperand: Number of operands
 *  - paction: The action code
 * Returns:
 *  - Integer indicating whether the operand count is correct 
 */
int chaking_operands(int amountoperand, int paction)
{
	 /* Actions from 0 to 4 require 2 operands */
   	 if(paction>=0 && paction<=FOUR)
   	 {
      	      if(amountoperand==TWO)
    	                return 1;
 	 }
	 /* Actions from 5 to 13 require 1 operand */
         else if(paction>=FIVE && paction<=THIRTEEN)
   	 {
    	        if(amountoperand==1)
     	               return 1;
 	 }
	  /* Actions 14 and 15 require 0 operands */
 	 else if(paction==FOURTEEN || paction==FIFTEEN)
   	 {
                if(amountoperand==0)
                    return 1;
  	 }
	return 0;/* Return 0 if operand count does not match requirements */
}

/*
 * Function: immediate_response
 * Purpose: Handles an operand of immediate sort type.
 * Entrances:
 * - numopernd: the number that needs to be coded - the number after the #
 * - paction: the action code
 * - Type: source (1) or destination (2) operand
 * - countline: the line number in the source code
 * - binaryarr[][16]: array to store the binary representation
 * - ic: Instruction counter
* Returns:
 * - an integer indicating success or failure
 */
int Immediate_response(int numopernd, int paction, int type, int countline,char binaryarr[][CELL_SIZE_MEMORY],int ic)
{
	int i,j;
	if(type==1)/*source operand*/
	{
		if(!((paction>=0 && paction<FOUR)||(paction==TWELVE)))/* Check if the action supports immediate addressing*/
		{
			printf("Error in line %d: This operation is not optimized to receive a target operand of this type\n",countline);
			return 0;
		}
		else
		{
			/* Encode the immediate operand into binary format */
			j=INDEX_THREE;
			for(i=1;j>=0;i<<=1)
			{
				binaryarr[ic+1][j]= (numopernd & i)? '1':'0';
				j--;
			}
			binaryarr[ic+1][A]='1';/* Set A bit */
			if(paction==TWELVE)
				binaryarr[ic][ADDRESS_0_TARGET]='1';
			else
				binaryarr[ic][ADDRESS_0_SOURCE]='1';/*Turning on address 0(source operand)*/
		}
	}

	else if(type==TWO)/*destination operand*/
	{
		if(!(paction==TWELVE || paction==1))/* Check if the action supports this type of addressing*/
		{
			printf("Error in line %d: This operation is not optimized to receive a target operand of this type\n",countline);
			return 0;
		}
		else
		{
			/* Encode the immediate operand into binary format */
			j=INDEX_THREE;
			for(i=1;j>=0;i<<=1)
			{
				binaryarr[ic+TWO][j]= (numopernd & i)? '1':'0';
				j--;
			}
			binaryarr[ic+TWO][A]='1';/* Set A bit */
			binaryarr[ic][ADDRESS_0_TARGET]='1';/*Turning on address 0(destination operand)*/
		}
	}
	return 1;/* Return 1 to indicate successful handling */
}

/*
 * Function: chaking_action
 * Purpose: Checks if the given word matches a valid action and retrieves its index.
 * Inputs:
 *  - word[]: The string containing the action name 
 *  - start: Start position in the string to check 
 *  - end: End position in the string to check 
 *  - countline: The line number in the source code 
 * Returns:
 *  - Integer indicating the index of the action if valid, otherwise 0 
 */
int chaking_action(char word[],int start,int end, int countline)
{
    	char *name_of_command[AMOUNT_ACTION]={"mov","cmp","add","sub","lea","clr","not","inc","dec","jmp","bne","red","prn","jsr","rts","stop"};
	/* Compare the provided word with the list of valid commands */
	int x=compare_with_words(word,name_of_command,AMOUNT_ACTION);

  	  return x;/* Return the index of the normal operation +1 because there is also a normal operation at position 0 */
}
/*
 Function to check if a string (char[]) represents an integer
 Input: char[] string
 Output: 1 if the string represents an integer, otherwise 0
 */
int is_integer(char c[]) {
	int i;
	for(i=0; i<strlen(c);i++)
	{
		if(c[i]=='.')
			return 0;
	}
    return 1;
}
