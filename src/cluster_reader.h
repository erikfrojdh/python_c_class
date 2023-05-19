#pragma once
#include <stdint.h>
#include <stdio.h>

//Pure C implementation to read a cluster file

int64_t read_clusters(FILE* fp, int64_t n_clusters, void* buf);