#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cprop.h"

typedef struct _Property {
    char *key;
    char *value;
} Property;

typedef struct _Node {
    Property p;
    struct _Node *next;
} Node;

struct _Properties {
    Node *head;
};

/*
 * Consumes all white-space characters up to the first non-white-space character
 * in the stream.
 */
void skip_whitespace(FILE *stream) {
    int c = 0;

    while ((c = fgetc(stream)) != EOF) {
        if (!isspace(c)) {
            ungetc(c, stream);
            break;
        }
    }
}

/*
 * Returns the next character in the stream without consuming it.
 */
int fpeek(FILE *stream) {
    int c = fgetc(stream);
    ungetc(c, stream);
    return c;
}

Properties *cprop_load(char *filename) {
    FILE *file = NULL;

    file = fopen(filename, "r");
    if (!file) {
        perror("cprop_load()");
        return NULL;
    }

    Properties *prop = (Properties *) malloc(sizeof(Properties));
    if (!prop) {
        fclose(file);
        perror("cprop_load()");
        return NULL;
    }

    prop->head = (Node *) malloc(sizeof(Node));
    if (!(prop->head)) {
        fclose(file);
        perror("cprop_load()");
        return NULL;
    }

    memset(prop->head, 0, sizeof(Node));

    fclose(file);
    return prop;
}

void cprop_free(Properties *prop) {
    if (!prop) {
        return;
    }

    Node *curr = prop->head;
    Node *next = NULL;

    while (curr) {
        next = curr->next;
        free(curr->p.key);
        free(curr->p.value);
        free(curr);
        curr = next;
    }

    free(prop);
}

char *cprop_get(Properties *prop, char *key) {
    if (!prop) {
        return NULL;
    }

    int res = 0;
    Node *curr = prop->head;

    while (curr) {
        res = strncmp(curr->p.key, key, strlen(curr->p.key));

        if (res < 0) {
            return NULL;
        } else if (res == 0) {
            return curr->p.value;
        }

        curr = curr->next;
    }

    return NULL;
}

int cprop_set(Properties *prop, char *key, char *value) {
    if (!prop) {
        return -1;
    }

    int res = 0;
    Node *curr = prop->head;

    while (curr->next) {
        res = strncmp(curr->next->p.key, key, strlen(curr->next->p.key));

        if (res == 0) {
            free(curr->next->p.value);
            curr->next->p.value = strdup(value);
            if (!(curr->next->p.value)) {
                perror("cprop_set()");
                return -1;
            }

            return 0;
        } else if (res > 0) {
            break;
        }

        curr = curr->next;
    }

    Node *n = (Node *) malloc(sizeof(Node));
    if (!n) {
        perror("cprop_set()");
        return -1;
    }

    n->p.key = strdup(key);
    if (!(n->p.key)) {
        free(n);
        perror("cprop_set()");
        return -1;
    }

    n->p.value = strdup(value);
    if (!(n->p.value)) {
        free(n->p.key);
        free(n);
        perror("cprop_set()");
        return -1;
    }

    n->next = curr->next;
    curr->next = n;

    return 0;
}

void cprop_print(Properties *prop) {
    Node *curr = prop->head->next;

    while (curr) {
        printf("%s = %s\n", curr->p.key, curr->p.value);
        curr = curr->next;
    }
}

