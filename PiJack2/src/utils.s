
;@ W32( address, data )
;@  write one word at the specified memory address
.global W32
W32:    str  r1, [r0]
        bx lr


;@ R32( address )
;@  read one word from the specified memory address
.global R32
R32:    ldr  r0, [r0]
        bx lr


.global quick_memcpy
quick_memcpy:
	push 	{r4-r9}
	mov	r4, r0
	mov	r5, r1

.loopb:
	ldmia	r5!, {r6-r9}
	stmia	r4!, {r6-r9}
	subs	r2, #16
	bhi	.loopb

	pop	{r4-r9}
	mov	pc, lr


;@ performs a memory barrier
;@ http://infocenter.arm.com/help/topic/com.arm.doc.ddi0360f/I1014942.html
;@
/*.global membarrier
membarrier:
    push {r3}
    mov r3, #0                      ;@ The read register Should Be Zero before the call
    mcr p15, 0, r3, C7, C6, 0       ;@ Invalidate Entire Data Cache
    mcr p15, 0, r3, c7, c10, 0      ;@ Clean Entire Data Cache
    mcr p15, 0, r3, c7, c14, 0      ;@ Clean and Invalidate Entire Data Cache
    mcr p15, 0, r3, c7, c10, 4      ;@ Data Synchronization Barrier
    mcr p15, 0, r3, c7, c10, 5      ;@ Data Memory Barrier
    bx lr*/


;@ byte2hexstr
;@  r0: input byte
;@  r1: output address (2bytes needed)
.global byte2hexstr
byte2hexstr:

    push {r0,r1,r2,r3,r4}
    add r4, r1, #2

;@    mov r2, #48     ;@ '0' character
;@    strb r2, [r1]
;@    add r1, #1
;@    mov r2, #120    ;@ 'x' character
;@    strb r2, [r1]
;@    add r1, #1

    mov r2, r0, LSR #4

3:  mov r3, #48         ;@ base ascii char '0'
    cmp r2, #9
    ble 1f
    add r3, #7

1:  add r2,r3
    strb r2,[r1]
    add r1, #1

    cmp r4,r1
    beq 1f
    and r2, r0, #0xF
    b   3b

1:
    pop {r0,r1,r2,r3,r4}
    bx lr



;@ word2hexstr
;@  r0: input word
;@  r1: output address (8bytes needed)
.global word2hexstr
word2hexstr:
    push {r0,r1,r2,lr}
    mov r2, r0

    lsr r0, #24
    and r0, #0xFF
    bl byte2hexstr
    add r1,r1,#2

    mov r0, r2
    lsr r0, #16
    and r0, #0xFF
    bl byte2hexstr
    add r1,r1,#2

    mov r0, r2
    lsr r0, #8
    and r0, #0xFF
    bl byte2hexstr
    add r1,r1,#2

    mov r0, r2
    and r0, #0xFF
    bl byte2hexstr
    add r1,r1,#2

    pop {r0,r1,r2,lr}
    bx lr



;@  r0: input char and output nibble
;@      if r0 is not in range
;@      0..9, A..F or a..f, F0 is returned.
.global hex2nibble
hex2nibble:

    push {r2}
    and r2, r0, #0xFF   ;@ take the rightmost byte
    mov r0, #0xF0

    cmp r2, #47
    ble 1f   ;@ invalid character if less than '0'
    cmp r2, #103
    bge 1f   ;@ invalid character if greater than 'g'

    cmp r2, #57
    ble isnum   ;@ char is between 0 and 9

    cmp r2, #97
    bge isaf    ;@ char is between a and f

    cmp r2, #64
    ble 1f      ;@ invalid character (between 9 and A)

    cmp r2, #70
    ble isAF    ;@ char is between A and F

isnum:
    sub r0, r2, #48
    b   1f

isAF:
    sub r0, r2, #55
    b   1f

isaf:
    sub r0, r2, #87
    b   1f

1:
    pop {r2}
    bx lr;


;@  r0: input hex 2-char string addr
;@  r0: output byte (a number >255 if conversion
;@      was not successful)
.global hex2byte
hex2byte:
    push {r1,r2,lr}

    mov r1, #0;
    mov r2, r0  ;@ r2=string address

    ldrb    r0, [r2]
    bl  hex2nibble
    add r1, r1, r0
    lsl r1, r1, #4

    ldrb    r0, [r2,#1]
    bl  hex2nibble
    add r1, r1, r0

    mov r0, r1

    pop {r1,r2,lr}
    bx lr

.globl dummy
dummy:
    bx lr

