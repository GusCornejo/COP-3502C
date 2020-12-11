                                /* COP3502C Programming Assignment #1
                            This program is written by: Gustavo Cornejo*/


//#include "leak_detector_c.h"
//I tried to run the leak detector but it did not work either in IDE or my console. I used it in EUSTIS (compiling the c file alongside with mine), but it does not create any output about possible leaks. It only gave me errors, yet it didn't stop my porgram from runnig. I include it in comments just in case it makes an error as it did to me. The good thing is that my program works in EUSTIS. The bad thing is that I don't know if I have leaks. Also, my lab TA never explained how to use the detector, as the professor states.

//library definitions
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
//constants asked by professor
#define MAXLENGTH 50
#define MAXPOPULATION 1000000

//Struct definitions
typedef struct monster{
    int id;//never used it
    char* name;
    char* element;
    int population;
} monster;

typedef struct region{
    char* name;
    int nmonster;
    int total_population;
    monster** monsters;
} region;

typedef struct itinerary{
    int nregions;
    region** regions;
    int captures;
} itinerary;

typedef struct trainer{
    char* name;
    itinerary* visits;
} trainer;

//Function definitions
monster* createMonster(char* monsterName, char* monsterElement, int monsterPopulation);
monster** readMonster(FILE* infile, int* monsterCount);
region* createRegion(char* regionName, int numberMonster);
region** readRegion(FILE* infile, int* regionCount, int monsterCount, monster** monsterInfo);
itinerary* createItinerary(int trainerVisits, int nCaptures, int regionCount, char** regionsToVisit, region** regionInfo);
trainer* readTrainers(FILE* infile, int trainerCount, int regionCount, int monsterCount, region** regionInfo);
void createFile(FILE* outfile, trainer* Trainer, int counterT);
void freeMemory(monster** M, region** R, trainer* T, int mCount, int rCount, int tCount);

//Program starts here!
int main(){
    //DID NOT WORK!
    //atexit(report_mem_leak);
    
    FILE* infile;
    FILE* outfile;
    
    //in case the file is missing or something like that
    infile = fopen("in.txt", "r");
    outfile = fopen("out.txt", "w");
    
    if (infile == NULL) {
        printf("Can't open the file\n");
        return 1;
    }
    if (outfile == NULL) {
        printf("Can't open the file\n");
        return 1;
    }
    
    int mcount;
    int rcount;
    int tcount;
    
    //malloc is used multiple times starting from here
    fscanf(infile, "%d", &mcount);
    monster** monsterList;
    monsterList = readMonster(infile, &mcount);
    
    fscanf(infile, "%d", &rcount);
    region** regionList;
    regionList = readRegion(infile, &rcount, mcount, monsterList);
    
    fscanf(infile, "%d", &tcount);
    trainer* trainerList;
    trainerList = readTrainers(infile, tcount, rcount, mcount, regionList);
    //malloc is no longer used beyond this point
    
    createFile(outfile, trainerList, tcount);
    
    freeMemory(monsterList, regionList, trainerList, mcount, rcount, tcount);
    
    //close files to avoid data overflow?
    fclose(infile);
    fclose(outfile);
    
    return 0; //Program ends here!
}


//This function creates a monster profile, and it's called multiple times throughout the program
monster* createMonster(char* monsterName, char* monsterElement, int monsterPopulation){
    
    monster* monsterSort;
    monsterSort = (struct monster*) malloc(sizeof(monster));
    monsterSort->name = (char*) malloc((MAXLENGTH + 1) * sizeof(char*));
    monsterSort->name = strcpy(monsterSort->name, monsterName);
    monsterSort->element = (char*) malloc((MAXLENGTH + 1) * sizeof(char*));
    monsterSort->element = strcpy(monsterSort->element, monsterElement);
    monsterSort->population = monsterPopulation;
    return monsterSort;
}

//This function reads input for monsters. It's called once, but its return is used later
monster** readMonster(FILE* infile, int* monsterCount){
    
    int i;
    char name[MAXLENGTH];
    char element[MAXLENGTH];
    int population;
    
    fscanf(infile, "%*[^\n]");
    monster** monsterInfo;
    monsterInfo = (struct monster**) malloc(*monsterCount * sizeof(monster*));
    for (i = 0; i < *monsterCount; i++) {
        fscanf(infile, "%s %s", name, element);
        fscanf(infile, "%d", &population);
        if (population <= MAXPOPULATION) {
            monsterInfo[i] = createMonster(name, element, population);
        } else {
            printf("The monster population is too high\n");//Just in case
        }
    }
    return monsterInfo;
}

//This does the same as createMonster function. It's used more than onece too
region* createRegion(char* regionName, int numberMonster){
    
    region* sortRegion;
    sortRegion = (struct region*) malloc(sizeof(region));
    sortRegion->name = (char*) malloc((MAXLENGTH + 1) * sizeof(char));
    sortRegion->monsters = (struct monster**) malloc(sizeof(monster*));
    sortRegion->name = strcpy(sortRegion->name, regionName);
    sortRegion->nmonster = numberMonster;
    
    return sortRegion;
}

//This function reads input about regions, and it calls both create functions
region** readRegion(FILE* infile, int* regionCount, int monsterCount, monster** monsterInfo){
    
    int i, j;
    char rName[MAXLENGTH];
    int nmonsters;
    char rMonster[MAXLENGTH];
    
    fscanf(infile, "%*[^\n]");//I added these to skip input no needed
    region** regionInfo;
    regionInfo = (struct region**) malloc(*regionCount * sizeof(region*));
    for (i = 0; i < *regionCount; i++) {
        
        fscanf(infile, "%s %d", rName, &nmonsters);
        fscanf(infile, "%*[^\n]");
        regionInfo[i] = createRegion(rName, nmonsters);
        regionInfo[i]->total_population = 0;

        for (j = 0; j < nmonsters; j++) {
            fscanf(infile, "%s", rMonster);
                for (int k = 0; k < monsterCount; k++) {
                if ((strcmp(rMonster, monsterInfo[k]->name)) == 0) {
                    regionInfo[i]->total_population += monsterInfo[k]->population;
                    regionInfo[i]->monsters[j] = (struct monster*) malloc(sizeof(monster));
                    regionInfo[i]->monsters[j] = createMonster(monsterInfo[k]->name, monsterInfo[k]->element, monsterInfo[k]->population);
                    
                }
            }
        }
    }
    return regionInfo;
}

//This function serves as bridge between the next function and the previus ones. It is also a create kind, but it's only called once (per trainer)
itinerary* createItinerary(int trainerVisits, int nCaptures, int regionCount, char** regionsToVisit, region** regionInfo){
    
    itinerary* itinerarySort;
    itinerarySort = (struct itinerary*) malloc(sizeof(itinerary));
    itinerarySort->captures = nCaptures;
    itinerarySort->nregions = trainerVisits;
    itinerarySort->regions = (struct region**) malloc(trainerVisits * sizeof(region*));
        
    for (int i = 0; i < regionCount; i++) {
        for (int temp = 0; temp < trainerVisits; temp++){
            if (strcmp(regionsToVisit[temp], regionInfo[i]->name) == 0) {
                itinerarySort->regions[temp] = (struct region*) malloc(sizeof(region));
                itinerarySort->regions[temp] = createRegion(regionsToVisit[temp], regionInfo[i]->nmonster);//
                itinerarySort->regions[temp]->total_population = regionInfo[i]->total_population;
                for (int j = 0; j < regionInfo[i]->nmonster; j++) {
                    itinerarySort->regions[temp]->monsters[j] = (struct monster*) malloc(sizeof(monster*));
                    itinerarySort->regions[temp]->monsters[j] = createMonster(regionInfo[i]->monsters[j]->name, regionInfo[i]->monsters[j]->element, regionInfo[i]->monsters[j]->population);
                }
            }
        }
    }
    return itinerarySort;
}

//This function reads input about trainer and only creates the name of each. It's called once
trainer* readTrainers(FILE* infile, int trainerCount, int regionCount, int monsterCount, region** regionInfo){
    
    int i, j;
    char tName[MAXLENGTH];
    char** rVisitName;
    int captures;
    int regionVisits;
    
    trainer* trainerInfo;
    trainerInfo = (struct trainer*) malloc(trainerCount * sizeof(trainer));

    fscanf(infile, "%*[^\n]");

    for (i = 0; i < trainerCount; i++) {
        
        fscanf(infile, "%s", tName);
        fscanf(infile, "%d", &captures);
        fscanf(infile, "%*[^\n]");
        fscanf(infile, "%d", &regionVisits);
        fscanf(infile, "%*[^\n]");
        
        trainerInfo[i].name = (char*) malloc(sizeof(char*));
        trainerInfo[i].name = strcpy(trainerInfo[i].name , tName);
        rVisitName = (char**)malloc(regionVisits * sizeof(char*));//allocating this memmory was the only solution I had to avoid overwriting info between each visits. It's used to storage the region names of each visit per trainer.
        
        for (j = 0; j < regionVisits; j++) {
            rVisitName[j] = (char*) malloc((MAXLENGTH+ 1) * sizeof(char*));
            fscanf(infile, "%s", rVisitName[j]);
        }
        trainerInfo[i].visits = (struct itinerary*) malloc(sizeof(itinerary));
        trainerInfo[i].visits = createItinerary(regionVisits, captures, regionCount, rVisitName, regionInfo);
        
        for (int k = 0; k < regionVisits; k++) {
            free(rVisitName[k]);
        }
        free(rVisitName);
        //I free the char array over here since by this point, it has completed its purpose
    }
    return trainerInfo;
}

//This function prints the output in both a file and the console
void createFile(FILE* outfile, trainer* Trainer, int counterT){
    
    for (int i = 0; i < counterT; i++) {
        
        fprintf(outfile, "%s\n", Trainer[i].name);
        printf("%s\n", Trainer[i].name);
        
        for (int j = 0; j < Trainer[i].visits->nregions; j++) {
            
            fprintf(outfile, "%s\n",Trainer[i].visits->regions[j]->name);
            printf("%s\n", Trainer[i].visits->regions[j]->name);
            
            for (int k = 0; k < Trainer[i].visits->regions[j]->nmonster; k++) {
                int count = round((1.0*Trainer[i].visits->regions[j]->monsters[k]->population/(Trainer[i].visits->regions[j]->total_population)*Trainer[i].visits->captures));
                if (count > 0) {
                    fprintf(outfile, "%d %s\n", count, Trainer[i].visits->regions[j]->monsters[k]->name);
                    printf("%d %s\n", count, Trainer[i].visits->regions[j]->monsters[k]->name);
                }
            }
        }
        fprintf(outfile, "\n");
        printf("\n");
    }
}

//This function releases all the allocated memory used in the program (I did my best in here)
void freeMemory(monster** M, region** R, trainer* T, int mCount, int rCount, int tCount){
    
    for (int i = 0; i < tCount; i++) {
        for (int j = 0 ; j < T[i].visits->nregions; j++) {
            for (int k = 0; k < T[i].visits->regions[j]->nmonster; k++) {
                free(T[i].visits->regions[j]->monsters[k]->element);
                free(T[i].visits->regions[j]->monsters[k]->name);
            }
            free(T[i].visits->regions[j]->monsters);
            free(T[i].visits->regions[j]->name);
            free(T[i].visits->regions[j]);
        }
        free(T[i].visits->regions);
        free(T[i].visits);
        free(T[i].name);
    }
    free(T);
    
    for (int l = 0; l < rCount; l++) {
        for (int m = 0; m < R[l]->nmonster; m++) {
            free(R[l]->monsters[m]->name);
            free(R[l]->monsters[m]->element);
            free(R[l]->monsters[m]);
        }
        free(R[l]->monsters);
        free(R[l]->name);
        free(R[l]);
    }
    free(R);
    
    for (int n = 0; n < mCount; n++) {
        free(M[n]->element);
        free(M[n]->name);
        free(M[n]);
    }
    free(M);
}
