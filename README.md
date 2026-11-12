# Trabalho Prático — Arquivos Binários em C

Projeto desenvolvido em **C** para manipulação de registros em arquivos binários.
Ele cobre dois trabalhos da disciplina:

- **Trabalho 1** (funcionalidades 1 a 7): arquivo de dados **conectaPoPs**, com
  conversão de CSV, listagem, buscas, acesso por RRN, remoção lógica, inserção e
  atualização.
- **Trabalho 2** (funcionalidades 8 a 14): arquivo de dados **PoPs** e seu índice
  primário por `idPoPs`, com criação, listagem, busca indexada, remoção,
  inserção com Worst Fit e atualização.

---

## Funcionalidades

Cada funcionalidade é indicada abaixo no formato `N.arquivo`, onde `N` é o número
da funcionalidade e `arquivo` é o módulo de `Source/` que a implementa.

### Trabalho 1

| Código | Funcionalidade | Arquivo | Descrição |
|:---:|---|---|---|
| `1` | CSV → binário | `1.criacao` | Lê um CSV e gera o arquivo binário correspondente. |
| `2` | Listagem | `2.consulta` | Exibe todos os registros válidos. |
| `3` | Busca por critérios | `3.consulta` | Busca com um ou mais pares `campo valor`. |
| `4` | Busca por RRN | `4.consulta` | Acessa diretamente um registro pelo RRN. |
| `5` | Remoção lógica | `5.manipulacao` | Remove registros que atendem aos critérios e atualiza a pilha de removidos. |
| `6` | Inserção | `6.manipulacao` | Insere registros, reutilizando posições removidas quando possível. |
| `7` | Atualização | `7.manipulacao` | Atualiza os campos dos registros que atendem aos critérios. |

### Trabalho 2

| Código | Funcionalidade | Arquivo | Descrição |
|:---:|---|---|---|
| `8` | CSV → dados PoPs | `8.pops_criacao` | Lê o CSV de PoPs e gera o arquivo de dados. |
| `9` | Criação do índice | `9.pops_criacao` | Gera o índice primário `indexaPoPs` a partir do arquivo de dados. |
| `10` | Listagem de PoPs | `10.pops_consulta` | Lista os PoPs válidos do arquivo de dados. |
| `11` | Busca de PoPs | `11.pops_consulta` | Busca com todos os critérios; usa o índice quando há `idPoPs`. |
| `12` | Remoção de PoPs | `12.pops_manipulacao` | Remove logicamente os registros, coloca o espaço na lista de removidos e tira a chave do índice. |
| `13` | Inserção de PoPs | `13.pops_manipulacao` | Insere com Worst Fit; quando nenhum espaço serve, grava no final. |
| `14` | Atualização de PoPs | `14.pops_manipulacao` | Atualiza no próprio espaço ou remove e reinsere quando o registro cresce. |

Campos nulos são exibidos como `NULO`.

---

## Estrutura do projeto

```text
.
├── Headers/
│   ├── fornecidas.h          funções fornecidas pela disciplina
│   ├── registros.h           estruturas e constantes do trabalho 1
│   ├── pops.h                estruturas e constantes do trabalho 2
│   ├── cabecalho.h           cabeçalho do arquivo (trabalho 1)
│   ├── registro.h            registros de dados (trabalho 1)
│   ├── criterios.h           critérios de busca e atualização (trabalho 1)
│   ├── erros.h               mensagens de falha (comum)
│   ├── criacao.h             funcionalidade 1
│   ├── consulta.h            funcionalidades 2, 3 e 4
│   ├── manipulacao.h         funcionalidades 5, 6 e 7
│   ├── pops_arquivo.h        arquivo e índice de PoPs (trabalho 2)
│   ├── pops_registro.h       registros de PoPs (trabalho 2)
│   ├── pops_operacoes.h      busca, remoção e inserção de PoPs (trabalho 2)
│   ├── pops_entrada.h        leitura da entrada de PoPs (trabalho 2)
│   ├── pops_criacao.h        funcionalidades 8 e 9
│   ├── pops_consulta.h       funcionalidades 10 e 11
│   └── pops_manipulacao.h    funcionalidades 12, 13 e 14
│
├── Source/
│   ├── main.c                leitura da opção e despacho
│   ├── fornecidas.c          funções fornecidas (não alteradas)
│   ├── cabecalho.c, registro.c, criterios.c, erros.c
│   ├── criacao.c, consulta.c, manipulacao.c
│   ├── pops_arquivo.c, pops_registro.c, pops_operacoes.c, pops_entrada.c
│   ├── pops_criacao.c, pops_consulta.c, pops_manipulacao.c
│   └── conectaPoPs.csv
│
├── tests/test_pops.py        testes de integração do trabalho 2
├── CMakeLists.txt
├── makefile
└── README.md
```

### Organização dos arquivos

Os arquivos são agrupados por tema:

- **`cabecalho`, `registro` e `criterios`**: operações de disco e de busca do trabalho 1.
- **`erros`**: mensagens padrão de falha e verificação de abertura de arquivo.
- **`criacao`, `consulta` e `manipulacao`**: funcionalidades 1, 2 a 4 e 5 a 7.
- **`pops_arquivo`**: cabeçalho, status, abertura e fechamento dos arquivos de dados e de índice, e o índice em memória.
- **`pops_registro`**: leitura, escrita e impressão de um registro de PoPs.
- **`pops_operacoes`**: busca, remoção e inserção, incluindo o Worst Fit.
- **`pops_entrada`**: leitura da entrada padrão, com strings, nulos e validação dos campos.
- **`pops_criacao`, `pops_consulta` e `pops_manipulacao`**: funcionalidades 8 e 9, 10 e 11, e 12 a 14.
- **`registros.h` e `pops.h`**: estruturas e constantes de cada trabalho.
- **`fornecidas.c`**: `BinarioNaTela` e `ScanQuoteString`, fornecidas pela disciplina.
- **`main.c`**: lê a opção, os argumentos e chama a funcionalidade correspondente.

---

## Compilação

Na raiz do projeto:

```bash
make
```

O comando usado pelo makefile é:

```bash
gcc -Wall -Wextra -IHeaders -o programaTrab Source/*.c
```

As flags `-Wall` e `-Wextra` ativam avisos adicionais, e `-IHeaders` adiciona a pasta de
cabeçalhos ao caminho de busca. O executável gerado é `programaTrab`.

Para executar:

```bash
./programaTrab
```

O projeto também pode ser compilado com o `CMakeLists.txt`.

---

## Como usar

O programa lê tudo da entrada padrão. A primeira linha começa com o número da
funcionalidade, seguido dos argumentos dela. Os exemplos abaixo mostram a entrada
e a saída esperadas.

### Trabalho 1

#### 1. Criar o arquivo binário a partir do CSV

```text
1 Source/conectaPoPs.csv conecta.bin
```

Cria `conecta.bin` e exibe o checksum do arquivo gerado (`BinarioNaTela`).

#### 2. Listar os registros

```text
2 conecta.bin
```

```text
47 63 1 "G"
23 31 10 "G"
50 55 155 "M"
```

Registros removidos não são exibidos.

#### 3. Buscar por critérios

A entrada tem `n` buscas. Cada busca começa com a quantidade de pares, seguida dos
pares `campo valor`:

```text
3 conecta.bin 1
2 idPoPs 73 idPoPsConectado 75
```

```text
73 75 10 "G"
```

Campos aceitos: `idPoPs`, `idPoPsConectado`, `velocidade` e `unidadeMedida`.

Quando nenhum registro atende aos critérios:

```text
Registro inexistente.
```

#### 4. Buscar por RRN

```text
4 conecta.bin 10
```

A posição do registro é calculada diretamente: `posição = 17 + RRN × 18`.
Se o RRN não existir ou o registro estiver removido, o programa exibe `Registro inexistente.`.

#### 5. Remover registros

A entrada tem `n` remoções. Cada uma segue o formato da busca:

```text
5 conecta.bin 1
1 idPoPs 73
```

Os registros encontrados são marcados como removidos, encadeados na pilha
(`topo_pilha` e `encadeamentoPilha`), e o restante do registro é preenchido com `$`.
`nroRegRem` é atualizado.

#### 6. Inserir registros

A entrada tem `n` inserções, cada uma com `idPoPs idPoPsConectado velocidade unidadeMedida`:

```text
6 conecta.bin 1
80 81 100 "M"
```

Se a pilha de removidos não estiver vazia, o programa reutiliza primeiro o RRN de
`topo_pilha`. Caso contrário, insere no final do arquivo.

#### 7. Atualizar registros

A entrada tem `n` atualizações. Cada uma tem os critérios e, em seguida, os campos novos:

```text
7 conecta.bin 1
1 idPoPs 80
2 velocidade 200 unidadeMedida "G"
```

Todos os registros com `idPoPs` igual a `80` têm `velocidade` e `unidadeMedida` atualizados.

### Trabalho 2

Os comandos 9 a 14 recebem os arquivos já criados. O índice é o arquivo `indexaPoPs`,
gerado pela funcionalidade 9.

#### 8. Criar o arquivo de dados PoPs

```text
8 Source/PoPs.csv PoPs.bin
```

Lê o CSV de PoPs, localiza as colunas pelo nome e grava o arquivo de dados.

#### 9. Criar o índice primário

```text
9 PoPs.bin indexaPoPs.bin
```

Grava o índice, ordenado por `idPoPs`, com uma entrada por registro válido. Se houver
dois registros válidos com o mesmo `idPoPs`, o índice não é criado.

#### 10. Listar os PoPs

```text
10 PoPs.bin
```

```text
23 NATAL BRAZIL BR
16 FLORIANOPOLIS BRAZIL NULO
31 FORTALEZA BRAZIL BR
```

Registros removidos não são exibidos. Se não houver nenhum registro válido, o programa
exibe `Registro inexistente.`.

#### 11. Buscar PoPs

```text
11 PoPs.bin indexaPoPs.bin
2
1 idPoPs 23
1 nomePais BRAZIL
```

A primeira linha depois dos argumentos é o número `n` de buscas. Cada busca começa
com a quantidade de pares. Se houver `idPoPs` entre os critérios, a busca usa o
índice. Campos aceitos: `idPoPs`, `nomePoPs`, `nomePais` e `siglaPais`.

#### 12. Remover PoPs

Mesmo formato de busca de `11`, com `n` remoções:

```text
12 PoPs.bin indexaPoPs.bin
1
1 idPoPs 50
```

Cada registro encontrado vai para a lista de removidos, e sua chave sai do índice.
Ao final, são exibidos os resultados de `BinarioNaTela` para o arquivo de dados e para o índice.

#### 13. Inserir PoPs

A entrada tem `n` inserções, cada uma com `idPoPs`, `nomePoPs`, `nomePais` e `siglaPais`.
Textos com espaços vão entre aspas:

```text
13 PoPs.bin indexaPoPs.bin
2
999 "NOVA CIDADE" "NOVO PAIS" BR
998 "CID X" "PAIS Y" ""
```

A inserção usa o maior espaço removido que comporte o registro (Worst Fit). Se nenhum
servir, grava no final. Uma sigla vazia é gravada como `NULO`.

#### 14. Atualizar PoPs

A entrada tem `n` atualizações. Cada uma tem os critérios e, em seguida, os campos novos:

```text
14 PoPs.bin indexaPoPs.bin
1
1
idPoPs
23
1
nomePoPs
"NOVO NOME"
```

Se o registro couber no espaço que já ocupa, ele é atualizado no lugar. Se crescer,
é removido e reinserido por Worst Fit, e o índice é corrigido.

---

## Formato dos arquivos

### Trabalho 1

**Cabeçalho (17 bytes)**

```text
status        1 byte
topo_pilha    4 bytes
proxRRN       4 bytes
nroRegRem     4 bytes
nroPares      4 bytes
```

`status` vale `'1'` quando o arquivo está consistente e `'0'` durante uma alteração.
`topo_pilha` guarda o RRN do primeiro registro removido, ou `-1` se a pilha estiver vazia.

**Registro de dados (18 bytes)**

```text
removido              1 byte
encadeamentoPilha     4 bytes
idPoPs                4 bytes
idPoPsConectado       4 bytes
velocidade            4 bytes
unidadeMedida         1 byte
```

A remoção é lógica: o registro continua no arquivo, marcado como removido e encadeado na pilha.

### Trabalho 2

**Cabeçalho do arquivo de dados (25 bytes)**

```text
status           1 byte
topoLista        8 bytes
proxByteOffset   8 bytes
nroRegRem        4 bytes
nroPoPs          4 bytes
```

Os offsets são absolutos. `proxByteOffset` começa em 25, o tamanho do cabeçalho.

**Registro de dados (tamanho variável)**

```text
removido            1 byte
tamanhoRegistro     4 bytes   tamanho total, incluindo estes campos de controle
encadeamentoLista   8 bytes
idPoPs              4 bytes
siglaPais           2 bytes
nomePoPs            variável, terminado por '|'
nomePais            variável, terminado por '|'
```

Os bytes que sobram quando um registro ocupa um espaço maior que o necessário são
preenchidos com `$`. A sigla nula é gravada como `$$`.

**Índice `indexaPoPs`**

```text
status          1 byte
entradas        12 bytes cada: idPoPs (4) + byteoffset (8), ordenadas por idPoPs
```

O índice só é gravado depois que o arquivo de dados foi lido por completo, com
`status = '0'` durante a escrita e `'1'` ao final.

### Valores nulos

| Campo | Representação |
|---|---|
| `velocidade` (trabalho 1) | `-1` |
| `unidadeMedida` (trabalho 1) | `$` |
| `siglaPais` (trabalho 2) | `$$` |
| `nomePoPs` e `nomePais` (trabalho 2) | string vazia |

Na saída, esses valores aparecem como `NULO`.

---

## Controle de consistência

Antes de qualquer alteração, o `status` do cabeçalho passa a `'0'` e só volta a `'1'`
no final da operação. Arquivos com `status` diferente de `'1'` não são consultados nem alterados.
Quando um arquivo está inconsistente ou truncado, o programa exibe
`Falha no processamento do arquivo.`.

---

## Testes

Os testes de integração do trabalho 2 estão em `tests/test_pops.py`. Eles usam pastas
temporárias e conferem a saída e o conteúdo dos arquivos binários:

```bash
make
python3 tests/test_pops.py --program ./programaTrab
```

---

## Conceitos trabalhados

- arquivos binários com `fread`, `fwrite` e `fseek`;
- registros de tamanho fixo (trabalho 1) e de tamanho variável (trabalho 2);
- acesso direto por RRN;
- remoção lógica, pilha de removidos e Worst Fit;
- índice primário ordenado, com busca por `idPoPs`;
- inserção e atualização de registros, com realocação quando o registro cresce;
- alocação dinâmica com `malloc`, `calloc`, `realloc` e `free`;
- modularização com arquivos `.c` e `.h`;
- filtragem por múltiplos critérios;
- controle de consistência por meio do cabeçalho.

---

> Este repositório possui finalidade acadêmica e foi desenvolvido seguindo as especificações fornecidas para os trabalhos.
