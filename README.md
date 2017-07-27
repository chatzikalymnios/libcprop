# libcprop

A C library for loading Java-style `.properties` files.
Currently, the library does not support Unicode escape sequences.

## API

The library consists of a few simple functions.
They are defined and documented in [include/cprop.h](include/cprop.h).
An online version of the documentation can be found
[here](https://chatzikalymnios.github.io/libcprop/cprop_8h.html).

```C
/* Loads properties from file */
Properties *cprop_load(char *filename);

/* Destroys Properties structure */
void cprop_free(Properties *prop);

/* Retrieves value associated with key */
char *cprop_get(Properties *prop, char *key);

/* Associates value with key */
int cprop_set(Properties *prop, char *key, char *value);

/* Deletes property marked by key */
int cprop_delete(Properties *prop, char *key);

/* Prints all properties to provided stream */
void cprop_print(FILE *stream, Properties *prop);
```

## Example

An example program is included in [example/src/main.c](example/src/main.c),
which showcases how the library should be used.
The sample [example/example.properties](example/example.properties) is used for this purpose.
Read [instructions](#run-example-program) on how to run the example.

## Usage

Clone this repository and `cd` into the `libcprop` directory.
Edit the `CC` variable in the `Makefile` to choose your preferred C compiler.

### Build the library
```sh
$ make                          # compile the library
```

The `lib` directory will contain the static library `libcprop.a` and the shared library `libcprop.so`.

### Install shared library
Edit the `PREFIX` variable in the `Makefile` to choose where the shared library will be installed.
The default path is `/usr/local`. Next, run the following.
```sh
$ make                          # compile the library
$ sudo make install             # install shared library in chosen location
                                # sudo may or may not be required depending on PREFIX
```

### Run example program
```sh
$ make example-static           # build and run example using the static version of libcprop
$ make example-shared           # build and run example using the shared version of libcprop
```

### Run unit tests
The [Check](https://github.com/libcheck/check) framework is required in order to run the included tests.

```sh
$ make test                     # build and run unit tests using the static version of libcprop
```
## Notes

This project has been tested against:
- `gcc version 7.1.1`, and
- `clang version 4.0.1`

## See also:
- https://en.wikipedia.org/wiki/.properties
- https://docs.oracle.com/javase/8/docs/api/java/util/Properties.html#load-java.io.Reader-
