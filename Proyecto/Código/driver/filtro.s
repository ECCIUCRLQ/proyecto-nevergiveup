# James Araya Rodríguez. B70528
# Lizeth Corrales Cortés. C02428
.globl filtro_red
.globl _filtro_red

.section .text
filtro_red:
_filtro_red:
    pushq %rbp
    movq %rsp, %rbp
    movq $0,%rax
    movsd (%rdi),%xmm1  # red
    movsd (%rsi),%xmm2  # blue
    movsd (%rdx),%xmm3  # green
    mulsd  %xmm0, %xmm3
    mulsd  %xmm0, %xmm2
    movss %xmm1, (%rdi)
    movss %xmm2, (%rsi)
    movss %xmm3, (%rdx)
    popq %rbp
    ret



.globl filtro_blue
.globl _filtro_blue

.section .text
filtro_blue:
_filtro_blue:
    pushq %rbp
    movq %rsp, %rbp
    movq $0,%rax
    movsd (%rdi),%xmm1  # red
    movsd (%rsi),%xmm2  # blue
    movsd (%rdx),%xmm3  # green
    mulsd  %xmm0, %xmm1
    mulsd  %xmm0, %xmm3
    movss %xmm1, (%rdi)
    movss %xmm2, (%rsi)
    movss %xmm3, (%rdx)
    popq %rbp
    ret


.globl filtro_green
.globl _filtro_green

.section .text
filtro_green:
_filtro_green:
    pushq %rbp
    movq %rsp, %rbp
    movq $0,%rax
    movsd (%rdi),%xmm1  # red
    movsd (%rsi),%xmm2  # blue
    movsd (%rdx),%xmm3  # green
    mulsd  %xmm0, %xmm1
    mulsd  %xmm0, %xmm2
    movss %xmm1, (%rdi)
    movss %xmm2, (%rsi)
    movss %xmm3, (%rdx)
    popq %rbp
    ret


.globl filtro_fucsia
.globl _filtro_fucsia

.section .text
filtro_fucsia:
_filtro_fucsia:
    pushq %rbp
    movq %rsp, %rbp
    movq $0,%rax
    movsd (%rdi),%xmm1  # red
    movsd (%rsi),%xmm2  # blue
    movq $0, (%rdx)
    movsd (%rdx),%xmm3  # green
    movss %xmm1, (%rdi)
    movss %xmm2, (%rsi)
    movss %xmm3, (%rdx)
    popq %rbp
    ret


