ROPDB_TARGET = oras_ropdb/ropdb_$(ORAS_VERSION).txt

.PHONY: all directories build/constants clean

all: directories build/constants build/oras_code.bin oras_save/main

directories:
	@mkdir -p build
		
build/constants: oras_ropdb/ropdb.txt
	@python scripts/makeHeaders.py build/constants "FIRM_VERSION=$(FIRM_VERSION)" $^

build/oras_code.bin: oras_code/oras_code.bin
	@cp oras_code/oras_code.bin build/
oras_code/oras_code.bin: $(wildcard oras_code/source/*)
	@cd oras_code && make
			
oras_save/main:
	@cd oras_save && make
			
oras_ropdb/ropdb.txt: $(ROPDB_TARGET)
	@cp $(ROPDB_TARGET) oras_ropdb/ropdb.txt

clean:
	@rm -rf build
	@rm oras_ropdb/ropdb.txt
	@cd oras_save && make clean
	@cd oras_code && make clean