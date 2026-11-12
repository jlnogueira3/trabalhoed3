# Padrões atuais do código (antes das correções)

Este documento descreve como o código está escrito **hoje**, antes das correções e da implementação da parte 02. Serve de referência para manter a consistência ao corrigir e ao escrever código novo. Ele descreve o que existe, não o que deveria existir.

Arquivos analisados: `Source/main.c`, `Source/fornecidas.c`, `Source/auxiliares/auxiliar_1.c`, `Source/implementadas/funcionalidade1.c` a `funcionalidade7.c`, `Headers/registros.h`, `Headers/implementadas.h`, `Headers/fornecidas.h`, `Headers/auxiliares/auxiliar_1.h` e `makefile`.

---

## 1. Organização de arquivos e diretórios

- Diretórios: `Source/` e `Headers/` com inicial maiúscula. Subpastas `auxiliares/` e `implementadas/` em minúsculas.
- Nomes de arquivos em minúsculas, sem acento e sem espaço:
  - `funcionalidade1.c` … `funcionalidade7.c`
  - `auxiliar_1.c` / `auxiliar_1.h` (separador `_`)
- Cada funcionalidade tem seu próprio `.c` em `Source/implementadas/`. Protótipos ficam em `Headers/implementadas.h`.
- Arquivos `.h` usam include guards no formato `TRABALHOED3_<NOME>_H`, por exemplo `TRABALHOED3_REGISTROS_H`.
- Alguns cabeçalhos têm o bloco gerado pela IDE (`// Created by joao-lucas-nogueira on …`), com datas diferentes (08/09/2026 e 11/09/2026).
- `makefile` (minúsculo) compila com `gcc -Wall -Wextra -IHeaders` e lista `Source/*.c`, `Source/auxiliares/*.c` e `Source/implementadas/*.c`. Tem as diretivas `all`, `run` e `clean`, e `.PHONY`.

## 2. Nomes de variáveis, funções e campos

**Idioma:** português em quase tudo (`cabecalho`, `binario`, `criterios`, `atualizacoes`, `quantidade`, `escrita`, `leitura`). Os nomes dos campos seguem a especificação do trabalho e ficam em inglês/camelCase (`idPoPs`, `idPoPsConectado`, `velocidade`, `unidadeMedida`, `removido`, `encadeamentoPilha`, `proxRRN`, `nroRegRem`, `nroPares`).

**Estilo de nomes (não é uniforme):**

| Tipo | Exemplos encontrados | Padrão observado |
|---|---|---|
| Funções auxiliares | `inicializar_cabecalho`, `escrever_dados`, `leitura_dados`, `atende_criterios`, `falha_processamento_arquivo` | snake_case |
| Funções de funcionalidade | `funcionalidade1`, `funcionalidade7` | camelCase sem separador |
| Função fornecida | `BinarioNaTela`, `ScanQuoteString` | PascalCase (não alterar) |
| Campos de struct | `proxRRN`, `nroRegRem`, `topo_pilha`, `encadeamentoPilha` | misturado (`topo_pilha` em snake_case, `encadeamentoPilha` em camelCase) |
| Variáveis locais | `f`, `f3`, `f4`, `dado`, `dados`, `dados3`, `tester`, `temp` | nomes curtos e sufixos numéricos (`f3`, `f4`, `dados3`) sem motivo claro |
| Variáveis de campo CSV | `campo1`…`campo4`, `virgula1`…`virgula3` | numeradas |
| Constantes | `17L`, `18L`, `13`, `49`, `100` | números mágicos repetidos, sem `#define` |

**Padrões de nome que se repetem:**
- Ponteiros de arquivo: `FILE *f`, `FILE *leitura`, `FILE *escrita`, `FILE *f3`, `FILE *f4`. Os nomes mudam de uma funcionalidade para outra.
- Variável de cabeçalho: `cabecalho` (sempre ponteiro quando passado como parâmetro, variável local quando declarado na função).
- Variável de dado: `dado` ou `dados` (o uso alterna sem regra).
- Índice de laço: `i` (externo), `j` (interno), `k`, `rrn` (quando o índice é um RRN).
- Quantidades: `n` (número de operações), `quantidade`, `quantidade_criterios`, `quantidade_atualizacoes`, `quantidade_pares`. Em `funcionalidade5` o mesmo conceito aparece como `quantidade`.

## 3. Comentários

- **Idioma:** português, com acentuação correta na maioria dos casos (algumas linhas não têm acento, como `nao pode`, `pq`, `nn precisa`, `pulo`).
- **Estilo `//`:** é o mais usado.
  - Sem espaço depois de `//` em algumas linhas (`//Lê os registros…`, `//Realiza a remoção…`).
  - Com espaço em outras (`// Primeiro devemos…`).
  - Linhas vazias apenas com `//` (`//`) para separar blocos.
- **Estilo `/* */`:** usado para textos longos. Às vezes com `*` no início de cada linha e às vezes sem.
  - Exemplo: `/* * Separamos manualmente… */` em `funcionalidade1.c`.
  - Indentação dos comentários de bloco às vezes não coincide com o código ao redor (ver seção 4).
- **Cabeçalho de arquivo:** a maioria dos `.c` das funcionalidades começa com um comentário `//` de 2 linhas descrevendo o que a função faz (ex.: `//Realiza a remoção lógica…`). Os arquivos `funcionalidade1.c`, `funcionalidade2.c`, `funcionalidade3.c`, `funcionalidade4.c`, `funcionalidade6.c` e `funcionalidade7.c` seguem isso, mas não de forma idêntica.
  - Os arquivos `main.c`, `fornecidas.c` e `auxiliar_1.c` não seguem esse padrão.
- **Cabeçalho do aluno (exigido pela especificação, item [7]):** não encontrado nos arquivos analisados. Não há NUSP nem nome no início do código.
- **Tom e conteúdo:** os comentários são explicativos e às vezes conversacionais, com primeira pessoa e observações de processo:
  - `funcionalidade1.c`: “Um detalhe interessante é que eu (João) fiquei me batendo para chegar em fwrite…”
  - `auxiliar_1.h`: “Criei esse arquivo para ajudar…”
  - `funcionalidade7.c`: “modifiquei a parte do binário…”
  - `auxiliar_1.c`: “Começamos com a função atualizar cabecalho” (o nome da função não é `atualizar_cabecalho`).
  - Alguns comentários estão desatualizados ou incorretos. Ex.: `funcionalidade1.c` menciona “os 17 bytes do cabeçalho”, o que bate com o código atual, mas o comentário `// Primeiro devemos nos certificar de que o arquivo está completo` abre o `fopen` sem verificar nada além da abertura.
  - Há comentários soltos, sem texto (`//` vazio) e marcações como `// ignorar espaços, \r, \n...`.
- **Comentários em código comentado:** não encontrei blocos de código antigo comentado (exceto a linha `fgets` solta em `funcionalidade1.c`, sem comentário explicando o motivo de existir).
- **Idioma em comentários de referência:** `// As funções auxiliares usadas nesse arquivo estão em Source/auxiliares/auxiliar_1.c` é um bom exemplo de comentário que aponta para o local certo.

## 4. Formatação e espaçamento

- **Indentação:** 4 espaços (não usa tab no código C). O `makefile` usa tab, como exigido.
- **Quebra de linha dentro de chaves:** estilo K&R (`{` na mesma linha do `if`, `for`, `while`, `void f(...)`).
- **Espaço entre palavra-chave e parêntese:**
  - Usa `if (`, `for (`, `while (`, `switch (` na maior parte dos arquivos.
  - Mas `funcionalidade5.c` e `funcionalidade7.c` usam `if(`, `for(`, `while(` sem espaço.
- **Espaço depois da vírgula em argumentos:**
  - Na maioria: `fwrite(&cabecalho->status, sizeof(char), 1, arquivo);`.
  - Em alguns lugares falta espaço: `escrever_dados(dados,escrita);`, `escrever_cabecalho(cabecalho,escrita);`, `funcionalidade1(arg1,arg2,&cabecalho);`, `fseek(f, 0, SEEK_SET)` (ok), `int tester = atende_criterios(&dado, criterios, quantidade);` (ok).
- **Espaço em operadores:** `for(int i = 0; i<n; i++)` (sem espaço em `i<n`) em `funcionalidade5.c` e `funcionalidade7.c`. Em outros lugares: `i < n`.
- **Espaço em `*` de ponteiro:** `char *binario`, `FILE *f` (asterisco colado no nome). Consistente.
- **Espaço antes de `{`:** em geral há espaço (`) {`). Em `funcionalidade7.c` a assinatura é `void funcionalidade7(const char *binario, int n){`, sem espaço antes do `{`. No `implementadas.h` a declaração de `funcionalidade7` termina com `;` em uma linha separada (`    int n)` seguido de `;`).
- **Linhas vazias:**
  - Muitas linhas vazias seguidas em `funcionalidade1.c` (ex.: linhas 36-37, 82-84).
  - Linhas com espaços em branco no fim (trailing whitespace), por exemplo `funcionalidade5.c` linha 60.
  - Arquivos sem `\n` no final (o último caractere é `}` ou `f` de `#endif`): `funcionalidade2.c`, `funcionalidade4.c`, `funcionalidade5.c`, `funcionalidade6.c`, `funcionalidade7.c`, `auxiliar_1.c` e `implementadas.h`. Os demais terminam com quebra de linha.
- **Indentação inconsistente:**
  - `funcionalidade1.c`, linhas 29 a 115: o bloco principal está indentado uma vez a mais que o resto da função, sem `{` correspondente. Parece um bloco que foi movido.
  - Comentários com recuo diferente do código ao redor (ex.: `funcionalidade1.c` linhas 73 e 98-101).
- **Inicializadores designados:** usam `.campo = valor,` com uma linha por campo.
  - Padrão encontrado: `registro_dados dados = { .idPoPs = ..., };`.
  - Em `funcionalidade1.c` (linhas 48-53) há atribuições encadeadas dentro do inicializador (`.idPoPs = dados.removido = '0'`), o que é confuso e depende da ordem de avaliação.
  - Em `main.c` (linhas 10-16) o inicializador de `cabecalho` também se referencia (`cabecalho.status = '0'` dentro do próprio `{}`).
- **Comprimento de linha:** em geral até ~100 caracteres. Linhas acima de 100 colunas: `funcionalidade3.c` linha 11 (121 colunas), `auxiliar_1.c` linha 189 (102), `auxiliar_1.h` linhas 20 e 22 (104 e 102).
- **Cabeçalho `.h` com tipos sem prefixo:** `filtrar_dados` recebe `struct registro_dados dados` por valor, enquanto as outras funções recebem `registro_dados` (typedef). Os dois estilos aparecem.

## 5. Tipos, structs e typedefs

- `registros.h` define `typedef struct registro_cabecalho`, `typedef struct chave_valor` e `typedef struct registro_dados`. Os nomes das structs são em snake_case e os nomes dos campos variam (ver seção 2).
- Comentários de estado ficam logo abaixo da declaração, por exemplo: `//sempre que for utilizar, inicializar proxRRN com valor 0` e `//Sempre que for inicializar: .proxRRN = 0; .nroRegRem = 0; .encadeamentoPilha = -1;`.
- Os campos `idPoPs` e `idPoPsConectado` são `int`, e nulos são representados por `-1`. `unidadeMedida` é `char`, e nulo é `'$'`.
- Nomes de tamanho são mágicos:
  - Cabeçalho: 17 bytes (`1 + 4 + 4 + 4 + 4`), usado como `17L` em todo o código.
  - Registro de dados: 18 bytes (`1 + 4 + 4 + 4 + 4 + 1`), usado como `18L`.
  - Lixo preenchido por remoção: `for(int k = 0; k < 13; k++)` em `funcionalidade5.c`.
- Para chaves de busca, `chave_valor` usa `char chave[50]` e `char valor[50]`. Nas funções os buffers aparecem com tamanhos fixos diferentes (`char linha[100]`, `char velocidade[50]`, `%49s`).
- Literais de `scanf` usam `%49s` (limite de buffer), mas em `funcionalidade3.c` aparece `scanf("%s", p[j].chave)` sem limite.

## 6. Estilo de escrita das funcionalidades

- Cada funcionalidade começa com `FILE *f` (ou `f3`, `f4`), abre o arquivo, verifica e lê o cabeçalho, verifica `status`, marca `status = '0'` antes de alterar e volta para `'1'` no final.
- Padrão de tratamento de erro:
  - Em `funcionalidade4.c`, `5.c`, `6.c`, `7.c`: usa helpers de `auxiliar_1.c`, como `verificar_abertura_arquivo(f)`, `leitura_cabecalho(f, &cabecalho)` e `falha_processamento_arquivo(f)`.
  - Em `funcionalidade1.c`, `2.c` e `3.c`: repete `printf("Falha no processamento do arquivo.\n")` e `fclose` inline.
  - A mensagem também tem formatação diferente: `funcionalidade1.c` usa `printf("Falha no processamento do arquivo.")` sem `\n`, enquanto os outros usam com `\n`.
- Padrão de laço de busca: `for (int rrn = 0; rrn < cabecalho.proxRRN; rrn++)` com `fseek(f, pos, SEEK_SET)` antes de cada leitura. A posição é calculada como `17L + (long)rrn*18L`, repetida em `funcionalidade4`, `5`, `6` e `7`.
- Padrão de leitura do CSV (`funcionalidade1.c`): lê com `fgets`, troca cada vírgula por `\0` com `strchr` e converte com `atoi`. Não valida se há vírgulas suficientes na linha.
- Padrão de entrada (`funcionalidades 3, 5, 6, 7`): lê com `scanf` e, para campos `unidadeMedida`, usa `ScanQuoteString`. Para os demais campos usa `scanf("%49s")` e trata `"NULO"` manualmente.
- Código duplicado (mesmo trecho em mais de um lugar):
  - Leitura do cabeçalho (`fread` campo a campo) aparece em `funcionalidade2.c` e está também em `leitura_cabecalho`.
  - Bloco de atualização por campo (`if (strcmp(chave, "idPoPs")…) else if …`) aparece em `funcionalidade7.c`.
  - Parsing de critérios e atualizações (`scanf` + `ScanQuoteString` + tratamento de `NULO`) aparece duplicado em `funcionalidade5.c` e `funcionalidade7.c`.
  - Escrita do cabeçalho com `status` (`fseek`, `escrever_cabecalho`, `fflush`, `fseek`, `fwrite`) aparece em `funcionalidade5.c`, `6.c` e `7.c`.
- `main.c`: lê a opção com `scanf("%99s %99s %99s")` e converte com `strtol`. Cada `case` é um bloco com chaves `{}`. Os comentários dentro dos `case` explicam a conversão de tipos.

## 7. Comentários de depuração e observações do autor

- Há comentários que relatam processo de depuração, como o do `funcionalidade1.c` sobre `fwrite` e lixo de memória, e o do `funcionalidade1.c` sobre a troca de `const char` por `char` para usar `BinarioNaTela`.
- Há comentários de decisão, como em `main.c`: “O próprio compilador reclamou no scanf do uso direto do %d, então decidi fazer a conversão usando o strtol”.
- Esses comentários são úteis para quem leu o código, mas devem ser revisados antes da entrega, porque o texto fica preso à história do desenvolvimento e não ao que o código faz.

## 8. Dependências e funções fornecidas

- `Source/fornecidas.c` contém `BinarioNaTela` e `ScanQuoteString`, que são fornecidas pela disciplina. Não devem ser alteradas.
- `Headers/fornecidas.h` declara essas duas funções.
- `Headers/implementadas.h` declara as sete funcionalidades. Há uma declaração com a assinatura quebrada de forma incomum (`void funcionalidade7(const char *binario, int n)` seguido de `;` na linha seguinte).

## 9. Pontos de atenção que observei (para a fase de correções)

Esta seção não corrige nada. Apenas registra o que chamou atenção durante a leitura, para que a correção tenha uma lista de referência:

1. **Layout de arquivo:** o código atual usa cabeçalho de 17 bytes e registro de 18 bytes, com campos `idPoPs`, `idPoPsConectado`, `velocidade` e `unidadeMedida` (arquivo `conectaPoPs.csv`). A especificação do segundo trabalho (`docs/SCC0607022026trabalho02 (1).pdf`) descreve o arquivo de dados `PoPs`, com cabeçalho de 25 bytes (`status`, `topoLista`, `proxByteOffset`, `nroRegRem`, `nroPoPs`) e campos `idPoPs`, `siglaPais`, `nomePoPs`, `nomePais`. Essa diferença precisa ser verificada antes da parte 02.
2. **Chamadas aninhadas e atribuições dentro de inicializadores** (`funcionalidade1.c` 48-53 e `main.c` 10-16) são difíceis de ler e de manter.
3. **Duplicação** de leitura de cabeçalho, verificação de status e tratamento de erro, citada na seção 6.
4. **Números mágicos** `17L` e `18L` e o lixo de tamanho 13 deveriam ser constantes nomeadas.
5. **Consistência de mensagens de erro**: a mensagem sem `\n` em `funcionalidade1.c` difere das demais.
6. **Laços `for` e `if` sem espaço** e espaçamento irregular em `funcionalidade5.c` e `funcionalidade7.c`. Também há arquivos sem quebra de linha final (seção 4).
7. **Indentação quebrada** em `funcionalidade1.c` (bloco principal 29-115).
8. **Cabeçalho com nome e NUSP ausente** (exigência do item [7] da especificação).
9. **Comentários pessoais e de processo** (seção 7) devem ser reescritos de forma impessoal antes da entrega.
10. **Buffers de tamanho fixo** com `%49s` em alguns lugares e sem limite em outros (`funcionalidade3.c`).
