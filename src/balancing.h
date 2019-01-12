#ifndef _BALANCING_H
#define _BALANCING_H

#include "jconf.h"

#define MAX_BALANCING_SIZE MAX_REMOTE_NUM

int init_balancing(size_t count);

size_t get_balanced_index(void);

int update_balancing_weights(size_t *weights, size_t length);

int get_balancing_weights(size_t **weights);

#endif
