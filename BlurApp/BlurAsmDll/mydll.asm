.data
myVar db 0, 4, 8, 12, 12 dup (-1)


.CODE

;############################################################################################
; BlurProc(unsigned char* imageData, unsigned char* blurredImageData, 
; unsigned long bytesPerLine, unsigned long linesToProcess)
; RCX - imageData
; RDX - blurredImageData
; R8 - bytesPerLine
; R9 - linesToProcess
;
; R10 - horizontal counter
; R11 - vertical counter
; R12 - start new line address counter (because line may not be dividable by 3)
; R14 - imageData current pointer
; R15 - blurredImageData current pointer
; XMM0 - currently calculated pixel
; XMM1 - register to store values of pixels used to calculated average value of current pixel
; XMM6 - vector of dword 3s used to multiply xmm0 values
; XMM7 - vector of dword 4s used to right-shift xmm0 values
;############################################################################################

BlurProc proc 
	mov r14, rcx						;move original image pointer to r14
	mov r15, rdx						;move blurred image pointer to r15
	mov r10, r8							;set horizontal counter to bytesPerLine value 
	mov r11, r9							;set vertical counter to linesToProcess value 
	mov r12, 0							;set new line adress counter to 0
	mov eax, 3							;move 4 dwords of value 3 to xmm6
	movd xmm6, eax						;
	vpbroadcastd xmm6, xmm6				;
	mov eax, 4							;move 4 dwords of value 4 to xmm7
	movd xmm7, eax						;
	vpbroadcastd xmm7, xmm7				;
VerticalLoop:
	cmp r11, 0							;if vertical counter == 0
	je Finish							;finish algorithm
HorizontalLoop:
	cmp r10, 0							;if horizontal counter == 0
	jle ExitRowLoop						;move on to next line
	pmovzxbd xmm0, dword ptr [r14]		;load 4 bytes of original image data to xmm0's dwords		
	pmovzxbd xmm1, dword ptr [r14-3]	;load 4 bytes of original image 3 bytes to the left to xmm1's dwords
	paddd xmm0, xmm1					;add xmm1's and xmm0's dwords
	pmovzxbd xmm1, dword ptr [r14-6]	;load 4 bytes of original image 6 bytes to the left to xmm1's dwords
	paddd xmm0, xmm1					;add xmm1's and xmm0's dwords
	pmovzxbd xmm1, dword ptr [r14+3]	;load 4 bytes of original image 3 bytes to the right to xmm1's dwords
	paddd xmm0, xmm1					;add xmm1's and xmm0's dwords
	pmovzxbd xmm1, dword ptr [r14+6]	;load 4 bytes of original image 6 bytes to the right to xmm1's dwords
	paddd xmm0, xmm1					;add xmm1's and xmm0's dwords
	pmulld xmm0, xmm6					;perform divide by 5 on xmm0's dwords (multiply dwords by 2^4 / 5 = 3)
	paddd xmm0, xmm6					;add 2^4 / 5 to xmm0's dwords
	vpsrlvd xmm0, xmm0, xmm7			;shift xmm0's dwords right by 4 bits
	pshufb xmm0, xmmword ptr myVar		;convert 4 xmm0's dwords to 4 bytes in last dword
	pextrd eax, xmm0, 0					;move xmm0's last dword value to eax
	mov [r15], dword ptr eax			;move eax's value to where blurred image pointer is pointing
	sub r10, 3							;lower horizontal counter by 3
	add r14, 3							;move original image pointer 3 bytes right
	add r15, 3							;move blurred image pointer 3 bytes right
	jmp HorizontalLoop					;jump to next iteration
ExitRowLoop:
	dec r11								;decrement vertical counter
	mov r10, r8							;reset horizontal counter
	add r12, r8							;increment new line address counter by bytesPerLine value
	mov rax, rcx						;move initial original image pointer value to rax
	add rax, r12						;add new line address counter to initial original image pointer value
	mov r14, rax						;set current original image pointer to rax's 4-byte aligned value
	mov rax, rdx						;move initial blurred image pointer value to rax
	add rax, r12						;add new line address counter to initial original image pointer value
	mov r15, rax						;set current blurred image pointer value to rax's 4-byte aligned value
	jmp VerticalLoop					;move on to next iteration
Finish:
	ret									;Return



;	mov r14, rcx
;	mov r15, rdx
;	mov r10, r8
;
;MyLoop:
;	cmp r10, 0
;	je Finish
;	mov al, byte ptr [r14]
;	add al, 50
;	mov byte ptr [r15], al
;	inc r14
;	inc r15
;	dec r10
;	jmp MyLoop
;Finish:
;	emms
;	ret

;	mov r14, rcx				;move original image pointer to r14
;	mov r15, rdx				;move blurred image pointer to r15
;	mov r10, r8					;set rowCounter to 8 (so it starts 8 bytes from left border)
;
;	mov eax, 3					;move 4 dwords of value 3 to xmm6
;	movd xmm6, eax				;
;	vpbroadcastd xmm6, xmm6		;
;	mov eax, 4					;move 4 dwords of value 4 to xmm7
;	movd xmm7, eax				;
;	vpbroadcastd xmm7, xmm7		;
;
;	mov eax, dword ptr [r14]	
;	mov dword ptr [r15], eax	
;	add r14, 4
;	add r15, 4
;	mov ax, word ptr [r14]
;	mov word ptr [r15], ax
;	add r14, 2					
;	add r15, 2					
;MyLoop:
;	cmp r10, 6
;	jle Finish
;	;
;	mov eax, dword ptr [r14]	;move 4 bytes of original image to xmm0's last dword
;	movd xmm0, eax				;
;	pmovzxbd xmm0, xmm0			;convert xmm0's last 4 bytes to xmm0's dwords
;	mov eax, dword ptr [r14-3]	;move 4 previous bytes of original image to xmm1's last dword
;	movd xmm1, eax				;
;	pmovzxbd xmm1, xmm1			;convert xmm1's last 4 bytes to xmm1's dwords
;	paddd xmm0, xmm1			;add xmm1's and xmm0's dwords
;	mov eax, dword ptr [r14-6]	
;	movd xmm1, eax
;	pmovzxbd xmm1, xmm1
;	paddd xmm0, xmm1
;	mov eax, dword ptr [r14+3]
;	movd xmm1, eax
;	pmovzxbd xmm1, xmm1
;	paddd xmm0, xmm1
;	mov eax, dword ptr [r14+6]
;	movd xmm1, eax
;	pmovzxbd xmm1, xmm1
;	paddd xmm0, xmm1
;	
;	pmulld xmm0, xmm6			;perform divide by 5 on xmm0's dwords (multiply dwords by 2^4 / 5 = 3)
;	paddd xmm0, xmm6			;add 2^4 / 5 to xmm0's dwords
;	vpsrlvd xmm0, xmm0, xmm7	;shift xmm0's dwords right by 4 bits
;	pshufb xmm0, xmmword ptr myVar
;	pextrd eax, xmm0, 0
;	mov [r15], dword ptr eax
;	;
;	sub r10, 3
;	add r15, 3
;	add r14, 3
;	jmp MyLoop
;Finish:
;	mov eax, dword ptr [r14]	
;	mov dword ptr [r15], eax	
;	add r14, 4
;	add r15, 4
;	mov ax, word ptr [r14]
;	mov word ptr [r15], ax
;	add r14, 2					
;	add r15, 2		
;	emms 
;	ret

;   PRETTY GOOD
;	mov r14, rcx				;move original image pointer to r14
;	mov r15, rdx				;move blurred image pointer to r15
;	mov r10, r8					;move bytesPerThread value to r10
;	mov eax, 3
;	movd xmm6, eax
;	vpbroadcastd xmm6, xmm6		;create vector of 3s for later multiplication
;	mov eax, 4
;	movd xmm7, eax
;	vpbroadcastd xmm7, xmm7		;create vector of 4s for later right bit shift
;
;ThreadLoop:
;	cmp r10, 0				
;	;jl PerformAdditionalBlur	;if byteCounter < 0 perform one additional blur
;	jl Finish					;if byteCounter = 0 jump straight to finish
;	;
;	mov eax, dword ptr [r14]	;move 4 bytes of original image data to EAX
;	movd xmm0, eax				;move data from EAX to XMM0's last dword
;	pmovzxbd xmm0, xmm0			;convert XMM0's last 4 bytes to XMM0's dwords
;	mov eax, dword ptr [r14-3]
;	movd xmm1, eax
;	pmovzxbd xmm1, xmm1
;	paddd xmm0, xmm1
;	mov eax, dword ptr [r14-6]
;	movd xmm1, eax
;	pmovzxbd xmm1, xmm1
;	paddd xmm0, xmm1
;	mov eax, dword ptr [r14+3]
;	movd xmm1, eax
;	pmovzxbd xmm1, xmm1
;	paddd xmm0, xmm1
;	mov eax, dword ptr [r14+6]
;	movd xmm1, eax
;	pmovzxbd xmm1, xmm1
;	paddd xmm0, xmm1
;	pmulld xmm0, xmm6
;	paddd xmm0, xmm6
;	vpsrlvd xmm0, xmm0, xmm7
;	pshufb xmm0, xmmword ptr myVar
;	pextrd eax, xmm0, 0
;	mov [r15], dword ptr eax
;	;
;	add r14, 3
;	add r15, 3
;	sub r10, 3
;	jmp ThreadLoop
;PerformAdditionalBlur:
;	mov eax, dword ptr [r14]	;move 4 bytes of original image data to EAX
;	movd xmm0, eax				;move data from EAX to XMM0's last dword
;	pmovzxbd xmm0, xmm0			;convert XMM0's last 4 bytes to XMM0's dwords
;	mov eax, dword ptr [r14-3]
;	movd xmm1, eax
;	pmovzxbd xmm1, xmm1
;	paddd xmm0, xmm1
;	mov eax, dword ptr [r14-6]
;	movd xmm1, eax
;	pmovzxbd xmm1, xmm1
;	paddd xmm0, xmm1
;	mov eax, dword ptr [r14+3]
;	movd xmm1, eax
;	pmovzxbd xmm1, xmm1
;	paddd xmm0, xmm1
;	mov eax, dword ptr [r14+6]
;	movd xmm1, eax
;	pmovzxbd xmm1, xmm1
;	paddd xmm0, xmm1
;	pmulld xmm0, xmm6
;	paddd xmm0, xmm6
;	vpsrlvd xmm0, xmm0, xmm7
;	pshufb xmm0, xmmword ptr myVar
;	pextrd eax, xmm0, 0
;	mov [r15], dword ptr eax
;Finish:	
;	emms 
;	ret

;HALF GOOD
;	mov r14, rcx				;move original image pointer to r14
;	mov r15, rdx				;move blurred image pointer to r15
;	sub r8, 8					;substract bytesPerRow by 8 (so row iteration ends 8 bytes from right border)
;	mov r10, 8					;set rowCounter to 8 (so it starts 8 bytes from left border)
;	mov r11, 0					;set lineCounter to 0 
;
;	mov eax, 3					;move 4 dwords of value 3 to xmm6
;	movd xmm6, eax				;
;	vpbroadcastd xmm6, xmm6		;
;	mov eax, 4					;move 4 dwords of value 4 to xmm7
;	movd xmm7, eax				;
;	vpbroadcastd xmm7, xmm7		;
;	
;LineLoop:
;	cmp r11, r9					;if lineCounter == linesToProcess
;	je Finish					;finish algorithm
;	;LINES CODE
;	mov rax, qword ptr [r14]	;move first 8 bytes of row of original image to same place in blurred image
;	mov qword ptr [r15], rax	;
;	add r14, 8					;move original image pointer 8 bytes right
;	add r15, 8					;move blurred image pointer 8 bytes right
;RowLoop:
;	cmp r10, r8					;if rowCounter == bytesPerRow - 8
;	je ExitRowLoop				;move on to next line
;	;ROW CODE
;
;	mov eax, dword ptr [r14]	;move 4 bytes of original image to xmm0's last dword
;	movd xmm0, eax				;
;	pmovzxbd xmm0, xmm0			;convert xmm0's last 4 bytes to xmm0's dwords
;	mov eax, dword ptr [r14-4]	;move 4 previous bytes of original image to xmm1's last dword
;	movd xmm1, eax				;
;	pmovzxbd xmm1, xmm1			;convert xmm1's last 4 bytes to xmm1's dwords
;	paddd xmm0, xmm1			;add xmm1's and xmm0's dwords
;	mov eax, dword ptr [r14-8]	
;	movd xmm1, eax
;	pmovzxbd xmm1, xmm1
;	paddd xmm0, xmm1
;	mov eax, dword ptr [r14+4]
;	movd xmm1, eax
;	pmovzxbd xmm1, xmm1
;	paddd xmm0, xmm1
;	mov eax, dword ptr [r14+8]
;	movd xmm1, eax
;	pmovzxbd xmm1, xmm1 ;To mo¿na od razu wczytaæ do xmm1
;	paddd xmm0, xmm1
;
;	pmulld xmm0, xmm6			;perform divide by 5 on xmm0's dwords (multiply dwords by 2^4 / 5 = 3)
;	paddd xmm0, xmm6			;add 2^4 / 5 to xmm0's dwords
;	vpsrlvd xmm0, xmm0, xmm7	;shift xmm0's dwords right by 4 bits
;	pshufb xmm0, xmmword ptr myVar
;	pextrd eax, xmm0, 0
;	mov [r15], dword ptr eax
;
;	;
;	add r10, 4					;move right by 4 bytes in row
;	add r14, 4					;move original image pointer 4 bytes right
;	add r15, 4					;move blurred image pointer 4 bytes right
;	jmp RowLoop					;continue row loop
;ExitRowLoop:
;	mov rax, qword ptr [r14]	;move last 8 bytes of row of original image to same place in blurred image
;	mov qword ptr [r15], rax	;
;	add r14, 8					;move original image pointer 8 bytes right (to next line)
;	add r15, 8					;move blurred image pointer 8 bytes right (to next line)
;	;
;	inc r11						;increment lineCounter (move to next line)
;	mov r10, 8					;reset rowCounter
;	jmp LineLoop				;continue line loop
;Finish:
;	emms 
;	ret 


BlurProc endp 

TestLoading proc
	ret
TestLoading endp

;	mov eax, 09000000h
;	movd xmm0, eax
;	vpbroadcastd xmm0, xmm0
;	mov eax, 00000004h
;	movd xmm1, eax
;	vpbroadcastd xmm1, xmm1
;	vpsrlvd xmm0, xmm0, xmm1
;	emms
;	ret

;dzielenie przez 9
;	mov rax, 900
;	mov rbx, 7281
;	mul rbx 
;	add rax, 7281
;	shr rax, 16
	
;zeruje wszystkie bajty
;	mov rax, r8
;	mul r9
;	mov r15, rax
;	mov r10, rcx
;MyLoop:
;	cmp r15, 0
;	je Finish
;	mov al, 0
;	mov [r10], byte ptr al
;	dec r15
;	inc r10
;	jmp MyLoop
;Finish:
;	emms
;	ret

;	mov eax, 05050505h				;move 4 bytes of decimal value 5 to eax
;	movd xmm2, dword ptr eax        ;move 4 bytes of decimal value 5 to xmm2
;	pmovzxbd xmm2, xmm2				;convert last dword's bytes to 4 dword (integer) values
;	cvtdq2ps xmm2, xmm2				;convert xmm2 integers to floats to perform divide operation later
;
;	movd xmm0, dword ptr [rcx]		;move pixel from memory to xmm0's last dword
;	pmovzxbd xmm0, xmm0				;convert last dword's bytes to 4 dword (integer) values
;	movd xmm1, dword ptr [rcx+4]	;move next pixel from memory to xmm1's last dword
;	pmovzxbd xmm1, xmm1				;convert last dword's bytes to 4 dword (integer) values
;	addps xmm0, xmm1				;add two pixel values
;	movd xmm1, dword ptr [rcx+8]	;move pixel located 2 pixels away to xmm1's last dword
;	pmovzxbd xmm1, xmm1				;convert last dword's bytes to 4 dword (integer) values
;	addps xmm0, xmm1				;sum pixel values
;	cvtdq2ps xmm0, xmm0				;convert xmm0's integer values to floats
;	divps xmm0, xmm2				;divide xmm0's floats by vector of 5s (xmm2)
;	cvtps2dq xmm0, xmm0				;convert xmm0's calculated average float values to integers
;	pshufb xmm0, xmmword ptr myVar	;convert xmm0's integer values to last dword byte values
;	pextrd eax, xmm0, 0				;move xmm0's last dword to EAX register
;	mov [rdx], dword ptr eax		;move calculated average values to proper place in memory

;	mov r11, 0
;	mov r10, 0
;
;	cmp r11, r9
;	je finish
;	;
;	cmp r10, r8
;
;	inc r11
;finish:

END


