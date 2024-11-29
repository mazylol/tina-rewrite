#include "intents.h"

struct Intents *init() {
    struct Intents *intents = malloc(sizeof(struct Intents));
    intents->intents = NULL;
    intents->size = 0;

    return intents;
}

void addIntent(struct Intents *intents, char *pattern, char *responses[]) {
    intents->size++;
    intents->intents = realloc(intents->intents, intents->size * sizeof(struct Intent *));
    struct Intent *intent = malloc(sizeof(struct Intent));
    intent->pattern = pattern;

    size_t response_count = 0;
    while (responses[response_count] != NULL) {
        response_count++;
    }

    intent->responses = malloc((response_count + 1) * sizeof(char *));
    for (size_t i = 0; i < response_count; i++) {
        intent->responses[i] = responses[i];
    }
    intent->responses[response_count] = NULL;

    intents->intents[intents->size - 1] = intent;
}

void deleteIntent(struct Intents *intents, char *pattern) {
    for (size_t i = 0; i < intents->size; i++) {
        if (intents->intents[i]->pattern == pattern) {
            free(intents->intents[i]->responses);
            free(intents->intents[i]);
            for (size_t j = i; j < intents->size - 1; j++) {
                intents->intents[j] = intents->intents[j + 1];
            }
            intents->size--;
            break;
        }
    }
}

void deleteIntentAt(struct Intents *intents, size_t index) {
    free(intents->intents[index]->responses);
    free(intents->intents[index]);
    for (size_t i = index; i < intents->size - 1; i++) {
        intents->intents[i] = intents->intents[i + 1];
    }
    intents->size--;
}

void addResponse(struct Intents *intents, char *pattern, char *response) {
    for (size_t i = 0; i < intents->size; i++) {
        if (strcmp(intents->intents[i]->pattern, pattern) == 0) {
            size_t size = 0;
            while (intents->intents[i]->responses[size] != NULL) {
                size++;
            }
            intents->intents[i]->responses = realloc(intents->intents[i]->responses, (size + 2) * sizeof(char *));
            intents->intents[i]->responses[size] = my_strdup(response);
            intents->intents[i]->responses[size + 1] = NULL;
            break;
        }
    }
}

void deleteResponse(struct Intents *intents, char *pattern, char *response) {
    for (size_t i = 0; i < intents->size; i++) {
        if (intents->intents[i]->pattern == pattern) {
            size_t size = 0;
            while (intents->intents[i]->responses[size] != NULL) {
                size++;
            }
            for (size_t j = 0; j < size; j++) {
                if (intents->intents[i]->responses[j] == response) {
                    for (size_t k = j; k < size - 1; k++) {
                        intents->intents[i]->responses[k] = intents->intents[i]->responses[k + 1];
                    }
                    intents->intents[i]->responses[size - 1] = NULL;
                    intents->intents[i]->responses = realloc(intents->intents[i]->responses, size * sizeof(char *));
                    break;
                }
            }
            break;
        }
    }
}

void deleteResponseAt(struct Intents *intents, char *pattern, size_t index) {
    for (size_t i = 0; i < intents->size; i++) {
        if (intents->intents[i]->pattern == pattern) {
            size_t size = 0;
            while (intents->intents[i]->responses[size] != NULL) {
                size++;
            }
            for (size_t j = index; j < size - 1; j++) {
                intents->intents[i]->responses[j] = intents->intents[i]->responses[j + 1];
            }
            intents->intents[i]->responses[size - 1] = NULL;
            intents->intents[i]->responses = realloc(intents->intents[i]->responses, size * sizeof(char *));
            break;
        }
    }
}

void freeIntents(struct Intents *intents) {
    for (size_t i = 0; i < intents->size; i++) {
        free(intents->intents[i]->responses);
        free(intents->intents[i]);
    }
    free(intents->intents);
    free(intents);
}

char *my_strdup(const char *s) {
    size_t len = strlen(s) + 1;
    char *dup = malloc(len);
    if (dup) {
        memcpy(dup, s, len);
    }
    return dup;
}

struct RawIntents *loadRawIntents(char *filename, size_t *num_intents) {
    FILE *file;
    char *buffer;
    long length;

    file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    length = ftell(file);
    fseek(file, 0, SEEK_SET);

    buffer = (char *)malloc(length + 1);
    if (buffer == NULL) {
        perror("Error allocating memory");
        fclose(file);
        return NULL;
    }

    fread(buffer, 1, length, file);
    buffer[length] = '\0';
    fclose(file);

    json_t *root;
    json_error_t error;

    root = json_loads(buffer, 0, &error);
    if (!root) {
        fprintf(stderr, "Error parsing JSON: %s\n", error.text);
        free(buffer);
        return NULL;
    }

    json_t *intents = json_object_get(root, "intents");
    if (!json_is_array(intents)) {
        fprintf(stderr, "Error: intents is not an array\n");
        json_decref(root);
        free(buffer);
        return NULL;
    }

    *num_intents = json_array_size(intents);
    struct RawIntents *rawIntents = malloc(*num_intents * sizeof(struct RawIntents));
    if (rawIntents == NULL) {
        perror("Error allocating memory for rawIntents");
        json_decref(root);
        free(buffer);
        return NULL;
    }

    for (size_t i = 0; i < *num_intents; i++) {
        json_t *intent = json_array_get(intents, i);
        json_t *pattern = json_object_get(intent, "pattern");
        json_t *responses = json_object_get(intent, "responses");

        rawIntents[i].pattern = my_strdup(json_string_value(pattern));

        size_t num_responses = json_array_size(responses);
        rawIntents[i].responses = malloc((num_responses + 1) * sizeof(char *));
        for (size_t j = 0; j < num_responses; j++) {
            rawIntents[i].responses[j] = my_strdup(json_string_value(json_array_get(responses, j)));
        }
        rawIntents[i].responses[num_responses] = NULL; // Null-terminate the responses array
    }

    json_decref(root);
    free(buffer);

    return rawIntents;
}

void freeRawIntents(struct RawIntents *rawIntents, size_t num_intents) {
    for (size_t i = 0; i < num_intents; i++) {
        free(rawIntents[i].pattern);
        for (size_t j = 0; rawIntents[i].responses[j] != NULL; j++) {
            free(rawIntents[i].responses[j]);
        }
        free(rawIntents[i].responses);
    }
    free(rawIntents);
}

void generateJson(struct Intents *intents, char *filename) {
    json_t *root = json_object();
    json_t *intents_array = json_array();

    for (size_t i = 0; i < intents->size; i++) {
        json_t *intent = json_object();
        json_t *pattern = json_string(intents->intents[i]->pattern);
        json_t *responses = json_array();

        for (size_t j = 0; intents->intents[i]->responses[j] != NULL; j++) {
            json_array_append_new(responses, json_string(intents->intents[i]->responses[j]));
        }

        json_object_set_new(intent, "pattern", pattern);
        json_object_set_new(intent, "responses", responses);

        json_array_append_new(intents_array, intent);
    }

    json_object_set_new(root, "intents", intents_array);

    char *json = json_dumps(root, JSON_INDENT(4));
    json_decref(root);

    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        perror("Error opening file");
    }

    fprintf(file, "%s", json);
    fclose(file);

    free(json);
}