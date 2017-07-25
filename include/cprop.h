/**
 * API functions for libcprop.
 *
 * This library loads Java-style <code>.properties</code> files.
 *
 * @see https://en.wikipedia.org/wiki/.properties
 */

#ifndef CPROP_H
#define CPROP_H

typedef struct _Properties Properties;

/**
 * Loads properties from the specified file.
 *
 * Creates a Properties structure containing the loaded properties returns a
 * pointer to it. The returned structure must be freed with cprop_free(). In
 * case of an error, a <code>NULL</code> pointer is returned.
 *
 * @param filename name of the properties file
 * @return a pointer to a Properties structure, or <code>NULL</code>
 */
extern Properties *cprop_load(char *filename);

/**
 * Frees a Properties structure.
 *
 * @param prop pointer to a Properties structure
 */
extern void cprop_free(Properties *prop);

/**
 * Returns the value associated with <code>key</code> within the provided
 * Properies structure. If the provided key is not found within the Properties,
 * <code>NULL<code> is returned.
 *
 * @param prop pointer to a Properties structure
 * @param key key to search for
 * @return value associated with the provided key, or <code>NULL</code>
 */
extern char *cprop_get(Properties *prop, char *key);

/**
 * Creates a new property within the provided Properties structure using copies
 * of the provided key and value or replaces the current value associated with
 * key.
 *
 * @param prop pointer to a Properties structure
 * @param key key to store to
 * @param value value to associate with key
 * @return <code>0</code> if successful; <code>-1</code> otherwise
 */
extern int cprop_set(Properties *prop, char *key, char *value);

extern void cprop_print(Properties *prop);

#endif

