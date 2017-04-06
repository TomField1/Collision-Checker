/* 
 * MAIN.C
 * 
 * This file contains the "Main" function for the application - the repeating
 * menu, as well as the file input functions.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "vector.h"
#include "polygon.h"
#include "collision.h"
#include "menu.h"

// Declare needed arrays and functions
char* polygonsPreConversion[20][20];
struct polygon* polygons[20];

int openFile(int firstRun);
int readFile(FILE* objectFile);
int vectorsListToVectorsObject();
int checkAllConvex();



/*====================================================
 * =============   MAIN ==============================
 * 
 * This method runs the main project through: it reads in the file and converts
 * it to an array of polygon objects. It then prints the menu and analyses the
 * player's response to this, and runs the relevant function.
 */
int main(int argc, char** argv) {
    //get the time for random seeds
    srand(time(NULL));
    
    //print the opening splash
    printOpening();
    
    //read in a file
    openFile(1);
    
    //check all shapes are convex
    checkAllConvex();
    
    //run the menu and get the character it returns
    char caseChar;
    caseChar = runMenu();
    
    //while this is not the exit instruction
    while (caseChar != 'x' && caseChar != 'X'){
        switch (caseChar){            
            case 'c' :
            case 'C' :
                //compare two objects
                compareTwoObjects();
                break;

            case 'a' :
            case 'A' :
                //compare all objects
                compareAllObjects();
                break;

            case 'b' :
            case 'B' :
                //check if an object is in a bounding box
                compareBoundingBox();
                break;

            case 'f' :
            case 'F' :
                //print the file details
                printFileDetails();
                break;
            
            case 'i' :
            case 'I' :
                //input a different file
                openFile(0);
                break;
                
            case 'e':
            case 'E':
                //export the file to html
                exportToHTML();
                break;
            
            case 's':
            case 'S':
                //Fit one object to another
                fitObject();
                break;
            
            //case 'd':
            //case 'D':
            //    debug();
            //    break;
                
            default :
                //print a warning and run the menu again
                printf(" Incorrect input. Please choose a correct option from above.\n");
        }
        
        caseChar = runMenu();
    }
    
    //exit the program
    return (EXIT_SUCCESS);
}

////////////////////////////////////////////////////////////////////////////////
//////////////////// OPEN FILE /////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/* This function gets the details needed for the input file, opens it, and
 * passes it to the readFile function. */
int openFile(int firstRun){
    
    //set up the needed file details
    char objectFilename[50];
    FILE *objectFile;
    
    //until a file is successfully loaded
    for(;;){

        //print an instruction to the user on opening the file
        printf(" Please input the filename of the object list.\n");

        if(firstRun == 0){
            //clear the polygonsPreConversion and polygons array by setting each to NULL
            int a, b;
            for(a = 0; a<20; a++){
                for(b = 0; b<20; b++){
                    polygonsPreConversion[a][b] = NULL;
                }
                polygons[a] = NULL;
            }
        } else if (firstRun == 1){
            printf(" Or press F to see the file details.\n");
        }

        //read the filename string from the user's response
        scanf("%s", objectFilename);
        
        //if the filename is equal to either F or f, print the file details and
        //repeat
        char* f1 = "F";
        char* f2 = "f";
        if(strcmp(objectFilename, f1) == 0 || strcmp(objectFilename, f2) == 0){
            printFileDetails();
            continue;
        }

        //create the file as a FILE object and open it
        objectFile = fopen (&objectFilename, "rt");
        
        //if the file is valid, break out of the loop.
        if(objectFile != NULL){
            printf(" Opening %s\n", objectFilename);
            break;
        }
    
        //if the program reaches here print a warning and return to the top
        printf(" Could not find file. Check filename. \n");
    }
        
    //continue on to reading the file
    readFile(objectFile);
    fclose(objectFile);
    
    return(EXIT_SUCCESS);
}
/* ======================================================================
 * =========================== READ FILE ================================
 * ======================================================================
 * 
 * This function reads in a correctly-formatted file into the 
 * "PolygonsPreConversion" array. In this 2D array, the first "Dimension" acts
 * as a list of the polygons, while the second acts as a list of the vertices of
 * each polygon in string format.
 * 
 * This function also prints out instructions to the user.
 * 
 */
int readFile(FILE* objectFile){
    
    //create the integers and the strings we'll be using to read the file
    int i= 0;
    int j= 0;
    
    //this is an array of strings, each 600char
    char currentObjectString[20][600]; 
    
    char* currentVectorString;
   
    /*This helps display the read-in objects more efficiently*/
    printf(" Reading:\n"); 
    
    /* for each line
     * -- the line is a polygon, the first [] in PolygonsPreConversion
     * -- read the vectors into a list
     * fgets gets the object string from the file
     * 
     * the while function runs if this does not return NULL
     * 
     * (I.E if the string is not empty)
     */
    while(fgets(currentObjectString[i], 256, objectFile) != NULL){
            
        //set J to 0
        j = 0;

        //read the first vector in by taking the token of the first object up to ;
        currentVectorString = strtok(currentObjectString[i], ";\0");

        //print which polygon is being read
        printf(" Reading polygon %d: \n", i+1);

        /*while the first point on the string currently being read is not 
         * null
         */
        while(currentVectorString !=NULL && currentVectorString[1] != NULL){

            //set the string for the vertex for polygon i to the read string
            polygonsPreConversion[i][j] = currentVectorString;

            //replace the read string with another, the next vertex
            currentVectorString = strtok(NULL, ";\0");

            //increment j, moving on to the next vertex of the same polygon
            j++;
        }

        /* when the end of the line is reached, the first char in
         * currentVectorString will be null and the while loop will break
         * When this happens, increment i, moving on to the next line and
         * next polygon
         */
        i++;
    }
    
    printf(" Reading complete. Analysing.\n");
    
    //Convert the vectors list to the objects
    vectorsListToVectorsObject();
    
    return(EXIT_SUCCESS);
}

/*===========================================================================
 *=================== VECTORS LIST TO VECTORS OBJECTS =======================
 * ==========================================================================
 * 
 * This method converts the PolygonsPreConversion array into the Polygons array.
 * 
 * It also converts the strings containing vector data into Vector objects, and
 * as part of setting up the Polygon objects gets the object centres.
 * 
 */
int vectorsListToVectorsObject(){
    
    //set up the integers and strings we will need.
    int i = 0;
    int j = 0;
    char *xString, *yString, *zString;
    float x, y, z;
    
    //while the first part of the current object is not null
    while(polygonsPreConversion[i][0] != NULL){
        
        //set/reset j to 0
        j = 0;
        
        /* Create PolygonVectors, an array of pointers to vector objects, and set
         * them to 0
         */
        vector* polygonVectors[20] = {NULL};
        
        //while the current point on the current part of the array is not null
        while(polygonsPreConversion[i][j] != NULL){
            
            //break the string down into individual strings for X Y and Z
            xString = strtok(polygonsPreConversion[i][j], ",");
            yString = strtok(NULL, ",");
            zString = strtok(NULL, ";");
            
            //check that none are null
            if(xString == NULL || yString == NULL || zString == NULL){
                printf(" ERROR: Could not convert polygon %d to a number.\n"
                        " Please read file instructions and try to input the file"
                        " again.\n", i+1);
                return -1 ;
            }
            
            //convert those strings to doubles. if any cannot be converted,
            //break the loop and return an error.
            if( sscanf(xString, "%f", &x) == -1 ||
                sscanf(yString, "%f", &y) == -1 ||
                sscanf(zString, "%f", &z) == -1){
                
                printf(" Could not convert polygon %d vector %d to a number.\n"
                        " Please read file instructions and try to input the file"
                        " again.\n", i+1, j+1);
                return -1 ;
            }
            
            //use those doubles to create a vector
            polygonVectors[j] = createVector(x,y,z);
        
            //increment j, moving on to the next vertex
            j++;
        }
        polygonVectors[j] = NULL;
        
        /*When the while loop breaks, now we have a full list of vectors for the
         * polygon. Use this to make a polygon and save it.
         */
        polygons[i] = buildPolygon(polygonVectors);
        
        //increment i, move on to the next polygon.
        i++;
    }
    
    return(EXIT_SUCCESS);
}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////// CHECK ALL CONVEX ////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/* This function checks that all polygons in the list are convex.*/
int checkAllConvex(){
    int i;
    int any;
    
    //for each polygon, check if it is convex
    for(i = 0; polygons[i] != NULL; i++){
        if(checkIfConvex(polygons[i]) == 0){
            //if any are not, print a warning for each
            printf(" WARNING: Polygon %d is concave!\n", i+1);
            any = 1;
        }
    }
    //if any polygon has been found convex, print a explanation once
    if(any == 1){
        printf(" Functions using concave polygons may not work correctly.\n");
    }
    //otherwise print that all are fine.
    else{
        printf(" All polygons convex.\n");
    }
    
    return(EXIT_SUCCESS);
}
