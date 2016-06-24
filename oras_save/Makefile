all: run

clean:
	@rm -f main

run: oras_save.s oras_constants.s oras_macros.s oras_rop.s
	armips oras_rop.s
	armips oras_save.s