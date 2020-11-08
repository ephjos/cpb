# cpb

> C Progress Bar

A simple header-only progress bar for C

## To use

- Put `cpb.h` in your project.
- `#include "cpb.h"`
- Use `new_pb(n)` to create a new bar up to `n`
- Use `update_pb` or `inc_pb` to update the bar's value
- User `free_pb` to clean the bar up
- Open `example.c` to see this in practice

## Other

This was thrown together quickly, late at night. Should be sufficient for my
purposes, by YMMV. PR's welcome.

