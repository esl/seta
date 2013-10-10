/*
 *  seta.h
 *  stealing-work scheduler
 *
 *  Created by Fabio Pricoco on 03/10/2013.
 *  Copyright 2013 __MyCompanyName__. All rights reserved.
 *
 */

#include "scheduler.h"


cont seta_create_cont(void *);

handle_spawn_next seta_prepare_spawn_next(void *, void *, context_t *);

void seta_spawn_next(handle_spawn_next);

void seta_spawn(void *, void *, context_t *);

void seta_send_argument(cont, void *, int, context_t *);

void seta_start(void *);

void seta_connect_cont(handle_spawn_next *, cont *);

void seta_assign_cont(cont *, cont);

cont seta_copy_cont(cont);