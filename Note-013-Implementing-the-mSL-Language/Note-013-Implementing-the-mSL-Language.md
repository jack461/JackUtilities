[b]Introduction[/b]

This part will focus on the general guidelines concerning the implementation of the [i]mSL[/i] language.
These guidelines have been largely influenced by:
[list=1][*]The JSFX language itself, as a "model".
[*]The JSFX language, as the implementation language.
[*]And of course, what the author wanted to obtain, and what he knew about the implementation of programming languages.[/list]
It happens that JSFX has very little to help such an implementation. For example, there is no memory management, or no tools to help in compiler design (contrary to "C", which provides lex and yacc, or flex and bison, or many other languages that have similar libraries). Also, there is no possibility that I know to implement in JSFX a true compiler and a code loader (although all needed tools exist in the implementation of JSFX, these are not accessible). Therefore, the ambitions have been somewhat reduced, and the decision was made to build a rather simple system, using a stack based bytecode interpreter.

The various steps have been the following:
[list][*]Build, as initial "bricks", a set of utililities, that was, a priori, supposed to be useful for the project.
[*]Choices in the representations of the various structures used.
[*]Design of the lexical analysis, based on a deterministic finite state automaton.
[*]Syntactic tree construction, based on a set of rewriting rules.
[*]Byte code generation, using the classical list/stack/list approach (don't know if there is a specific name for that).
[*]Byte code execution, using a stack machine.
[*]Process handling, using a cooperative/preemptive scheduler.
[/list]

[b]Utilities[/b]

Such a project needs a lot of "utilities" procedures. Some of them have already been described: [url=https://forum.cockos.com/showthread.php?p=2601042#post2601042]static[/url] and [url=https://forum.cockos.com/showthread.php?p=2607201#post2607201]dynamic[/url] memory management, [url=https://forum.cockos.com/showthread.php?p=2597398#post2597398]strings[/url] and [url=https://forum.cockos.com/showthread.php?p=2603165#post2603165]symbols[/url] operations, and provided as downloadable external libraries. As this kind of "log" progresses (and while the cockos team tolerate me here:-), I'll try to document the most interesting and reusable components that I have developped.

Asides of the already presented utilities, one very useful component that I wrote is a "Log Box".

[b]The Log Box[/b]

The "Log Box" is a simple, practical tracing tool. It manages a log, that you can display in the graphical part of the plug-in. You enter successive lines of text, as strings, that are kept in a data structure. In the "@gfx" section, you call a function that displays the contents of the log in a [i]viewport[/i]. This specific beast is a part of the user interface [url=https://geraintluff.github.io/jsfx-ui-lib/]ui-lib.jsfx-inc[/url] for JSFX plug-ins, designed by Geraint Luff (see the [url=https://geraintluff.github.io/jsfx-ui-lib/doc/html/]online manual[/url]). With this library, you can create in the JSFX graphic area different viewports that you can populate with various widgets. The "Log Box" is one of them.

You use the "Log Box" by importing the library "mSL_ui_Log_Box.jsfx-inc", [i]after[/i] "ui-lib.jsfx-inc". It also needs the "mSL_StM_Memory.jsfx-inc" library.

First, here are the interfaces provided:
[list][*][b]LB = mSL_log_box_new(first, last)[/b]: creates, and returns, a new log box. [i]first[/i] and [i]last[/i] [u]should[/u] be the identifiers of the first and the last strings in a serie of strings consecutively declared in the source text. As an example of this:
[code]#S00="";#S01="";#S02="",#S03="";#S04="";#S05="";#S06="";#S07="";#S08="";#S09="";
#S10="";#S11="";#S12="",#S13="";#S14="";#S15="";#S16="";#S17="";#S18="";#S19="";[/code]is an exemple of such a serie of 20 strings consecutively declared. They can be provided to the "Log Box" constructor by [b]mSL_log_box_new(#S00,#S19)[/b]. The capacity of the log box corresponds therefore to the numbers of strings provided. You will find such sets of up to 10000 strings in this [url=https://github.com/jack461/JackUtilities/tree/main/Utilities]github Utilities folder[/url]. 

[*][b]mSL_log(logbox, string)[/b]: enter a new line in the logbox. The line, provided as a string, can be as long as necessary. The string is copied into the log box, so it can later be modified without affecting what is displayed by the log box.

[*][b]mSL_Log_Set(logbox, field, value)[/b]: modify some parameter of the logbox. The parameter is defined as a [i]field name[/i], in the form of a multichar character. As an example, [b]mSL_Log_Set(LB, 'cols', 3)[/b] switches the displayed text in the logbox LB to 3 columns. The various possible fields are:[list]
[*][b]'cols'[/b], [i]value[/i] should be an integer, between 1 and 24, that determines the number of [i]columns[/i] used to display the text (default: 1). The returned value is the previous number of columns. Passing 0 as value returns the current number of columns, without changing it.
[*][b]'wdtm'[/b], [i]value[/i] should be an integer, between 50 and 2000, that determines the [i]minimum[/i] width, in pixels, of a column. According to the width of the viewport, some columns may be suppressed to enforce this minimum width (default: 250). The returned value is the previous minimum width. Passing 0 as value returns the current minimum width, without changing it.
[*][b]'up'[/b] or [b]'down'[/b], [i]value[/i] should be a positive integer, which is a number of lines; this moves up or down the displayed text, of the number of lines specified.
[*][b]'left'[/b] ([i]value unused[/i]): display the whole text, starting at the first line. This cancels the effect of previous 'up' and 'down' commands.
[*][b]'rght'[/b] ([i]value unused[/i]): display only the last line of the text, the rest of the screen being blank. Nothing is erased, 'down' or 'left' can be used to display the previous text.
[*][b]'clir'[/b] ([i]value unused[/i]): [u]erase[/u] all the lines in the logbox. Any previously entered text is definitely lost. This, like all the other commands, can't be undone.
[*][b]'line'[/b], [i]value[/i]: value is the number of a line in the text; returns the string corresponding to the corresponding line. 0 is the first one. If value is negative, lines refer to the end of the text: -1 is the last entered line, -2 the previous one, etc. The resut is a string reference, so it can be used, for example, as a parameter of strcpy to retrieve or change the contents of the line actually displayed in the log box.
[*][b]'size'[/b], [i]value[/i]: value, between 6 and 180, sets the [i]size[/i] of the characters of the displayed text in the log box. A value of 0 doesn't change this size, but returns the current value. A value of 1 increases of 1 point the size of the characters, a value of -1 decreases of 1 point this size.
[*][b]'font'[/b], [i]value[/i]: if 0, this returns a string that contains the [i]name[/i] of the font used to display the text in the logbox (default: "Arial"). If value is a string, this defines the new front used to display the text. On a Mac, strings like "Arial", "Courier", "Monaco", "Times", "Verdana", etc. are legit values.
[*][b]'padd'[/b], [i]value[/i]: change the space padding around the colums of the log box. Values should be between 1 and 12 pixels.
[*][b]'flgs'[/b], [i]value[/i]: changes some options of the log box. Flags can be an additive combinations of the following values:
[list][*]1: draw a frame around each column.
[*]2: padd (width given by the 'padd' option) the outer part of the frame.
[*]4: padd (width given by the 'padd' option) the inner part of the frame.
[*]8: display the name of the log box as the first line
[*]16: display in reverse order, the first line of the column being the last line entered.
[/list]
[*][b]'name'[/b], [i]value[/i]: if 0, this returns a string that contains the [i]name[/i] assigned to the logbox (default: "Log Box"). If value is a string, this defines the new name of the logbox.
[/list]

[*][b]mSL_log_Disp(logbox, flags)[/b]: this function, to be called only in the [b]@gfx[/b] section, is used to display the contents of the "logbox" in the current viewport, taking in account all the parameters that can be set with the mSL_Log_Set function. "flags" can be 0 or 1. If 1, the procedure also manages some basic interactions with commands entered as keystrokes by the user. The following commands are recognized:
[list][*]a digit (1 to 9, 0) selects 1 to 10 columns, a digit with (Mac: ctrl, Windows: Windows key) selects 11 to 20 columns. 
[*]'up' or 'down', move up or down the displayed text, of 1 line; with shift or command: 10 lines, with shift and command, 100 lines.
[*]'left': display the whole text, starting at the first line. 
[*]'right': display, starting at the last line of the text.
[*]ctrl+cr: enter a blank line in the text
[*]ctrl+'+' and ctrl+'-': increase or decrease the font size.
[*]ctrl+'=': switch between up-down and down-up display mode.
[*]ctrl+shift+backspace: [u]erase[/u] all the lines in the logbox.
[/list]
[/list]

[b]Using the log box[/b]

In the REAPER Folder (the one you get by the menu "Options > Show REAPER resource path in explorer/finder..."), locate the subfolder "Effects". In this subfolder, create, if you haven't already done that, a new folder, that you will name "JackUtilities". In here, you can copy the files that are in my [url=https://github.com/jack461/JackUtilities/tree/main/Utilities]github Utilities folder[/url].

An example code, "JJ-JSFX-TestBed-01.jsfx" is in my [url=https://github.com/jack461/JackUtilities/tree/main/Note-13-Implementing-the-mSL-Language]JackUtilities github Note-013[/url]. To use that, you will need most of my utilities libaries, AND the [url=https://geraintluff.github.io/jsfx-ui-lib/]ui-lib.jsfx-inc[/url] of Geraint Luff. Just put in on a track.

This plug-in just displays some events that it receives: mouse clics, keystrokes, MIDI in, sliders actions and drag and drop of filenames.

[b]Fifty ways to use your logbox[/b]

Actally, just a few notes about how useful the log box can be.
[list=1][*]You may build, and use, as many log boxes as you want, and in the @gfx part, you may choose which ones you display, where, and how. Log boxes are resizable, so they can display a lot of text, or very little...
[*]L=mSL_log(logbox, string) returns the identification of the string of the logbox where the second parameter has been copied. You can later modify this entry if useful by copying something else in L.
[*]An empty line (i.e. the string "") is not displayed in the log. You can "clear" a line, by selecting it and setting it to the empty string (with [b]L=mSL_Log_Set(logbox, 'line', number); strcpy(L, "");[/b]
[*]The default commands (described in the "mSL_log_Disp" entry) are recognized only when the mouse pointer is inside the log box. So, in the "JJ-JSFX-TestBed-01.jsfx", all input characters are displayed when the mouse pointer is inside the plug-in window, but not in the log box viewport.
[*]You can create a logbox with only a few lines, that can be used to display the status of some variables or other informations. Here is a code example:[code]// in the @init, to memorize the first three enties:
l1 = mSL_log(logbox, " "); l2 = mSL_log(logbox, " "); l2 = mSL_log(logbox, " "); 
// in the @block
sprintf(l1, "A is now: %d", A); sprintf(l2, "B is %f", B); etc.
etc.[/code]
[*]Using a large font size, you can use a logbox to display an information visible from a large distance, like a clock, a timer, a beat counter, etc.
[/list]
The logbox could have further developments. Let me know about bugs, change requests, etc.



All the best !

J. Jack.

Get text and code in my [url=https://github.com/jack461/JackUtilities/tree/main/Note-013-Implementing-the-mSL-Language]JackUtilities github Note-013[/url].

[b]TL;DR summary:[/b] mainly about the "log Box", a widget for the "ui-lib".

[i]Back to the [url=https://forum.cockos.com/showthread.php?p=2597175#post2597175]Table of contents[/url].[/i]
