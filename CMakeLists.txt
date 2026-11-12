# José Eduardo Constantino Nordemann - N usp: 16877738
cmake_minimum_required(VERSION 3.20)

project(trabalhoed3 C)

set(CMAKE_C_STANDARD 11)

add_executable(trabalhoed3
        Source/main.c
        Source/cabecalho.c
        Source/consulta.c
        Source/criacao.c
        Source/criterios.c
        Source/erros.c
        Source/fornecidas.c
        Source/manipulacao.c
        Source/pops_arquivo.c
        Source/pops_consulta.c
        Source/pops_criacao.c
        Source/pops_entrada.c
        Source/pops_manipulacao.c
        Source/pops_operacoes.c
        Source/pops_registro.c
        Source/registro.c
        Headers/cabecalho.h
        Headers/consulta.h
        Headers/criacao.h
        Headers/criterios.h
        Headers/erros.h
        Headers/fornecidas.h
        Headers/manipulacao.h
        Headers/pops.h
        Headers/pops_arquivo.h
        Headers/pops_consulta.h
        Headers/pops_criacao.h
        Headers/pops_entrada.h
        Headers/pops_manipulacao.h
        Headers/pops_operacoes.h
        Headers/pops_registro.h
        Headers/registro.h
        Headers/registros.h
)
