#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cprop.h"

static int const INITIAL_STR_LEN = 32;

static char const KEY_DELIM[] = {'=', ':', ' ', '\t', '\f', '\n'};
static int const KEY_DELIM_COUNT = 6;

static char const VAL_DELIM[] = {'\n'};
static int const VAL_DELIM_COUNT = 1;

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
 * Consumes an entire line from the stream.
 */
static void consume_line(FILE *stream) {
    int c = 0;

    do {
        c = fgetc(stream);
    } while(c != EOF && c != '\n');
}

/*
 * Consumes all whitespace characters up to the first non-whitespace character
 * in the stream or up to the EOF. Returns the number of lines skipped.
 */
static int skip_whitespace(FILE *stream) {
    int c = 0;
    int lines = 0;

    while ((c = fgetc(stream)) != EOF) {
        if (!isspace(c)) {
            ungetc(c, stream);
            break;
        }

        if (c == '\n') {
            lines++;
        }
    }

    return lines;
}

/*
 * Consumes all blank characters (' ', '\t') up to the first non-blank character
 * in the stream or up to the EOF.
 */
static void skip_blanks(FILE *stream) {
    int c = 0;

    while ((c = fgetc(stream)) != EOF) {
        if (!isblank(c)) {
            ungetc(c, stream);
            break;
        }
    }
}

/*
 * Returns the next character in the stream without consuming it.
 */
static int fpeek(FILE *stream) {
    int c = fgetc(stream);
    ungetc(c, stream);
    return c;
}

/*
 * Checks whether the character c is contained in the provided char array.
 * If so, the function returns 1. Otherwise the function returns 0.
 */
static int contains(int size, const char *arr, char c) {
    int i = 0;

    for (i = 0; i < size; i++) {
        if (arr[i] == c) {
            return 1;
        }
    }

    return 0;
}

/*
 * Reads a string from a .properties file and stores a pointer to it in the
 * output param value. In case of an error, the function returns a negative
 * value. The string is considered to end when one of the characters in the
 * provided delim array is encountered unescaped or the EOF is reached.
 */
static int read_string(FILE *stream, int delimc, const char *delim, char **str) {
    int len = INITIAL_STR_LEN;
    int i = 0;
    int c = 0;
    *str = (char *) calloc(sizeof(char), len + 1);

    skip_blanks(stream);

    while (1) {
        c = fpeek(stream);

        if (c == '\\') {
            // Consume backslash
            fgetc(stream);
            c = fpeek(stream);
            if (c == '\n') {
                // String continues on the next line
                fgetc(stream);
                skip_blanks(stream);
                continue;
            }
        } else if (contains(delimc, delim, c) || (c == EOF)) {
            // End of string reached
            return 0;
        }
        
        // Add character to string
        (*str)[i++] = fgetc(stream);

        // Check if array is full
        if (i == len) {
            // Double array size
            int new_len = 2 * len;
            char *new_str = realloc(*str, sizeof(char) * (new_len + 1));
            if (!new_str) {
                free(*str);
                *str = NULL;
                return -1;
            }

            // Zero-fill new part of array
            memset((new_str + sizeof(char) * (len + 1)), 0,
                   sizeof(char) * (new_len - len));
            *str = new_str;
            len = new_len;
        }
    }
}

/*
 * Skips over the blank characters and consumes the assignment operator (if it
 * exists) and skips all remaining blank characters until the start of the next
 * string, the newline char or EOF.
 */
static void read_assignment(FILE *stream) {
    skip_blanks(stream);
    
    int c = fgetc(stream);
    if ((c != '=') && (c != ':')) {
        ungetc(c, stream);
    }

    skip_blanks(stream);
}

/*
 * Reads a key-value pair from a .properties file and stores it in the provided
 * Properties structure. In case of an error, the function returns a negative
 * value.
 */
static int read_property(FILE *stream, Properties *prop) {
    char *key = NULL;
    char *value = NULL;

    int res = read_string(stream, KEY_DELIM_COUNT, KEY_DELIM, &key);
    if (res < 0) {
        return -1;
    }

    read_assignment(stream);

    res = read_string(stream, VAL_DELIM_COUNT, VAL_DELIM, &value);
    if (res < 0) {
        free(key);
        return -1;
    }

    res = cprop_set(prop, key, value);
    free(key);
    free(value);

    if (res < 0) {
        return -1;
    }

    return 0;
}

/*
 * Reads the entire .properties file and stores all properties in the provided
 * Properties structure. In case of an error, the function returns a negative
 * value.
 */
static int read_file(FILE *stream, Properties *prop) {
    int c = 0;
    int res = 0;

    while (1) {
        skip_whitespace(stream);
        c = fpeek(stream);

        if (c == EOF) {
            return 0;
        } else if (c == '#' || c == '!') {
            // Comments are ignored
            consume_line(stream);
            continue;
        }

        res = read_property(stream, prop);
        if (res < 0) {
            return -1;
        }
    }

    return 0;
}

/*
 * Creates an empty Properties structure and returns a pointer to it. In case of
 * an error, the function returns NULL.
 */
static Properties *cprop_init() {
    Properties *prop = (Properties *) malloc(sizeof(Properties));
    if (!prop) {
        return NULL;
    }

    prop->head = (Node *) malloc(sizeof(Node));
    if (!(prop->head)) {
        free(prop);
        return NULL;
    }

    memset(prop->head, 0, sizeof(Node));
    return prop;
}

Properties *cprop_load(char *filename) {
    FILE *stream = fopen(filename, "r");
    if (!stream) {
        return NULL;
    }

    Properties *prop = cprop_init();
    if (!prop) {
        fclose(stream);
        return NULL;
    }

    int res = read_file(stream, prop);
    if (res < 0) {
        free(prop);
        prop = NULL;
    }

    fclose(stream);
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

/*
 * Frees the current Property value within the provided Node and sets it equal
 * to a copy of the provided value. In case of an error, the function returns a
 * negative value.
 */
static int cprop_node_update(Node *n, char *value) {
    char *copy = strdup(value);
    if (!copy) {
        return -1;
    }

    free(n->p.value);
    n->p.value = copy;
    return 0;
}

/*
 * Creates a new Node with the provided key, value and next pointer and returns
 * a pointer to it. In case of an error, the function returns NULL.
 */
static Node *cprop_node_new(char *key, char *value, Node *next) {
    Node *n = (Node *) malloc(sizeof(Node));
    if (!n) {
        return NULL;
    }

    n->p.key = strdup(key);
    if (!(n->p.key)) {
        free(n);
        return NULL;
    }

    n->p.value = strdup(value);
    if (!(n->p.value)) {
        free(n->p.key);
        free(n);
        return NULL;
    }

    n->next = next;
    return n;
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
            res = cprop_node_update(curr->next, value);
            if (res < 0) {
                return -1;
            }

            return 0;
        } else if (res > 0) {
            break;
        }

        curr = curr->next;
    }

    Node *n = cprop_node_new(key, value, curr->next);
    if (!n) {
        return -1;
    }

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

