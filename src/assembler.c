#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>      /* Includes standard library for general utility functions */
#include "assem.h"

/**
 * main - Entry point of the program.
 * 
 * This program processes a list of input files, appending the `.txt` and `.am` extensions to the
 * file names provided as command-line arguments. It reads each `.txt` file, processes it with
 * the `pro_assembler` function, and writes the result to the corresponding `.am` file.
 * 
 * @param argc: The number of command-line arguments.
 * @param argv: An array of strings representing the command-line arguments. The first argument is
 *               the program name, and the subsequent arguments are the base names of the files to process.
 * 
 * @return: Returns 0 on successful completion of the program. Returns 1 if no input files are provided.
 * 
 * The program performs the following steps:
 * 1. Checks if at least one file name argument is provided. If not, it prints an error message and exits.
 * 2. Iterates over each file name argument:
 *    - Appends the `.txt` extension to create the input file name.
 *    - Appends the `.am` extension to create the output file name.
 *    - Attempts to open the input file for reading. If it fails, it prints an error message and continues to the next file.
 *    - Attempts to open the output file for writing. If it fails, it prints an error message, closes the input file, and continues to the next file.
 *    - Calls the `pro_assembler` function to process the input file and write to the output file.
 *    - Closes both input and output files after processing.
 * 3. Exits the program with a status code of 0.
 */
int main(int argc, char *argv[])
{

    char *file_name, *file_name2; /* File names with maximum length of 30 characters */
    FILE *ifp, *ofp;
    int length_name_file;/*אורך השם של הקובץ*/

    /* Check if no file arguments are provided */
    if (argc == 1)
    {
        printf("no file\n");
        return 1;
    }

    length_name_file=strlen(*++argv)+4;/*אורך שם הקובץ + הסיומת*/
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
        exit(1);
    }


    /* Process each file argument */
    while (--argc > 0) /* Iterate over all file arguments */
    {
        strcpy(file_name, *argv); /* Copy base file name */
        strcat(file_name, ".as"); /* Append .txt extension to the input file name */

        strcpy(file_name2, *argv); /* Copy base file name again */
        strcat(file_name2, ".am"); /* Append .am extension to the output file name */

        /* Open the input file for reading */
        if ((ifp = fopen(file_name, "r")) == NULL)
        {
            printf("Cannot open file %s, continue with next file.\n\n", file_name);
            continue;
        }
        if ((ofp = fopen(file_name2, "w+")) == NULL) /* Open the output file for writing and reading*/
        {
            printf("Cannot open file %s, continue with next file.\n\n", file_name2);
            fclose(ifp); /* Close the input file if output file opening fails */
            continue;
        }

        printf("over on file: %s\n", file_name);
	pro_assembler(ifp, ofp, *argv, length_name_file);

        fclose(ifp); /* Close the input file */
        fclose(ofp); /* Close the output file */
	argv++;

    }

	free(file_name);
	free(file_name2);

    return 0; /* Program completed successfully */
}
