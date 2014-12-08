.386
.model flat, stdcall
option casemap:none
includelib		msvcrt.lib
printf		PROTO C: ptr sbyte, :vararg
;
.data
szMsg		byte	"hello, world!", 0ah, 0
;
.code
start:
	invoke	printf, OFFSET szMsg
	ret
end start