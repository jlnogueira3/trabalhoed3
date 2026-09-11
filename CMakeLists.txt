cmake_minimum_required(VERSION 3.20)

project(trabalhoed3 C)

set(CMAKE_C_STANDARD 11)

add_executable(trabalhoed3
        Source/main.c
        Headers/registros.h
        Source/fornecidas.c
        Source/implementadas.c
        Headers/implementadas.h
        Headers/fornecidas.h
        Headers/auxiliares/auxiliar_1.h
        Source/aux/auxiliar_1.c
)