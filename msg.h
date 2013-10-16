/*
 *  utils.h
 *  seta
 *
 *  Created by Fabio Pricoco on 11/10/2013.
 *  Copyright 2013 Erlang Solutions Ltd. All rights reserved.
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

typedef char *msg_t;

msg_t msg_new();

void msg_destroy(msg_t);

void msg_cat(msg_t *, char *);

void msg_ncat(msg_t *, char *, int);

void msg_catf(msg_t *, const char *, ...);
