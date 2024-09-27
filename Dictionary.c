#include "List.h"
#include "HashTable.h"
#include "Dictionary.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>


typedef struct Dictionary {
    int slots;
    int size;
    ListPtr *hash_table;
} Dictionary;

typedef struct AuthorMax {
    char * author;
    int count;
} AuthorMax;

// Helper function to find a node with a specific key in a list and return its index.
// Returns -1 if the key is not found.
static int find_key_index(ListPtr L, char *key) {
    for (int i = 0; i < lengthList(L); i++) {
        KVPair *pair = (KVPair *)getList(L, i);
        if (strcmp(pair->key, key) == 0) {
            return i;
        }
    }
    return -1;
}

Dictionary *dictionary_create(int hash_table_size, void (*dataPrinter)(void *data)) {
    Dictionary *D = (Dictionary *)malloc(sizeof(Dictionary)); //create a dictionary, it holds the slotes avaliable, current size and a pointer to 
    if (!D) {
        fprintf(stderr, "Error: Memory allocation failed for Dictionary\n");
        exit(EXIT_FAILURE);
    }

    D->slots = hash_table_size;
    D->size = 0;
    D->hash_table = (ListPtr *)malloc(hash_table_size * sizeof(ListPtr));
    if (!D->hash_table) {
        fprintf(stderr, "Error: Memory allocation failed for Hash Table in Dictionary\n");
        free(D);
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < hash_table_size; i++) {
        D->hash_table[i] = createList(dataPrinter);
        D->hash_table[i]->head = NULL;
        D->hash_table[i]->length = 0;
    }

    return D;
}

//A helper function that frees the KVPair struct
static void freeKVPair(void *data) {
    KVPair *pair = (KVPair *)data;
    free(pair->key);
    free(pair->value);
    free(pair);
}

//Frees the dictionary by freeing every struct of its misery.
void dictionary_destroy(Dictionary *d) {
    if (d) {
        for (int i = 0; i < d->slots; i++) {
            ListPtr list = d->hash_table[i];
            for (int j = 0; j < lengthList(list); j++) {
                KVPair *pair = (KVPair *)getList(list, j);
                freeKVPair(pair);
            }

            destroyList(&list);
        }
        free(d->hash_table);
        free(d);
    }
}

//Inserts the KVPair in a hashed index in the dictionary
bool dictionary_insert(Dictionary *D, KVPair *elem) {
    int index = ht_hash(elem->key, D->slots);
    ListPtr list = D->hash_table[index];
    int key_index = find_key_index(list, elem->key);

    if (key_index == -1) {
        appendList(list, elem);
        D->size++;
        return true;
    } else {
        //free the element that was supposed to inserted, since it was malloced somewhere else
        free(elem->key);
        free(elem->value);
        free(elem);
        return false;
    }
}

//Take the key, find the index using the hash function and then find the specific struct node 
//in that linked list in the hash table. Free's the stuff before calling deleteList
KVPair *dictionary_delete(Dictionary *D, char *key) {
    int index = ht_hash(key, D->slots);
    ListPtr list = D->hash_table[index];
    int key_index = find_key_index(list, key);
    KVPair* deletingPAIR = (KVPair*)getList(list, key_index);

    if (key_index == -1) {
        return NULL;
    } else{
        free(deletingPAIR->key);
        free(deletingPAIR->value);
        free(deletingPAIR);
        D->size--;
        KVPair* removedPair;
        return removedPair = (KVPair *)deleteList(list, key_index);
    }

}

//Finds the index of the key.
KVPair *dictionary_find(Dictionary *D, char *k) {
    int index = ht_hash(k, D->slots);
    ListPtr list = D->hash_table[index];
    int key_index = find_key_index(list, k);

    if (key_index != -1) {
        return (KVPair *)getList(list, key_index);
    }

    return NULL;
}

//Function for printing the dictionary.
void dictionary_print(Dictionary *D) {
    for (int i = 0; i < D->slots; i++) {
        ListPtr list = D->hash_table[i];
        // printf("%d: ", i);
        for (int j = 0; j < lengthList(list); j++) {
            KVPair *pair = (KVPair *)getList(list, j);
            printf("%s by %s\n", pair->key, (char *)pair->value);
        }   
    }
    // printf("\n");
}

//Print function for intializing lists objects.
void voidPrintList(void *data) {
    ListObj *listObj = (ListObj *)data;
    if (listObj != NULL) {
        NodePtr n = listObj->head;
        while (n != NULL) {
            (*listObj->dataPrinter)(n->data);
            n = n->next;
        }
    }
}

// void getAuthorWithMostSongs(Dictionary * D){
//     //make a new linked list which purpose is to store the unique authors and their maximum number of songs
//     //each node has the AuthorMax struct

//     //first, we iterate through each hash_table linked list, find the author
//         //check if that author is already in the linked list,
//             //if yes, increase count
//             //else make a new struct and add a new node with authorname and count as 1

void getAuthorWithMostSongs(Dictionary *D) {
    // Create a linked list to store unique authors and their song counts
    ListPtr authorList = createList(voidPrintList);
    for (int i = 0; i < D->slots; i++) {
        ListPtr list = D->hash_table[i];
        for (int j = 0; j < lengthList(list); j++) {
            KVPair *pair = (KVPair *)getList(list, j);
            char *author = pair->value;

            int authorIndex = find_key_index(authorList, author);
            if (authorIndex != -1) {
                // update the song count if in the list
                AuthorMax *authorMax = (AuthorMax *)getList(authorList, authorIndex);
                authorMax->count++;
            } else {
                AuthorMax *newAuthorMax = (AuthorMax *)malloc(sizeof(AuthorMax));
                if (newAuthorMax == NULL) {
                    return;
                }

                newAuthorMax->author = (char *)malloc(strlen(pair->value) + 1);
                if (newAuthorMax->author == NULL) {
                    free(newAuthorMax); 
                    return;
                }

                strcpy(newAuthorMax->author, pair->value);
                newAuthorMax->count = 1;
                appendList(authorList, newAuthorMax);

            }
        }
    }

    int maxSongCount = 0;
    ListPtr maxAuthors = createList(voidPrintList);
    for (int k = 0; k < lengthList(authorList); k++) {
        AuthorMax *authorMax = (AuthorMax *)getList(authorList, k);
        if (authorMax->count > maxSongCount) {
            maxSongCount = authorMax->count;
            destroyList(&maxAuthors);  
            maxAuthors = createList(voidPrintList);
            appendList(maxAuthors, authorMax);
        } else if (authorMax->count == maxSongCount) {
            appendList(maxAuthors, authorMax);
        }
    }

    // printf("Top Composer(s):\n");
    for (int i = 0; i < lengthList(maxAuthors); i++) {
        AuthorMax *authorMax = (AuthorMax *)getList(maxAuthors, i);
        printf("%s: %d song(s)\n", authorMax->author, authorMax->count);
    }

    // Cleanup
    for (int i = 0; i < lengthList(authorList); i++) {
        AuthorMax *authors = (AuthorMax *)getList(authorList, i);
        free(authors->author);
        free(authors);
    }

    destroyList(&authorList);
    destroyList(&maxAuthors);
}

KVPair *createKVPair(char *key, void *value) {
    KVPair *pair = (KVPair *)malloc(sizeof(KVPair));
    if (!pair) {
        fprintf(stderr, "Error: Memory allocation failed for KVPair\n");
        exit(EXIT_FAILURE);
    }

    pair->key = (char *)malloc(strlen(key) + 1);
    if (!pair->key) {
        fprintf(stderr, "Error: Memory allocation failed for KVPair key\n");
        free(pair);
        exit(EXIT_FAILURE);
    }

    strcpy(pair->key, key);
    pair->value = value;

    return pair;
}

/*
This function identifies and prints words with the highest occurrences in a music database. 
After processing, it prints words with counts equal to or greater than the maximum occurrence. 
*/

void getMostFrequentWords(Dictionary *D) {
    Dictionary *words = dictionary_create(100, voidPrintList);
    int maxOccurrence = 0;
    
    int i = 0;
    while (i < D->slots) {
        ListPtr songs = D->hash_table[i];
        int j = 0;
        while (j < lengthList(songs)) {
            KVPair *songPair = (KVPair *)getList(songs, j);
            char *songTitle = (char *)malloc(strlen(songPair->key) + 1);
            if (!songTitle) {
                exit(EXIT_FAILURE);
            }
            strcpy(songTitle, songPair->key);

            char *token = strtok(songTitle, " ");
            while (token != NULL) {
                KVPair *checkWord = dictionary_find(words, token);
                if (checkWord == NULL) {
                    int *count = (int *)malloc(sizeof(int));
                    *count = 1;
                    KVPair *newWord = createKVPair(token, count);
                    dictionary_insert(words, newWord);

                    if (*count > maxOccurrence) {
                        maxOccurrence = *count;
                    }
                } else {
                    int *existingCount = (int *)checkWord->value;
                    (*existingCount)++;

                    if (*existingCount > maxOccurrence) {
                        maxOccurrence = *existingCount;
                    }
                }
                token = strtok(NULL, " ");
            }
            free(songTitle);
            j++;
        }
        i++;
    }

    int p = 0;
    while (p < words->slots) {
        ListPtr wordList = words->hash_table[p];
        int k = 0;

        while (k < lengthList(wordList)) {
            KVPair *wordPair = (KVPair *)getList(wordList, k);
            int *count = (int *)wordPair->value;
            if (*count >= maxOccurrence) {
                printf("%s: %d occurrence(s)\n", wordPair->key, *count);
            }

            k++;
        }
        p++;
    }
    dictionary_destroy(words);
}
