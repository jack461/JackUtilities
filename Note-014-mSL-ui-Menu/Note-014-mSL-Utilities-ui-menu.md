[b]Introduction[/b]

As a part of a few widgets I introduced in ui-lib, the pop-up menu has been quite useful in offering a large set of interactions possibilities.
This software merely uses the native gfx menu function, [b]gfx_showmenu[/b], with a few lines of code around it to make it more "user friendly".

[b]About gfx_showmenu[/b]

This function accepts a single parameter, which is a [i]string[/i] describing the pop-up menu you want to display. This string is a sequence of substrings, separated by a single [b]'|'[/b] character. Each of these substrings can start with one or more characters from the set "!#><", to indicate a ticked item [b]'√'[/b], an inactivated item, the start/title of a sub-menu, or the end of a sub menu. Also, an empty substring indicates a separator in the displayed menu. In these various types of items, some can't be choosed: the "title" of a sub-menu, and a seperator. As an exemple, in the following menu description string:
[code]"ss1-ch1|ss2-ch2||#ss4-ch3|>ss5---|ss6-ch4||ss8-ch5|<ss9-ch6|ss10-ch7"[/code]
I choose here notations like "ss8-ch5" to indicates that this entry is the 8th substring in the menu description (entries 3 and 7 are empty, entry 5 is a submenu head, but not a choice), but only the 5th possible choice. Note that "#ss4-ch3" can't be chosen because it is greyed, but is still considered a choice.

The result of a [b]gfx_showmenu[/b] call is the number of the [i]choice[/i] made by the user (here, 0 for no choice, or 1 to 7), but not the number of the corresponding [i]substring[/i], which makes things a little confusing.

[b]Concepts[/b]

As I was thinking about what I would do with pop-up menus, I found I will probably find the use of three kinds of interactions:[list][*]Action menus: any choice corresponds to a specific action, that is typically immediately executed. No items are ever checked.
[*]Choice menus: a single item can be chosen from a list, like "which printer are you going to use". The last chosen item is the only one checked.
[*]Option menus: each item correspond to a specific option, and any of them can be checked or unchecked by choosing the corresponding item.
[/list]

The proposed widget is to be used within the [url=https://geraintluff.github.io/jsfx-ui-lib/]ui-lib[/url] framework designed by Geraint Luff (see the [url=https://geraintluff.github.io/jsfx-ui-lib/doc/html/]online manual[/url]). As such, the pop-up menu is active when executing the code corresponding to the viewport in which the operation to show the menu is called, this viewport being possibly as large as the whole plug-in window, or as small as a square of 2 by 2 pixels.  

Then, a menu is to be associated with a specific mouse click (left, right, or middle), and a specific combination of modifiers keys (cmd/ctrl, shift, opt/alt, ctrl/wind). Also, you can choose to display (or not) the title of the menu as a static text in the current viewport. Clicking on the text reveals the pop-menu itself. With these combinations of mouse buttons and modifiers keys, you may have many different pop-up menus available in any point of the screen.

The result of the interaction is the [i]choice[/i] made by the user, and, according to the menu type, the function takes care of decorating the menu with the appropriate tick marks. All in all, the whole thing is rather comprehensive, and provides the user with a clean and friendly way to interact with its plugin.

Creation of the pop-up menu is made with the function [b]menu = make_jmenu(type, mName, mItems)[/b], where:[list][*][b]type[/b] is the menu type, and is a combination (by "+" or "|") of these various constants:[list=a]
[*]ui_jmenu_typ_action: the menu is of type "action"
[*]ui_jmenu_typ_sel: the menu is of type "selection"
[*]ui_jmenu_typ_opt: the menu is of type "option"
[*]ui_jmenu_opt_notitle: do not display the menu title
[*]ui_jmenu_opt_rect: draw a rectangular border around the menu title
[*]ui_jmenu_opt_rndrect: draw a rounded rectangle around the menu title
[*]ui_jmenu_opt_usesel: use last selected item as menu title
[*]ui_jmenu_opt_montop: display the pop-up menu on top of the title, rather than just under it
[*]ui_jmenu_opt_dynalloc: build the menu structure by a dynamic allocation of memory, rather than a static one.
[*]ui_jmenu_opt_lclick: react on left click, no modifiers
[*]ui_jmenu_opt_alclick: react on "alt" + left click
[*]ui_jmenu_opt_kmlclick: react on "cmd" + left click
[*]ui_jmenu_opt_ctlclick: react on "ctrl" + left click
[*]ui_jmenu_opt_rclick: react on right click, no modifiers
[*]ui_jmenu_opt_arclick: react on "alt" + right click
[*]ui_jmenu_opt_kmrclick: react on "cmd" + right click
[*]ui_jmenu_opt_ctrclick: react on "ctrl" + right click
[*]ui_jmenu_opt_cclick: react on center click, no modifiers
[*]ui_jmenu_opt_kmcclick: react on "cmd" + center click
[/list]
In this list, options "a", "b" and "c" are mutually exclusive, and so are options "j" to "s".
[*][b]mName[/b] is a string, the menu title
[*][b]mItems[/b] is a string, the menu contents, as explained in the [i]gfx_showmenu[/i] paragraphe. Il is however possible in this string to add spaces (blanks, tabulations, CR, LF...) around the "|" delimiter, to help/simplify writing the string.
[/list] 
The result of the function is an array describing the menu.

The function [b]menu = make_jmenu(type, mName, cItems, aItems)[/b] is quite similar. This time, you provide in [b]aItems[/b] an [i]array[/i] of strings, each one intended to be an item of the menu, and in [b]cItems[/b] the number of elements of this array.

The interaction function, to be used in the [b]@gfx[/b] section, is [b]ent = control_jmenu(menu)[/b]. The parameter is a menu, such as returned by [b]make_jmenu[/b], and the function manages the interaction when the appropriate combination of mouse button and modifier keys is used. The result [b]ent[/b] is the choice number, as in a call of [b]gfx_showmenu[/b]. This result is also available in [b]menu[ui_jmenu_res][/b].

The function [b]tick = ui_menu_tick(menu, ent)[/b] indicates if choice [b]ent[/b] of the menu is ticked, or not (where [b]ent[/b] is the result of [b]ent = control_jmenu(menu)[/b]).

The function [b]ui_menu_tick(menu, ent, tick)[/b] lets you set (for tick == 1), reset (for tick == 0) or invert (for tick == -1) the tick mark of choice [b]ent[/b]

If you need more information about the result, on any possible choice, use [b]sub = ui_menu_ent(menu, ent)[/b]. [b]sub[/b] is the number of the [i]menu substring[/i] corresponding to the choice [b]ent[/b], and, as described in the [b]About gfx_showmenu[/b] section, can be different from the choice itself. The function result is also made available in [b]menu[ui_jmenu_entitm][/b], and the corresponding substring in [b]menu[ui_jmenu_entstr][/b]. This substring is the [i]internal representation[/i] of the entry, and starts with 5 specific chars. If [b]s == menu[ui_jmenu_entitm][/b], then:[list]
[*]str_getchar(s, 0, 'cu') is '*' if the item is a choice, '-' otherwise.
[*]str_getchar(s, 1, 'cu') is '!' if the item is ticked, '-' otherwise.
[*]str_getchar(s, 2, 'cu') is '#' if the item is disabled, '-' otherwise.
[*]str_getchar(s, 3, 'cu') is '>' if the item is the title of a submenu, '-' otherwise.
[*]str_getchar(s, 4, 'cu') is '<' if the item is the last item in a submenu, '-' otherwise.
[/list]You can use this string to modify some options of the menu. For example, use str_setchar(s, 1, '!') to add a tick to the entry, or str_setchar(s, 2, '#') to disable it.

The code [i]JJ-JSFX-TestBed-02.jsfx[/i] contains some examples of the use of these menus, with traces displayed using the [url=https://forum.cockos.com/showthread.php?p=2611657#post2611657]"Log Box" widget for ui-lib[/url], already presented in this thread. In this code, a first menu has a button displaying its name, "My Title", and is activated by a mouse "left click" on the button. A second menu is associated to the whole area of the log, and is activated by a "right click". The third menu is also associated to the whole area of the log, and is activated by a "ctrl+right click".

All the best !

J. Jack.

Get text and code in my [url=https://github.com/jack461/JackUtilities/tree/main/Note-014-mSL-ui-Menu]JackUtilities github Note-014[/url].

[b]TL;DR summary:[/b] pop-up menus, a widget for ui-lib.

[i]Back to the [url=https://forum.cockos.com/showthread.php?p=2597175#post2597175]Table of contents[/url].[/i]
