
LABEL1: .data 62, 8  , -5   ,  77,    3
LABEL2: cmp  LABEL1, r2

   ;wrong addressing method for add
add r4  ,#3

;wrong addressing method for move
mov #7, #2

    ;wrong addressing method for not
not       #-25

.entry MAIN

;wrong addressing method for jmp
jmp #2
.entry LOOP

;wrong addressing method for lea
lea #51 , r6

;wrong addressing method for clr
   clr #-4
   
;wrong addressing method for sub
sub LABEL2   ,   #1

;wrong addressing method for inc
 inc #6

  ;wrong addressing method for dec 
           dec #8



.extern LABEL2
