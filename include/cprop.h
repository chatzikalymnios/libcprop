/**
 * @file include/cprop.h
 *
 * API functions for libcprop.
 *
 * This library loads Java-style `.properties` files.
 * However, it does not yet support Unicode escape sequences.
 *
 * @see https://en.wikipedia.org/wiki/.properties
 */

#ifndef CPROP_H
#define CPROP_H

#include <stdio.h>

/**
 * A Properties structure contains Java-style properties and is used by the user
 * to interact with the library in order to get/set properties.
 */
typedef struct _Properties Properties;

/**
 * Loads properties from the specified file.
 *
 * Creates a Properties structure containing the loaded properties returns a
 * pointer to it. The returned structure must be freed with cprop_free(). In
 * case of an error, a `NULL` pointer is returned.
 *
 * @param filename name of the properties file
 * @return a pointer to a Properties structure, or `NULL`
 */
extern Properties *cprop_load(char *filename);

/**
 * Frees a Properties structure.
 *
 * @param prop pointer to a Properties structure
 */
extern void cprop_free(Properties *prop);

/**
 * Returns the value associated with `key` within the provided Properties
 * structure. If the provided `key` is not found within the Properties, `NULL`
 * is returned.
 *
 * @param prop pointer to a Properties structure
 * @param key key to search for
 * @return value associated with the provided key, or `NULL`
 */
extern char *cprop_get(Properties *prop, char *key);

/**
 * Creates a new property within the provided Properties structure using copies
 * of the provided `key` and `value` or replaces the current value associated
 * with `key`. In case of an error, the function returns a negative `int` and the
 * Properties structure remains unchanged.
 *
 * @param prop pointer to a Properties structure
 * @param key key to store to
 * @param value value to associate with key
 * @return `0` if successful; a negative `int` otherwise
 */
extern int cprop_set(Properties *prop, char *key, char *value);

/**
 * Deletes the property associated with `key` from the provided Properties
 * structure. Returns `0` if the property was found and deleted. If the key was
 * not found or deletion was not successful, the function returns a negative
 * `int`.
 *
 * @param prop pointer to a Properties structure
 * @param key key of property to delete
 * @return `0` if found and deleted; a negative `int` otherwise
 */
extern int cprop_delete(Properties *prop, char *key);

/**
 * Prints all properties contained in the provided Properties structure to the
 * provided stream.
 *
 * @param stream stream to write to
 * @param prop pointer to a Properties structure
 */
extern void cprop_print(FILE *stream, Properties *prop);

#endif

