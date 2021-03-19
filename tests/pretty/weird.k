proc p (ref int i)                        
i := 6*i + 4 
end                                          
                                                          
proc main ()                                              
{   read n;                                               
    while n-1 do                                      
    {    m := n;                                      
         while m do                                   
             if m then                                
             {   n := n-1;                            
                 m := m-1;                    
                 if m then skip else skip
             }                            
             else { m := n-m; m := m-1 }
    }
}
end



