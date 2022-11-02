.data
myVar db 0, 4, 8, 12, 12 dup (-1)


.CODE

;############################################################################################
; BlurProc(unsigned char* imageData, unsigned long bytesPerRow, unsigned long linesToProcess)
; RCX - imageData
; RDX - bytesPerRow
; mm7 - 0
;############################################################################################

BlurProc proc 
;	mov rax, 0
;	movq mm7, rax					;wyzeruj mm7
;	movd mm0, dword ptr [rcx]		;przenieœ pixel do mm0
;	punpcklbw mm0, mm7				;zamieñ bajty na wordy
;
;	movd mm1, dword ptr [rcx+4]
;	punpcklbw mm1, mm7	
;	paddw mm0, mm1
;	movd mm1, dword ptr [rcx+8]
;	punpcklbw mm1, mm7	
;	paddw mm0, mm1
	
	mov eax,04030201h
	movd xmm0,eax
	pxor xmm1,xmm1
	;punpcklbw xmm0,xmm1
	pmovzxbd xmm1, xmm0
	mov eax,05050505h
	movd xmm0, eax
	pmovzxbd xmm2, xmm0
	CVTDQ2PS xmm2, xmm2
	CVTDQ2PS xmm1, xmm1
	;VCVTUDQ2PS xmm2, xmm2
	;VCVTUDQ2PS xmm1, xmm1
	mulps xmm1, xmm2
	CVTPS2DQ xmm1, xmm1

	pshufb  xmm1, xmmword ptr myVar
    movd    dword ptr [rcx], xmm1
	
       

	emms
	ret

	;mov rax, 0
	;movq mm1, rax ;przenieœ 0 do mm1
	;movd mm0, dword ptr [rcx] ;przenieœ 4 bajty z pamiêci (pixel) do mm0
	;movq mm0, mm1 ;wyzeruj pixel
	;movd dword ptr [rcx], mm0 ;zapisz now¹ wartoœæ pixela do odpowiedniego miejsca w pamiêci
	;emms ;prze³¹cz FPU x87 spowrotem do trybu zmiennoprzecinkowego
	;ret

	;mov al, 0
	;mov byte ptr [rcx], al
	;inc rcx
	;mov byte ptr [rcx], al
	;inc rcx
	;mov byte ptr [rcx], al
	;inc rcx
	;mov byte ptr [rcx], al
	;ret
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


