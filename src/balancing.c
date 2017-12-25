#include <stdlib.h>

#include "balancing.h"

#define BALANCING_MAP_SIZE 4096
#define MAX_WEIGHT 255

size_t converted_weights[MAX_BALANCING_SIZE];
unsigned char balancing_map[BALANCING_MAP_SIZE];
int balancing_size=0;

int init_balancing(size_t count)
{
	if(count == 0 || count > MAX_BALANCING_SIZE){
		return -1;
	}

	balancing_size = count;

	for(int i=0; i<BALANCING_MAP_SIZE; i++){
		balancing_map[i] = i % balancing_size;
	}
	for(int i=0; i<balancing_size; i++){
		converted_weights[i] = 1;
	}

	return 0;
}

size_t get_balanced_index(void)
{
	return balancing_map[rand() % BALANCING_MAP_SIZE];
}

int update_balancing_weights(size_t *weights, size_t length)
{
	size_t total_weight = 0;

	if(length == 0 || length > balancing_size){
		return -1;
	}

	for(int i = 0; i < balancing_size; i++){
		if(i < length){
			converted_weights[i] = (weights[i]>MAX_WEIGHT)?MAX_WEIGHT:weights[i];
		}else{
			converted_weights[i] = 0;
		}
		total_weight += converted_weights[i];
	}

	for(int x = 0; x < BALANCING_MAP_SIZE; x++){
		int n = x % total_weight;
		int i=0, t=0;
		balancing_map[x] = 0;
		for(; i<balancing_size; i++){
			t += converted_weights[i];
			if(t>n){
				balancing_map[x] = i;
				break;
			}
		}
	}

	return 0;
}

int get_balancing_weights(size_t **weights)
{
	*weights = converted_weights;
	return balancing_size;
}
