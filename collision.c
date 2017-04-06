/*
 * COLLISION.C
 * 
 * This file contains functions to run the code for objects colliding.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vector.h"
#include "polygon.h"
#include "collision.h"


/////////////////////////////////////////////////////////////////
/////////////////CHECK COLLISION ON AXIS/////////////////////////
/////////////////////////////////////////////////////////////////
/* This function takes two objects, and a single axis, and compares whether the
 * projection of the objects onto the axis collides.
 * 
 * If the projections collide, there is no gap between the objects on this axis.
 * In this case, the function returns 0.
 * 
 * If they do not, there is at least one gap between the objects, and thus they
 * can be proved not to collide. The function returns 1.
 * 
 */
int checkCollisionOnAxis(polygon* a, polygon* b, vector* axis){
    
    //create new arrays of vectors to operate on
    vector* vectorsA[20] = {NULL};
    vector* vectorsB[20] = {NULL};
    
    //for each vertex in the polygon
    int i = 0;
    for(i=0;a->vertices[i] != NULL;i++){
        
        //add a new vector, from 0.0 to this vertex, to the array
        vectorsA[i] = a->vertices[i];
    }
    vectorsA[i] = NULL;
    for(i=0;b->vertices[i] != NULL;i++){
        //repeat for the other object
        vectorsB[i] = b->vertices[i];   
    }
    vectorsB[i] = NULL;
    
    /* for the first polygon, set the minimum and maximum projection on the axis
    * to the first one as a default value 
    */
    
    //get the dot product of the vector with the axis
    double minProjectionA = dotProduct(vectorsA[0], axis);
    double maxProjectionA = dotProduct(vectorsA[0], axis);
    
    //store the vertex for the min and the max: set it to the first as a default
    int minVertexA = 0;
    int maxVertexA = 0;
    
    //for each vector in polygon a that isnt null
    int j =0;
    for (j = 0; vectorsA[j] != NULL; j++){
        
        //if the minimum projection on the axis is greater than the current
        //vector's projection on the axis, make it the new minimum
        if (minProjectionA > dotProduct(vectorsA[j], axis)){
            minProjectionA = dotProduct(vectorsA[j], axis);
            
            //set the minimum vertex index to the index of the current vertex
            minVertexA = j;
        }
        
        //then the same in inverse for the maximum 
        if (maxProjectionA < dotProduct(vectorsA[j], axis)){
            maxProjectionA = dotProduct(vectorsA[j], axis);
            maxVertexA = j;
        }
    }
        
    
    //then do the same for polygon b
    //get the dot product of the vector with the axis
    double minProjectionB = dotProduct(vectorsB[0], axis);
    double maxProjectionB = dotProduct(vectorsB[0], axis);
    //set the min and max dot to 1
    int minVertexB = 0;
    int maxVertexB = 0;
    
    //for each vector in polygon a
    j =0;
    for (j = 0; vectorsB[j] != NULL; j++){
        
        //if the minimum projection on the axis is greater than the
        //vector's projection on the axis, make it the new minimum
        if (minProjectionB > dotProduct(vectorsB[j], axis)){
            minProjectionB = dotProduct(vectorsB[j], axis);
            minVertexB = j;
        }
        
        //same in inverse for max
        if (maxProjectionB < dotProduct(vectorsB[j], axis)){
            maxProjectionB = dotProduct(vectorsB[j], axis);
            maxVertexB = j;
        }
    }
    
    //now compare along the axis for collision
    
    //if max of 2 is less than min of 1, 
    if(maxProjectionB < minProjectionA){
        //then a gap has been found
        //b is lower on the axis than a
        return 1;
    }
    //or max of 1 is less than min of 2
    else if(maxProjectionA < minProjectionB){
        //then a gap has been found
        //a is lower on the axis than b
        return -1;
    }
    else {
        //if not, the two projections overlap and no gap has been found
        return 0;
    }
}

/////////////////////////////////////////////////////////////////////
////////////////// CHECK COLLISIONS//////////////////////////////////
/////////////////////////////////////////////////////////////////////
/*This function checks the collisions between two objects. It does this by
 * generating an axis for comparison along each of the object's side's normals
 * and using the checkCollisionOnAxis function for each of these axes.
 * 
 * If any one axis has no collision, the objects do not collide and this function
 * returns 1. If all axes collide, the objects do collide and this function
 * returns 0.
 * 
 * RETURN 1: OBJECTS DO NOT COLLIDE
 * RETURN 0: OBJECTS COLLIDE
 */
int checkCollisions(polygon* a, polygon* b){
    
    //instantiate an integer for use as a counter
    int i = 1;
    
    //for each non-null vertex in the first polygon
    while(a->vertices[i] != NULL){
        
        //run checkCollisionOnAxis for polygon a, polygon b, and the normal for
        //the line between vertex i-1 and vertex i
        vector* n = getLineNormal(a->vertices[i-1], a->vertices[i]);
        if (checkCollisionOnAxis(a, b, n) != 0){
            //if it equals 1, return 1 (there is no collision), otherwise dont
            //return
            free(n);
            return 1;
        }else free(n);
        //increment i and repeat
        i++;
    }
    
    //once the collision has been checked for all lines between the first and
    //last vertices, check for the line between last and first, completing the
    //shape
    vector* v = getLineNormal(a->vertices[i-1], a->vertices[0]);
    if (checkCollisionOnAxis(a, b, v) != 0){
        free(v);
        return 1;
    }else free(v);

    //reset i to 1 and repeat the process for polygon b
    i = 1;
    while(b->vertices[i] != NULL){
        vector* v = getLineNormal(b->vertices[i-1], b->vertices[i]);
        if (checkCollisionOnAxis(a, b, v) != 0){
            free(v);
            return 1;
        }else free(v);
        i++;
    }
    vector* n = getLineNormal(b->vertices[i-1], b->vertices[0]);
    if (checkCollisionOnAxis(a, b, n) != 0){
        free(n);
        return 1;
    }else free (n);
    
    //if after all the above have run, and not returned, then return 0 - the two
    //objects collide on all normals, and so definitely collide
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
///////////////// CHECK INSIDE BOUNDING BOX ////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/*
 * This function takes two polygons and checks that one is inside the other. It 
 * does this by breaking the outer box into a series of single-side lines and
 * comparing the inner box to the line normal of these.
 */
int checkInsideBoundingBox(polygon* a, polygon* bound){
    //break the box down into it's component lines as seperate polygons
    polygon* boundPolygons[21];
    vector* boundVertices[3];
    int i;
    
    for(i = 0; bound->vertices[i+1] != NULL; i++){
        boundVertices[0] = bound->vertices[i];
        boundVertices[1] = bound->vertices[i+1];
        boundVertices[2] = NULL;
        
        boundPolygons[i] = buildPolygon(boundVertices);
    }
    boundVertices[0] = bound->vertices[i];
    boundVertices[1] = bound->vertices[0];
    boundVertices[2] = NULL;
    
    boundPolygons[i] = buildPolygon(boundVertices);
    boundPolygons[i+1] = NULL;
    
    //check collision for each line along it's normal
    for(i = 0; boundPolygons[i] != NULL; i++){
        vector* n = getLineNormal(boundPolygons[i]->vertices[0], 
                boundPolygons[i]->vertices[1]);
        int result = checkCollisionOnAxis(a, boundPolygons[i],n);
        free(n);
        
        if(result == 0 || result == 1){
            //if a collides with b a 0 is returned
                //this means the object collides with the edge of the bounding box
                //this means the object is not fully inside the box, so return 0

            //if a is higher up the axis than b, 1 is returned
                //this means a is outside the box on this axis
                //this means a is outside the box, so return 0
            
            //first free the function objects
            int k;
            for(k = 0; boundPolygons[k] != NULL; k++){
                free(boundPolygons[k]->transform->translation);
                free(boundPolygons[k]->centre);
                free(boundPolygons[k]);
            }
            //return
            
            return 0;
            
            
        }
        //if a is lower on the axis than b, -1 is returned
            //this means a is inside the box on this axis
            //continue 
    }
    //if the function has not returned, a is inside the box on all axes
    
    //free the function memory
    int k;
    for(k = 0; boundPolygons[k] != NULL; k++){
        free(boundPolygons[k]->transform->translation);
        free(boundPolygons[k]->centre);
        free(boundPolygons[k]);
    }

    //return 1
    
    return 1;
}

////////////////////////////////////////////////////////////////////////////////
/////////// CHECK MULTIPLE IN BOUND ////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/*
 * This function iterates through a list of polygons and checks that all of them
 * are inside a given boundary.
 */
int checkMultipleInBound(polygon* interiors[], polygon* bound){
    int i = 0;
    for(i = 0; interiors[i] != NULL; i++){
        if(checkInsideBoundingBox(interiors[i], bound) == 0){ //if one object is out
            return 0; //return 0: not all in bound
        }
    }
    return 1; //otherwise return 1: all are in
}