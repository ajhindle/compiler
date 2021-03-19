proc main()
int z;
{   
    z := 3;
    p(z,z);
    write z
}
end

proc p(val int x, val int y)
{ 
    x := 4;
    y := y+x
}
end
