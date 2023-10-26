Colliding Dots
==============

A experiment with colliding dots with mass, size, speed, angular momentum.
Implemented both in python and c++.

The python program:

    python3 rotating_dots.py

The c++ program is build using make and cmake:

    make
    ./build/dots


Use keys to control what happpens:
 - use 1-4 to add/remove dots in various styles.
 - 1 adds dots in a different color each time.
 - use r/g/b  to select dots by color
 - use m/d to select which parameter to change
 - use +/- to change that parameter for the selected colored dots.

The c++ version also displays a histogram of speed and directions.

## Dependencies

 - the python version needs `PyQt5`
 - the c++ version needs Qt5

Author
======

Willem Hengeveld <itsme@xs4all.nl>

