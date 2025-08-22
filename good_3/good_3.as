MAIN:   mov #-1,r2          
        cmp r3,#5           
        add r6,r7           
        sub r1,r4           
        
        lea STR,r6          
        clr r2             
        not r2             
        inc r2             

           
        red r1             
        prn #-5            
        prn r1              
        
        mov M1[r2][r7],LENGTH  
        add r2,STR
