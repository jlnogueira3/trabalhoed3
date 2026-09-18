cmake_minimum_required(VERSION 3.20)

project(trabalhoed3 C)

set(CMAKE_C_STANDARD 11)

add_executable(trabalhoed3
        Source/main.c
        Headers/registros.h
        Source/fornecidas.c
        Headers/implementadas.h
        Headers/fornecidas.h
        Headers/auxiliares/auxiliar_1.h
        Source/auxiliares/auxiliar_1.c
        Source/implementadas/funcionalidade1.c
        Source/implementadas/funcionalidade2.c
        Source/implementadas/funcionalidade3.c
        Source/implementadas/funcionalidade4.c
        Source/implementadas/funcionalidade5.c
        Source/implementadas/funcionalidade6.c
        Source/implementadas/funcionalidade7.c
)