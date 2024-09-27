#include "Dictionary.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define MAX_STR_SIZE 100

//a function to use while initalizing the dictionary data structure
void printSong(void *data) {
    KVPair *song = (KVPair *)data;
    printf("%s by %s\n", song->key, (char *)song->value);
}

//since Makefile won't allow the usage of strdup, I made
//my own strdup function using malloc and strcpy.
char *STRDUPPED(const char *str) {
    if (str == NULL) {return NULL;}
    char *newStr = (char *)malloc(strlen(str) + 1);
    if (newStr != NULL) {strcpy(newStr, str);}
    return newStr;
}

//This function reads the songs from the music data base, and 
//inserts them into the dictionary data structure
void readSongs(const char * filename, Dictionary *musicDatabase) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error opening file: %s\n", filename);
        return;
    }

    char line[100];
    char author[100];
    char song[100];

    // Read each line from stdin
    while (fgets(line, sizeof(line), file) != NULL) {
        // Use sscanf to parse the line and retrieve author and song
        if (sscanf(line, "%99[^,], %99[^\n]\n", author, song) == 2) {
            // Create a new KVPair and insert it into the dictionary
            KVPair *newPair = (KVPair *)malloc(sizeof(KVPair));
            newPair->key = STRDUPPED(song);
            newPair->value = STRDUPPED(author);
            dictionary_insert(musicDatabase, newPair);
        } else {
            printf("Invalid input format: %s\n", line);
        }
    }
    fclose(file);


}

//handleCommands uses switch cases to handle the different commands from the 
//input files such as add, delete, update, and display information.
void handleCommands(Dictionary *musicDatabase) {
    char filename[100];

    if (scanf("%s", filename) != 1) {
        return;
    }

    getchar();

    //read the file, which is the first line in the input file
    readSongs(filename, musicDatabase);
    // dictionary_print(musicDatabase);    

    int choice;
    char songName[100], authorName[100];

    while (scanf("%d", &choice) == 1) {
        getchar();
        switch (choice) {
            case 1:
                // printf("am i getting to case 1?\n");
                fgets(songName, sizeof(songName), stdin);
                // printf("%s\n", songName);

                fgets(authorName, sizeof(authorName), stdin);
                // printf("%s\n", authorName);

                //this makes sures to remove the leading newline characters in the line we just read.
                //not having these lines caused a lot of problems since a line with newline character
                //is not exactly equal to a line with no newline character.
                songName[strcspn(songName, "\n")] = '\0';
                authorName[strcspn(authorName, "\n")] = '\0';

                KVPair *newPair = (KVPair *)malloc(sizeof(KVPair));
                newPair->key = STRDUPPED(songName);
                newPair->value = STRDUPPED(authorName);
                dictionary_insert(musicDatabase, newPair);
                break;
            case 2:
                // printf("am i getting to case 2?\n");

                fgets(songName, sizeof(songName), stdin);
                songName[strcspn(songName, "\n")] = '\0';

                dictionary_delete(musicDatabase, songName);
                break;
            case 3:
                // printf("Update Song Details:\n");

                fgets(songName, sizeof(songName), stdin);
                songName[strcspn(songName, "\n")] = '\0';

                KVPair *existingPair = dictionary_find(musicDatabase, songName);
                if (existingPair != NULL) {
                    // printf("Current author of %s is %s\n", songName, (char *)existingPair->value);
                    fgets(authorName, sizeof(authorName), stdin);
                    authorName[strcspn(authorName, "\n")] = '\0';

                    free(existingPair->value);
                    existingPair->value = STRDUPPED(authorName);

                    // printf("Updated author of %s to %s\n", songName, (char *)existingPair->value);
                } else {
                    printf("Song not found!\n");
                }
                break;
            case 4:
                // printf("am i getting to case 4?\n");
                fgets(songName, sizeof(songName), stdin);
                songName[strcspn(songName, "\n")] = '\0';

                KVPair *foundPair = dictionary_find(musicDatabase, songName);
                if (foundPair != NULL) {
                    printf("Found song: %s by %s\n", foundPair->key, (char *)foundPair->value);
                } else {
                    printf("Song not found!\n");
                }
                break;
            case 5:
                // printf("am i getting to case 5?\n");
                // printf("Music Database:");
                dictionary_print(musicDatabase);
                break;
            case 6:
                // printf("am i getting to case 6?\n");
                getAuthorWithMostSongs(musicDatabase);
                break;
            case 7:
                // printf("am i getting to case 7?\n");
                getMostFrequentWords(musicDatabase);
                break;
            case 8:
                // Exit
                printf("Goodbye!");
                dictionary_destroy(musicDatabase);
                exit(0);
            default:
                printf("Invalid choice! Please select a valid option.\n");
                break;
        }

    }
}


//I like keeping my main short!
int main() {
    Dictionary *musicDatabase;
    musicDatabase = dictionary_create(MAX_STR_SIZE, &printSong);

    handleCommands(musicDatabase);

    //free shit
    dictionary_destroy(musicDatabase);

}