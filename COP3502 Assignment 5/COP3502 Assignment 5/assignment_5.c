/* COP 3502C Programming Assignment 5
This program is written by: Gustavo Cornejo
This program generates output to both the console and a .txt file */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define NUM_LETTERS 26 
#include "leak_detector_c.h"

//trie_node declaration
typedef struct trie_node {
    
    int count;
    int maxCountChild;
    struct trie_node* next[NUM_LETTERS];
    
} trie_node;

//Used Functions
trie_node* intitTrie(void);
void commandOne(trie_node* root, FILE* input);
void insert(trie_node* root, char word[], int k, int count);
void commandTwo(trie_node* root, FILE* input, FILE* output);
void insertMaxCount(trie_node* root, char word[], int k);
void prediction(trie_node* root, char word[], int k, FILE* output);
void freeTrie(trie_node* root);

//Program starts here
int main() {
    
    atexit(report_mem_leak);
    //Trie root declaration
    trie_node* root = intitTrie();
    FILE* input = fopen("in.txt", "r");
    FILE* output = fopen("out.txt", "w");
    //File exceptions handling
    if (input == NULL || output == NULL) {
        printf("Warning file could not be opened or written\n");
        return 1;
    }
    int n, i, command;
    fscanf(input, "%d", &n);
    for (i = 0; i < n; i++) {
        fscanf(input, "%d", &command);
        if (command == 1) {
            commandOne(root, input);
        } else if(command == 2) {
            commandTwo(root, input, output);
        } else {
            //In case input is invalid
            printf("---Invalid input file---\n");
            return 1;
        }
    }
    //Free stuff
    freeTrie(root);
    fclose(input);
    fclose(output);
    return 0;
}

//Creates and initializes a trie node
trie_node* intitTrie(){
    
    trie_node* temp = (trie_node*) malloc(sizeof(trie_node));
    temp->count = 0;
    temp->maxCountChild = 0;
    for (int i = 0; i < NUM_LETTERS; i++) {
        temp->next[i] = NULL;
    }
    return temp;
}

//Wrapper function for 1st command
void commandOne(trie_node* root, FILE* input) {
    
    char word[50];
    int count;
    fscanf(input, "%s %d", word, &count);
    insert(root, word, 0, count);
}

//Modified trie insert() function
void insert(trie_node* root, char word[], int k, int count) {
   
    if (k == strlen(word)) {
        return;
    }
    int nextIndex = word[k] - 'a';
    if (root->next[nextIndex] == NULL) {
        root->next[nextIndex] = intitTrie();
    }
    root->next[nextIndex]->count += count;
    insert(root->next[nextIndex], word, k+1, count);
}

//Wrapper function for 2nd command
void commandTwo(trie_node* root, FILE* input, FILE* output) {
    
    char word[50];
    fscanf(input, "%s", word);
    insertMaxCount(root, word, 0);
    prediction(root, word, 0, output);
}

//Finds the child with max count
void insertMaxCount(trie_node* root, char word[], int k) {
    
    int nextIndex = word[k] - 'a';
    if (strlen(word) == k) {
        for (int i = 0; i < NUM_LETTERS; i++) {
            if (root->next[i] != NULL && root->next[i]->count > root->maxCountChild) {
                root->maxCountChild = root->next[i]->count;
            }
        }
    } else if (root->next[nextIndex] != NULL){
        insertMaxCount(root->next[nextIndex], word, k+1);
    }
}

//Predicts the next char for a given word or string
void prediction(trie_node* root, char word[], int k, FILE* output){
    
    int nextIndex = word[k] - 'a';
    if (k == strlen(word)) {
        char PossibleChar = '\0';
        for (int i = 0; i < NUM_LETTERS; i++) {
            if (root->next[i] != NULL && root->maxCountChild == root->next[i]->count) {
                PossibleChar = i + 'a';
                fprintf(output, "%c", PossibleChar);
                printf("%c", PossibleChar);
            }
        }
        if (PossibleChar == '\0') {
            printf("unknown word");
            fprintf(output, "unknown word");
        }
        fprintf(output, "\n");
        printf("\n");
        return;
    } else if (root->next[nextIndex] == NULL) {
        printf("unknown word\n");
        fprintf(output, "unknown word\n");
        return;
    } else {
        prediction(root->next[nextIndex], word, k+1, output);
    }
}

//Frees allocated memory for the whole trie since there is not del() function
void freeTrie(trie_node* root) {
    
    int i;
    for (i = 0; i < NUM_LETTERS; i++) {
        if (root->next[i] != NULL) {
            freeTrie(root->next[i]);
        }
    }
    free(root);
}
