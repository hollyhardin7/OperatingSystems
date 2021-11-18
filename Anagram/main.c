/*
Name: Holly Hardin
DuckID: hollyh
Assignment: CIS 415 Project 0

This is my own work except that I referenced Joe Sventek's notes,
titled Heap Review & Abstract Data Types,
which was provided in the lectures notes
I also received help on the main program through the lab of week 2
*/

#include <stdio.h>
#include <stdlib.h>
#include "anagram.h"

int main(int argc, char *argv[])
{
	// Open input file
	if (argc > 3)
		exit(0);

	// Open input file
	FILE *input = NULL;
	if (argc >= 2)
		input = fopen(argv[1], "r");
	else
		input = stdin;
	if (input == NULL)
		exit(0);

	// Open output file
	FILE *output= NULL;
	if (argc == 3)
		output = fopen(argv[2], "w");
	else
		output = stdout;
	if (output == NULL)
		exit(0);

	// Referenced from Sventek's notes, Heap Review & Abstract
	// Data Types, p. 19
	// While getline from input file, add word to a list
	char buffer[1024];
	int len;
	int i = 0;
	struct AnagramList *alist = NULL;
	while(fgets(buffer, sizeof(buffer), input) != NULL)
	{
		len = strlen(buffer) - 1;
		if (buffer[len] == '\n')
			buffer[len] = '\0';

		AddWordAList(&alist, buffer);
		i++;
	}

	// Print a list
	PrintAList(output, alist);

	// Free a list
	FreeAList(&alist);

	// Close input and output files
	fclose(input);
	fclose(output);

	// Exit program
	return 0;
}
