# Trabalho Introdutório — Arquivos Binários em C

Projeto desenvolvido em **C** com foco na manipulação de registros em arquivos binários. O programa realiza conversão de dados em CSV, consultas sequenciais, buscas com filtros, acesso por RRN e remoção lógica de registros.

O projeto foi estruturado de forma modular, separando as definições dos registros, as funcionalidades principais, funções auxiliares e funções fornecidas pela disciplina.

---

## Funcionalidades

O programa implementa as seguintes operações:

| Código | Funcionalidade | Descrição |
|:---:|---|---|
| `1` | CSV → binário | Lê um arquivo CSV e gera o arquivo binário correspondente. |
| `2` | Listagem | Exibe todos os registros válidos armazenados no arquivo binário. |
| `3` | Busca por critérios | Busca registros a partir de um ou mais pares `campo valor`. |
| `4` | Busca por RRN | Acessa diretamente um registro utilizando seu RRN. |
| `5` | Remoção lógica | Remove logicamente registros que atendem aos critérios informados e atualiza a pilha de removidos. |

Campos nulos são tratados conforme a especificação do trabalho, sendo exibidos como `NULO` quando necessário.

---

## Estrutura do projeto

```text
.
├── Headers/
│   ├── auxiliares/
│   │   └── auxiliar_1.h
│   ├── fornecidas.h
│   ├── implementadas.h
│   └── registros.h
│
├── Source/
│   ├── auxiliares/
│   │   └── auxiliar_1.c
│   ├── conectaPoPs.csv
│   ├── fornecidas.c
│   ├── implementadas.c
│   └── main.c
│
├── CMakeLists.txt
├── makefile
└── README.md
```

### Organização dos arquivos

- **`registros.h`**: definição das estruturas de cabeçalho e de dados.
- **`implementadas.c`**: implementação das funcionalidades principais do trabalho.
- **`auxiliar_1.c`**: funções auxiliares de leitura, escrita, impressão e filtragem de registros.
- **`fornecidas.c`**: funções disponibilizadas pela disciplina, como `BinarioNaTela` e `ScanQuoteString`.
- **`main.c`**: leitura da entrada e seleção da funcionalidade a ser executada.

---

## Estruturas utilizadas

### Registro de cabeçalho

O cabeçalho possui **17 bytes** e armazena informações de controle do arquivo:

```text
status        1 byte
topo_pilha    4 bytes
proxRRN       4 bytes
nroRegRem     4 bytes
nroPares      4 bytes
-------------------
Total        17 bytes
```

### Registro de dados

Cada registro de dados possui **18 bytes**:

```text
removido              1 byte
encadeamentoPilha      4 bytes
idPoPs                 4 bytes
idPoPsConectado        4 bytes
velocidade             4 bytes
unidadeMedida          1 byte
-----------------------------
Total                 18 bytes
```

A remoção é feita de forma **lógica**: o registro permanece no arquivo, mas passa a ser marcado como removido e é encadeado na pilha de registros removidos.

---

## Compilação

O projeto possui um `makefile`. Para compilar, execute na raiz do projeto:

```bash
make
```

O comando utiliza:

```bash
gcc -Wall -Wextra -IHeaders -o programaTrab Source/*.c Source/auxiliares/*.c
```

As flags `-Wall` e `-Wextra` habilitam avisos adicionais do compilador, enquanto `-IHeaders` adiciona a pasta de cabeçalhos ao caminho de busca do GCC.

O executável gerado será:

```text
programaTrab
```

Para executar:

```bash
make run
```

ou diretamente:

```bash
./programaTrab
```

Para remover o executável gerado:

```bash
make clean
```

---

## Exemplos de uso

### 1. Criar o arquivo binário

```text
1 conectaPoPs.csv conecta.bin
```

O programa lê o CSV e cria `conecta.bin`.

### 2. Listar os registros

```text
2 conecta.bin
```

Exemplo de saída:

```text
47 63 1 "G"
23 31 10 "G"
50 55 155 "M"
```

### 3. Buscar registros por critérios

Entrada:

```text
3 conecta.bin 1
2 idPoPs 73 idPoPsConectado 75
```

Saída:

```text
73 75 10 "G"
```

Também é possível combinar outros campos, como `velocidade` e `unidadeMedida`.

### 4. Buscar por RRN

```text
4 conecta.bin 10
```

O programa calcula diretamente a posição do registro no arquivo usando:

```text
posição = 17 + RRN × 18
```

### 5. Remover registros

A funcionalidade 5 recebe um conjunto de critérios, localiza os registros correspondentes e realiza a remoção lógica, atualizando:

- `topo_pilha`;
- `nroRegRem`;
- campo `removido`;
- `encadeamentoPilha`.

Durante operações de alteração, o campo `status` do cabeçalho é utilizado para indicar se o arquivo está consistente.

---

## Conceitos trabalhados

O projeto explora conceitos importantes de programação em C e organização de arquivos, incluindo:

- arquivos binários com `fread`, `fwrite` e `fseek`;
- registros de tamanho fixo;
- acesso direto por RRN;
- remoção lógica;
- pilha de registros removidos;
- alocação dinâmica com `calloc` e `free`;
- modularização com arquivos `.c` e `.h`;
- filtragem por múltiplos critérios;
- controle de consistência do arquivo por meio do cabeçalho.

---


> Este repositório tem finalidade acadêmica e foi desenvolvido seguindo as especificações fornecidas para o trabalho.