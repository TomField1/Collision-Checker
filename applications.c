////////////////////////////////////////////////////////////////////////////////
//
//                          APPLICATIONS
//
////////////////////////////////////////////////////////////////////////////////
/* This file contains the methods to test some applications for the rest of the
 * program, including finding the minimum size at which one object can be and
 * still contain another, the same if the objects are allowed to rotate, and the
 * same if one object can be moved within another.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "vector.h"
#include "polygon.h"
#include "collision.h"
#include "applications.h"

////////////////////////////////////////////////////////////////////////////////
/////////////// Find Min Scale With Rotation And Translation ///////////////////
////////////////////////////////////////////////////////////////////////////////
/*
 * This function calls "FindMinScaleWithRotation multiple times at different 
 * locations a number of times equal to "iterations."
 */
transformation* findMinScaleWithTranslation(polygon* polyInside, 
        polygon* polyOutside, double precision, int iterations){
    
    //save original scale, rotation, and position for both objects.
    transformation* polyInsideTransform = buildTransformation(polyInside->transform->scale,
            polyInside->transform->rotationZ, polyInside->centre);
    transformation* polyOutsideTransform = buildTransformation(polyOutside->transform->scale,
            polyOutside->transform->rotationZ, polyOutside->centre);
    
    //find the minimum scale when the two objects are centred on each other
    transformation* start = findMinScaleWithRotation(polyInside, polyOutside, 
                                        precision, polyOutside->centre);
    
    double minScale = start->scale;
    double angleAtMin = start->rotationZ;
    vector* translationAtMin = start->translation;
    
    //apply this translation, so we're at min scale at centred position to start
    scalePolygonTo(polyOutside, start->scale);
    rotatePolygonZTo(polyInside, start->rotationZ);
    translatePolygonTo(polyInside, start->translation);
    
    //for a large number of times
    int i;
    for(i=0;i<=iterations;i++){
        //choose a random direction, with a random magnitude
        vector* rand = rand2DVector();
        
        //and translate polyInside by a distance in it
        vector* newCentre = createVector(polyOutside->centre->x + rand->x*1600,
                                        polyOutside->centre->y + rand->y*1600,
                                        polyOutside->centre->z + rand->z*1600);
        
        translatePolygonTo(polyInside,newCentre);
        
        double j = 1600;
        //then reduce this direction by half each time until polyInside fits
        while(checkInsideBoundingBox(polyInside, polyOutside) != 1){
            j = j/2;
            
            //if J is very small, just return to the centre (avoids infinite loops)
            if (j <= 0.001){
                newCentre = createVector(polyOutside->centre->x,
                                polyOutside->centre->y,0.0);
                translatePolygonTo(polyInside, newCentre);
                break;
            }
            //otherwise find a new point closer to the centre
            newCentre = createVector(polyOutside->centre->x + rand->x*j,
                                polyOutside->centre->y + rand->y*j,
                                0.0);
            translatePolygonTo(polyInside,newCentre);
        }
        printf("|| Testing at %f, %f ", polyInside->centre->x,
                                polyInside->centre->y);      
        //then findMinScaleWithRotation at this point
        transformation* result = findMinScaleWithRotation(polyInside, polyOutside, 
                                        precision, newCentre);
        
        //if the scale is less, save the position and rotation
        if(result->scale < minScale){
            minScale = result->scale;
            angleAtMin = result->rotationZ;
            translationAtMin->x = polyInside->centre->x;
            translationAtMin->y = polyInside->centre->y;
        }    
        
        //free the created 
        free(rand);
        free(newCentre);
        free(result);
    } //keep doing this a few hundred to thousand times 
    
    //at the end, reset the objects to their original locations, scales, etc
    scalePolygonTo(polyOutside, polyOutsideTransform->scale);
    rotatePolygonZTo(polyInside, polyInsideTransform->rotationZ);
    translatePolygonTo(polyInside, polyInsideTransform->translation);
    
    //free(polyInsideTransform->translation);
    free(polyInsideTransform);
    free(polyOutsideTransform);
    free(start);
  
    //return the smallest scale and info
    transformation* returnTransform = buildTransformation(minScale,angleAtMin,
            translationAtMin);
    return returnTransform;
}

////////////////////////////////////////////////////////////////////////////////
//////////////// Find Min Scale With Rotation //////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/* This function uses the FindMinimumScale function to find the minimum scale
 * at which a polygon will fit inside another, and does the same for a variety
 * of angles in order to get the minimum scale possible when the angle can be
 * adjusted. It returns both polygons to their original scale and orientation.
 * 
 * NOTE: While this function returns a transformation, the scale and rotation of
 * the transformation are to be applied to different objects.
 */
transformation* findMinScaleWithRotation(polygon* polyInside, polygon* polyOutside, 
                                            double precision, vector* newCentre){
    
    //save the start position of the inside polygon
    vector* startVector = createVector(polyInside->centre->x,
                                        polyInside->centre->y,
                                        polyInside->centre->z);
    
    //move polyInside to the new position
    translatePolygonTo(polyInside, newCentre);
    
    double angleAtMin, scale;
    double minScale = 100;
    double i = 0.0;
    //for every ten degrees, get the minimum scale the outside polygon can be to
    //fit it inside
    for(i; i<=360.0; i=i+10.0){
        
        scale = findMinScale(polyInside, polyOutside, precision);
        
        //if its smaller than the current minimum, set it to the minimum
        if(scale < minScale){
            minScale = scale;
            angleAtMin = i;
        }
        
        //rotate the polygon to the next position
        rotatePolygonZ(polyInside, 10.0);
    }
    
    //rotate polyInside back to the minimum -10 to iterate through possible
    //minimums
    rotatePolygonZTo(polyInside, angleAtMin-10);
    
    //repeat for every 1 degree, working in a 10-degree band on either side of
    //the previous minimum
    for(i = angleAtMin - 10; i<=angleAtMin+10.0; i++){
        
        scale = findMinScale(polyInside, polyOutside, precision);
        
        //if its smaller than the current minimum, set it to the minimum
        if(scale < minScale){
            minScale = scale;
            angleAtMin = i;
        }
        
        //rotate the polygon to the next position
        rotatePolygonZ(polyInside, 1.0);
    }
    
    //rotate polyInside back to the minimum -1 to iterate through possible
    //minimums
    rotatePolygonZTo(polyInside, angleAtMin-1);
    
    //repeat for every 1/10th degree, working in a 1-degree band on either side of
    //the previous minimum
    for(i = angleAtMin - 1; i<=angleAtMin+1.0; i = i + 0.1){
        
        double scale = findMinScale(polyInside, polyOutside, precision);
        
        //if its smaller than the current minimum, set it to the minimum
        if(scale < minScale){
            minScale = scale;
            angleAtMin = i;
        }
        
        //rotate the polygon to the next position
        rotatePolygonZ(polyInside, 0.1);
    }
    
    //rotate and move the polygons back to the start
    rotatePolygonZTo(polyInside, 0);
    translatePolygonTo(polyInside, startVector);
    
    free(startVector);
    
    //return the transformation for the required changes
    return buildTransformation(minScale, angleAtMin, polyOutside->centre);
}

////////////////////////////////////////////////////////////////////////////////
//////////////// Find Minimum Scale/////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/* This function takes two convex shapes, and reports the smallest size at which
 * the outer one can be scaled to and still have the other fit completely into
 * it. It restores the original scale of the outer polygon.
 * 
 * At the end, this function restores the outside polygon to its original scale, 
 * and returns the minimum scale as a double.
 */
double findMinScale(polygon* polyInside, polygon* polyOutside, int precision){
    double startScale = polyOutside->transform->scale;
    
    //firstly, make the Outside polygon larger until the inside poly fits
    //inside it
    while(checkInsideBoundingBox(polyInside, polyOutside) != 1){
        scalePolygonTo(polyOutside, polyOutside->transform->scale+1);
    }
    
    //then make it smaller in until the inside polygon no longer fits
    //do this by subtracting multiples of ten of the original scale
    //so for a shape at 77.4% with precision 3, go 90%, 80%, 79%,78%,78.9% etc
    int i = 1;
    double scaleStart = polyOutside->transform->scale;
    double scaleDiff; double newScale;
    for(i; i <= precision; i++){
        //calculate what 1/10^i of the start scale is
        scaleDiff = scaleStart/pow(10.0,i);
        
        //scale down, test if still larger, if so continue
        do{
            //calculate what to scale down to 
            newScale = polyOutside->transform->scale - scaleDiff;
            //scale to it
            scalePolygonTo(polyOutside, newScale);
        //if still larger, repeat
        }while(checkInsideBoundingBox(polyInside, polyOutside) == 1);
        
        //the other box is smaller, so scale it back to the old scale
        scalePolygonTo(polyOutside, newScale+scaleDiff);
        
        //if not then break and repeat for a smaller interval.
    }
    
    //reset the scale of the outside object and return the smallest scale it
    //reached.
    double r = polyOutside->transform->scale;
    scalePolygonTo(polyOutside,startScale);
    return r;
}