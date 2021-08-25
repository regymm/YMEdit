# YMEdit - the editor

This is my editor project. It contains a simple editor, which can edit text files. No undo/redo, no code highlighting, no text selection, NO WARRANTY. 

Some part of the code is copied and modified from [the kilo editor](github.com/antirez/kilo).

## Compile

To compile the project just type `make`.

`./ymedit` is the executable. 

## Usage

Just use `./ymedit <file>`, read the welcome text, then press `e` to edit or `r` to read only. You can use the `victim.txt` to have a try. 

In editor use arrows to move, ^S to save, ^Q to quite, ^H to view help, ^Q three times to quit without save. 

## Terminal

Only the ^[[\*;\*H(move cursor) escape control code is necessary though some others are used to control colors. Partial refreshing makes this editor usable via serial port at low baud rate. 