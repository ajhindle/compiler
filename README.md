# A WIP compiler for a demo procedural language 'n07'

The language is called 'n07'. The compiler does the following:
- Scans the n07 source code for tokens using [Flex](https://en.wikipedia.org/wiki/Flex_(lexical_analyser_generator))
- Parses the sequence of tokens and builds an abstract syntax tree (AST) using 
  [Bison](https://en.wikipedia.org/wiki/GNU_Bison)
- (optional) Pretty prints the given n07 program to stdout
- Runs a semantic analysis (rules checking) over the AST of the program
- Generates 'intermediate representation' abstract three-address code (see 
  below). This code is input for a target machine 't07' (not supplied *yet*).


### Translation example:

Contents of file "test.k"
```
proc main ()
    int i;
    i := 6 * 4;
    write i
end
```

Running "n7c test.k" would produce this three-address code:

```
proc_main:
    push_stack_frame 1
# variable i is in stack slot 0
    int_const r0, 0
    store 0, r0
# assignment
    int_const r1, 6
    int_const r2, 4
    mul_int r1, r2, r3
    store 0, r3
# write
    load 0, r0
    call_builtin print_int
    pop_stack_frame 1
    return
```


### Pretty printer

Contents of file "test_messy.k"

```
proc 
main (    ) int i;
i := 6 * 4; write        i end
```

Running "n7c -p test_messy.k" would prettily print this:

```
proc main ()
    int i;
    i := 6 * 4;
    write i
end
```

The code can be very messy but provided that it's syntactically correct, the 
pretty printer will fix it.


### TODO 
Codegen support for:
- if-then-else
- loops
- calls
- read
- unary operators

Semantic analysis:
- undeclared variables, procedures, etc.
- type errors

General:
- supply test cases
- code clean-up
- more doco on the syntax and semantics of 'n07' ( see parser.y and scanner.l )

<!-- ![Visualisation](Dataflow-visual.PNG) -->
