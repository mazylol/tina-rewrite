#ifndef INTENTS_H
#define INTENTS_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <jansson.h>

struct Intent {
    char *pattern;
    char **responses;
};

struct Intents {
    struct Intent **intents;
    size_t size;
};

struct Intents *init();

void addIntent(struct Intents *intents, char *pattern, char *responses[]);
void deleteIntent(struct Intents *intents, char *pattern);
void deleteIntentAt(struct Intents *intents, size_t index);

void addResponse(struct Intents *intents, char *pattern, char *response);
void deleteResponse(struct Intents *intents, char *pattern, char *response);
void deleteResponseAt(struct Intents *intents, char *pattern, size_t index);

void freeIntents(struct Intents *intents);

struct RawIntents {
    char *pattern;
    char **responses;
};

char *my_strdup(const char *s);
struct RawIntents *loadRawIntents(char *filename, size_t *num_intents);
void freeRawIntents(struct RawIntents *rawIntents, size_t num_intents);

void generateJson(struct Intents *intents, char *filename);

#endif