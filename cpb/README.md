# cpb

> C Progress Bar

A simple header-only progress bar for C

## To use

- Put `cpb.h` in your project.
- `#include "cpb.h"`
- Use `new_pb(n, label)` to create a new bar up to `n` with a string label
  (pass NULL to not use label)
- Use `update_pb` or `inc_pb` to update the bar's value
- User `free_pb` to clean the bar up
- Open `example.c` to see this in practice

## Other

This was thrown together quickly, late at night. Should be sufficient for my
purposes, but YMMV.

PR's welcome.

