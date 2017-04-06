/* 
 * File:   collision.h
 * Author: tof1
 *
 * This header file externalises the functions in the Collision.c file.
 * 
 * For further details on any function, check there.
 */

#ifndef COLLISION_H
#define	COLLISION_H

#ifdef	__cplusplus
extern "C" {
#endif

int checkCollisionOnAxis(polygon* a, polygon* b, vector* axis);
int checkCollisions(polygon* a, polygon* b);
int checkInsideBoundingBox(polygon* a, polygon* bound);
int checkMultipleInBound(polygon* interiors[], polygon* bound);



#ifdef	__cplusplus
}
#endif

#endif	/* COLLISION_H */

