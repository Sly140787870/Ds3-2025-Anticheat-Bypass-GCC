.macro loadnjmp sym
.global \sym\()_
.lcomm \sym\()_,8
.section .text
.global \sym
\sym:
call LoadDllStub
jmp *\sym\()_(%rip)
.endm

.section .text
LoadDllStub:
pushq %r9;pushq %r8;pushq %rdx;pushq %rcx
subq $0x30,%rsp
call LoadGenuineDll
addq $0x30,%rsp
popq %rcx;popq %rdx;popq %r8;popq %r9;ret

loadnjmp DirectInput8Create
loadnjmp DllCanUnloadNow
loadnjmp DllGetClassObject
loadnjmp DllRegisterServer
loadnjmp DllUnregisterServer
loadnjmp GetdfDIJoystick
