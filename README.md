# A WIP compiler for a demo procedural language 'n07'

The language is called 'n07'. The compiler does the following:
- Scans the n07 source code for tokens using [Flex](https://en.wikipedia.org/wiki/Flex_(lexical_analyser_generator))
- Parses the sequence of tokens and builds an abstract syntax tree (AST) using 
  [Bison](https://en.wikipedia.org/wiki/GNU_Bison)
- Runs semantic analysis (rules checking) over the AST of the program
- Generates 'intermediate representation' three-address code (see 
  below). This code is input for a target machine 't07' (not supplied *yet*)
- Optionally pretty prints the given n07 program to stdout (using -p flag option)


### Translation example:

Contents of file "test.k"
```
proc main ()                        
int i;
{ 
    read i ;

    while i > 0 do {
        write i;
        write "\n";
        i := i - 1
    }
}
```

Running "n7c test.k" would produce this target machine t07 code:
```
    call proc_main
    halt
proc_main:
    push_stack_frame 1
# variable i is in stack slot 0
    int_const r0, 0
    store 0, r0
# block
# read
    call_builtin read_int
    store 0, r0
# while
label1:
    load r1, 0
    int_const r2, 0
    cmp_gt_int r0, r1, r2
    branch_on_false r0, label0
# block
# write
    load r0, 0
    call_builtin print_int
# write
    string_const r0, "\n"
    call_builtin print_string
# assignment
    load r1, 0
    int_const r2, 1
    sub_int r0, r1, r2
    store 0, r0
    branch_uncond label1
label0:
    pop_stack_frame 1
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

The code can be written messily (like in test_messy.k above) but provided 
that it's syntactically correct, the pretty printer will fix it.


### TODO 
Codegen support for:
- for loops (while loops are supported)
- calling other procedures

Semantic analysis:
- undeclared procedures, etc.
- type errors

General:
- supply test cases
- code clean-up
- more doco on the syntax and semantics of 'n07' ( see parser.y and scanner.l )

<!-- ![Visualisation](Dataflow-visual.PNG) -->
