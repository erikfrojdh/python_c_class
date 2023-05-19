#include "cluster_reader.h"
#include "data_types.h"


int64_t read_clusters(FILE* fp, int64_t n_clusters, void* buf){
    printf("Item size: %d n_clusters: %d\n", sizeof(Cluster), n_clusters);
    size_t n_read = fread(buf, sizeof(Cluster), n_clusters, fp);
    printf("Read: %d items\n", n_read);
    return n_read;
}