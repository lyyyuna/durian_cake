.386
.model	flat, stdcall
option 	casemap:none
MessageBoxA		PROTO : dword, :dword, :dword, :dword
MessageBox		equ		<MessageBoxA>
includelib		user32.lib
NULL			equ		0
MB_OK			equ		0
.stack		4098
.data
szTitle		byte	'Hi', 0
szMsg		byte	'Hello, world!', 0
.code
start:
	invoke	MessageBox,
			NULL,
			offset	szMsg,
			offset	szTitle,
			MB_OK
	ret
end start