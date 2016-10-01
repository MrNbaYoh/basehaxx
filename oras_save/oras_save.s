.nds

.create "main", 0x0

.include "oras_constants.s"
.include "oras_macros.s"

.orga 0x23F58
	.fill (0x23FA4 - .), 0xDA

.orga 0x23FA4
	.word ORAS_2ND_JMP_PTR ; the game dereference this => point to the address where the 0x67c00+0x10 block is stored
	.word ORAS_1ST_JMP_PTR ; the game dereference this => point to the address where the 0x67c00 block is stored
	.word 0x00000001 ; have to be != 0 to make the code jump

.orga 0x67C00
	.area 0x4FF8 ; maximum allowed size of rop+code
			.incbin "../build/rop.bin" ; pivot+rop
			
			.incbin "../build/oras_code.bin"

	.endarea

.orga 0x67C00 + HAX_PAYLOAD_OFFSET - 0x4
	.word haxx_payload_end - haxx_payload ; store the size of hax payload 

.orga 0x67C00 + HAX_PAYLOAD_OFFSET
	.area 0xE058-0x5000 ; maximum size of the compressed hax payload

			haxx_payload:
				.incbin "../build/haxx_payload.bin"
			haxx_payload_end:
		
	.endarea
	
.fill (0x76000 - .), 0x00
		
.close