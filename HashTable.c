#include "HashTable.h"

unsigned long ht_string2int(char *str) {
    unsigned long hash = 5381;
    int c;

    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c;
    }

    return hash;
}

unsigned int ht_hash(char *key, unsigned int slots) {
    unsigned long hash_val = ht_string2int(key);
    return (unsigned int)(hash_val % slots);
}
