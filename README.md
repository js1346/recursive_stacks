# Recursive Stacks Library

This project implements a dynamically loaded library in C for managing recursive stacks. The library is designed to handle cycles and provides a strong guarantee of resilience against memory allocation failures.

## About the Project

The elements of a recursive stack can be either integers within the range of the `uint64_t` type or other stacks. Pushing a stack onto another does not copy its contents; instead, it pushes a reference. Because of this, changes made to a stack's content are visible everywhere a reference to that stack is held.

In the library's architecture, the structure of stacks can form cycles. A stack is automatically removed from memory when its reference count drops to zero, eliminating memory leaks – the project provides a strong guarantee against memory allocation failures. The implementation imposes no artificial limits on the size of stored data, bounded only by available system memory and machine word size.

## API Interface

The library provides the following functions, defined in the `rstack.h` header file:

* **`rstack_t* rstack_new()`**: Creates a new empty stack. Returns a pointer to the structure or `nullptr` if memory allocation fails (setting `errno` to `ENOMEM`).
* **`void rstack_delete(rstack_t *rs)`**: Deletes the stack pointed to by `rs`. If `rs` is `nullptr`, the function does nothing. The pointer `rs` should not be used after this call.
* **`int rstack_push_value(rstack_t *rs, uint64_t value)`**: Pushes the specified `value` onto the stack `rs`. Returns `0` on success, or `-1` if the pointer is null or memory allocation fails (setting `errno` to `EINVAL` or `ENOMEM`).
* **`int rstack_push_rstack(rstack_t *rs1, rstack_t *rs2)`**: Pushes stack `rs2` onto stack `rs1`. Returns `0` on success, or `-1` on argument or memory errors.
* **`void rstack_pop(rstack_t *rs)`**: Non-recursively pops the top of the given stack. If the stack is empty or the parameter is `nullptr`, the function does nothing.
* **`bool rstack_empty(rstack_t *rs)`**: Recursively checks if the given stack contains any numbers. Returns `false` if the stack contains a number, and `true` if the stack is empty or contains no numbers.
* **`result_t rstack_front(rstack_t *rs)`**: Recursively finds the first number closest to the top of the stack. Returns a structure containing a validity flag (`flag == true` if a result was found) and the value itself.
* **`rstack_t* rstack_read(char const *path)`**: Creates a new stack based on data read from the specified file. It expects numbers in the file to be in base-10 format, separated by whitespace. Returns `nullptr` in case of errors.
* **`int rstack_write(char const *path, rstack_t *rs)`**: Writes all numbers stored on the stack to the specified file, placing each on a separate line. The output uses base-10 format without leading zeros. Writing is aborted if a cycle is detected during traversal. Returns `0` on success or `-1` on errors.

## Building and Compilation

The project includes a `Makefile` that automates the build process. The required compiler is `gcc`.

1. **Building the Dynamic Library**: 
   To compile the code into a shared object file `librstack.so`, run:
   ```bash
   make librstack.so
