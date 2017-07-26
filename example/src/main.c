#include <stdio.h>

#include "cprop.h"

int main(int argc, char **argv) {
    int res = 0;

    Properties *prop = cprop_load("example/example.properties");
    if (!prop) {
        fprintf(stderr, "cprop_load() error\n");
        return 0;
    }

    printf("Properties as loaded from the file:\n");
    printf("-----------------------------------\n");
    cprop_print(stdout, prop);
    printf("-----------------------------------\n\n");

    // Adding properties at run-time
    res = cprop_set(prop, "run-time-1", "value 1");
    if (res < 0) {
        fprintf(stderr, "cprop_set() error\n");
    }

    // Getting properties
    char *run_time_1 = cprop_get(prop, "run-time-1");
    char *example = cprop_get(prop, "example");

    printf("Assert: \"%s\" == \"%s\"\n", run_time_1, "value 1");
    printf("Assert: \"%s\" == \"%s\"\n", example, "exampleValue");

    // Setting an existing property
    res = cprop_set(prop, "run-time-1", "new value 1");
    if (res < 0) {
        fprintf(stderr, "cprop_set() error\n");
    }

    run_time_1 = cprop_get(prop, "run-time-1");

    printf("Assert: \"%s\" == \"%s\"\n", run_time_1, "new value 1");

    // Deleting property
    res = cprop_delete(prop, "example");
    if (res < 0) {
        fprintf(stderr, "cprop_delete() error\n");
    }

    // cprop_get should return NULL now
    example = cprop_get(prop, "example");
    if (example) {
        fprintf(stderr, "property not deleted\n");
    }

    printf("\nProperties after operations:\n");
    printf("-----------------------------------\n");
    cprop_print(stdout, prop);
    printf("-----------------------------------\n\n");

    // Free Properties structure
    cprop_free(prop);

    return 0;
}

