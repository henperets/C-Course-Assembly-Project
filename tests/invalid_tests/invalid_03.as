;comment
;
clr B, D
.extern G

;too many operands for "rts"
rts #7, #-2
.entry A

;not enough operands for "mov"
mov K
