/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   applications.h
 * Author: Tom
 *
 * Created on 13 April 2016, 13:05
 */

#ifndef APPLICATIONS_H
#define APPLICATIONS_H

#ifdef __cplusplus
extern "C" {
#endif
transformation* findMinScaleWithTranslation(polygon* polyInside, polygon* polyOutside, 
                                            double precision, int iterations);
transformation* findMinScaleWithRotation(polygon* polyInside, polygon* polyOutside, 
                                            double precision, vector* newCentre);
double findMinScale(polygon* polyInside, polygon* polyOutside, int precision);

#ifdef __cplusplus
}
#endif

#endif /* APPLICATIONS_H */

