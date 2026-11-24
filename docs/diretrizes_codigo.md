# Diretrizes de código

Regras derivadas das correções apontadas em `docs/correcao_01.txt`. Valem para qualquer código novo ou alterado neste projeto. A descrição do estado anterior do código está em `docs/padroes_codigo.md`.

---

## 1. Abertura de arquivos (`fopen`)

Use o modo que corresponde ao que a função vai fazer com o arquivo:

| Operação | Modo | Exemplo no projeto |
|---|---|---|
| Criar arquivo binário novo (só escrita) | `wb` | funcionalidade 1 |
| Ler arquivo binário | `rb` | funcionalidades 2, 3, 4 |
| Modificar arquivo binário existente | `rb+` | funcionalidades 5, 6, 7 |
| Ler arquivo texto (CSV) | `r` | funcionalidade 1 (`conectaPoPs.csv`) |

- Não use `w+b` quando a função só escreve. Use `wb`.
- Não abra o arquivo em modo de escrita para apenas ler.
- O CSV é texto. Por isso usa `r` e não `rb`. O modo binário vale apenas para o arquivo de dados.

## 2. Cabeçalho

- **O cabeçalho não é criado no `main`.** Cada funcionalidade declara o próprio `registro_cabecalho` local, quando precisa dele.
- **Verifique o status antes de ler o cabeçalho inteiro.** Use `leitura_cabecalho(f, &cabecalho)`. Ela lê primeiro o byte de `status`, retorna 0 se o arquivo estiver inconsistente, e só então lê os demais campos.
- **Inicialize o cabeçalho apenas na criação do arquivo** (`inicializar_cabecalho`, usada na funcionalidade 1).
- **Ao alterar o arquivo**, marque `status` como inconsistente antes de começar e como consistente ao final, sempre com `salvar_cabecalho`:

```c
cabecalho.status = STATUS_INCONSISTENTE;
salvar_cabecalho(f, &cabecalho);
// ... alterações ...
cabecalho.status = STATUS_CONSISTENTE;
salvar_cabecalho(f, &cabecalho);
```

- Use as constantes `STATUS_CONSISTENTE` e `STATUS_INCONSISTENTE`, nunca `'0'` ou `'1'` diretamente.

## 3. Registros removidos

- **Não leia o conteúdo de um registro removido.** `leitura_dados` lê primeiro o byte `removido`. Se for `'1'`, ela pula os bytes restantes com `fseek` e retorna `LEITURA_REMOVIDO`.
- Trate o retorno sempre com os três casos:

```c
int resultado = leitura_dados(&dado, f);
if (resultado == LEITURA_ERRO) { falha_processamento_arquivo(f); return; }
if (resultado == LEITURA_REMOVIDO) continue;
```

- **Exceção documentada:** ao inserir em posição removida (funcionalidade 6), é preciso saber o campo `encadeamentoPilha` do registro do topo. Nesse caso, só esse campo é lido, com `fseek(f, pos + sizeof(char), SEEK_SET)`, e o restante do registro não é lido.

## 4. Inicialização de variáveis

- **Não inicialize registros e cabeçalhos quando o valor vem do arquivo.** Se a função vai ler o registro do disco, a inicialização é desnecessária (funcionalidades 2, 3, 4, 5, 7).
- **Atribua valores padrão apenas quando o registro vai ser escrito e há campos que podem ser nulos.** Isso acontece na funcionalidade 1 (campos vazios no CSV) e na funcionalidade 6 (campos informados como `NULO`).
- Nesses casos, atribua campo a campo, sem atribuições encadeadas dentro do inicializador:

```c
// Correto
dado.removido = '0';
dado.encadeamentoPilha = -1;
dado.velocidade = -1;
dado.unidadeMedida = '$';

// Evitar
registro_dados dados = { .idPoPs = dados.removido = '0', ... };
```

## 5. Modularização: use os helpers, não repita código

Operação de disco que aparece em mais de uma funcionalidade deve estar em `Source/auxiliares/auxiliar_1.c`. Helpers existentes:

| Helper | Uso |
|---|---|
| `leitura_cabecalho(f, &c)` | lê o cabeçalho, verificando o status antes |
| `salvar_cabecalho(f, &c)` | grava o cabeçalho inteiro no byte 0 e faz `fflush` |
| `posicao_registro(rrn)` | calcula o byte offset de um registro |
| `leitura_dados(&d, f)` | lê um registro e retorna `LEITURA_OK`, `LEITURA_REMOVIDO` ou `LEITURA_ERRO` |
| `escrever_dados(d, f)` | grava um registro completo na posição atual |
| `ler_pares(n)` | lê `n` pares campo/valor de busca ou atualização |
| `atende_criterios` / `filtrar_dados` | verificam se um registro atende aos critérios |
| `falha_processamento_arquivo(f)` | imprime a mensagem de falha e fecha o arquivo |
| `verificar_abertura_arquivo(f)` | imprime a falha se o `fopen` retornou `NULL` |
| `print_dados(d)` | imprime um registro com `NULO` no lugar dos nulos |

Regras:
- Antes de escrever uma sequência de `fread`/`fwrite`/`fseek` numa funcionalidade, verifique se já existe helper.
- Se a mesma sequência aparecer em duas funcionalidades, crie um helper em `auxiliar_1.c` e declare-o em `auxiliar_1.h`.
- Não use números mágicos para tamanhos. Use `TAM_CABECALHO`, `TAM_REGISTRO` e as demais constantes de `registros.h`.
- Não use valores literais de status. Use `STATUS_CONSISTENTE` e `STATUS_INCONSISTENTE`.
- Não use valores literais de retorno de leitura. Use `LEITURA_OK`, `LEITURA_REMOVIDO` e `LEITURA_ERRO`.

## 6. Mensagens de saída

- Falha: sempre `falha_processamento_arquivo(f)` ou `verificar_abertura_arquivo(f)`. A mensagem é `Falha no processamento do arquivo.` seguida de quebra de linha.
- Busca sem resultado: `Registro inexistente.` seguida de quebra de linha.
- Cada registro impresso ocupa uma linha.

## 7. Entrada da busca e da atualização

- Leia critérios e atualizações com `ler_pares`. Ele trata `NULO` (valor vazio) e `unidadeMedida` (lido com `ScanQuoteString`).
- Não leia pares com `scanf` diretamente dentro das funcionalidades.
- Não use `ScanQuoteString` para campos inteiros. Essa função usa `scanf("%s")` depois do primeiro caractere e lê o token seguinte quando o valor tem um dígito.

## 8. Comentários

- **Descreva cada função** com um comentário logo acima da assinatura, dizendo o que ela faz e o que retorna.
- **Comente antes de cada loop relevante** e antes de cada operação direta no disco (`fseek`, `fwrite`, `fread`, `fflush`), explicando o motivo e não a sintaxe.
- **Não deixe trechos longos sem comentário** no meio da função.
- Escreva em português, de forma impessoal. Não use primeira pessoa (`eu`, `fiquei`) nem registre o processo de depuração.
- Comentários de bloco (`/* ... */`) ficam para explicações longas. Para o restante, use `//` com espaço depois.
- Não deixe comentários desatualizados. Se mudar o código, atualize o comentário ao lado.

## 9. Formatação

- Indentação de 4 espaços, sem tabs no código C.
- Estilo de chaves: `{` na mesma linha da assinatura, `if`, `for`, `while`.
- Espaço depois de palavras-chave: `if (`, `for (`, `while (`, `switch (`.
- Espaço depois de vírgula em argumentos e em operadores: `i < n`, `a, b`.
- Asterisco de ponteiro colado ao nome: `char *binario`, `FILE *f`.
- Sem espaço em branco no fim das linhas.
- Linha em branco entre funções. Não use mais de uma linha em branco seguida.
- Linhas com até 100 colunas.
- Arquivo termina com quebra de linha.

## 10. Nomes

- Funções auxiliares: `snake_case` (`leitura_dados`, `salvar_cabecalho`).
- Funções de funcionalidade: `funcionalidadeN` (camelCase, sem separador).
- Constantes e macros: `MAIÚSCULAS_COM_SUBLINHADO` (`TAM_REGISTRO`, `STATUS_CONSISTENTE`).
- Campos de struct: use os nomes da especificação (`idPoPs`, `unidadeMedida`, `proxRRN`, `nroRegRem`). Não crie nomes novos para campos já definidos.
- Variáveis: nomes descritivos. Evite `f3`, `f4`, `dados3`, `tester`, `temp`.
- Ponteiro de arquivo: `f`, ou nome que descreva o papel (`leitura`, `escrita`), nunca `fN` com número.

## 11. Estrutura de arquivos

- `main.c` apenas lê a opção, converte os argumentos e chama a funcionalidade. Não declare cabeçalho nem registros no `main`.
- Protótipos ficam nos arquivos `.h`. Nenhuma função é usada antes de ser declarada.
- Cada funcionalidade tem seu próprio `.c` em `Source/implementadas/`.
- Operações de disco compartilhadas ficam em `Source/auxiliares/auxiliar_1.c`.
- Arquivos de cabeçalho usam include guard no formato `TRABALHOED3_NOME_H`.
- `Source/fornecidas.c` (com `BinarioNaTela` e `ScanQuoteString`) não é alterado.

## 12. Ordem no fim da funcionalidade

1. Gravar o cabeçalho final (`salvar_cabecalho`).
2. Fechar o arquivo (`fclose`).
3. Chamar `BinarioNaTela` com o nome do arquivo, depois de fechá-lo.

Em todos os caminhos de erro, o arquivo deve ser fechado antes do `return`. Use `falha_processamento_arquivo(f)`, que já fecha o arquivo.

## 13. Antes de considerar a alteração pronta

- Compilar com `make`, sem avisos (o makefile usa `-Wall -Wextra`).
- Gerar os arquivos binários com a versão antiga e com a nova, a partir da mesma entrada, e comparar com `cmp`. Se a alteração não deve mudar a escrita, os arquivos têm de ser idênticos.
- Testar os caminhos de erro: arquivo inexistente, arquivo com status `'0'` e busca sem resultado.

## 14. Documentação (pareceres dos monitores)

- **Comentário acima de cada função** dizendo o que ela faz, o que retorna e seus efeitos colaterais. Exemplo: "deixa o ponteiro do arquivo logo após o cabeçalho" ou "move o ponteiro para o fim do registro".
- **Comente antes de cada loop relevante** e antes de cada operação direta no disco. Em funções longas, comente também os blocos internos (separar campos, gravar registro, atualizar pilha).
- **Use o mesmo formato de comentário em todo o projeto**: mesmo idioma, mesmo estilo de `//`, mesmo espaçamento. Não misture estilos entre arquivos.
- **Mantenha os comentários atualizados.** Comentário que descreve algo que o código não faz deve ser corrigido ou removido na mesma alteração.
- **Explique o raciocínio, não só o passo.** Ex.: "não usamos `fseek` aqui porque o cursor já está no byte correto".
- **Indentação consistente** em todo o código, com linhas em branco separando blocos lógicos dentro das funções.
- **Cabeçalho obrigatório no início de cada arquivo `.c` e de cada `.h`:** nome e NUSP de cada integrante do grupo, como comentário. Restrição [7] da especificação: sem isso, a nota do aluno cujos dados não constam no código fonte não é atribuída. Use um bloco padrão, por exemplo:

```c
/*
 * João L A Nogueira - N usp: 16986970
 */
```

Quando o grupo tiver mais de um integrante, uma linha por integrante, no mesmo formato.

- **Documentação de procedimentos, funções, variáveis e blocos** (restrição [8]). Documente também variáveis globais e campos de struct que não são óbvios. A ausência de documentação reduz a nota de forma expressiva.

## 15. Modularização (pareceres dos monitores)

- **Separe em arquivos `.c` e `.h`, agrupando funcionalidades relacionadas.** Esta é a regra da especificação do trabalho 1 (itens (i) e (ii) de "Modularização"): separação em `.c` e `.h`, e agrupamento de funcionalidades semelhantes ou relacionadas no mesmo arquivo. Não coloque cada funcionalidade num arquivo separado, porque vários pareceres pediram isso e a especificação não pede.
  - Estrutura sugerida (a confirmar antes da correção final):
    - `consulta.c/.h`: funcionalidades 2, 3 e 4 (somente leitura);
    - `manipulacao.c/.h`: funcionalidades 5, 6 e 7 (remoção, inserção e atualização, todas com pilha de removidos);
    - `criacao.c/.h`: funcionalidade 1 (criação do arquivo a partir do CSV);
    - `cabecalho.c/.h`: leitura, escrita, status e salvamento do cabeçalho;
    - `registro.c/.h`: posicionamento, leitura, escrita e impressão de registros;
    - `criterios.c/.h`: `ler_pares`, `atende_criterios` e `filtrar_dados`;
    - `erros.c/.h`: mensagens de falha e verificação de abertura de arquivo;
    - `main.c`: apenas despacha a opção.
  - Cada `.h` declara só as funções do seu `.c`. Não declare protótipos de um módulo dentro de outro.
- **Mantenha a separação de pastas:** headers em `Headers/` e código em `Source/`. Esta regra já está sendo seguida.
- **Crie funções pequenas, com uma responsabilidade cada.** Exemplos de funções úteis: escrever cabeçalho, ler cabeçalho, escrever registro, ler registro, mudar status do arquivo, montar filtro de busca/atualização, verificar status, abrir arquivo.
- **Centralize a abertura de arquivos.** Uma única função de abertura, com a mensagem de erro padrão e o retorno padronizado, evita repetição e facilita a leitura.
- **Se um bloco se repete em duas funcionalidades, ele vira função.** Isso vale também para trechos pequenos, como verificações de status e montagem de filtros.
- **Declare o tipo de retorno correto.** Função que não retorna valor é `void`. Função que retorna código de status deve retornar esse código. Não deixe funções declaradas como `int` sem `return`.
- **Use `#define` para códigos de retorno e tamanhos**, como já feito com `LEITURA_OK`, `TAM_REGISTRO` e `STATUS_CONSISTENTE`.
- **Depois de chamar uma função de erro, retorne.** Chamar a função de erro e continuar a execução causa comportamento errado.
- **Separe leitura de escrita.** Função que só escreve um registro não deve também atualizar contadores do cabeçalho. Quem atualiza o contador é a funcionalidade que chama a escrita.

## 16. Implementação com arquivo (pareceres dos monitores)

- **Escreva e leia campo a campo.** Nunca use `fwrite(&struct, ...)` para o registro ou o cabeçalho. O padding da struct muda o tamanho dos bytes e quebra a especificação. Cada campo deve ser gravado com o tamanho exato definido no trabalho.
- **Use o cursor do arquivo.** Depois de `fread` ou `fwrite`, o ponteiro já avança para o próximo byte. Não use `fseek` quando o cursor já está no lugar certo:
  - escrita sequencial de registros (funcionalidade 1): não use `fseek` entre um registro e outro;
  - leitura sequencial após o cabeçalho (funcionalidades 2 e 3): depois de `leitura_cabecalho`, o cursor já está no primeiro registro;
  - use `fseek` só para ir a um RRN específico (`posicao_registro`), para voltar ao início de um registro já lido ou para pular um registro removido.
- **Não use `ftell` dentro de loops** nem para descobrir o tamanho do arquivo. O número de registros vem do cabeçalho (`proxRRN`).
- **Registro removido: pule com `fseek`, não leia o conteúdo.** Já implementado em `leitura_dados`.
- **Não leia o mesmo registro duas vezes.** Na remoção e na atualização, leia uma vez e use os valores lidos. Na busca, faça uma única passagem.
- **Não carregue os registros em memória.** Não crie vetor com todos os registros nem com todos os RRNs. Processe um registro por vez.
- **Status do cabeçalho:**
  - Funcionalidades que alteram o arquivo (5, 6, 7) gravam `status = '0'` **no disco** antes de começar e `status = '1'` **no disco** ao final. Não basta alterar a variável em memória.
  - Para marcar o status, grave só o byte de status, com um helper que faz `fseek(f, 0)` e `fwrite` de um byte. Não regrave o cabeçalho inteiro só para isso.
  - O cabeçalho inteiro é regravado no fim, porque os contadores (`proxRRN`, `nroRegRem`, `topo_pilha`, `nroPares`) mudaram.
  - Funcionalidades somente de leitura (2, 3, 4) **não** alteram o status.
  - Na funcionalidade 1, o cabeçalho é escrito com `status = '0'` antes dos registros e regravado com `status = '1'` no fim (a especificação exige isso). Um parecer defendeu escrever o cabeçalho só no fim, mas ele contraria a especificação. Siga a especificação.
- **Contador `nroPares` (trabalho 1):** a especificação define `nroPares` como a "quantidade de pares (idPoPs, idPoPsConectado) diferentes que estão armazenados no arquivo de dados". O par não pode se repetir.
  - **Inserção (funcionalidade 6): sempre incrementar `nroPares`**, tanto quando reaproveita registro removido quanto quando acrescenta no fim. Cada inserção adiciona um par novo. **Hoje a funcionalidade 6 não faz isso: é um erro a corrigir.**
  - **Remoção (funcionalidade 5): a especificação não diz se o par removido deixa de contar.** A palavra "armazenados" sugere que registros logicamente removidos ainda estão no arquivo, o que mantém o contador. A outra leitura é que só pares de registros válidos contam, e nesse caso a remoção decrementa. Confirme com o monitor antes da correção final e registre a decisão aqui.

## 17. Apresentação em vídeo (pareceres dos monitores)

- Máximo de 7 minutos. Vários pareceres apontaram vídeo longo como problema. Ensaie o tempo.
- Todos os integrantes participam com tempo parecido de fala. Divisão desigual foi apontada em vários grupos.
- Explique cada funcionalidade e como ela foi implementada, e também a modularização. Não gaste tempo com introdução longa.
- Webcam obrigatória. Tela legível: fonte grande, boa iluminação e código sem janelas sobrepostas.
- Fale em ritmo que o monitor consiga acompanhar. Áudio sem falhas e sem dessincronia com a tela.

## 18. Pendências do código atual (levantadas com os pareceres)

- **Estrutura:** `Source/auxiliares/auxiliar_1.c` e `Headers/auxiliares/auxiliar_1.h` concentram quatro responsabilidades (cabeçalho, registro, critérios de busca e erro). Separar em arquivos próprios, cada um com seu `.h`.
- **Status dos arquivos 5, 6 e 7:** `salvar_cabecalho` regrava o cabeçalho inteiro no início só para marcar `status = '0'`. Trocar por um helper que grava só o byte de status (seção 16).
- **`fseek` desnecessário no loop:** as funcionalidades 5 e 7 fazem `fseek(f, pos, SEEK_SET)` no início de cada iteração, mesmo quando o cursor já está no registro seguinte. Usar o cursor e chamar `fseek` apenas antes de regravar o registro.
- **Lixo da remoção:** a funcionalidade 5 grava os 13 bytes de lixo com um `fwrite` por byte. Gravar com um único `fwrite` de um buffer de `'$'`.
- **Funcionalidade 6:** o `fseek` para `posicao_registro(proxRRN)` está correto, mas precisa de comentário explicando que é o único caso de escrita em posição que não é o cursor atual.
- **`nroPares` na funcionalidade 6:** verificar a regra com a especificação do trabalho 1 (seção 16).
- **Funções com efeito colateral não documentado:** `leitura_cabecalho` deixa o ponteiro logo após o cabeçalho. Esse efeito precisa estar escrito no comentário da função.
- **`filtrar_dados` recebe `registro_dados` por valor.** Passar por ponteiro `const`, como `atende_criterios` já faz, para manter o padrão.
- **Mensagens de erro e de busca** estão repetidas em várias funcionalidades. Avaliar um helper para "Registro inexistente.".
- **Cabeçalho com NUSP e nome ausente em todos os arquivos.** Pela restrição [7], é o item de maior risco de nota zero. Corrigir antes de tudo.
- **`nroPares` não incrementado na funcionalidade 6.** Erro certo (seção 16).
- **Estrutura de arquivos:** o projeto tem um arquivo por módulo auxiliar, mas `funcionalidade1.c` a `funcionalidade7.c` ficam separados. Agrupar conforme a seção 15.
- **Comentários de depuração e de primeira pessoa** podem ter sobrado em arquivos antigos. Conferir todos os `.c` e `.h`.

## 19. Pendências conhecidas (não tratadas nesta rodada)

- `filtrar_dados` e `atende_criterios` tratam `idPoPs` e `idPoPsConectado` com `strtol`. Um valor `NULO` informado para `idPoPsConectado` vira `0`, e a busca por nulo não acha os registros. Pela especificação, a busca por nulo deve usar `NULO` e comparar com `-1`.

## 20. Requisitos de entrega e critérios de correção (especificações dos trabalhos 1 e 2)

**Entrega (run.codes):**
- Um `.zip` com o código-fonte documentado (`.c` e `.h`) e o `makefile`. O zip é enviado ao run.codes. Código de matrícula indicado na especificação: `31DA`.
- No `makefile`, a diretiva `all` contém só o comando de compilação (`gcc -o programaTrab ...`), e `run` contém só `./programaTrab`. Usar tabs nas linhas de comando.
- Nome do executável: `programaTrab`. O programa recebe a funcionalidade como primeira entrada da entrada padrão.

**Vídeo (e-disciplinas):**
- No máximo 7 minutos. Mostrar cada funcionalidade e uma breve descrição de como foi implementada, e a modularização.
- Todos os integrantes participam, com tempo de fala aproximadamente igual. Webcam obrigatória.
- Enviar o link informando o nome de cada integrante e o número do grupo. Link que não abre recebe nota zero.
- Integrante que não participa da apresentação recebe zero no trabalho correspondente.

**Critérios de correção (a especificação não dá pesos numéricos):**
- Corretude da execução.
- Atendimento às especificações do cabeçalho e dos registros de dados.
- Atendimento à sintaxe de cada comando e ao formato de saída.
- Qualidade da documentação interna, que tem "peso considerável".
- Estruturação (arquivos e diretórios) e modularização, sem duplicação de código.
- Casos de teste: 70% disponíveis no run.codes, 30% usados na correção.

**Penalidades explícitas:**
- Erro de compilação: nota zero na parte do trabalho.
- Ausência de documentação: diminuição expressiva da nota.
- Não atendimento às especificações do cabeçalho, dos registros ou da sintaxe de comandos: diminuição expressiva.
- Trabalho feito com alunos de turmas diferentes: diminuição expressiva.
- Palavras ofensivas em arquivos ou em qualquer material entregue: nota zero na parte.
- Plágio: nota zero para todos os trabalhos envolvidos.
- **Uso de ferramentas de inteligência artificial.** A regra consta no trabalho 1, com desconto rigoroso, e está ausente dos critérios do trabalho 2. O [enunciado do Trabalho Prático 3](SCC0607022026trabalho03.pdf#page=17) também prevê desconto rigoroso pelo uso de IA na implementação.
- **Observação da capa (trabalhos 1 e 2):** "A solução deve ser proposta exclusivamente pelos alunos com base nos conhecimentos adquiridos nas aulas." Essa frase não é critério de penalidade, mas vale ter em mente ao revisar o código.

**Restrições técnicas (trabalho 2, reforçadas pelo trabalho 1):**
- Dados gravados em modo binário. Modo texto não pode ser usado para o arquivo de dados.
- Nomes e ordem dos campos não podem ser alterados. Todos os campos devem estar presentes, e nenhum campo adicional pode ser incluído.
- Escrita campo a campo, nunca registro a registro (restrição [6]).
- Funções da biblioteca `<stdio.h>` para leitura e escrita dos arquivos. Não usar outra linguagem.
- Sem truncamento de dados e sem tratamento de página de disco.
- Mensagens de erro e avisos conforme a especificação de cada funcionalidade.

## 21. Arquivo de dados PoPs (trabalho 2): valores confirmados pelo checksum do run.codes

- `tamanhoRegistro` é o **tamanho total do registro**, incluindo `removido` (1 byte) e o próprio campo `tamanhoRegistro` (4 bytes). Confirmado: com a versão que não contava esses 5 bytes, o checksum saiu 540 abaixo do esperado (108 registros × 5).
- `proxByteOffset` começa em **25** (tamanho do cabeçalho), porque os byte offsets são absolutos. A especificação diz "iniciado com '0'", mas o checksum esperado só bate com o valor 25 no cabeçalho.
- Saída esperada para `8 PoPs.csv PoPs.bin` com o `PoPs.csv` de 108 registros: `3778.840000`.

## 22. Índice primário indexaPoPs (funcionalidade 9)

- O índice tem 1 byte de cabeçalho (`status`) e registros de 12 bytes: `idPoPs` (4) + `byteoffset` (8). Os valores de `byteoffset` são os offsets absolutos do registro no arquivo de dados.
- Os registros do índice são ordenados por `idPoPs` crescente (`qsort`). Só entram registros válidos do arquivo de dados (`removido == '0'`).
- O índice só é criado depois que o arquivo de dados foi lido por completo. Se o arquivo de dados estiver inconsistente ou corrompido, nenhum índice é gravado.
- Status do índice: `'0'` durante a gravação e `'1'` ao final.
- Os dados de entrada são lidos em `rb`, e o arquivo de dados não é alterado.

## 23. Trabalho Prático 3

O [guia do TP3](TRABALHO3.md) descreve as funcionalidades 15 a 20.
Na ordenação da 15, o enunciado permite manter os dados em RAM e exige
um novo binário sem registros logicamente removidos. O layout herdado
de cada tipo determina a leitura e a escrita campo a campo.

As funções 16 a 20 recebem os arquivos de dados ordenados. A seção de
mensagens do guia registra as diferenças entre a junção e as
funcionalidades de grafo. Para os critérios de entrega do TP3, consulte
as [páginas 13 a 18 do enunciado](SCC0607022026trabalho03.pdf#page=13).
