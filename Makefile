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

build/haxx_payload.bin: utils/lzss
	@curl -sL -o otherapp.bin http://smea.mtheall.com/get_payload.php?version=NEW-10-6-0-31-JPN
	@utils/lzss otherapp.bin
	@rm otherapp.bin
	@mv otherapp.bin.lzss build/haxx_payload.bin

utils/lzss:
	@cd utils && make

oras_save/main: build/haxx_payload.bin
	@cd oras_save && make
			
oras_ropdb/ropdb.txt: $(ROPDB_TARGET)
	@cp $(ROPDB_TARGET) oras_ropdb/ropdb.txt

clean:
	@rm -rf build
	@rm oras_ropdb/ropdb.txt
	@cd oras_save && make clean
	@cd oras_code && make clean
	@cd utils && make clean
