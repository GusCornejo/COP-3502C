/* COP 3502C Assignment 4
This program is written by: Gustavo Cornejo */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define MAXLEN 30 //Max length for tree strings
#include "leak_detector_c.h"

FILE* output; //Global pointer variable for output

typedef struct itemNode {
    char name[MAXLEN];
    int count;
    struct itemNode* left;
    struct itemNode* right;
} itemNode;

typedef struct treeNameNode {
    char treeName[MAXLEN];
    struct treeNameNode* left;
    struct treeNameNode* right;
    itemNode* theTree;
} treeNameNode;

//Required Functions
treeNameNode* createTreeNameNode(char* treeName);
treeNameNode* buildNameTree(treeNameNode* root, FILE* input, int N);
void traverse_in_traverse(treeNameNode *root);
treeNameNode* searchNameNode(treeNameNode* root, char treeName[50]);
//Helper Functions Found in Main
treeNameNode* buildItemTree(treeNameNode* root, FILE* input, int I);
treeNameNode* performQueries(treeNameNode* root, FILE* input, int Q);
void freeTrees(treeNameNode* root);

//Main starts here
int main() {
    
    atexit(report_mem_leak);
    
    FILE* input = fopen("in.txt", "r");
    output = fopen("out.txt", "w");
    
    //File Exceptions Handling
    if (input == NULL) {
        printf("Warning: system could not read the input file");
        return 1;
    }
    if (output == NULL) {
        printf("Warning: systme could not write the output file");
        return 1;
    }
    
    treeNameNode* root = NULL;  // Root declaration, which serves as root of the whole BST.
    
    int N, I, Q;
    fscanf(input, "%d %d %d", &N, &I, &Q);  //Get number of name trees(N), number of items(I), and number of queries(Q).
    
    root = buildNameTree(root, input, N);   //Creates names BST
    root = buildItemTree(root, input, I);   //Creates items BST on repective names pointers
    traverse_in_traverse(root);             //Prints the whole tree before queries
    root = performQueries(root, input, Q);  //Performs all given queries
    
    //Free Stuff
    freeTrees(root);
    fclose(input);
    fclose(output);
}

//This creates and initializes a treeNameNode
treeNameNode* createTreeNameNode(char* treeName) {
    
    treeNameNode* temp;
    temp = (treeNameNode*) malloc(sizeof(treeNameNode));
    strcpy(temp->treeName, treeName);
    temp->left = NULL;
    temp->right = NULL;
    temp->theTree = NULL;
    return temp;
}

//BST insert for treeNameNode
treeNameNode* insertNameNode(treeNameNode* root, treeNameNode* element) {
    
    if (root == NULL) {
        return element;
    } else {
        if (strcmp(element->treeName, root->treeName) > 0) {
            if (root->right != NULL) {
                root->right = insertNameNode(root->right, element);
            } else {
                root->right = element;
            }
        } else {
            if (root->left != NULL) {
                root->left = insertNameNode(root->left, element);
            } else {
                root->left = element;
            }
        }
        return root;
    }
}

//Wrapper Function for 1st part of input (Ns)
treeNameNode* buildNameTree(treeNameNode* root, FILE* input, int N){
    
    treeNameNode* tempNameNode;
    for (int i = 0; i < N; i++) {
        char name[MAXLEN];
        fscanf(input, "%s", name);
        tempNameNode = createTreeNameNode(name);
        root = insertNameNode(root, tempNameNode);
    }
    return root;
}

//This creates and initializes an itemNode
itemNode* createItemTreeNode(char* treeName, char* itemName, int count) {
    
    itemNode* temp;
    temp = (itemNode*) malloc(sizeof(itemNode));
    strcpy(temp->name, itemName);
    temp->count = count;
    temp->left = NULL;
    temp->right = NULL;
    return temp;
}

//BST insert for itemNode
itemNode* insertItemNode(itemNode* root, itemNode* element) {
    
    if (root == NULL) {
        return element;
    } else {
        if (strcmp(element->name, root->name) > 0) {
            if (root->right != NULL) {
                root->right = insertItemNode(root->right, element);
            } else {
                root->right = element;
            }
        } else {
            if (root->left != NULL) {
                root->left = insertItemNode(root->left, element);
            } else {
                root->left = element;
            }
        }
        return root;
    }
}

//Wrapper Function for 2nd part of input (Is)
treeNameNode* buildItemTree(treeNameNode* root, FILE* input, int I) {
    
    itemNode* tempItemNode;
    for (int j = 0; j < I; j++) {
        char treeName[MAXLEN];
        char itemName[MAXLEN];
        int count;
        fscanf(input, "%s %s %d\n", treeName, itemName, &count);
        tempItemNode = createItemTreeNode(treeName, itemName, count);
        treeNameNode* temp = searchNameNode(root, treeName);
        temp->theTree = insertItemNode(temp->theTree, tempItemNode);
    }
    return root;
}

//Command Functions Used in performQueries()
void searchCommand(treeNameNode* root, FILE* input);
void itemBeforeCommand(treeNameNode* root, FILE* input);
void heightBalanceCommand(treeNameNode* root, FILE* input);
void countCommand(treeNameNode* root, FILE* input);
void deleteCommand(treeNameNode* root, FILE* input);
void deleteNameCommand(treeNameNode* root, FILE* input);
void reduceCommand(treeNameNode* root, FILE* input);

//Wrapper Function for 3rd part of input (Qs)
treeNameNode* performQueries(treeNameNode* root, FILE* input, int Q) {
    //All command funtions print to output file using data from the trees. The exception is searchCommand when items/names do not exist.
    //None delete functions return the deleted node, so their data come from search functions.
    for (int k = 0; k < Q; k++) {
        char querie[MAXLEN];
        fscanf(input, "%s", querie);
        if (strcmp(querie, "search") == 0)
            searchCommand(root, input);
        else if (strcmp(querie, "item_before") == 0)
            itemBeforeCommand(root, input);
        else if (strcmp(querie, "height_balance") == 0)
            heightBalanceCommand(root, input);
        else if (strcmp(querie, "count") == 0)
            countCommand(root, input);
        else if (strcmp(querie, "delete") == 0)
            deleteCommand(root, input);
        else if (strcmp(querie, "delete_name") == 0)
            deleteNameCommand(root, input);
        else if (strcmp(querie, "reduce") == 0)
            reduceCommand(root, input);
    }
    return root;
}

//Search treeNameNode by using name a key (it's used multiple times throughout the program)
treeNameNode* searchNameNode(treeNameNode* root, char treeName[50]) {
    
    if (root == NULL) {
        return 0;
    } else {
        if (strcmp(root->treeName, treeName) == 0) {
            return root;
        } else if (strcmp(root->treeName, treeName) > 0) {
            return searchNameNode(root->left, treeName);
        } else {
            return searchNameNode(root->right, treeName);
        }
    }
}

//Search itemNode by using name a key (it's used multiple times throughout the program)
itemNode* searchItemNode(itemNode* root, char itemName[50]) {
    
    if (root == NULL) {
        return 0;
    } else {
        if (strcmp(root->name, itemName) == 0) {
            return root;
        } else if (strcmp(root->name, itemName) > 0) {
            return searchItemNode(root->left, itemName);
        } else {
            return searchItemNode(root->right, itemName);
        }
    }
}

//Counts itemNode(s) that come before a itemNode with specific name
int findItemsBefore(itemNode* root, char itemName[50]) {
    
    int count = 0;
    if (root == NULL) {
        return 0;
    } else if (strcmp(root->name, itemName) < 0) {
        count++;
        count+= findItemsBefore(root->left, itemName);
        count+= findItemsBefore(root->right, itemName);
    } else {
        count += findItemsBefore(root->left, itemName);
    }
    return count;
}

//Returns the height of a given tree/subtree
int height(itemNode* root) {
    
    int leftHeight;
    int rightHeight;
    if (root == NULL) {
        return -1;
    }
    leftHeight = height(root->left);
    rightHeight = height(root->right);
    
    if (leftHeight > rightHeight) {
        return 1 + leftHeight;
    } else {
        return 1 + rightHeight;
    }
}

//Sums all the counts of an itemTree
int countSum(itemNode* root) {
    
    int count = 0;
    if (root != NULL) {
        count += countSum(root->left);
        count += countSum(root->right);
        return count + root->count;
    }
    return 0;
}

//Finds minimum itemNode of an itemTree (it's only used in deleteItemNode())
itemNode* minItemNode(itemNode* root) {
    itemNode* temp = root;
    while (temp && temp->left != NULL) {
        temp = temp->left;
    }
    return temp;
}

//Deletes a itemNode by using its name as key (it's called in only one command)
itemNode* deleteItemNode(itemNode* root, char itemName[50]) {
    
    if (root == NULL) {
        return root;
    }
    if (strcmp(itemName, root->name) < 0) {
        root->left = deleteItemNode(root->left, itemName);
    } else if (strcmp(itemName, root->name) > 0){
        root->right = deleteItemNode(root->right, itemName);
    } else {
        if (root->left == NULL) {
            itemNode* temp = root->right;
            free(root);
            return temp;
        } else if (root->right == NULL) {
            itemNode* temp = root->left;
            free(root);
            return temp;
        }
        itemNode* temp = minItemNode(root->right);
        strcpy(root->name, temp->name);
        root->count = temp->count;
        root->right = deleteItemNode(root->right, temp->name);
    }
    return root;
}

//Finds minimum treeNameNode of an nameTree (it's only used in deleteNameNode())
treeNameNode* minNameNode(treeNameNode* root) {
    treeNameNode* temp = root;
    while (temp && temp->left != NULL) {
        temp = temp->left;
    }
    return temp;
}

//Deletes a treeNameNode by using its name as key (it's called in only one command)
treeNameNode* deleteNameNode(treeNameNode* root, char treeName[50]) {
    
    if (root == NULL) {
        return root;
    }
    if (strcmp(treeName, root->treeName) < 0) {
        root->left = deleteNameNode(root->left, treeName);
    } else if (strcmp(treeName, root->treeName) > 0){
        root->right = deleteNameNode(root->right, treeName);
    } else {
        if (root->left == NULL) {
            treeNameNode* temp = root->right;
            free(root);
            return temp;
        } else if (root->right == NULL) {
            treeNameNode* temp = root->left;
            free(root);
            return temp;
        }
        treeNameNode* temp = minNameNode(root->right);
        strcpy(root->treeName, temp->treeName);
        root->right = deleteNameNode(root->right, temp->treeName);
    }
    return root;
}

//Deletes a whole itemTree (it's called in more than one command)
void deleteItemTree(itemNode* root) {
    
    if (root != NULL) {
        deleteItemTree(root->left);
        deleteItemTree(root->right);
        free(root);
    }
}

//searchCommand Fucntion (it doesn't assume that given node exists)
void searchCommand(treeNameNode* root, FILE* input) {
    
    char treeName[MAXLEN];
    char itemName[MAXLEN];
    fscanf(input, "%s %s", treeName, itemName);
    treeNameNode* tempNameNode = searchNameNode(root, treeName);
    if (tempNameNode == 0)
        //Since this nameNode doesn't exist, the output comes from input
        fprintf(output, "%s does not exist\n", treeName);
    else {
        itemNode* tempItemNode;
        tempItemNode = searchItemNode(tempNameNode->theTree, itemName);
        if (tempItemNode == 0)
            //Since this itemNode isn't in given itemTree, the output comes from input
            fprintf(output, "%s not found in %s\n", itemName, tempNameNode->treeName);
        else
            fprintf(output, "%d %s found in %s\n", tempItemNode->count, tempItemNode->name, tempNameNode->treeName);
    }
}

//itemBeforeCommand Function
void itemBeforeCommand(treeNameNode* root, FILE* input) {
    
    char treeName[MAXLEN];
    char itemName[MAXLEN];
    fscanf(input, "%s %s", treeName, itemName);
    treeNameNode* temp = searchNameNode(root, treeName);
    itemNode* outputItem = searchItemNode(temp->theTree, itemName);
    int count = 0;
    count = findItemsBefore(temp->theTree, itemName);
    fprintf(output, "item before %s: %d\n", outputItem->name, count);
}

//heightBalanceCommand Function
void heightBalanceCommand(treeNameNode* root, FILE* input) {
    
    char treeName[MAXLEN];
    fscanf(input, "%s", treeName);
    treeNameNode* tempNameNode = searchNameNode(root, treeName);
    int left_height = height(tempNameNode->theTree->left);
    int right_height = height(tempNameNode->theTree->right);
    int difference = abs(left_height - right_height);
    if (difference < 2) {
        fprintf(output, "%s: left height %d, right height %d, difference %d, balanced\n", tempNameNode->treeName, left_height, right_height, difference);
    } else
        fprintf(output, "%s: left height %d, right height %d, difference %d, not balanced\n", tempNameNode->treeName, left_height, right_height, difference);
}

//countCommand Function
void countCommand(treeNameNode* root, FILE* input) {
    
    char treeName[MAXLEN];
    fscanf(input, "%s", treeName);
    treeNameNode* tempNameNode = searchNameNode(root, treeName);
    int sum = countSum(tempNameNode->theTree);
    fprintf(output, "%s count %d\n", tempNameNode->treeName, sum);
}

//deleteCommand Function (it assumes that given node exists)
void deleteCommand(treeNameNode* root, FILE* input) {
    
    char treeName[MAXLEN];
    char itemName[MAXLEN];
    fscanf(input, "%s %s", treeName, itemName);
    treeNameNode* tempNameNode = searchNameNode(root, treeName);
    itemNode* outputItem = searchItemNode(tempNameNode->theTree, itemName);
    char itemNameFromTree[MAXLEN];
    strcpy(itemNameFromTree, outputItem->name);
    tempNameNode->theTree = deleteItemNode(tempNameNode->theTree, itemName);
    fprintf(output, "%s deleted from %s\n", itemNameFromTree, tempNameNode->treeName);
}

//deleteNameCommand Function (it assumes that given node exists)
void deleteNameCommand(treeNameNode* root, FILE* input) {
    
    char treeName[MAXLEN];
    fscanf(input, "%s", treeName);
    treeNameNode* tempNameNode = searchNameNode(root, treeName);
    char treeNameFromTree[MAXLEN];
    strcpy(treeNameFromTree, tempNameNode->treeName);
    deleteItemTree(tempNameNode->theTree); //Deletes the entire itemTree pointed by given nameNode
    root = deleteNameNode(root, treeName); //Deletes the nameNode in nameTree
    fprintf(output, "%s deleted\n", treeNameFromTree);
}

//reduceCommand is the only command that uses already existing functions
void reduceCommand(treeNameNode* root, FILE* input) {
    
    char treeName[MAXLEN];
    char itemName[MAXLEN];
    int reduce;
    fscanf(input, "%s %s %d", treeName, itemName, &reduce);
    treeNameNode* tempNameNode = searchNameNode(root, treeName);
    itemNode* tempItemNode = searchItemNode(tempNameNode->theTree, itemName);
    tempItemNode->count -= reduce;
    if (tempItemNode->count <= 0) {
        fprintf(output, "%s reduced\n", tempItemNode->name);
        tempNameNode->theTree = deleteItemNode(tempNameNode->theTree, itemName);
    }
    fprintf(output, "%s reduced\n", tempItemNode->name);
}

//Traverse Functions

//inOrder function for treeNameNodes (it omits pointed trees)
void inOrderForNames(treeNameNode* root) {
    if (root != NULL) {
        inOrderForNames(root->left);
        fprintf(output, "%s ", root->treeName);
        inOrderForNames(root->right);
    }
}

//inOrder function for itemNodes
void inOrderForItems(itemNode* root) {
    if (root != NULL) {
        inOrderForItems(root->left);
        fprintf(output, "%s ", root->name);
        inOrderForItems(root->right);
    }
}

//Modified inOrder function for treeNameNodes (it doesn't omit pointed trees)
void inOrderIncludingItemTree(treeNameNode* root) {
    if (root != NULL) {
        inOrderIncludingItemTree(root->left);
        fprintf(output, "===%s===\n", root->treeName);
        inOrderForItems(root->theTree);
        fprintf(output, "\n");
        inOrderIncludingItemTree(root->right);
    }
}

//Wrapper funtion for tree output before commands
void traverse_in_traverse(treeNameNode *root) {
    inOrderForNames(root);
    fprintf(output, "\n");
    inOrderIncludingItemTree(root);
}

//Free function (it reuses deleteItemTree)
void freeTrees(treeNameNode* root) {
    if (root != NULL) {
        if (root->theTree != NULL)
            deleteItemTree(root->theTree);
        freeTrees(root->left);
        freeTrees(root->right);
        free(root);
    }
}
