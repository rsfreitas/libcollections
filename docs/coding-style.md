# Internal coding style

A description of the internal coding style used inside the library.

## Base reference

This library follows much of [the linux kernel Coding Style](https://www.kernel.org/doc/html/v4.10/process/coding-style.html),
except it uses:
* 4 (four) spaces as indentation.

## Naming convention

Every exported data, such as functions, variables and constants, must use the
following rules when created:

* Functions and global variables (when needed) must have the **cl\_** prefix
in their names.
* Constants must have the **CL\_** prefix.

### Internal API

Functions that belong to the internal API should have a prefix related to its
source file when available.

### Types

New types must also follow this rule and have as a suffix a **\_t**.

## Arrangement of functions within a source file

Each source file must have distinct blocks grouping functions with a certain
visibility. The recommendation is as follows:

* One block for internal source file functions (_static _functions);
* One block for internal library functions (**CL\_INTERNAL\_API** functions);
* One block for exported functions.

Between each block there should be a comment indicating its beginning.
Example:
```
#include <stdlib.h>

/*
 *
 * Internal functions
 *
 */

static void internal_foo(void)
{
}

/*
 *
 * Internal API
 *
 */

CL\_INTERNAL\_API
void internal_api_foo(void)
{
}

/*
 *
 * API
 *
 */

void foo(void)
{
}
```

#### Notes

By default, all library functions are exported by the shared library version.
However, we explicitly hide all those that we want to keep their visibility
internal.

There two different type of internal functions:

* Those that belong to the source file itself.
* Those that can be called from other files.

The former should be declared as _static_ while the latter should use the
internal visibility constant **CL\_INTERNAL\_API**.

Every other function that doesn't follow these rules will be exported.

## Documentation

Every exported function **must** be documented and use [doxygen](http://www.stack.nl/~dimitri/doxygen/)
to its style.

The comment should be placed at the beginning of the function and must
exist both in the source file and in the header.

An example:
```
/**
 * @namef cl_foo
 * @brief An example of a brief.
 *
 * Details of what this function does.
 *
 * @param [in] obj1: An argument that "is" read-only inside the function.
 * @param [in,out] obj2: An argument that can be modified inside the function.
 * @param [in] s: Another argument
 *
 * @return On success returns a value.
 */
int cl_foo(const cl_type_t *obj1, cl_type_t *obj2, const char *s)
{
    return 0;
}
```

