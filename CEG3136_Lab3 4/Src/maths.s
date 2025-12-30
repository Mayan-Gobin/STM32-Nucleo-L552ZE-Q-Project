// Math functions
.syntax unified
.cpu cortex-m33
.thumb
.section .text
// Increment
// uint32_t Increment(uint32_t num);
.global Increment
.type Increment, %function
Increment:
	add r0, r0, #1
	bx lr
// Decrement
// uint32_t Decrement(uint32_t num);
.global Decrement
.type Increment, %function
Decrement:
	sub r0, r0, #1
	bx lr
// Factorial
.global Factorial
.type Factorial, %function
Factorial:
	mov r1, #1
	cmp r0, #2
	BLT endf
loop:
	mul r1, r0, r1
	subs r0, r0, #1
	bne loop
endf:
	mov r0, r1
	bx lr
// Fibonacci
// Uses recursion
// uint32_t Fibonacci(uint32_t n);
.global Fibonacci
.type Fibonacci, %function
Fibonacci:
	cmp r0, #0
	beq fib_base0
	cmp r0, #1
	beq fib_base1
	push {r4, r5, lr}
	mov r4, r0
	sub r0, r0, #1
	bl Fibonacci
	mov r5, r0
	mov r0, r4
	sub r0, r0, #2
	bl Fibonacci
	add r0, r0, r5
	pop {r4, r5, lr}
	bx lr
fib_base0:
	mov r0, #0
	bx lr
fib_base1:
	mov r0, #1
	bx lr
// Average
// uint32_t Average(uint32_t *array, uint32_t count);
.global Average
.type Average, %function
Average:
	cmp r1, #0
	beq avg_zero
	push {r4, r5, r6, r7, lr}
	mov r4, r0
	mov r7, r1
	mov r5, r1
	mov r6, #0
avg_loop:
	ldr r0, [r4]
	mov r2, #10
	mul r0, r0, r2
	add r6, r6, r0
	add r4, r4, #4
	subs r5, r5, #1
	bne avg_loop
	mov r0, r6
	mov r1, r7
	mov r2, #0
avg_div:
	cmp r0, r1
	blt avg_div_done
	sub r0, r0, r1
	add r2, r2, #1
	b avg_div
avg_div_done:
	mov r0, r2
	pop {r4, r5, r6, r7, lr}
	bx lr
avg_zero:
	mov r0, #0
	bx lr
// GCD
// uint32_t GCD(uint32_t a, uint32_t b);
.global GCD
.type GCD, %function
GCD:
	cmp r1, #0
	beq gcd_done
	push {r4, lr}
gcd_loop:
	cmp r1, #0
	beq gcd_end
	mov r4, r0
	mov r2, r1
mod_loop:
	cmp r4, r2
	blt mod_done
	sub r4, r4, r2
	b mod_loop
mod_done:
	mov r0, r1
	mov r1, r4
	b gcd_loop
gcd_end:
	pop {r4, lr}
	bx lr
gcd_done:
	bx lr
// Sort
// uint32_t Sort(uint32_t *array, uint32_t count);
.global Sort
.type Sort, %function
Sort:
	cmp r1, #1
	ble sort_done
	push {r4, r5, r6, r7, lr}
	mov r4, r0
	mov r5, r1
	mov r6, r5
	sub r6, r6, #1
sort_outer:
	cmp r6, #0
	beq sort_end
	mov r7, #0
	mov r0, r4
sort_inner:
	cmp r7, r6
	bge sort_outer_done
	ldr r1, [r0]
	ldr r2, [r0, #4]
	cmp r1, r2
	ble sort_no_swap
	str r2, [r0]
	str r1, [r0, #4]
sort_no_swap:
	add r0, r0, #4
	add r7, r7, #1
	b sort_inner
sort_outer_done:
	sub r6, r6, #1
	b sort_outer
sort_end:
	pop {r4, r5, r6, r7, lr}
	mov r0, r5
	bx lr
sort_done:
	mov r0, r1
	bx lr
// FourFunction
// uint32_t FourFunction(uint32_t op, uint32_t a, uint32_t b);
.global FourFunction
.type FourFunction, %function
FourFunction:
	push {r4, r5, lr}
	mov r4, r1
	mov r5, r2
	mov r1, r0
	cmp r1, #1
	beq op_add
	cmp r1, #2
	beq op_sub
	cmp r1, #3
	beq op_mul
	cmp r1, #4
	beq op_div
	mov r0, #0
	b fourfunc_done
op_add:
	mov r0, r4
	add r0, r0, r5
	b fourfunc_done
op_sub:
	mov r0, r4
	sub r0, r0, r5
	b fourfunc_done
op_mul:
	mov r0, r4
	mul r0, r0, r5
	b fourfunc_done
op_div:
	cmp r5, #0
	beq div_zero
	mov r0, r4
	mov r1, r5
	mov r2, #0
div_loop:
	cmp r0, r1
	blt div_done
	sub r0, r0, r1
	add r2, r2, #1
	b div_loop
div_done:
	mov r0, r2
	b fourfunc_done
div_zero:
	mov r0, #0
	b fourfunc_done
fourfunc_done:
	pop {r4, r5, lr}
	bx lr
