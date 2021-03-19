proc main()
int z;
{   
    z := 3;
    p(z,z);
    write z
}
end

proc p(ref int x, valres int y)
{ 
    x := 4;
    y := y+x
}
end
