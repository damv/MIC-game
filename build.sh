#!/bin/bash

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

cd src
c51 main.c
c51 smbus.c
c51 spi.c
c51 utils.c
bl51 main.OBJ, smbus.OBJ, spi.OBJ, utils.OBJ
