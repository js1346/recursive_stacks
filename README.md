# Recursive Stacks Library

This project implements a dynamically loaded library in C for managing recursive stacks. The library is designed to handle reference cycles using a custom Mark-and-Sweep garbage collection mechanism and provides a strong exception-safety guarantee against memory allocation failures.

## About the Project

The elements of a recursive stack can be either 64-bit unsigned integers (`uint64_t`) or references to other stacks. Pushing a stack onto another does not copy its contents; instead, it pushes a reference. Because of this, changes made to a stack's content are visible everywhere a reference to that stack is held.

Because stacks can be pushed onto themselves or mutually reference each other, the structure can form arbitrary directed graphs with cycles. To prevent memory leaks when references are dropped or stacks are deleted, the library tracks active stack instances and uses a custom **Mark-and-Sweep** reachability algorithm (`mark_and_sweep`) to automatically reclaim unreachable cycles. The implementation imposes no artificial limits on the size of stored data, bounded only by available system memory and machine word size.

## API Interface

The library provides the following functions, defined in the `rstack.h` header file:

* **`rstack_t* rstack_new()`**: Creates a new empty stack. Returns a pointer to the structure or `nullptr` if memory allocation fails (setting `errno` to `ENOMEM`).
* **`void rstack_delete(rstack_t *rs)`**: Marks the stack pointed to by `rs` as destroyed and triggers garbage collection of unreachable stacks. If `rs` is `nullptr`, the function does nothing. The pointer `rs` should not be used after this call.
* **`int rstack_push_value(rstack_t *rs, uint64_t value)`**: Pushes the specified `value` onto the stack `rs`. Returns `0` on success, or `-1` if the pointer is null or memory allocation fails (setting `errno` to `EINVAL` or `ENOMEM`).
* **`int rstack_push_rstack(rstack_t *rs1, rstack_t *rs2)`**: Pushes a reference to stack `rs2` onto stack `rs1`. Returns `0` on success, or `-1` on invalid arguments or memory allocation errors.
* **`void rstack_pop(rstack_t *rs)`**: Non-recursively pops the top element of the given stack and reclaims any unreachable substacks. If the stack is empty or the parameter is `nullptr`, the function does nothing.
* **`bool rstack_empty(rstack_t *rs)`**: Recursively checks if the given stack contains any numbers (safely handling cycles via visited flags). Returns `false` if the stack contains at least one number, and `true` if the stack is empty, `nullptr`, or contains only empty/cyclic substacks.
* **`result_t rstack_front(rstack_t *rs)`**: Recursively finds the first number closest to the top of the stack. Returns a `result_t` structure containing a validity flag (`flag == true` if a number was found) and the `value` itself.
* **`rstack_t* rstack_read(char const *path)`**: Creates a new stack populated with base-10 numbers read from the specified file (separated by arbitrary whitespace). Strictly validates file contents and sets `errno` (`EINVAL`, `ENOMEM`, `EIO`, etc.) and returns `nullptr` in case of errors.
* **`int rstack_write(char const *path, rstack_t *rs)`**: Writes all numbers stored on the stack to the specified file in base-10 format without leading zeros, placing each on a separate line. Writing is cleanly aborted if a cycle is detected during traversal. Returns `0` on success or `-1` on errors.

## Building and Compilation

The project includes a `Makefile` that automates the build process using `gcc`.

1. **Building the Dynamic Library**: 
   To compile the code into a shared object file `librstack.so`, run:

       make librstack.so

   The source code is compiled using the `gnu23` standard, with `-O2` optimization, `-Wall -Wextra` warnings, and position-independent code (`-fPIC`).

2. **Memory Allocation Tracking**:
   The build process uses linker wrapping options (`-Wl,--wrap=malloc`, `-Wl,--wrap=free`, etc.) to intercept memory management calls, supporting fault-injection and leak diagnostics via the `memory_tests.c` module.

3. **Cleaning the Project**:
   All generated object files and the shared library can be removed with the command:

       make clean

## Author
**Jakub Skalany**  
Computer Science and Mathematics (JSIM) Student at the University of Warsaw
