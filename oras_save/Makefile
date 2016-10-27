all: run

clean:
	@rm -f main

run: oras_save.s oras_constants.s oras_macros.s oras_rop_stage1.s oras_rop_stage0.s
	armips oras_rop_stage1.s
	armips oras_rop_stage0.s
	armips oras_save.s