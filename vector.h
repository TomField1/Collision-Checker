/* 
 * File:   vector.h
 * Author: Tom Field
 *
 * Created on 16 February 2016, 14:077
 * 
 * Thus header file externalises all the functions and structs needed for the 
 * vector.c class. For further information on these functions, see there.
 */

#ifndef VECTOR_H
#define	VECTOR_H

typedef struct vector{
    double x;
    double y;
    double z;
}vector;

vector* createVector(double a, double b, double c);
//printVector(vector* v);

double dotProduct(vector* a, vector* b);

vector* getLineNormal(vector* a, vector* b);
vector* rand2DVector();

#ifdef	__cplusplus
extern "C" {
#endif




#ifdef	__cplusplus
}
#endif

#endif	/* VECTOR_H */

