/* 
 * File:   polygon.h
 * Author: tof1
 *
 * Created on 25 February 2016, 17:14
 * 
 * This Header file contains the information needed to externalise all functions
 * 
 */

#ifndef POLYGON_H
#define	POLYGON_H

#include "vector.h"

#ifdef	__cplusplus
extern "C" {
#endif
    
typedef struct transformation{
    double scale;
    double rotationZ;
    vector* translation;
}transformation;
    
typedef struct polygon{
    vector* vertices[20];
    vector* centre;
    transformation* transform;
}polygon;

polygon* buildPolygon(vector* v[20]);
int freePolygon(polygon* p);
transformation* buildTransformation(double scale, double rotationZ, vector* v);

vector* getCentre(polygon* p);
vector* getMultipleCentres(polygon* p[]);
int checkIfConvex(polygon* p);
int scalePolygon(polygon* p, double scale);
int scalePolygonTo(polygon* p, double scale);
int translatePolygon(polygon* p, vector* v);
int translatePolygonTo(polygon* p, vector* v);
int rotatePolygonZ(polygon* p, double angle);
int rotatePolygonZTo(polygon* p, double newAngle);




#ifdef	__cplusplus
}
#endif

#endif	/* POLYGON_H */

