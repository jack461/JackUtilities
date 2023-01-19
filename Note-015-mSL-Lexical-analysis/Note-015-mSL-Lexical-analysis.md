[b]Introduction[/b]

This note documents the lexical analysis implementation in [i]mSL[/i]. While the analyser was initially quite specific to the language, it has been rewriten so it could be used in other environments.

[b]What is [i]lexical analysis[/i], BTW ?[/b]

You may wonder: why should I care about that? Well, if you want to easily analyse a "configuration file" in a JSFX, rather that design it filled with numbers (something you can read with the primitive operations [i]file_var(handle,variable)[/i] or [i]file_mem(handle,offset, length)[/i] ), or as a sequence of strings that you'll need to decode character by character (with a combination of [i]file_string(handle,str)[/i] and [i]str_getchar(str, offset[, type])[/i]), or even worse, as an XML format, you can use a simple and friendly grammar that will be much more easy to understand and use.

[i]Lexical analysis[/i] is the operation that turns a character string, or a text file, into a sequence of tokens, labelling each one with its purpose in the analysed grammar: for a programming language, this could be "keyword", "variable name", "constant", "operator", "string", "separator", etc. For a simpler grammar, it could be "parameter name", "equal sign", "numeric value", "end of line", etc. Lexical analysis is a long known computer device, and many tools have been created to simplify it. In the "C" world, "lex" and "yacc" let you easily create, from a set of rules, a "C" program that can analyse any "simple" grammar (technically, these "simple" grammars are known as "LALR", and almost all programming languages uses for their definitions "LALR" grammars - but we don't really care here about that). The generic word for a program such as "lex" is [i]lexer[/i], and the best way to create a lexical analyser is to prepare the appropriate specification for the lexer you want to use. You will easily find thousands of references about "lexers" on the internet, and hundreds of lexers implemented in many programming languages.

However, we are working in JSFX, and, to my knowledge, there exists no lexer witten in eel2. So we have to create one. This is not such a difficult task. Many approaches can be used, the classical ones being the [i]regular expressions[/i] and the [i]finite state automata[/i]. Or even the plain programming, which, for a language like [i]lisp[/i], I would recommand. While we have a JSFX [i]match()[/i] function, that can analyses some regular expressions, it is not really appropriate to the work we have to do. We could also write a function that turns a set of regular expressions in a finite state automaton (the work that lex precisely does), but that would be overkill! So I finally decide to choose the finite state automaton approach.


[b]Finite state automata[/b]

The [i]finite state automata[/i] ([i]FSA[/i]) or [i]finite state machines[/i] ([i]FSM[/i]) are a computational model, where the machine can be in any of a finite number of [i]states[/i], and can change (this operation is called a [i]transition[/i]) from one state to another in response to some [i]input[/i]. An FSA has an initial state, and one or more final states. We will use [i]deterministic[/i] FSA (i.e. ones in which any input triggers at most one transition - knowing that any [i]non deterministic[/i] FSA can be rewritten as a [i]deterministic[/i] one).

Note that there exists a specification as a lex input for the "eel2" language, [url=https://github.com/justinfrankel/WDL/blob/main/WDL/eel2/eel2.l]eel2.l[/url]. With a little work, the regular expressions used to define the tokens (lines 63 to 99 of the source code) could be transformed in a [i]non deterministic[/i] FSA, and then reduced to a [i]deterministic[/i] FSA).

A nice way to describe an FSA is to represent it as an oriented [i]graph[/i], where each [i]node[/i] of the graph represents one [i]state[/i], and each [i]arc[/i] of the graph represent a [i]transition[/i], and where we label each transition with the corresponding input. 

In the case of an FSA aimed at lexical analysis, the inputs are the successive characters read from the input string or the input file. As a first example, here is an FSA that recognizes the number "641" [url=https://github.com/jack461/JackUtilities/blob/main/Note-14-mSL-Lexical-analysis/graphe-2.png]graph 2[/url] 
[i]Note: sorry for linking the image - it seems that the expression:
[center] [noparse] [img]https://github.com/jack461/JackUtilities/blob/main/Note-14-mSL-Lexical-analysis/graphe-2.png[/img] [/noparse] [/center]
is not working like I expected.[/i]
Circles represent the different states. They have been numbered, starting at 1. The arrows represent the transitions. The arcs of the graph have been decorated with the input triggering the transition. The initial state, 1, is marked by an incoming double arrow, while the final states (there is only one in this example, the state 4) are marked with an outcoming double arrow. The way this FSA works is simple. We start in state 1. If the input is the character '6', we skip to state 2. Otherwise, we signal an error. While in state 2, we expect a '4' to go to step 3, otherwise it is an error. There, we expect a '1' to skip to state 4, otherwise, etc. And the state 4 is a final state, with no other transition than an exit. When we hare here, we have recognized the number "641".

Let's show an other example. In JSFX, the '/' character, used alone, represents the divide operation. Followed by an '=' character, it corresponds to the "divide and set" operation. It can also be followed by another '/', or by a '*', to introduce the two syntaxes for comment operations. In this last case, we have to discard all the input chars, up to the end of the comment. [url=https://github.com/jack461/JackUtilities/blob/main/Note-14-mSL-Lexical-analysis/graphe-3.png]Graph 3[/url] is the representation of the corresponding FSA.

We see that there may be more than one transition issued from a state. In state "S2", these transitions are triggered repectively by '=', '/', or '*'. If the input is not one of these characters, then "S2" acts as a final state -- and we have recognized the [i]divide[/i] operator. When, in state 2, '=' is found, we have recognized the [i]divide and set[/i] operation, and the transition leads us to state 0, which is a final state. A sequence of two '/' leads us to state 3, where the task is to discard all the input characters, up to the end of line. If we decide that the end of line is represented either by the "CR" or the "LF" character (noted '\r' and '\n'), then we go to the "S1" state. Otherwise, any input character (the transition is labelled "any") brings us back to state 3. When the end-of-line has been detected, we have recognized a comment, and we could leave the automaton. However, we are just interested in eliminating comments, and discover the next token. Hence, we go back to "S1", the initial state, to continue analysis of the input stream. Note that we should have represented two differents transitions from "S3" to "S1", but we simplified the graph, putting two labels for the same arc. 
The other states, "S4" and "S5" are used to eliminate the "/*" .. "*/" comments. Understanding why it is done this way is left as an exercise to the reader :-). 

Let's show a last example, the FSA that recognizes a [url=https://github.com/jack461/JackUtilities/blob/main/Note-14-mSL-Lexical-analysis/graphe-4.png]numeric constant[/url]. There again, we have a unique initial state, "S1", various final states, "S2", "S3", "S6", "S7", "S9" and "S11". We labelled the arcs of the graph with notations like 'e'/'E', meaning either of the character 'e' or 'E', or '0'..'9', 'A'..'F', meaning any character from the set ('0', '1', '2', etc. up to '9'), or ('A', 'B', 'C', etc. up to 'F'). Finally, there is an "empty" transition between "S4" and "S5", noted by a little circle on the arc, that means: if the input is not a '+' or a '-', skip to state "S5". This FSA recognizes constants like "12.5", "1E10", "3.7245e-18", and even "0E303", this last one being not so much useful. And of course, we allow notations such as "0x3F" or "0B1101101".

Of course, the FSA does only a part of the real work. In this last example, it [i]recognizes[/i] a number, but doesn't [i]compute[/i] its value. For this, we have to associate semantic actions to some transitions. We will see that this is most of the times quite simple.

[b]Implementing the analyzer[/b]

It is rather easy to transform an FSA into a program. However, we have some additionnal things to do:
[list][*]provide a char reader, that delivers successive characters from the input stream.
[*]implement the semantics associated to each transition.
[*]build a way to stock the generated tokens.
[/list]

[i]The char reader[/i]

Here is an example of a char reader. It uses an opened file (whose handle is in the global variable "mSL_C_fH"), reads lines out of it, and provide one character at each call, or "-1" at end-of-file. It also keeps, in "mSL_C_chPos" and "mSL_C_numLine" the position of the character in the line, and the number of the line in the file. 
[code]function mSL_getchar()
(
    while (mSL_C_cavail == 0)  (
        // read a new line
        ((mSL_C_fH >= 0) && file_avail(mSL_C_fH)) ? (
            mSL_C_cavail = file_string(mSL_C_fH, #mSL_cline);
            mSL_C_numLine += 1;
            mSL_C_chPos = 0;
        ) : (
            (mSL_C_fH >= 0) ? (file_close(mSL_C_fH); mSL_C_fH = -1;);
            mSL_C_cavail = -1;
        );
    );
    (mSL_C_cavail > 0) ? (
        mSL_C_chcurr = str_getchar(#mSL_cline, mSL_C_chPos, 'cu');
        mSL_C_chPos += 1; mSL_C_cavail -= 1; 
    ) : (
        mSL_C_chcurr = -1;
    );
    mSL_C_chcurr;
);[/code]

Note that [i]mSL_getchar()[/i] can be called either after the sequence:
[code]// fname is the file to compile
mSL_C_fH = file_open(fname);
mSL_C_cavail = (mSL_C_fH >= 0) ? 0 : -1;
mSL_C_chcurr = 0; mSL_C_numLine = 0; mSL_C_chPos = 0;[/code]which opens the file "fname", or after:
[code]// srcstr is the code to compile
strcpy(#mSL_cline, srcstr);
mSL_C_cavail = strlen(#mSL_cline); mSL_C_fH = -1;
mSL_C_chcurr = 0; mSL_C_numLine = 1; mSL_C_chPos = 0; [/code]which lets you compile from a string.

[i]The tokens management[/i]

Keeping the generated tokens is more complex. We have first to choose their representation. Tokens that we have to recognize are the results of lexical analysis, and are instances of what is called "terminal symbols" in the grammar world (or "G" world). They are "typed" objects in the acceptation of the grammar of the language, types being "numeric constant", "identifier", "string", "operator", "separator", etc., and they have an associated value, as in "number:175.33", "identifier:truc", "string:'hello world'", etc. Tokens can also represent "non-terminal symbols", i.e. results of applying rules of the grammar to sequences of tokens.

Tokens are initially linked together, in the order of their apparitions in the source text. To this list, we add a first element, the 'HEAD', and a last element, the 'TAIL', that will be useful in applying the reducing rules. These later operations will transform this structure into a tree, with specific tokens ("non-terminals") being the roots of the subtree. In our implantation, the representation of a token uses six words:
[list][*]the token type,
[*]the token value,
[*]the token position in the source code,
[*]the 'next' pointer (horizontal right chaining),
[*]the 'previous' pointer (horizontal left chaining),
[*]the 'down' pointer (vertical chaining), when the token is the root of a subtree.[/list]
Each block of six words could be obtained from the dynamic memory. However, while fast, this operation implies a lot of overhead. We therefore use a solution from the "L" world (this "L", in this context, is for "Lisp"), and create a specific fast allocator for cells of size 6. Basically, we use a "main page", of size 4096, that can contains up to 4096 pointers to memory blocks, and "secondary pages" of size 6144 (i.e. 6 x 1024), which can represent up to 1024 tokens. Starting with the main page and one secondary page, we already can represent 1024 tokens. By adding new secondary page, we can reach 4M tokens, which can represent mSL files with millions of lines. All these cells are chained to constitute a free list, from which we can extrace a cell when we need one. When the free list is empty, we allocate a new 6144 words block, which gives us 1024 new cells. When a cell is note used anymore, we simply add it to the free list. All these operations are extremely fast, and do not impart the performance of the compiler.

[i]The analyser[/i]

Now that we know where we come from, the char reader, and where we go to, the token manager, we have to design the way the tokens are constructed. We do that with very simple JSFX instructions sequences, associated to each transition in our FSA. Typical actions are:
[list][*]keep the current input (a char). In the next analysis step, after a state change, the same input is used.
[*]discard the current input. In the next step, a new input char will be resuested from the char reader.
[*]enter the current imput in a string, then discard it.
[*]compute a number, using the current input.
[/list]
A first approach is to write an ad-hoc analyser, directly programmed as a JSFX function.
