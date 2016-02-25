#ifndef STRUCT_H
#define STRUCT_H
#include "process.h"
//structures

typedef struct message
{	
	int n_id;//requesting node id.. needed when we process all the rqs_list at once
	process prcs;
	float q_time;	
	//paramter t  guaranteee...
} message;


typedef struct bidNode
{
	process prcs;
	float q_time;	
} bidNode;

#endif /* STRUCT_H */
