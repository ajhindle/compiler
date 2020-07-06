proc q ( val     float x, valres int    k                  )                                  
int n; float y,z;                         
{ skip; { skip; skip } }                  
end                                       
                                          
proc p (ref int i)                        
                                                          
i := 6*i + 4 end                                          
                                                          
proc main ()                                              
int m, n;                                                 
{   read n;                                               
    while n-1 do                                      
    {    m := n;                                      
         while m do                                   
             if m then                                
             {   n := n-1;                            
                 m := m-1;                    
                 if m then skip else p(n) 
             }                            
             else { m := n-m; m := m-1 }
    }
}
end



