/*
Name: Holly Hardin
DuckID: hollyh
Assignment: CIS 415 Project 0

This is my own work except that I referenced the function Quick_Sort
from beginnersbook.com, as noted above the function
As well as the function Iterator_create from Joe Sventek's notes,
Heap Review & Abstract Data Types, p. 49, referenced for the functions
MallocAList and MallocSList
Roscoe has also helped me a lot while completing this project
*/

#include "anagram.h"

// Referenced from beginnersbook.com/2015/02/quicksort-program-in-c/
void Quick_Sort(char *s, int first, int last)
{
        int i;
        int j;
        int pivot;
        int temp;

        if (first < last)
        {
                pivot = first;
                i = first;
                j = last;

                while (i < j)
                {
                        while (s[i] <= s[pivot] && i < last)
                                i++;
                        while(s[j] > s[pivot])
                                j--;
                        if (i < j)
                        {
                                temp = s[i];
                                s[i] = s[j];
                                s[j] = temp;
                        }
                }

                temp = s[pivot];
                s[pivot] = s[j];
                s[j] = temp;
                Quick_Sort(s, first, j-1);
                Quick_Sort(s, j+1, last);
        }
}

void Uncapitalize(char *s, char *t)
{
        // Referenced from Sventek's notes
        int i;
        i = 0;
        while((s[i] = tolower(t[i])) != '\0')
                i++;
}

// Referenced from Joe Sventek's notes, Heap Review & Abstract
// Data Types, p. 49
// Create a new string list node
struct StringList *MallocSList(char *word)
{
	struct StringList *slist = malloc(sizeof(struct StringList));
	if (slist != NULL)
	{
		slist->Word = malloc(strlen(word) + 1);
		if (slist->Word != NULL)
		{
			slist->Next = NULL;
			strcpy(slist->Word, word);
		}
	}
	return slist;
}

// Referenced from Joe Sventek's notes, Heap Review & Abstract
// Data Types, p. 49
// Create a new anagram node, store the word as sorted char array
// Add S list node with the word
struct AnagramList *MallocAList(char *word)
{
	struct AnagramList *alist = malloc(sizeof(struct AnagramList));
	if (alist != NULL)
	{
		alist->Words = MallocSList(word);
		if (alist->Words != NULL)
		{
			alist->Next = NULL;
			alist->Anagram = malloc(strlen(word) + 1);
			strcpy(alist->Anagram, word);
			Uncapitalize(alist->Anagram, word);
			Quick_Sort(alist->Anagram, 0, strlen(word)-1);
		}
	}
	return alist;
}

// Append a string list node to the end/tail of a string list
void AppendSList(struct StringList **head, struct StringList *node)
{
	while (*head != NULL)
		head = &((*head)->Next);
	*head = node;
}

// Free a string list, including all children
void FreeSList(struct StringList **node)
{
	struct StringList *cursor = *node;
	struct StringList *next;
	while (cursor != NULL)
	{
		next = cursor->Next;
		free(cursor->Word);
		free(cursor);
		cursor = next;
	}
}

// Free an anagram list, including anagram children and string list words
void FreeAList(struct AnagramList **node)
{
	struct AnagramList *cursor = *node;
	struct AnagramList *next;
	while (cursor != NULL)
	{
		next = cursor->Next;
		free(cursor->Anagram);
		FreeSList(&(cursor->Words));
		free(cursor);
		cursor = next;
	}
}

// Format output to a file according to specification
void PrintSList(FILE *file, struct StringList *node)
{
	while (node != NULL)
	{
		if (node->Next == NULL)
			fprintf(file, "\t%s", node->Word);
		else
			fprintf(file,"\t%s\n", node->Word);
		node = node->Next;
	}
}

// Format output to a file, print anagram list with words, according to specifications
void PrintAList(FILE *file, struct AnagramList *node)
{
	struct AnagramList *cursor = node;
	while (cursor != NULL)
	{
		if (SListCount(cursor->Words) > 1)
		{
			fprintf(file, "%s:%d\n", cursor->Anagram, SListCount(cursor->Words));
			PrintSList(file, cursor->Words);
			fprintf(file, "\n");
			cursor = cursor->Next;
		}
		else
			cursor = cursor->Next;
	}
}

// Return the number of strings in the string list
int SListCount(struct StringList *node)
{
	int count = 0;
	while (node != NULL)
	{
		count++;
		node = node->Next;
	}
	return count;
}

// Add a new word to the anagram list: Search the list and add the word
// Search with a sorted lower case version of the word
void AddWordAList(struct AnagramList **node, char *word)
{
	if (*node == NULL)
	{
		*node = MallocAList(word);
	}
	else
	{
		char *buf = malloc(strlen(word) + 1);
		strcpy(buf, word);
		Uncapitalize(buf, word);
		Quick_Sort(buf, 0, strlen(word) - 1);
		int found = 0;
		while (*node != NULL)
		{
			if (strcmp((*node)->Anagram, buf) == 0)
			{
				struct AnagramList **alist_temp = node;
				struct StringList **slist_head_temp = &((*alist_temp)->Words);
				struct StringList *slist_new_temp = MallocSList(word);
				AppendSList(slist_head_temp, slist_new_temp);
				found = 1;
			}
			else if ((strcmp((*node)->Anagram, buf) != 0) && ((*node)->Next == NULL))
			{
				struct AnagramList *temp = MallocAList(word);
				(*node)->Next = temp;
				found = 1;
			}

			if (found == 1)
			{
				free(buf);
				return;
			}
			else
				node = &((*node)->Next);
		}
	}
}
