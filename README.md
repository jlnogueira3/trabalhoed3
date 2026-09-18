# Trabalho Introdutório — Arquivos Binários em C

Projeto desenvolvido em **C** com foco na manipulação de registros em arquivos binários. O programa realiza conversão de dados em CSV, listagem, buscas com filtros, acesso por RRN, remoção lógica, inserção e atualização de registros.

O projeto foi estruturado de forma modular, separando as definições dos registros, as funcionalidades principais, as funções auxiliares e as funções fornecidas pela disciplina.

---

## Funcionalidades

O programa implementa as seguintes operações:

| Código | Funcionalidade | Descrição |
|:---:|---|---|
| `1` | CSV → binário | Lê um arquivo CSV e gera o arquivo binário correspondente. |
| `2` | Listagem | Exibe todos os registros válidos armazenados no arquivo binário. |
| `3` | Busca por critérios | Busca registros utilizando um ou mais pares `campo valor`. |
| `4` | Busca por RRN | Acessa diretamente um registro utilizando seu RRN. |
| `5` | Remoção lógica | Remove logicamente registros e atualiza a pilha de removidos. |
| `6` | Inserção | Insere registros, reutilizando posições removidas quando possível. |
| `7` | Atualização | Atualiza os campos dos registros que atendem aos critérios informados. |

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
│   ├── implementadas/
│   │   ├── funcionalidade1.c
│   │   ├── funcionalidade2.c
│   │   ├── funcionalidade3.c
│   │   ├── funcionalidade4.c
│   │   ├── funcionalidade5.c
│   │   ├── funcionalidade6.c
│   │   └── funcionalidade7.c
│   ├── conectaPoPs.csv
│   ├── fornecidas.c
│   └── main.c
│
├── CMakeLists.txt
├── makefile
└── README.md
```

### Organização dos arquivos

- **`registros.h`**: definição das estruturas de cabeçalho, dados e chave-valor.
- **`implementadas.h`**: protótipos das funcionalidades implementadas.
- **`Source/implementadas/`**: implementação das sete funcionalidades, separadas em arquivos individuais.
- **`auxiliar_1.c`**: funções auxiliares de leitura, escrita, impressão e filtragem de registros.
- **`fornecidas.c`**: funções fornecidas pela disciplina, como `BinarioNaTela` e `ScanQuoteString`.
- **`main.c`**: leitura da entrada e seleção da funcionalidade executada.

---

## Estruturas utilizadas

### Registro de cabeçalho

O cabeçalho possui **17 bytes**:

```text
status        1 byte
topo_pilha    4 bytes
proxRRN       4 bytes
nroRegRem     4 bytes
nroPares      4 bytes
-------------------
Total        17 bytes
```

O campo `status` indica a consistência do arquivo:

- `'0'`: arquivo inconsistente ou em processo de alteração;
- `'1'`: arquivo consistente.

O campo `topo_pilha` armazena o RRN do primeiro registro logicamente removido. Quando a pilha está vazia, seu valor é `-1`.

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

A remoção é feita de forma lógica: o registro permanece no arquivo, mas passa a ser marcado como removido e é encadeado na pilha de registros removidos.

---

## Compilação

O projeto possui um `makefile`. Para compilar, execute na raiz do projeto:

```bash
make
```

O comando utilizado é equivalente a:

```bash
gcc -Wall -Wextra -IHeaders -o programaTrab Source/*.c Source/auxiliares/*.c Source/implementadas/*.c
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

Ou diretamente:

```bash
./programaTrab
```

Para remover o executável:

```bash
make clean
```

O projeto também pode ser compilado utilizando o `CMakeLists.txt`.

---

## Exemplos de uso

### 1. Criar o arquivo binário

```text
1 Source/conectaPoPs.csv conecta.bin
```

O programa lê o arquivo CSV e cria o arquivo `conecta.bin`.

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

Registros logicamente removidos não são exibidos.

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

É possível combinar campos como:

- `idPoPs`;
- `idPoPsConectado`;
- `velocidade`;
- `unidadeMedida`.

Quando nenhum registro atende aos critérios, o programa exibe:

```text
Registro inexistente.
```

### 4. Buscar por RRN

```text
4 conecta.bin 10
```

A posição do registro é calculada diretamente:

```text
posição = 17 + RRN × 18
```

Se o RRN não existir ou o registro estiver removido, o programa exibe:

```text
Registro inexistente.
```

### 5. Remover registros

```text
5 conecta.bin 1
1 idPoPs 73
```

A funcionalidade recebe um conjunto de critérios, localiza os registros correspondentes e realiza a remoção lógica.

Durante a remoção, são atualizados:

- `topo_pilha`;
- `nroRegRem`;
- campo `removido`;
- campo `encadeamentoPilha`.

Os demais bytes do registro removido são preenchidos com o caractere `$`.

### 6. Inserir registros

```text
6 conecta.bin 1
80 81 100 "M"
```

Quando a pilha de removidos não está vazia, o programa reutiliza primeiro o RRN armazenado em `topo_pilha`.

Quando não existem registros removidos, o novo registro é inserido no final do arquivo.

### 7. Atualizar registros

```text
7 conecta.bin 1
1 idPoPs 80
2 velocidade 200 unidadeMedida "G"
```

Nesse exemplo, todos os registros cujo `idPoPs` seja igual a `80` terão os campos `velocidade` e `unidadeMedida` atualizados.

---

## Valores nulos

Os valores nulos são representados internamente da seguinte forma:

| Campo | Representação de valor nulo |
|---|---|
| `velocidade` | `-1` |
| `unidadeMedida` | `$` |

Na saída do programa, esses valores são exibidos como:

```text
NULO
```

---

## Controle de consistência

Antes de uma operação que altera o arquivo, o campo `status` do cabeçalho é definido como `'0'`.

Depois que todas as alterações são concluídas, o cabeçalho é atualizado e o campo `status` volta a receber `'1'`.

Arquivos com status diferente de `'1'` não podem ser consultados ou alterados.

---

## Conceitos trabalhados

O projeto utiliza conceitos importantes de programação em C e organização de arquivos:

- arquivos binários com `fread`, `fwrite` e `fseek`;
- registros de tamanho fixo;
- acesso direto por RRN;
- remoção lógica;
- pilha de registros removidos;
- reutilização de espaços removidos;
- inserção e atualização de registros;
- alocação dinâmica com `calloc` e `free`;
- modularização com arquivos `.c` e `.h`;
- filtragem por múltiplos critérios;
- controle de consistência por meio do cabeçalho.

---

> Este repositório possui finalidade acadêmica e foi desenvolvido seguindo as especificações fornecidas para o trabalho.