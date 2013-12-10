ESDK=${EPIPHANY_HOME}
ELIBS=${ESDK}/tools/host/lib
EINCS=${ESDK}/tools/host/include
EHDF=${EPIPHANY_HDF}
ELDF=${ESDK}/bsps/current/fast.ldf
FIB = 10

all: host device

host: build_p/seta.elf 

device: build_p/e_seta.srec

build_p/seta.elf: host/host.c
	gcc host/host.c -o build_p/seta.elf -I ${EINCS} -L ${ELIBS} -le-hal -g

build_p/e_seta.elf: $(wildcard device/*.c) $(wildcard device/*.h)
	e-gcc -T ${ELDF} device/*.c -o build_p/e_seta.elf -le-lib -g
    
build_p/e_seta.srec: build_p/e_seta.elf
	e-objcopy --srec-forceS3 --output-target srec build_p/e_seta.elf build_p/e_seta.srec
    
run:
	@cd build_p && sudo -E LD_LIBRARY_PATH=${ELIBS}:${LD_LIBRARY_PATH} EPIPHANY_HDF=${EHDF} ./seta.elf ${FIB}
    
debug:
	cd build_p && sudo -E LD_LIBRARY_PATH=${ELIBS}:${LD_LIBRARY_PATH} EPIPHANY_HDF=${EHDF} gdb ./seta.elf
    
.PHONY: clean
clean:
	rm -f build_p/*.elf build_p/*.srec
