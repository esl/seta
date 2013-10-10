/*
 *  closure.h
 *  stealing-work scheduler
 *
 *  Created by Fabio Pricoco on 10/09/2013.
 *  Copyright 2013 __MyCompanyName__. All rights reserved.
 *
 */

typedef struct _closure {
	int id;
	int processor;
	int level;
	int join_counter;
	list *args;	
} closure;