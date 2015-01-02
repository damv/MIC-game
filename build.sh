# a51 is the Keil assembler compiler
function a51 {
    wine /opt/Keil/C51/BIN/A51.exe $@;
}

# c51 is the Keil compiler
function c51 {
    wine /opt/Keil/C51/BIN/C51.exe $@;
}

# bl51 is the Keil linker
# BL51.exe seems to always return a non-zero status code,
# so we parse the output and return ourselves
function bl51 {
    wine /opt/Keil/C51/BIN/BL51.exe $@ > buildoutput.txt;
    cat buildoutput.txt
    if grep -q " 0 ERROR(S)" buildoutput.txt; then
        exit 0
    else
        exit 1
    fi
}

c51 src/main.c
c51 src/smbus.c
c51 src/spi.c
c51 src/utils.c
bl51 src/main.OBJ, src/smbus.OBJ, src/spi.OBJ, src/utils.OBJ
