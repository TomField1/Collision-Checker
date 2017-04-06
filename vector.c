#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vector.h"

////////////////////////////////////////////////////////////////
///////////// DOT PRODUCT //////////////////////////////////////
////////////////////////////////////////////////////////////////
/* Generate the dot product of two vectors.
 * 
 * The dot product of two vectors is Ax*Bx + Ay*By + Az*Bz to give a scalar.
 *
 */
double dotProduct(vector* a, vector* b){
    return (a->x * b->x) + (a->y * b->y) + (a->z * b->z);
}

/////////////////////////////////////////////////////////////////
////////// CREATE VECTORS ///////////////////////////////////////
/////////////////////////////////////////////////////////////////
/* This function creates a vector from three doubles: in order x, y, and z
 * 
 */
vector* createVector(double a, double b, double c){
    
    //allocate the vector size
    vector* newVector = malloc(sizeof(vector));
    
    //set the x, y and z values to a, b, and c respectively
    newVector->x = a;
    newVector->y = b;
    newVector->z = c;
    
    //return the vector
    return newVector;
}

////////////////////////////////////////////////////////////////////
///////////////// GET LINE NORMAL //////////////////////////////////
////////////////////////////////////////////////////////////////////
/* Get the normal of a line from one vertex to another.
 * 
 * This algorithm gets the LEFT normal: on the left-hand side when standing at a
 * looking at b. When the vector AB runs clockwise around
 * the object this is the outside. For collision calculations it doesn't matter 
 * which normal it is (since it generates an effectively infinite axis), but for 
 * other uses bear this in mind. 
 * 
 * NOTE THAT THIS DOES NOT CALCULATE THE NORMAL IN THE Z AXIS
 * 
 * N, the normal of a line, is Nx = -By-Ay, Ny = Bx-Ax, 
 */
vector* getLineNormal(vector* a, vector* b){

    //generate x y and z values from the two vectors as above
    double x = -(b->y - a->y);
    double y = b->x - a->x;
    double z = 0;
    
    //create a vector from them
    return createVector(x, y, z);
}

////////////////////////////////////////////////////////////////////////////////
////////////////Random 2D Vector ///////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/* This function returns a vector object with random values for the X and Y 
 * components between one and 0. These components can then be multiplied to create
 * a random vector with a given length.
 * 
 */
vector* rand2DVector(){
    double x = (double)rand()*2.0 / (double)RAND_MAX;
    double y = (double)rand()*2.0 / (double)RAND_MAX;
    
    vector* r = createVector(x-1,y-1,0.0);
    return r;
}