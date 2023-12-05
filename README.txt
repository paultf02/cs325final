University of Warwick CS325 Compiler Design Coursework Term 1 2023-24
Written by u2020216

EVERY USER NEEDS to update pathprefix in mccomp.cpp line 64 to ensure supporting files are loaded correctly.

run the following command to build:
make

run the following command to generate first and followsets from the grammar:
python3 firstfollow.py

run the following command to run all of the tests (this code passes all of provided tests and additional tests written):
./tests/tests.sh

We can change a global variable from can only be defined once (first definition being the first when the source is read top to bottom)
to always mutable (but can still only be declared once).
This is done by commenting and uncommenting code in lines 573 - 595 of astnodes.cpp 