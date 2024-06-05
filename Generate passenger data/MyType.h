#ifndef MYTYPE_H
#define MYTYPE_H

#include "time.h"
#include <assert.h> 
#ifndef INFINITY
#define INFINITY 999999
#endif
#define EPSILON 1.0E-10f
#define MAXINFO 500000

#define ALPHA 10
#define WEIGHT_RANGE 10000
#define WEIGHT_SETTING 1//zipf(ALPHA,WEIGHT_RANGE)
#define ROADNET_NAME "col.roadnet"
//MAXINFO��·�������ж���������

#define Min(a,b) ((a)<(b))?(a):(b)
#define Max(a,b) ((a)>(b))?(a):(b)

typedef double LENGTH;
typedef double WEIGHT;

enum
{
	TYPE_OF_OTHER = 0,
	TYPE_OF_CUSTOMER = 1,
	TYPE_OF_QUERY = 2
};

LENGTH MyRandom(LENGTH max);
//#define TEST_PROB
#endif
