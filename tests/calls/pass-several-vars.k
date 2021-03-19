proc p (val int i, val float j)                        
{
    write "result of p: ";
    write i + 5 * j;
    write "\n"
}
end                                          
                                                          
proc main ()                                              
int n;
float m;
{   
    read n;

    m := 2.5;

    p(n, m);

    write "n: ";
    write n;
    write "\n";

    write "m: ";
    write m;
    write "\n"
}
end



