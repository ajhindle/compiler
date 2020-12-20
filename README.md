# A WIP compiler for a demo procedural language

The language is called 'n07'. The compiler does the following:
- scans the n07 source code for tokens using [Flex] (https://en.wikipedia.org/wiki/Flex_(lexical_analyser_generator)
- parses the sequence of tokens and builds an abstract syntax tree (AST) using 
  [Bison] (https://en.wikipedia.org/wiki/GNU_Bison)
- (optional) pretty prints the given n07 program to stdout
- runs a semantic analysis (rules checking) over the AST of the program
- generates 'intermediate representation' abstract three-address code (see 
  below)


Example:

Contents of file "test.k"
```
proc p (int i)                        
    i := (6*i) / 4.6777 
end
```

Running "n7c test.k" would produce:

```
proc_p:
    push_stack_frame 1
# argument i is in stack slot 0
    store 0 r0
# assignment
    int_const r3 6
    load r4 2
    mul_int r1 r3 r4
    real_const r5 4.67770
    div_real r0 r1 r5
    store 1 r0
    pop_stack_frame 1
    return
```


<!-- ![Visualisation](Dataflow-visual.PNG) -->
