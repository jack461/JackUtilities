[b]Introduction[/b]

This note documents the lexical analysis implementation in [i]mSL[/i]. While the analyser was initially quite specific to the language, it has been rewriten so it could be used in other environments.

[b]What is [i]lexical analysis[/i], BTW ?[/b]

You may wonder: why should I care about that? Well, if you want to easily analyse a "configuration file" in a JSFX, rather that design it filled with numbers (something you can read with the primitive operations [i]file_var(handle,variable)[/i] or [i]file_mem(handle,offset, length)[/i] ), or as a sequence of strings that you'll need to decode character by character (with a combination of [i]file_string(handle,str)[/i] and [i]str_getchar(str, offset[, type])[/i]), or even worse, as an XML format, you can use a simple and friendly grammar that will be much more easy to understand and use.

[i]Lexical analysis[/i] is the operation that turns a character string, or a text file, into a sequence of token, labelling each one with its purpose in the analysed grammar: for a programming language, this could be "keyword", "variable name", "constant", "operator", "string", "separator", etc. For a simpler grammar, it could be "parameter name", "equal sign", "numeric value", "end of line", etc. Lexical analysis is a long known computer device, and many tools have been created to simplify it. In the "C" world, "lex" and "yacc" let you easily create, from a set of rules, a "C" program that can analyse any "simple" grammar (technically, these "simple" grammars are known as "LALR", and almost all programming languages uses for their definitions "LALR" grammars - but we don't really care here about that). The generic word for a program such as "lex" is [i]lexer[/i], and the best way to create a lexical analyser is to prepare the appropriate specification for the lexer you want to use. You will easily find thousands of references about "lexers" on the internet, and hundreds of lexers implemented in many programming languages.

However, we are working in JSFX, and, to my knowledge, there exists no lexer witten in eel2. So we have to create one. This is not such a difficult task. Many approaches can be used, the classical ones being the [i]regular expressions[/i] and the [i]finite state automata[/i]. Or even the plain programming, which, for a language like [i]lisp[/i], I would recommand. While we have a JSFX [i]match()[/i] function, that can analyses some regular expressions, it is not really appropriate to the work we have to do. We could also write a function that turns a set of regular expressions in a finite state automaton (the work that lex precisely does), but that would be overkill! So I finally decide to choose the finite state automaton approach.


[b]Finite state automata[/b]

The [i]finite state automata[/i] ([i]FSA[/i]) or [i]finite state machines[/i] ([i]FSM[/i]) are a computational model, where the machine can be in any of a finite number of [i]states[/i], and can change (this operation is called a [i]transition[/i]) from one state to another in response to some [i]input[/i]. An FSA has an initial state, and one or more final states. We will use [i]deterministic[/i] FSA (i.e. ones in which any input triggers at most one transition - knowing that any [i]non deterministic[/i] FSA can be rewritten as a [i]deterministic[/i] one).

Note that there exists a specification as a lex input for the "eel2" language, [url=https://github.com/justinfrankel/WDL/blob/main/WDL/eel2/eel2.l]eel2.l[/url]. With a little work, the regular expressions used to define the token (lines 63 to 99 of the source code) could be transformed in a [i]non deterministic[/i] FSA, and then reduced to a [i]deterministic[/i] FSA).

A nice way to describe an FSA is to represent it as an oriented [i]graph[/i], where each [i]node[/i] of the graph represents one [i]state[/i], and each [i]arc[/i] of the graph represent a [i]transition[/i], and where we label each transition with the corresponding input. 

In the case of an FSA aimed at lexical analysis, the inputs are the successive characters read from the input string or the input file. As a first example, here is an FSA that recognizes the number "641":
