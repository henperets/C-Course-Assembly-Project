; file ps.as

sub r5, r4
.extern K

      macr first
        inc r2
        mov r5,W
      endmacr
lea STR, r2
LOOP: prn #22
.extern W
inc r6
      macr second
        sub r4,r6
        inc r2
      endmacr
mov r1, W

first
MAIN: add r2, LIST

second

cmp value, #-12
dec K

.entry MAIN
STR: .string "shalom"

.data -100
LIST: .data 44, -32

.extern value