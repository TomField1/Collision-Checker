/*
* MENU.C
 * 
 * This file acts as an interface for the program between the user's instructions
 * and the collision or utilisation code. 
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "menu.h"
#include "vector.h"
#include "polygon.h"
#include "collision.h"
#include "applications.h"

//externalise the polygons function
extern struct polygon* polygons[20];
/*===========================================================================
 *======================= PRINT OPENING =====================================
 * ==========================================================================
 * 
 * This method prints a nice little opener for the collision checker
 * program.
 * 
 */
int printOpening(){
    printf(" ===============================\n"
           " ====== Collision Checker ======\n"
           " ===============================\n"
           "\n"
           " Welcome to Collision Checker!\n");
    return(EXIT_SUCCESS);
}

/*===========================================================================
 *=========================== PRINT MENU ====================================
 * ==========================================================================
 * 
 * This method prints the menu for the program, and reads in the first character
 * of the user's response. It then returns this character for use in the switch
 * statement in the main.
 * 
 */
char runMenu(){
    
    //print the menu
    printf("\n MENU\n"
            "  Type a character for the following response:\n"
            "  C: Compare two objects           A: Compare all objects\n"
            "  B: Check object inside bound.    R: Fit object to bound.\n" 
            "  F: See input file format.        I: Input different file.\n"
            "  E: Export objects as image.      S: Shrink objects to fit.\n"
            "  X: Close.\n");
            
    //create a char for menu response
    char returnChar;
    
    //read the first char from the user's response
    scanf(" %c", &returnChar);
    
    //return that character
    return returnChar;
}

/*===========================================================================
 *=========================== COMPARE TWO OBJECTS ===========================
 * ==========================================================================
 * 
 * This method gathers the necessary information from the user to identify two
 * polygons, before comparing these polygons for collision and printing depending
 * on the output
 * 
 */
int compareTwoObjects(){
    //create chars for responses
    int num1 = 1, num2 = 1;
    
    //loop through the function until we get a correct response and can break
    for(;;){
        //read the first number from the user's response
        printf(" Input the number of the first polygon to compare.\n");
        if(scanf("%d", &num1) > 0){
            break;
        }
        else{
            printf(" Please input a number.\n");
        }
    }
    for(;;){
        //read the first number from the user's response
        printf(" Input the number of the second polygon to compare.\n");
        if(scanf("%d", &num2) > 0){
            break;
        }
        else{
            printf(" Please input a number.\n");
        }
    }
    
    //check collisions between the two given shapes
    if(checkCollisions(polygons[num1-1], polygons[num2-1]) == 1){
        //if the return value from the function is one, a gap has been found
        printf(" RESULT: Gap found. Objects %d and %d do not collide.\n", 
                num1, num2);
    }else{
        //otherwise a gap has not been found.
        printf(" RESULT: Gap not found. Objects %d and %d collide.\n", 
                num1, num2);
    }
    return(EXIT_SUCCESS);
}

/*===========================================================================
 *=========================== COMPARE ALL OBJECTS ====================================
 * ==========================================================================
 * 
 * This method iterates through all pairs of objects in the scene and checks
 * each for collision, printing out for each one whether they collide or not.
 * 
 */
int compareAllObjects(){
    printf(" Comparing all objects...");
    int i = 1, j = 0;

    //for each polygon (note that we skip i = 0, since there would be nothing to
    //compare it to.)
    for(i=1; polygons[i] != NULL; i++){
        
        //for each polygon with a lower number than it
        for(j=0; j < i; j++){
            
            //check collisions between the two given shapes
            if(checkCollisions(polygons[i], polygons[j]) == 1){
                //if the return value from the function is one, a gap has been found
                printf(" Objects %d and %d do not collide.\n", j+1, i+1);
            }else{
                //otherwise a gap has not been found.
                printf(" Objects %d and %d do collide.\n", j+1, i+1);
            }
        }
        
        //reset J to 0 to start the cycle again
        j=0;
    }
    return(EXIT_SUCCESS);
}

/*===========================================================================
 *=========================== PRINT FILE DETAILS ============================
 * ==========================================================================
 * 
 * This method prints the instructions on correctly writing the input file.
 * 
 */
int printFileDetails(){
    printf("\n FILE DETAILS\n"
            " To read the file correctly, it must abide by the following rules:\n"
            " > Each polygon must be on a new line.*\n"
            " > Each vector of each polygon must be separated by semicolons.\n"
            " > Each co-ordinate of each vertex must be separated by commas.\n\n"
            " *Note that windows Notepad has some strange habits regarding new "
            " lines.\n"
            " On windows, I would suggest using Notepad++\n\n"
            " For all functions to work properly, vertices should be in\n"
            " clockwise order around a polygon, and polygons should be convex.\n");
    return(EXIT_SUCCESS);
}

/*===========================================================================
 *=========================== COMPARE BOUNDING BOX ============================
 * ==========================================================================
 * 
 * This method allows the user to input the number of two polygons: this method
 * checks that one polygon is inside the other.
 * 
 */
int compareBoundingBox(){
    //create chars for responses
    int num1 = 1, num2 = 1;
    
    //loop through the function until we get a correct response and can break
    for(;;){
        //read the first number from the user's response
        printf(" Input the number of the INTERIOR polygon.\n");
        if(scanf("%d", &num1) > 0){
            break;
        }
        else{
            printf(" Please input a number.\n");
        }
    }
    for(;;){
        //read the first number from the user's response
        printf(" Input the number of the BOUNDING polygon to compare.\n");
        if(scanf("%d", &num2) > 0){
            break;
        }
        else{
            printf(" Please input a number.\n");
        }
    }
    
    //check if polygon num1 is inside polygon num2
    if(checkInsideBoundingBox(polygons[num1-1], polygons[num2-1]) == 1){
        //if the return value from the function is one, it is inside
        printf(" RESULT: Object %d is fully inside object %d.\n", 
                num1, num2);
    }else{
        //otherwise it is at least partially outside.
        printf(" RESULT: Object %d is not fully inside object %d.\n", 
                num1, num2);
    }
    return(EXIT_SUCCESS);
}

/*===========================================================================
 *=========================== EXPORT TO HTML ============================
 * ==========================================================================
 * 
 * This method gets the information needed from the user to export the current
 * state of the polygons as an SVG image file, wrapped in an HTML shell so it
 * should be viewable in any browser.
 * 
 */
int exportToHTML(){
    //print the instructions
    printf(" This function exports the list of polygons into the .SVG image \n"
            " format, wrapped in an html shell for easy viewing in a modern \n"
            " web browser.\n"
            " Please type the filename for the export file, ending .html\n");
    
    //get a filename from the user and use it to open a new file
    char filename[100];
    scanf("%s", filename);
    FILE* exportFile = fopen(filename, "w");
    
    //initialise a list of colors for the polygons, as strings
    char *colors[] = {
        "Blue","Red","Black",
        "Green","Fuchsia","Purple",
        "Navy","Gray","Maroon","Olive",
        "Blue","Red","Black",
        "Green","Fuchsia","Purple",
        "Navy","Gray","Maroon","Olive"
    };
    
    int height = 0;
    int width = 0;
    
    //loop through the function until we get a correct response and can break
    for(;;){
        //read the first number from the user's response
        printf(" Please input the canvas height in pixels:\n");
        if(scanf("%d", &height) > 0){
            break;
        }
        else{
            printf(" Please input a number.\n");
        }
    }
    for(;;){
        //read the first number from the user's response
        printf(" Please input the canvas width in pixels:\n");
        if(scanf("%d", &width) > 0){
            break;
        }
        else{
            printf(" Please input a number.\n");
        }
    }
    
    //print the opening of the file to the file, including the given dimensions
    fprintf(exportFile, "<!DOCTYPE html>\n"
                        "<html>\n"
                        "<body>\n"
                        "\n"
                        "<svg height=\"%d\" width=\"%d\">\n", height, width);


    
    //print each polyline
    int polylines;
    for(polylines = 0; polygons[polylines] != NULL; polylines++){
        //open the polyline
        fprintf(exportFile, "    <polyline points=\"");
        
        //print each vector of the polyline
        int v;
        for(v = 0; polygons[polylines]->vertices[v] != NULL; v++){
            fprintf(exportFile, "%f,%f ", polygons[polylines]->vertices[v]->x,
                                          polygons[polylines]->vertices[v]->y);
        }
        
        //print the first vector again to close the polyline
        fprintf(exportFile, "%f,%f ", polygons[polylines]->vertices[0]->x,
                                      polygons[polylines]->vertices[0]->y);
        
        //close the polyline with some style info, including a picked color
        fprintf(exportFile, "\"\n");
        fprintf(exportFile, "    style=\"fill:none;stroke: %s ;stroke-width:2\" />\n", 
                                                            colors[polylines]);
    }
    
    //finish the file, including a note for if SVG is not supported
    fprintf(exportFile, "Your browser does not support inline SVG.\n"
                        "</svg>\n"
                        "<div>Generated by Collision Checker\n"
                        "</body>\n"
                        "</html>");

    printf("Exported to %s\n", filename);  
    fclose(exportFile);
    return(EXIT_SUCCESS);
}
/*===========================================================================
 *=========================== FIT OBJECT ============================
 * ==========================================================================
 * 
 * This method gets the information needed from the user to export the current
 * state of the polygons as an SVG image file, wrapped in an HTML shell so it
 * should be viewable in any browser.
 * 
 */
int fitObject(){
    printf(" This function takes two polygons and attempts to fit one as closely\n"
            " around the other as possible by shrinking (and rotating) it to fit\n"
            " without changing it's shape or proportion. Answer is returned as\n"
            " a multiple of the original size.\n");
    //create chars for responses
    int num1 = 1, num2 = 1, p = 0, iterations = 0;
    
    //for each of polygon numbers, precision, and iterations,
    //loop through the function until we get a correct response and can break
    for(;;){
        //read the first number from the user's response
        printf(" Input the number of the interior polygon.\n");
        if(scanf("%d", &num1) > 0){
            break;
        }
        else{
            printf(" Please input a number.\n");
        }
    }
    for(;;){
        //read the first number from the user's response
        printf(" Input the number of the outside polygon to adjust to fit.\n");
        if(scanf("%d", &num2) > 0){
            break;
        }
        else{
            printf(" Please input a number.\n");
        }
    }
    printf(" The precision of the scaling is indicated by a integer from 1 to\n"
            " 8. This indicates the number of decimal points to which the scale\n"
            " should be returned.");
    for(;;){
        //read the first number from the user's response
        printf(" Please input the precision of the scale.\n");
        if(scanf("%d", &p) > 0 && p > 0 && p < 9){
            break;
        }
        else{
            printf(" Please input a number between 1 and 8.\n");
        }
    }
    printf(" The software runs a number of times at different positions. Please\n"
            " input the number of iterations you want (Suggested: 500-3000)\n");
    for(;;){
        //read the first number from the user's response
        printf(" Input the number of iterations.\n");
        if(scanf("%d", &iterations) > 0){
            break;
        }
        else{
            printf(" Please input a valid number.\n");
        }
    }
    
    transformation* t = findMinScaleWithTranslation(polygons[num1-1], 
                            polygons[num2-1], p, iterations);
    
    printf("\n The minimum scale that object %d can be to still contain object %d\n"
            " is %f times it's original scale. This occurs  at %f degrees of \n"
            " rotation from its original position, at centre %f %f.\n", num2, num1, 
            t->scale, t->rotationZ, t->translation->x, t->translation->y);
    
    printf(" \n Input S to save the polygons' adjusted locations and scales\n"
            " or anything else to leave them in their previous position.\n");
    
    char c;
    scanf(" %c", &c);
    if(c == 's' || c == 'S'){
        //if the user wants the objects saved, move them to their translated
        //positions
        scalePolygonTo(polygons[num2-1], t->scale);
        rotatePolygonZTo(polygons[num1-1], t->rotationZ);
        translatePolygonTo(polygons[num1-1], t->translation);
        printf(" Saving polygons.\n");
    }
    else{
        //otherwise leave them where they were before running the test.
        printf(" Returning polygons to original positions.\n");
    }
    return(EXIT_SUCCESS);
}

/// debug
//by uncommenting the section in the *run Menu* function, this can be used to
//run code directly.
int debug(){
   return(EXIT_SUCCESS);
}
