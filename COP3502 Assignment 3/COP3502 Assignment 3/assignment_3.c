/* COP 3502C Assignment 3
This program is written by: Gustavo Cornejo */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define NUM_INPUT_FILES 6
#define NUM_OUTPUT_FILES 3
#include "leak_detector_c.h"

typedef struct monster{
    int id;
    char name[64];
    char element[64];
    int population;
    double weight;
} monster;

typedef struct{
    long long int compares;
    long long int copies;
} sort_results;

//Function prototypes
void selectionWrapper(monster* monsterArr, int n, int criteria, FILE* output);
void selectionSort(monster* monsterArr, int n, int criteria, sort_results* counter);
void bubbleWrapper(monster* monsterArr, int n, int criteria, FILE* output);
void bubbleSort(monster* monsterArr, int n, int criteria, sort_results* counter);
void insertionWrapper(monster* monsterArr, int n, int criteria, FILE* output);
void insertionSort(monster* monsterArr, int n, int criteria, sort_results* counter);
void mergeWrapper(monster* monsterArr, int n, int criteria, FILE* output);
void mergeSort(monster* monsterArr, int l, int r, int criteria, sort_results* counter);
void merge2Wrapper(monster* monsterArr, int n, int criteria, FILE* output);
void mergeSort2(monster* monsterArr, int l, int r, int criteria, sort_results* counter);
void insertionSort2(monster* monsterArr, int l, int r, int criteria, sort_results* counter);
void quickWrapper(monster* monsterArr, int n, int criteria, FILE* output);
void quickSort(monster* monsterArr, int low, int high, int criteria, sort_results* counter);
//Required functions
int compareTo(monster* m1, monster* m2, int criteria);
int isSorted(monster* mosnterArr, int length, int criteria);
//Helper funtions
void arrayStatus(monster* monsterArr, int n, int criteria);
void copyArray(monster* arr1, monster* arr2, int n);
void swap(monster* monster1 , monster* monster2);
void print_clocks(clock_t clocks, FILE* output);
void printResults(sort_results* results);

//Main starts here
int main() {

    atexit(report_mem_leak);
    //File creation
    FILE* input[NUM_INPUT_FILES];
    FILE* output[NUM_OUTPUT_FILES];
    input[0] = fopen("10K.txt", "r");
    input[1] = fopen("20K.txt", "r");
    input[2] = fopen("30K.txt", "r");
    input[3] = fopen("40K.txt", "r");
    input[4] = fopen("50K.txt", "r");
    input[5] = fopen("60K.txt", "r");
    output[0] = fopen("Criteria_1.csv", "w+");
    output[1] = fopen("Criteria_2.csv", "w+");
    output[2] = fopen("Criteria_3.csv", "w+");
    //Check file creation
    for (int i = 0; i < NUM_INPUT_FILES; i++) {
        if (input[i] == NULL) {
            printf("Can't open file %dK.txt\n", (i+1)*10);
            return 1;
        }
    }
    for (int i = 0; i < NUM_OUTPUT_FILES; i++) {
        if (output[i] == NULL) {
            printf("Can't write file Criteria_%d.csv\n", i+1);
            return 1;
        }
    }
    //Start variables
    int n = 10000;
    int criteria = 1;
    //Arrays creation
    monster* monsterArr[6];
    monsterArr[0] = (monster*) malloc(n * sizeof(monster));
    monsterArr[1] = (monster*) malloc((2*n) * sizeof(monster));
    monsterArr[2] = (monster*) malloc((3*n) * sizeof(monster));
    monsterArr[3] = (monster*) malloc((4*n) * sizeof(monster));
    monsterArr[4] = (monster*) malloc((5*n) * sizeof(monster));
    monsterArr[5] = (monster*) malloc((6*n) * sizeof(monster));
    
    //Scan input from .txt files
    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < n; j++) {
            fscanf(input[i], "%s %s %d %lf", monsterArr[i][j].name, monsterArr[i][j].element, &monsterArr[i][j].population,  &monsterArr[i][j].weight);
        }
        n += 10000;
    }
    
    while (criteria <= 3) {
        n = 10000;
        fprintf(output[criteria-1], "DataSize, SelectionSortCompare, SelectionSortCopy, SelectionSortTIme, BubbleSortCompare, BubbleSortCopy, BubbleSortTime, InsertionSortCompare, InsertionSortCopy, InsertionSortTime, MergeSortCompare, MergeSortCopy, MergeSortTime, Merge_InsertionSortCompare, Merge_InsertionSortCopy, Merge_InsertionSortTime, QuickSortCompare, QuickSortCopy, QuickSortTime");
        for (int i = 0; i < NUM_INPUT_FILES; i++) {
            printf("==============================================\n");
            printf("=== Processing Criteria %d and file %dK.txt ===\n", criteria, (i+1)*10);
            printf("==============================================\n");
            fprintf(output[criteria-1], "\n%d", n);
            selectionWrapper(monsterArr[i], n, criteria, output[criteria-1]);
            bubbleWrapper(monsterArr[i], n, criteria, output[criteria-1]);
            insertionWrapper(monsterArr[i], n, criteria, output[criteria-1]);
            mergeWrapper(monsterArr[i], n, criteria, output[criteria-1]);
            merge2Wrapper(monsterArr[i], n, criteria, output[criteria-1]);
            quickWrapper(monsterArr[i], n, criteria, output[criteria-1]);
            printf("\n");
            n += 10000;
        }
        criteria++;
    }
    //Close files and free allocated memory
    for (int i = 0; i < NUM_INPUT_FILES; i++) {;
        free(monsterArr[i]);
        fclose(input[i]);
    }
    for (int j = 0; j < NUM_OUTPUT_FILES; j++) {
        fclose(output[j]);
    }
    return 0;
}

void selectionWrapper(monster* monsterArr, int n, int criteria, FILE* output) {
    
    clock_t start_cpu, end_cpu;
    sort_results* selectionResults = (sort_results*) malloc(sizeof(sort_results));
    selectionResults->compares = 0;
    selectionResults->copies = 0;
    //Copy of array allocation
    monster* copyArr = (monster*) malloc(n * sizeof(monster));
    copyArray(monsterArr, copyArr, n);
    //Status of copy before sorting
    arrayStatus(copyArr, n, criteria);
    printf(" before calling selection sort\n");
    start_cpu = clock();
    selectionSort(copyArr, n, criteria, selectionResults);
    end_cpu = clock();
    //Status of copy after sorting
    arrayStatus(copyArr, n, criteria);
    printf(" after calling selection sort\n");
    fprintf(output, ",%lld, %lld", selectionResults->compares, selectionResults->copies);
    print_clocks(end_cpu - start_cpu, output);
    printResults(selectionResults);
    free(selectionResults);
    free(copyArr);
}

void bubbleWrapper(monster* monsterArr, int n, int criteria, FILE* output) {
    
    clock_t start_cpu, end_cpu;
    sort_results* bubbleResults = (sort_results*) malloc(sizeof(sort_results));
    bubbleResults->compares = 0;
    bubbleResults->copies = 0;
    //Copy of array allocation
    monster* copyArr = (monster*) malloc(n * sizeof(monster));
    copyArray(monsterArr, copyArr, n);
    //Status of copy before sorting
    arrayStatus(copyArr, n, criteria);
    printf(" before calling bubble sort\n");
    start_cpu = clock();
    bubbleSort(copyArr, n, criteria, bubbleResults);
    end_cpu = clock();
    //Status of copy after sorting
    arrayStatus(copyArr, n, criteria);
    printf(" after calling bubble sort\n");
    fprintf(output, ",%lld, %lld", bubbleResults->compares, bubbleResults->copies);
    print_clocks(end_cpu - start_cpu, output);
    printResults(bubbleResults);
    free(bubbleResults);
    free(copyArr);
}

void insertionWrapper(monster* monsterArr, int n, int criteria, FILE* output) {
    
    clock_t start_cpu, end_cpu;
    sort_results* insertionResults = (sort_results*) malloc(sizeof(sort_results));
    insertionResults->compares = 0;
    insertionResults->copies = 0;
    //Copy of array allocation
    monster* copyArr = (monster*) malloc(n * sizeof(monster));
    copyArray(monsterArr, copyArr, n);
    //Status of copy before sorting
    arrayStatus(copyArr, n, criteria);
    printf(" before calling insertion sort\n");
    start_cpu = clock();
    insertionSort(copyArr, n, criteria, insertionResults);
    end_cpu = clock();
    //Status of copy after sorting
    arrayStatus(copyArr, n, criteria);
    printf(" after calling insertion sort\n");
    fprintf(output, ",%lld, %lld", insertionResults->compares, insertionResults->copies);
    print_clocks(end_cpu - start_cpu, output);
    printResults(insertionResults);
    free(insertionResults);
    free(copyArr);
}

void mergeWrapper(monster* monsterArr, int n, int criteria, FILE* output) {
    
    clock_t start_cpu, end_cpu;
    sort_results* mergeResult = (sort_results*) malloc(sizeof(sort_results));
    mergeResult->compares = 0;
    mergeResult->copies = 0;
    //Copy of array allocation
    monster* copyArr = (monster*) malloc(n * sizeof(monster));
    copyArray(monsterArr, copyArr, n);
    //Status of copy before sorting
    arrayStatus(copyArr, n, criteria);
    printf(" before calling merge sort\n");
    start_cpu = clock();
    mergeSort(copyArr, 0, n-1, criteria, mergeResult);
    end_cpu = clock();
    //Status of copy after sorting
    arrayStatus(copyArr, n, criteria);
    printf(" after calling merge sort\n");
    fprintf(output, ",%lld, %lld", mergeResult->compares, mergeResult->copies);
    print_clocks(end_cpu - start_cpu, output);
    printResults(mergeResult);
    free(mergeResult);
    free(copyArr);
}

void merge2Wrapper(monster* monsterArr, int n, int criteria, FILE* output) {
    
    clock_t start_cpu, end_cpu;
    sort_results* merge2Result = (sort_results*) malloc(sizeof(sort_results));
    merge2Result->compares = 0;
    merge2Result->copies = 0;
    //Copy of array allocation
    monster* copyArr = (monster*) malloc(n * sizeof(monster));
    copyArray(monsterArr, copyArr, n);
    //Status of copy before sorting
    arrayStatus(copyArr, n, criteria);
    printf(" before calling merge_insertion sort\n");
    start_cpu = clock();
    mergeSort2(copyArr, 0, n-1, criteria, merge2Result);
    end_cpu = clock();
    //Status of copy after sorting
    arrayStatus(copyArr, n, criteria);
    printf(" after calling merge_insertion sort\n");
    fprintf(output, ",%lld, %lld", merge2Result->compares, merge2Result->copies);
    print_clocks(end_cpu - start_cpu, output);
    printResults(merge2Result);
    free(merge2Result);
    free(copyArr);
}

void quickWrapper(monster* monsterArr, int n, int criteria, FILE* output) {
    
    clock_t start_cpu, end_cpu;
    sort_results* quickResult = (sort_results*) malloc(sizeof(sort_results));
    quickResult->compares = 0;
    quickResult->copies = 0;
    //Copy of array allocation
    monster* copyArr = (monster*) malloc(n * sizeof(monster));
    copyArray(monsterArr, copyArr, n);
    //Status of copy before sorting
    arrayStatus(copyArr, n, criteria);
    printf(" before calling quick sort\n");
    start_cpu = clock();
    quickSort(copyArr, 0, n-1, criteria, quickResult);
    end_cpu = clock();
    //Status of copy after sorting
    arrayStatus(copyArr, n, criteria);
    printf(" after calling quick sort\n");
    fprintf(output, ",%lld, %lld", quickResult->compares, quickResult->copies);
    print_clocks(end_cpu - start_cpu, output);
    printResults(quickResult);
    free(quickResult);
    free(copyArr);
}

//SELECTION SORT ALGORITHM
void selectionSort(monster* monsterArr, int n, int criteria, sort_results* counter){

    int i, j, min_index;
    for (i = 0; i < n-1; i++) {
        min_index = i;
        for (j = i+1; j < n; j++){
            if (compareTo(&monsterArr[j], &monsterArr[min_index], criteria) < 0) {
                min_index = j;
            }
            counter->compares++;
        }
        swap(&monsterArr[i], &monsterArr[min_index]);
        counter->copies += 3;
    }
}


//BUBBLE SORT ALGORITHM
void bubbleSort(monster* monsterArr, int n, int criteria, sort_results* counter){
    
    int i, j;
    for (i = 0; i < n-1; i++){
        for (j = 0; j < n-i-1 ; j++){
            if (compareTo(&monsterArr[j], &monsterArr[j+1], criteria) > 0){
                swap(&monsterArr[j], &monsterArr[j+1]);
                counter->copies += 3;
            }
            counter->compares++;
        }
    }
}


//INSERTION SORT ALGORITHM
void insertionSort(monster* monsterArr, int n, int criteria, sort_results* counter){
    
    int i, j;
    monster item;
    
    for (i = 1; i < n; i++) {
        item = monsterArr[i];
        counter->copies++;
        for (j = i-1; j >= 0; j--) {
            counter->compares++;
            if (compareTo(&monsterArr[j], &item, criteria) > 0) {
                monsterArr[j+1] = monsterArr[j];
                counter->copies++;
            }
            else
                break;
        }
        monsterArr[j+1] = item;
        counter->copies++;
    }
}

//MERGE ALGORITHM
void merge(monster* monsterArr, int l, int m, int r, int criteria, sort_results* counter){
    
    int i, j, k;
    int n1 = m - l + 1;
    int n2 = r - m;
    
    //Half-arrays allocation
    monster* L = (monster*) malloc(n1 * sizeof(monster));
    monster* R = (monster*) malloc(n2 * sizeof(monster));
    
    //Copy to allocated arrays
    for (i = 0; i < n1; i++) {
        L[i] = monsterArr[l + i];
        counter->copies++;
    }
    for (j = 0; j < n2; j++) {
        R[j] = monsterArr[m + 1 + j];
        counter->copies++;
    }
    
    i = 0; //Initial index of L[]
    j = 0; //Initial index of R[]
    k = l; //Initial index of merged subarray
    
    while (i < n1 && j < n2) {
        if (compareTo(&L[i], &R[j], criteria) <= 0) {
            monsterArr[k] = L[i];
            counter->copies++;
            i++;
        } else {
            monsterArr[k] = R[j];
            counter->copies++;
            j++;
        }
        counter->compares++;
        k++;
    }
    //Copy what is left in L[], if there is any
    while (i < n1) {
        monsterArr[k] = L[i];
        counter->copies++;
        i++;
        k++;
    }
    //Copy what is left in R[], if there is any
    while (j < n2) {
        monsterArr[k] = R[j];
        counter->copies++;
        j++;
        k++;
    }
    //Free allocated memory
    free(L);
    free(R);
}
void insertionSort2(monster* monsterArr, int l, int r, int criteria, sort_results* counter){
    
    int i, j;
    for (i = l; i <= r; i++) {
        monster item = monsterArr[i];
        counter->copies++;
        
        for (j = i; j > l; j--) {
            counter->compares++;
            if (compareTo(&monsterArr[j-1], &item, criteria) > 0) {
                monsterArr[j] = monsterArr[j - 1];
                counter->copies++;
            }
            else
                break;
        }
        monsterArr[j] = item;
        counter->copies++;
    }
}

//MERGE SORT ALGORITHM
void mergeSort(monster* monsterArr, int l, int r, int criteria, sort_results* counter) {
    
    if (l < r) {
        int m = (l+r)/2; //m: midpoint
        mergeSort(monsterArr, l, m, criteria, counter); //Sort left half
        mergeSort(monsterArr, m+1, r, criteria, counter); //Sort right half
        merge(monsterArr, l, m, r, criteria, counter); //Merge sorted halves
    }
}

//MERGE SORT WITH INSERTION SORT AT n <= 25
void mergeSort2(monster* monsterArr, int l, int r, int criteria, sort_results* counter) {
    
    if (l < r) {
        if (r-l < 25) {
            insertionSort2(monsterArr, l, r, criteria, counter);
        } else {
            int m = (l+r)/2; //m: midpoint
            mergeSort2(monsterArr, l, m, criteria, counter); //Sort left half
            mergeSort2(monsterArr, m+1, r, criteria, counter); //Sort right half
            merge(monsterArr, l, m, r, criteria, counter); //Merge sorted halves
        }
    }
}


//PARTITION ALGORITHM
int partition(monster* monsterArr, int low, int high, int criteria, sort_results* counter) {
    
    int i = low + rand()%(high-low+1);
    swap(&monsterArr[low], &monsterArr[i]);
    counter->copies += 3;
    int lowpos = low;
    low++;
    
    while (low <= high) {
        while (low <= high && compareTo(&monsterArr[low], &monsterArr[lowpos], criteria) <= 0) {
            counter->compares++;
            low++;
        }
        while (high >= low && compareTo(&monsterArr[high], &monsterArr[lowpos], criteria) > 0) {
            counter->compares++;
            high--;
        }
        if (low < high) {
            swap(&monsterArr[low], &monsterArr[high]);
            counter->copies += 3;
        }
    }
    swap(&monsterArr[lowpos], &monsterArr[high]);
    counter->copies += 3;
    return high;
}

//QUICK SORT ALGORITHM
void quickSort(monster* monsterArr, int low, int high, int crititeria, sort_results* counter) {
    
    if (low < high) {
        int split = partition(monsterArr, low, high, crititeria, counter);
        quickSort(monsterArr, low, split-1, crititeria, counter);
        quickSort(monsterArr, split+1, high, crititeria, counter);
    }
}
//Monster index swap function
void swap(monster* monster1 , monster* monster2){
    monster temp = *monster1;
    *monster1 = *monster2;
    *monster2 = temp;
}

int compareTo(monster* m1, monster* m2, int criteria){
    if (criteria == 1)
        return strcmp(m1->name, m2->name);
    if (criteria == 2) {
        if (m1->weight > m2->weight)
            return 1;
        else if (m1->weight == m2->weight)
            return 0;
        else
            return -1;
    }
    if (criteria == 3) {
        if (strcmp(m1->name, m2->name) > 0)
            return 1;
        else if (strcmp(m1->name, m2->name) < 0)
            return -1;
        else {
            if (m1->weight > m2->weight)
                return 1;
            else
                return -1;
        }
    }
    else
        printf("Wrong criterion given\n");
        return 0;
}

int isSorted(monster* mosnterArr, int length, int criteria) {
    
    if (criteria == 1) {
        for (int i = 0; i < length-1; i++) {
            if (strcmp(mosnterArr[i].name, mosnterArr[i+1].name) > 0)
                return 0;
        }
        return 1;
    } else if (criteria == 2) {
        for (int i = 0; i < length-1; i++) {
            if (mosnterArr[i].weight > mosnterArr[i+1].weight)
                return 0;
        }
        return 1;
    } else if (criteria == 3) {
        for (int i = 0; i < length-1; i++) {
            if (strcmp(mosnterArr[i].name, mosnterArr[i+1].name) > 0)
                return 0;
            else if (strcmp(mosnterArr[i].name, mosnterArr[i+1].name) == 0)
                if (mosnterArr[i].weight > mosnterArr[i+1].weight)
                    return 0;
        }
        return 1;
    } else {
        printf("Wrong criteria given\n");
        return -9999;
    }
}

void copyArray(monster* arr1, monster* arr2, int n){
    for (int i = 0; i < n; i++) {
        arr2[i] = arr1[i];
    }
}

void arrayStatus(monster* monsterArr, int n, int criteria){

    if (isSorted(monsterArr, n, criteria) == 0) {
        printf("Array status: not sorted by ");
        if (criteria == 1) {
            printf("name");
        } else if (criteria == 2) {
            printf("weigth");
        } else {
            printf("name and weight");
        }
    } else if (isSorted(monsterArr, n, criteria) == 1) {
        printf("Array status: sorted by ");
        if (criteria == 1) {
            printf("name");
        } else if (criteria == 2) {
            printf("weigth");
        } else {
            printf("name and weight");
        }
    }
}
//This prints the run-time
void print_clocks(clock_t clocks, FILE* output){
    printf("Total time taken: %lfs\n", ((double) clocks) / CLOCKS_PER_SEC);
    fprintf(output, ",%lf", ((double) clocks) / CLOCKS_PER_SEC);
}
//This prints the counters
void printResults(sort_results* results) {
    printf("Total number of comparisons %lld\n", results->compares);
    printf("Total number of copy operations %lld\n", results->copies);
}
