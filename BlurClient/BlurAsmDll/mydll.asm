.data
myVar db 0, 4, 8, 12, 12 dup (-1)


.CODE

;############################################################################################
; BlurProc(unsigned char* imageData, unsigned char* blurredImageData, 
; unsigned long bytesPerRow, unsigned long linesToProcess)
; RCX - imageData
; RDX - blurredImageData
; R8 - bytesPerRow
; R9 - linesToProcess
; R10 - rowCounter
; R11 - lineCounter
; R14 - imageData
; R15 - blurredImageData
; XMM0 - currently calculated pixel
; XMM1 - register to store values of pixels used to calculated average value of current pixel
; XMM2 - register of 4 float 5s
;############################################################################################

BlurProc proc 
	mov r14, rcx
	mov r15, rdx
	sub r8, 8
	mov r10, 8
	mov r11, 0

LineLoop:
	cmp r11, r9
	je Finish
	;LINES CODE
RowLoop:
	cmp r10, r8
	je ExitRowLoop
	;ROW CODE
	;
	inc r10 ;potem +4
	jmp RowLoop
ExitRowLoop:
	;
	inc r11
	mov r10, 8
	jmp LineLoop
Finish:
	emms 
	ret 


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

	

	emms
	ret 
BlurProc endp 

MyProc1 proc x: DWORD, y: DWORD

;xor	eax,eax
;mov	eax,x
;mov	ecx,y
;ror	ecx,1
;shld	eax,ecx,2
;jnc 	ET1
;mul	y
;ret
;ET1:	
;Mul	x
;Neg	y
;ret

mov rax, 15
ret

MyProc1 endp

END


