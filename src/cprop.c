#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cprop.h"

#define INITIAL_KEY_LEN 32
#define INITIAL_VALUE_LEN 64

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
 * Consumes all white-space characters up to the first non-white-space character
 * in the stream. Returnes the number of lines skipped.
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
 * Returns the next character in the stream without consuming it.
 */
static int fpeek(FILE *stream) {
    int c = fgetc(stream);
    ungetc(c, stream);
    return c;
}

/*
 * Reads a value from a .properties file and stores a pointer to it in the
 * output param value. Returns the number of lines that the value spans. In case
 * of an error, the function returns a negative value.
 */
static int read_value(FILE *stream, char **value) {
    int len = INITIAL_VALUE_LEN;
    int i = 0;
    int c = 0;
    int lines = 1;
    *value = (char *) calloc(sizeof(char), len + 1);

    lines += skip_whitespace(stream);

    while (1) {
        c = fpeek(stream);

        if (c == '\\') {
            // Consume backslash
            fgetc(stream);
            c = fpeek(stream);
            if (c == '\n') {
                // Value continues on the next line
                lines += skip_whitespace(stream);
                continue;
            }
        } else if ((c == '\n') || (c == EOF)) {
            // End of value reached
            return lines;
        }
        
        // Add character to value
        (*value)[i++] = fgetc(stream);

        // Check if key array is full
        if (i == len) {
            // Double key array size
            int new_len = 2 * len;
            char *new_value = realloc(*value, sizeof(char) * (new_len + 1));
            if (!new_value) {
                free(*value);
                *value = NULL;
                return -1;
            }

            // Zero-fill new part of array
            memset((new_value + sizeof(char) * (len + 1)), 0,
                   sizeof(char) * (new_len - len));
            *value = new_value;
            len = new_len;
        }
    }
}

/*
 * Reads a key from a .properties file and stores a pointer to it in the output
 * param key. In case of an error, the function returns a negative value;
 */
static int read_key(FILE *stream, char **key) {
    int len = INITIAL_KEY_LEN;
    int i = 0;
    int c = 0;
    *key = (char *) calloc(sizeof(char), len + 1);

    while (1) {
        c = fpeek(stream);

        if (c == '\\') {
            // Consume backslash
            // Escaped char will be added below
            fgetc(stream);
        } else if ((c == ' ') || (c == '\t') || (c == '\f') || (c == '=') ||
                   (c == ':') || (c == EOF)) {
            // End of key reached
            return 0;
        }
        
        // Add character to key
        (*key)[i++] = fgetc(stream);

        // Check if key array is full
        if (i == len) {
            // Double key array size
            int new_len = 2 * len;
            char *new_key = realloc(*key, sizeof(char) * (new_len + 1));
            if (!new_key) {
                free(*key);
                *key = NULL;
                return -1;
            }

            // Zero-fill new part of array
            memset((new_key + sizeof(char) * (len + 1)), 0,
                   sizeof(char) * (new_len - len));
            *key = new_key;
            len = new_len;
        }
    }
}

/*
 * Skips over the whitespace characters that are valid between a key and an
 * assignment operator (' ', '\t', '\f') and reads the next char. If that char
 * is an assignment operator, the function returns 0. Otherwise, the function
 * returns a negative value.
 */
static int read_assignment(FILE *stream) {
    int c = 0;

    while ((c = fgetc(stream)) != EOF) {
        if ((c == '=') || (c == ':')) {
            break;
        } else if ((c == ' ') || (c == '\t') || (c == '\f')) {
            continue;
        }

        return -1;
    }

    return 0;
}

static int read_file(char *filename, FILE *stream, Properties *prop) {
    int c = 0;
    int res = 0;
    int lines = 0;
    char *key = NULL;
    char *value = NULL;

    while (1) {
        lines += skip_whitespace(stream);
        c = fpeek(stream);

        switch(c) {
            case EOF:
                return 0;
            case '#':
            case '!':
                consume_line(stream);
                lines++;
                break;
            default:
                res = read_key(stream, &key);
                if (!key) {
                    return -1;
                }

                res = read_assignment(stream);
                if (res < 0) {
                    fprintf(
                        stderr,
                        "cprop_load(): missing assignment operator (%s:%d)\n",
                        filename, lines);
                    free(key);
                    return -1;
                }

                res = read_value(stream, &value);
                if (res < 0) {
                    return -1;
                }

                lines += res;

                res = cprop_set(prop, key, value);
                free(key);
                free(value);

                if (res < 0) {
                    return -1;
                }

                break;
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
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("cprop_load()");
        return NULL;
    }

    Properties *prop = cprop_init();
    if (!prop) {
        fclose(file);
        perror("cprop_load()");
        return NULL;
    }

    int res = read_file(filename, file, prop);
    if (res < 0) {
        free(prop);
        prop = NULL;
        perror("cprop_load()");
    }

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
                perror("cprop_set()");
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
        perror("cprop_set()");
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

