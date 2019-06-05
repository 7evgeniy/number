global make_string
global copy_string
global normalise
global add
global sub
global cmp
global mul_str
global decimal
global approximate

extern malloc
extern memcpy

; Вспомогательные процедуры для обращения со строками 64-разрядных цифр,
; представляющих целые числа.

make_string:
; Создать строку указанного размера и заполнить нулями.
; Формат строки:
; (0) — вместимость строки: количество байт в отсеке цифр;
; (1) — размер строки: количество цифр, участвующих в вычислениях;
; (2 — ∞) — 64-разрядные цифры в строке: хранятся, начиная с младшей.
; Учитывать надо все цифры вплоть до последней ненулевой.
; Возвращаемое значение: указатель на вместимость созданной строки.
	push r12
	add rdi, 2
	mov r12, rdi
	shl rdi, 3
	call malloc wrt ..plt
	mov r8, rax
	mov r9, r12
	.loop:
		mov qword [r8], 0
		add r8, 8
		dec r12
		jnz .loop
	sub r9, 2
	mov [rax], r9
	pop r12
	ret

copy_string:
; Создать копию строки. Аргументы:
; (1) исходная строка — указатель на вместимость её;
; Возвращаемое значение:
; указатель на вместимость созданной строки
	push rdi
	mov rdi, [rdi]
	add rdi, 2
	shl rdi, 3
	push rdi
	call malloc wrt ..plt
	pop rdx
	pop rsi
	mov rdi, rax
	call memcpy wrt ..plt
	ret

normalise:
; Вычисление размера строки и запись его в строку.
; Аргументы:
; (1) указатель на строку.
	mov rcx, [rdi]
	test rcx, rcx
	jz .end
	lea rsi, [rdi+rcx*8+16]
	.loop:
		sub rsi, 8
		cmp qword [rsi], 0
		jne .end
		dec rcx
		jz .end
		jmp .loop
	.end:
	mov [rdi+8], rcx
	ret

add:
; Записать сумму двух строк в третью строку.
; Аргументы:
; (1) указатель на первую строку;
; (2) указатель на вторую строку;
; (3) указатель на результат (нужной вместимости);
; (4) смещение влево, с которым берётся вторая строка.
; Если вместимость результата недостаточная, то поведение не определено.

	push r12
	push r13
	push r14

	mov r14, [rdi+8]
	mov r15, [rsi+8]
	lea r11, [rsi+16]
	lea r12, [rdi+r14*8+16]
	lea r13, [rsi+r15*8+16]
	lea r8, [rdi+16]
	mov r9, r11
	shl rcx, 3
	sub r9, rcx
	mov rcx, [rdx]
	lea rsi, [rdx+16]
	mov rdi, rdx
	sub r14b, r14b
	sub r10b, r10b

	.loop:
	
		sub rcx, 1
		jb .end

		sub rax, rax
		cmp r8, r12
		jae .skip1
		mov rax, [r8]
		.skip1:

		sub rdx, rdx
		cmp r9, r11
		jb .skip2
		cmp r9, r13
		jae .skip2
		mov rdx, [r9]
		.skip2:

		cmp r14b, r10b
		adc rax, rdx
		setc r10b
		mov [rsi], rax

		add r8, 8
		add r9, 8
		add rsi, 8
		jmp .loop

	.end:

	call normalise

	pop r14
	pop r13
	pop r12
	ret

sub:
; Записать разность двух строк в третью строку.
; Аргументы:
; (1) указатель на первую строку;
; (2) указатель на вторую строку;
; (3) указатель на результат (нужной вместимости);
; (4) смещение влево, с которым берётся вторая строка.
; Если вместимость результата недостаточная, то поведение не определено.

	push r12
	push r13

	mov r14, [rdi+8]
	mov r15, [rsi+8]
	lea r11, [rsi+16]
	lea r13, [rsi+r15*8+16]
	lea r8, [rdi+16]
	mov r9, r11
	shl rcx, 3
	sub r9, rcx
	mov rcx, [rdi+8]
	lea rsi, [rdx+16]
	mov rdi, rdx
	sub r10b, r10b
	sub r12b, r12b

	.loop:

		sub rcx, 1
		jb .end

		mov rax, [r8]

		sub rdx, rdx
		cmp r9, r11
		jb .skip2
		cmp r9, r13
		jae .skip2
		mov rdx, [r9]
		.skip2:

		cmp r12b, r10b
		sbb rax, rdx
		setc r10b
		mov [rsi], rax

		add r8, 8
		add r9, 8
		add rsi, 8
		jmp .loop

	.end:

	call normalise

	pop r13
	pop r12
	ret

cmp:
; Сравнить две строки.
; Аргументы:
; (1) указатель на первую строку;
; (2) указатель на вторую строку;
; Возвращаемое значение: 1, если больше; -1, если меньше; 0, если равны.
	sub rax, rax
	mov rcx, [rdi+8]
	cmp rcx, [rsi+8]
	ja .greater
	jb .lesser
	add rdi, 16
	add rsi, 16
	.loop:
		sub rcx, 1
		jl .end
		mov rdx, [rdi+rcx*8]
		cmp rdx, [rsi+rcx*8]
		jb .lesser
		ja .greater
		jmp .loop
	.end:
	ret
.greater:
	inc rax
	ret
.lesser:
	dec rax
	ret

mul_str:
; Записать произведение строки на цифру в другую строку.
; Аргументы:
; (1) указатель на строку;
; (2) цифра;
; (3) указатель на результат (нужной вместимости);
; Если вместимость результата недостаточная, то поведение не определено.
	push r12

	mov r9, [rdi+8]
	sub rcx, rcx
	add rdi, 16
	lea r10, [rdx+16]
	sub rdx, rdx
	sub r12b, r12b
	sub r11b, r11b
	.loop:
		cmp rcx, r9
		je .end
		mov r8, rdx
		mov rax, [rdi+rcx*8]
		mul rsi
		cmp r12b, r11b
		adc rax, r8
		setc r11b
		mov [r10+rcx*8], rax
		inc rcx
		jmp .loop
	.end:
	cmp r12b, r11b
	adc rdx, 0
	mov [r10+rcx*8], rdx
	lea rdi, [r10-16]
	call normalise

	pop r12
	ret

decimal:
; Перевести строку в десятичный формат.
; Аргументы:
; (1) указатель на переводимую строку;
; (2) указатель на блок памяти, где будет помещён результат.
; Блок памяти должен быть нужного размера; процедура не проверяет его размер.
; Процедура помещает десятичные цифры в блок памяти, начиная с младшей.
; На место старой строки она записывает нуль.
; Возвращаемое значение: количество записанных десятичных цифр.
; Замечание: процедура возвращает строку цифр, а не соответствующих ASCII-символов.
	mov r8, 10
	add rdi, 8
	sub r10, r10
	mov r9, [rdi]
	.loop1:
		test r9, r9
		jz .end1
		mov rcx, r9
		sub rdx, rdx
		mov rax, [rdi+rcx*8]
		div r8
		test rax, rax
		jnz .skip
			dec r9
		.skip:
		mov [rdi+rcx*8], rax
		dec rcx
		jz .end
		.loop2:       ; Алгоритм деления строки на 10 с остатком описан в div-by-ten.pdf:
			mov rax, [rdi+rcx*8]
			div r8
			mov [rdi+rcx*8], rax
			dec rcx
			jnz .loop2
		.end:
		mov [rsi+r10], dl
		inc r10
		jmp .loop1
	.end1:
	mov rax, r10
	ret

approximate:
; Найти такую цифру, что произведение (со сдвигом) делителя на неё меньше, чем делимое.
; Чем больше найденная цифра, тем лучше. Процедура не возвращает нуль. Если подобрать
; ненулевую цифру нельзя, то поведение не определено.
; Аргументы:
; (1) указатель на делимое;
; (2) указатель на делитель;
; (3) позиция цифры (сдвиг).
; Возвращаемое значение: искомая цифра.
	mov rcx, [rsi+8]
	mov r8, [rsi+rcx*8+8]
	mov r9, rcx
	add rcx, rdx
	mov rax, [rdi+rcx*8+8]
		sub rdx, rdx
	cmp rcx, [rdi+8]
	jnb .skip1
		mov rdx, [rdi+rcx*8+16]
	.skip1:
	inc r8
	div r8
	cmp rax, 0
	jnz .skip2
		inc rax
	.skip2:
	ret

