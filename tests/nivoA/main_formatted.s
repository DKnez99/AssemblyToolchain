.extern mathAdd,mathSub,mathMul,mathDiv
.extern logicNot,logicAnd,logicOr,logicXor,logicTest,logicShl,logicShr
.global my_start
.global value0,value1,value2,value3,value4,value5,value6
.section my_code
my_start:
ldr r6,$0xFEFE
ldr r0,$4
int r0
ldr r0,$0xff5
not r0
str r0,value1
ldr r0,$0xF0
push r0
ldr r0,$0xFF0
push r0
call logicAnd
str r0,value2
ldr r0,$0xA0
push r0
ldr r0,$0xF23
push r0
call logicOr
str r0,value3
ldr r0,$0xA0
push r0
ldr r0,$0xF23
push r0
call logicXor
str r0,value4
ldr r0,$0
push r0
ldr r0,$1
push r0
call logicTest
jne end
ldr r0,$0x2
push r0
ldr r0,$0xF0
push r0
call logicShl
str r0,value5
ldr r0,$0x2
push r0
ldr r0,$0xF0
push r0
call logicShr
str r0,value6
ldr r0,value0
ldr r1,value1
ldr r2,value2
ldr r3,value3
ldr r4,value4
ldr r5,value5
ldr r6,value6
end:
halt
.section my_data
value0:
.word 0
value1:
.word 0
value2:
.word 0
value3:
.word 0
value4:
.word 0
value5:
.word 0
value6:
.word 0
value7:
.word 0
destinations:
.word logicNot
.word logicAnd
.word logicOr
.word logicXor
.word logicTest
.word logicShl
.word logicShr
.end