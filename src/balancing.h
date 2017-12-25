#ifndef _BALANCING_H
#define _BALANCING_H

#define MAX_BALANCING_SIZE 16

int init_balancing(size_t count);

size_t get_balanced_index(void);

int update_balancing_weights(size_t *weights, size_t length);

int get_balancing_weights(size_t **weights);

#endif
