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
	add rcx, 9							;set proper starting point for original image pointer
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
BlurProc endp 

END


