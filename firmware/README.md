# Ducklynx Firmware

## Coding conventions
 - A simple write to one register should not be put into a function. The raw register should be written.
 - A function can be created when more logic than the above is needed
 - See register.h for how register names should work

## Building
Build with cmake however you see fit. On linux I do the following from the root of the project:
 1. `cd firmware`
 2. `mkdir build`
 3. `cmake ..`
 4. `make`