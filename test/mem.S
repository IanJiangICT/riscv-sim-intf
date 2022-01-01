.section .text.init;
.align	6;
.globl _start;

_start:
	nop
	addi	x1, x0, 0xB1
	addi	x2, x1, 1
	addi	x3, x2, 1
	addi	x4, x3, 1
	addi	x5, x4, 1
	addi	x6, x5, 1
	addi	x7, x6, 1
	addi	x8, x7, 1
	addi	x9, x8, 1
	addi	x10, x9, 1
	addi	x11, x10, 1
	addi	x12, x11, 1
	addi	x13, x12, 1
	addi	x14, x13, 1
	addi	x15, x14, 1
	addi	x16, x15, 1
	addi	x17, x16, 1
	addi	x18, x17, 1
	addi	x19, x18, 1
	addi	x20, x19, 1
	addi	x21, x20, 1
	addi	x22, x21, 1
	addi	x23, x22, 1
	addi	x24, x23, 1
	addi	x25, x24, 1
	addi	x26, x25, 1
	addi	x27, x26, 1
	addi	x28, x27, 1
	addi	x29, x28, 1
	addi	x30, x29, 1
	addi	x31, x30, 1

set_mem_base:
	la	t0, val0
	la	t1, val1
	la	t2, val2

load_add_store_v0:
	ld	a0, 0(t0)
	addi	a0, a0, 0x40
	sd	a0, 0(t0)
	addi	s0, s0, 1
	addi	s0, s0, 2

	j	load_add_store_v1
	nop
	nop
	j	check_vals

load_add_store_v1:
	ld	a1, 0(t1)
	addi	a1, a1, 0x40
	sd	a1, 0(t1)
	addi	s1, s1, 1
	addi	s1, s1, 2

check_vals:
	ld	a0, 0(t0)
	ld	a1, 0(t1)
	ld	a2, 0(t2)

.section .data;
.align	6;
val0:
	.DWORD 0x0102030405060708
val1:
	.DWORD 0x1112131415161718
val2:
	.DWORD 0x2122232425262728
