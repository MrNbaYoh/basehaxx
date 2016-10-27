.nds

.create "main", 0x0

.include "oras_constants.s"
.include "oras_macros.s"

.orga 0x23F58
	.incbin "../build/rop_stage0.bin" ; pivot+stage0
	
.fill (0x76000 - .), 0x00
		
.close