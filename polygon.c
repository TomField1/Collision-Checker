#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "vector.h"

////////////////////////////////////////////////////////////////////
//////////////////// TRANSFORMATION TYPE DEFINITION ////////////////
////////////////////////////////////////////////////////////////////
/* This defines a struct for the Transformation object, allowing the scale and
 * rotation of a polygon to be defined and maniplated together.
 * 
 * When expanding to 3D, a rotationX and rotationY could be added
 */
typedef struct transformation{
    double scale;
    double rotationZ;
    vector* translation;
}transformation;

/////////////////////////////////////////////////////////////////////////
//////////////// BUILD POLYGON //////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
/* This function creates a new polygon struct and fills the list of vertices
 * before taking the list and generating a centre from it. It also sets the
 * default scale and rotation.
 * 
 */
transformation* buildTransformation(double newScale, double rotateZ, vector* v){
    //allocate space for the transformation
    transformation* newTransform = malloc(sizeof(transformation));
    
    //set the transformation's settings
    newTransform->rotationZ = rotateZ;
    newTransform->scale = newScale;
    newTransform->translation = v;

    //return the new Transformation
    return newTransform;
}

/////////////////////////////////////////////////////////////////
////////////////////POLYGON TYPE DEFINITION /////////////////////
/////////////////////////////////////////////////////////////////
/* This defines a struct for the polygon object. A polygon is defined by
 * a list of up to twenty vertices - sets of cartesian co-ordinates - and the
 * centre, which does not define any aspect of the polygon but is stored so as
 * to simplify the multiple calls for the centre.
 */
typedef struct polygon{
    vector* vertices[20];
    vector* centre;
    transformation* transform;
}polygon;



/////////////////////////////////////////////////////////////////
/////////// GET THE CENTRE OF A POLYGON /////////////////////////
/////////////////////////////////////////////////////////////////
/* This function takes the list of vertices for a polygon and finds it's centre
 * by finding the average of the X, Y, and Z values for each vertex.
 * 
 * This function is run during the creation of a polygon to store the position
 * of it's centre, which will later be referenced extensively.
 */
vector* getCentre(polygon* p){
    //instantiate at 0 the needed values
    int i = 0;
    double x=0, y=0, z=0;

    // for each non-null vertex in the polygon
    while(p->vertices[i] != NULL){
        
        // add the values of each axis position, to get the total.
        x = x + p->vertices[i]->x;
        y = y + p->vertices[i]->y;
        z = z + p->vertices[i]->z;
        
        //increment i to move on to the next vertex
        i++;
    }
    
    //when you reach a null vertex, i is the number of vertices in the polygon
    //divide each of the totals by i to get the average values for each
    x = x/i;
    y = y/i;
    z = z/i;

    //create a vector from the averages and return it
    vector* v = createVector(x, y, z);
    return v;
}

/////////////////////////////////////////////////////////////////////////
//////////////// BUILD POLYGON //////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
/* This function creates a new polygon struct and fills the list of vertices
 * before taking the list and generating a centre from it. It also sets the
 * default scale and rotation.
 * 
 */
polygon* buildPolygon(vector* v[20]){
    
    //allocate space for the polygon
    polygon* newPoly = malloc(sizeof(polygon));

    //for each vertex in the list
    int i = 0;
    for(i=0; v[i] != NULL; i++){
        //if it is not null, copy it into the polygon
        newPoly->vertices[i] = v[i];
    }
    newPoly->vertices[i] = NULL;
    //get centre of the polygon and save it
    newPoly->centre = getCentre(newPoly);

    //set the default scale and rotation of the new polygon
    newPoly->transform = buildTransformation(1.0, 0.0, createVector(0.0,0.0,0.0));
        
    //return the new polygon
    return newPoly;
}

////////////////////////////////////////////////////////////////////////////////
//////// FREE POLYGON //////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/*
 * This function frees the polygon at a particular point, and frees all elements
 * of its Transformation.
 */
int freePolygon(polygon* p){
    int i;
    for(i = 0; p->vertices[i] != NULL; i++){
        free(p->vertices[i]);
    }
    free(p->transform->translation);
    free(p->centre);
    free(p);
    
    return(EXIT_SUCCESS);
}

/////////////////////////////////////////////////////////////////////////
//////////////// CHECK IF CONVEX ////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
/* This function iterates through a polygon and checks if it is convex or
 * concave. If the polygon is concave collision detection and other functions
 * will not function properly.
 * 
 * The function checks concavity by going around the shape and, for each
 * set of three vertices, checking that the middle one is not on the "inside"
 * of the other two.It does this by taking the dot product of the normal.
 * 
 */
int checkIfConvex(polygon* p){
    int i;
    double projection;
    vector* normal;
    vector* aBVector;
    
    //for each vertex on the polygon a that is followed by b and c
    for(i = 0; p->vertices[i+2]!= NULL; i++){
        //get the normal of the vector from a-c
        normal = getLineNormal(p->vertices[i], p->vertices[i+2]);
        
        //get the vector from a to b
        aBVector = createVector(p->vertices[i+1]->x - p->vertices[i]->x,
                                p->vertices[i+1]->y - p->vertices[i]->y,
                                p->vertices[i+1]->z - p->vertices[i]->z);
        
        //get the projection of the vector from a-b onto the normal of a-c
        projection = dotProduct(aBVector, normal);
        
        if(projection <= 0){
            
            //if projection is negative, polygon is concave at b
            //if polygon is concave at any point, polygon is concave
            return 0;
        }
        //if projection is positive, polygon is convex at b
        
    }
    //perform the same checks for those including the final-to-first vector
    normal = getLineNormal(p->vertices[i], p->vertices[0]);
    aBVector = createVector(p->vertices[i+1]->x - p->vertices[i]->x,
                                p->vertices[i+1]->y - p->vertices[i]->y,
                                p->vertices[i+1]->z - p->vertices[i]->z);
    projection = dotProduct(aBVector, normal);
    if(projection <= 0){
        return 0;
    }
    
    normal = getLineNormal(p->vertices[i+1], p->vertices[1]);
    aBVector = createVector(p->vertices[0]->x - p->vertices[i+1]->x,
                                p->vertices[0]->y - p->vertices[i+1]->y,
                                p->vertices[0]->z - p->vertices[i+1]->z);
    projection = dotProduct(aBVector, normal);
    if(projection <= 0){
        return 0;
    }
    
    //if polygon is convex at all points, polygon is convex.
    return 1;   
}

/////////////////////////////////////////////////////////////////////////
//////////////// SCALE POLYGON ////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
/* This function scales a polygon up or down by the given value. (Scale 2
 * doubles the size, Scale 0.25 quarters it, etc etc.) The polygon keeps its
 * original centre.
 * 
 */
int scalePolygon(polygon* p, double scale){
    int i;
    
    //for each vertex in the polygon
    for(i=0;p->vertices[i]!=NULL;i++){
        
        //get the distance in each axis from the centre to that point
        double newX = p->vertices[i]->x - p->centre->x;
        double newY = p->vertices[i]->y - p->centre->y;
        double newZ = p->vertices[i]->z - p->centre->z;
        
        //scale up that distance
        newX = newX*scale;
        newY = newY*scale;
        newZ = newZ*scale;
        
        //add the new distance to the centre
        p->vertices[i]->x = p->centre->x + newX;
        p->vertices[i]->y = p->centre->y + newY;
        p->vertices[i]->z = p->centre->z + newZ;
    }
    //change the polygon's scale to represent the new value
    p->transform->scale = p->transform->scale*scale;
    
    return(EXIT_SUCCESS);
}

/////////////////////////////////////////////////////////////////////////
//////////////// SCALE POLYGON TO////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
/* This function scales a polygon up or down TO a given value - it converts that
 * value to a proportion of the old scale and then runs scalePolygon for it
 * 
 */
int scalePolygonTo(polygon* p, double scale){
    double s = scale/p->transform->scale;
    scalePolygon(p, s);
    
    return(EXIT_SUCCESS);
}
/////////////////////////////////////////////////////////////////////////
//////////////// TRANSLATE POLYGON ////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
/* This function translates a polygon by the given vector. 
 */
int translatePolygon(polygon* p, vector*v){
    int i;
    
    //for each vertex in the polygon
    for(i=0;p->vertices[i]!=NULL;i++){
        //translate the vertex
        p->vertices[i]->x = p->vertices[i]->x + v->x;
        p->vertices[i]->y = p->vertices[i]->y + v->y;
        p->vertices[i]->z = p->vertices[i]->z + v->z;
    }
    
    //translate the centre
    p->centre->x = p->centre->x + v->x;
    p->centre->y = p->centre->y + v->y;
    p->centre->z = p->centre->z + v->z;
    
    return(EXIT_SUCCESS);
}
/////////////////////////////////////////////////////////////////////////
//////////////// TRANSLATE POLYGON TO////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
/* This function translates a polygon so that it's centre is at the given vector. 
 */
int translatePolygonTo(polygon* p, vector* v){
    
    double dx = v->x - p->centre->x;
    double dy = v->y - p->centre->y;
    double dz = v->z - p->centre->z;
    
    //translate the centre
    p->centre->x = v->x;
    p->centre->y = v->y;
    p->centre->z = v->z;
    
    int i;
    
    //for each vertex in the polygon
    for(i=0;p->vertices[i]!=NULL;i++){
        //translate the vertex
        p->vertices[i]->x = p->vertices[i]->x + dx;
        p->vertices[i]->y = p->vertices[i]->y + dy;
        p->vertices[i]->z = p->vertices[i]->z + dz;
    }
    
    return(EXIT_SUCCESS);
}

/////////////////////////////////////////////////////////////////////////
//////////////// Rotate Polygon in Z ////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
/* This function rotates a polygon by a given number of degrees around the Z
 * axis, intersecting the polygon at its centre. The centre of the polygon 
 * remains the same, as does the size. The polygon rotates clockwise as seen 
 * from the positive direction.
 * 
 * In a 2D system, only Z-axis rotation is needed: it would be easy to add
 * "rotatePolygonX" or "rotatePolygonY" axes on the same pattern
 */
int rotatePolygonZ(polygon* p, double angle){
    int i;
    double angleRad = angle*M_PI/180;
    
    //for each vertex in the polygon
    for(i=0;p->vertices[i]!=NULL;i++){
        double x = p->vertices[i]->x - p->centre->x;
        double y = p->vertices[i]->y - p->centre->y;
        
        //adjust each of the vertex's co-ordinates by the rotation operator
        double xMod = x*cos(angleRad) - y*sin(angleRad);
        double yMod = x*sin(angleRad) + y*cos(angleRad);
        
        //copy the new value back in
        p->vertices[i]->x = p->centre->x + xMod;
        p->vertices[i]->y = p->centre->y + yMod;
    }
    
    //increment the rotation value
    p->transform->rotationZ = p->transform->rotationZ+angle;
    
    return(EXIT_SUCCESS);
}
/////////////////////////////////////////////////////////////////////////
//////////////// Rotate Polygon in Z TO ////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
/* This function rotates a polygon to a given number of degrees around the Z
 * axis, intersecting the polygon at its centre. In this case, 0 is the polygons
 * original orientation.
 * 
 * In a 2D system, only Z-axis rotation is needed: it would be easy to add
 * "rotatePolygonX" or "rotatePolygonY" axes on the same pattern
 */
int rotatePolygonZTo(polygon* p, double newAngle){
    //get the difference between the current angle and the new
    double angle = newAngle - p->transform->rotationZ;
    //rotate by that difference
    rotatePolygonZ(p, angle);
    
    return(EXIT_SUCCESS);
}

