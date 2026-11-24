# Trabalho Prático 3 — ordenação, junção e grafos

O TP3 integra os binários **conectaPoPs** do TP1 e **PoPs** do TP2. A
funcionalidade 15 prepara os arquivos ordenados. A 16 mostra a junção. A 17
monta o grafo da rede, que serve de entrada para as consultas 18 a 20.
O [enunciado do TP3](SCC0607022026trabalho03.pdf) define o contrato.
As [orientações da aula](TRABALHO3_AULA.md) são leitura obrigatória e
registram as decisões de estrutura e algoritmo.
As [referências dos materiais](REFERENCIAS_TRABALHO3.md) indicam as páginas
de estudo e a aplicação de cada fonte.

## Entradas e mensagens

Cada comando representa uma execução independente de `programaTrab`.
O código e os argumentos são lidos da entrada padrão.

| Código | Primeira linha | Linhas seguintes | Resultado |
|---|---|---|---|
| 15 | `15 entrada.bin tipo ordenado.bin n` | `m campo`, uma linha por chave | Novo binário exibido por `BinarioNaTela`. |
| 16 | `16 PoPsOrdenado.bin idPoPs conectaPoPsOrdenado.bin idPoPs` | — | Junção agrupada por PoP. |
| 17 | `17 PoPsOrdenado.bin idPoPs conectaPoPsOrdenado.bin idPoPs` | — | Vértices e vizinhos, com velocidades em Mbps. |
| 18 | `18 PoPsOrdenado.bin idPoPs conectaPoPsOrdenado.bin idPoPs` | — | `Quantidade de ciclos: X`, incluindo zero para grafo acíclico. |
| 19 | `19 PoPsOrdenado.bin idPoPs conectaPoPsOrdenado.bin idPoPs n` | `m origem destino`, uma linha por consulta | Fluxo máximo em Mbps. |
| 20 | `20 PoPsOrdenado.bin idPoPs conectaPoPsOrdenado.bin idPoPs n` | `m origem destino parada`, uma linha por consulta | Menor soma de velocidades com a parada, em Mbps. |

- **15 e 16.** Em caso de erro, `Falha no processamento do arquivo.`.
  Junção sem qualquer correspondência retorna `Registro inexistente.`.
- **17 a 20.** Em caso de erro, `Falha na execução da funcionalidade.`.
  Nas consultas 19 e 20, a falta de um resultado tem resposta `-1` dentro
  da linha da consulta. O exemplo de 19 omite a unidade nesse caso.
- **16 a 20.** O primeiro binário está ordenado por `idPoPs`. O segundo,
  por `idPoPs` e depois `idPoPsConectado`. Ambos os nomes de campo na entrada
  são `idPoPs`. Isso vale também para a forma de entrada de 20 na página 12.
- **19 e 20.** Formatos de saída das páginas 11 e 13, respectivamente.
  `Fluxo máximo entre ORIGEM e DESTINO: X Mbps` e
  `Comprimento do caminho entre ORIGEM e DESTINO parando em PARADA: X Mbps`.

[Enunciado, pp. 3–13](SCC0607022026trabalho03.pdf#page=3).

## 15 — Ordenação interna e reconstrução do arquivo

A prioridade das chaves vem de `m`. A primeira chave que distingue dois
registros determina a ordem, sempre crescente. Em cada campo, um valor
presente vem antes do nulo. Dois nulos empatam e a comparação passa ao
próximo campo. Assim, `velocidade = -1` representa ausência e fica depois
de qualquer velocidade presente.

| Tipo | Binário de entrada | Campos de dados para ordenar | Como avançar na leitura |
|---|---|---|---|
| 1 | conectaPoPs (TP1) | `idPoPs`, `idPoPsConectado`, `velocidade`, `unidadeMedida` | Cabeçalho de 17 bytes, registros de 18 bytes. A posição do RRN é `17 + 18 × RRN`. |
| 2 | PoPs (TP2) | `idPoPs`, `nomePoPs`, `nomePais`, `siglaPais` | Cabeçalho de 25 bytes. Avançar pelo `tamanhoRegistro` físico, inclusive quando houver sobra de reuso. |

No tipo 1, `unidadeMedida` nula é `$`. No tipo 2, use `$$` para
`siglaPais` nula e um campo vazio delimitado por barra vertical para cada
nome nulo.

```text
15 PoPs.bin 2 PoPsOrdenado.bin 1
1 idPoPs
```

```text
15 conectaPoPs.bin 1 conectaPoPsOrdenado.bin 2
1 idPoPs
2 idPoPsConectado
```

O arquivo cabe em RAM. Leia, ordene e grave sequencialmente. A aula
orientou usar `qsort_r` para passar os critérios ao comparador como
contexto local. Compare pela prioridade das chaves e trate nulos antes
de comparar valores. Empates em todas as chaves ficam sem ordem
relativa especificada.

A saída é um novo arquivo com os registros ativos e o layout do tipo
selecionado. Recalcule os campos de controle a partir da saída. No tipo 1,
a pilha fica vazia, `nroRegRem` fica em 0 e o próximo RRN coincide com a
quantidade de registros gravados. `nroPares` conta os pares distintos
gravados. No tipo 2, a lista fica vazia, `nroRegRem` fica em 0, `nroPoPs`
conta os IDs distintos gravados e `proxByteOffset` aponta para o fim real,
começando em 25 num arquivo vazio. Gravar os registros variáveis com seu
tamanho recalculado elimina a fragmentação interna do arquivo novo. O TP3
explicita o descarte de removidos e deixa a sobra de reuso sem exemplo
específico. A saída conserva os nulos no formato binário de origem.

O formato de ambos os arquivos usa I/O binária campo a campo. Confira o
conteúdo e o cabeçalho final com status `'1'` antes de fechar e chamar
`BinarioNaTela`. Mantenha o arquivo original íntegro, inclusive se os
nomes de entrada e saída apontarem para o mesmo arquivo.

[TP3, pp. 3–4 e 13–14](SCC0607022026trabalho03.pdf#page=3),
[TP1, pp. 3–5](SCC0607022026TrabPrat01.pdf#page=3),
[TP2, pp. 3–5](SCC0607022026trabalho02.pdf#page=3),
[materiais de ordenação e arquivos](REFERENCIAS_TRABALHO3.md#ordenação-junção-e-arquivos).

## 16 — Junção cosequencial

Compare o `idPoPs` corrente dos dois fluxos ordenados. A chave menor pode
avançar porque os próximos registros do outro fluxo têm chave igual ou
maior que a chave corrente. Quando houver igualdade, reúna o grupo
consecutivo de conexões daquele ID antes de avançar o PoP. Essa é uma junção
por igualdade com uma linha por ID correspondente. As conexões do grupo
já vêm em ordem de `idPoPsConectado`. O percurso faz uma passagem pelos
arquivos, com custo linear no número de registros percorridos e campos
emitidos.

Separe o percurso da impressão. A funcionalidade 17 reutiliza o percurso
da junção para construir o grafo.

```text
idPoPs nomePoPs nomePais siglaPais idPoPsConectado velocidade unidadeMedida ...
```

A linha começa com os dados do PoP, seguidos de uma tripla por conexão do
grupo. Mostre `NULO` para cada campo ausente. Preserve a velocidade e a
unidade do registro de conexão. A conversão de unidades pertence ao grafo.
Pela definição da página 5, um ID sem igualdade não emite uma linha.

[Enunciado, pp. 5–7](SCC0607022026trabalho03.pdf#page=5),
[slides de processamento cosequencial, pp. 16–23](REFERENCIAS_TRABALHO3.md#ordenação-junção-e-arquivos).

## 17 — Grafo da rede

- **Vértices.** Cada PoP ativo tem uma posição no vetor ordenado por
  `idPoPs`, com `nomePoPs`, `nomePais` e `siglaPais`. Mantenha os PoPs sem
  adjacências no grafo e faça a correspondência entre ID e posição do vetor.
- **Arestas.** Cada conexão válida liga os dois IDs. Um enlace entre
  `u` e `v` está nas listas de ambos, com o mesmo peso. Conte o enlace
  uma única vez ao calcular o total de arestas. A lista de cada vértice
  fica em ordem crescente de ID do vizinho, inclusive após inserir
  as adjacências no sentido inverso do registro de entrada.
- **Pesos.** A p. 7 pede pesos em Gbps e a p. 8 pede saída em Mbps. Use a
  convenção do enunciado, `1 Gbps = 1024 Mbps`. Por exemplo,
  `200 M = 0,1953125 Gbps` e `1 G = 1024 Mbps`. O peso em Gbps precisa
  preservar a parte fracionária. Somas e capacidades podem ser conferidas
  na escala inteira de Mbps com largura suficiente para o produto e a soma.
- **Saída.** Use a ordem crescente de vértices e vizinhos. A p. 8 apresenta
  instruções e exemplo divergentes quanto à disposição dos vizinhos.
  Confira a tabela de pontos em aberto.

Listas de adjacência ocupam `O(V + A)` e permitem percorrer os vizinhos de
um PoP proporcionalmente ao grau. Os exemplos dos slides com uma inserção
por arco precisam ser lidos à luz do requisito de grafo não direcionado.

[Enunciado, pp. 7–8](SCC0607022026trabalho03.pdf#page=7),
[aulas de conceitos e TAD](REFERENCIAS_TRABALHO3.md#grafos).

## 18 — Ciclos simples

A DFS de aula detecta a existência de ciclos por arestas de retorno.
Num grafo não direcionado, a volta pela mesma aresta usada para chegar ao
vértice não forma ciclo. A contagem literal da p. 9 inclui todos os ciclos
simples, independentemente de onde começam e do sentido de percurso.
Marcar vértices do caminho corrente permite impedir repetições. Fixar o
menor ID do ciclo como início e aceitar apenas um sentido são formas de
contar cada ciclo uma vez num grafo simples.

Um quadrado com uma diagonal contém **três** ciclos simples, dois triângulos
e o quadrado. O posto do espaço de ciclos é **dois** nesse caso. A fórmula
`A − V + C`, com `C` componentes conexas, calcula esse posto. Cada aresta
fora da floresta de DFS dá um ciclo fundamental. Enumerar ciclos simples
pode exigir tempo exponencial, ainda que só se queira a contagem. A DFS
linear dos slides serve como base para detecção.

[Enunciado, p. 9](SCC0607022026trabalho03.pdf#page=9),
[aulas de busca e teste de ciclos](REFERENCIAS_TRABALHO3.md#grafos).

## 19 — Fluxo máximo

Nesta consulta, a velocidade representa uma **capacidade**. O fluxo pode
usar vários caminhos ao mesmo tempo, respeitando as capacidades dos enlaces
e a conservação do fluxo em cada PoP intermediário. Dois caminhos
independentes com gargalos de 3 e 4 Mbps permitem fluxo de 7 Mbps.

As anotações da aula indicam Ford–Fulkerson. Edmonds–Karp é a variante
que escolhe caminhos aumentantes na rede residual com BFS. Cada aumento
usa a menor capacidade residual do caminho. A capacidade reversa
permite redirecionar fluxo de aumentos anteriores. Para um enlace
bidirecional de capacidade `c`, a modelagem usual com capacidade `c` em
cada sentido permite cancelar fluxos opostos e obter o fluxo líquido
na aresta. Cada consulta começa com o estado residual inicial, mantendo
as capacidades da topologia disponíveis para as demais consultas.

[Enunciado, pp. 10–11](SCC0607022026trabalho03.pdf#page=10),
[aula de busca em largura e lacuna sobre fluxo](REFERENCIAS_TRABALHO3.md#grafos).

## 20 — Caminho mínimo com parada

A medida é a **soma** dos pesos. Dijkstra calcula distâncias com pesos
não negativos. Conforme a aula, execute Dijkstra em dois trechos, da
origem até a parada e da parada até o destino. Se ambos forem alcançáveis,
some as duas distâncias e converta para Mbps. Sem um dos trechos, o
resultado é `-1`.

Essa soma considera a possibilidade de os dois trechos passarem pelo
mesmo vértice. Se o corretor exigir um caminho globalmente simples, é uma
condição adicional e a soma pode produzir um percurso inadmissível.
O enunciado explicita a condição de simplicidade na [18] e usa a expressão
“caminho mínimo” na [20].

[Enunciado, pp. 12–13](SCC0607022026trabalho03.pdf#page=12),
[slides de caminho mais curto](REFERENCIAS_TRABALHO3.md#grafos).

## Pontos em aberto para conferir nos casos oficiais

| Ponto | Evidência e impacto |
|---|---|
| 15 — empates e strings | O PDF define ordem crescente e nulos por último. Ficam sem regra o desempate total, a colação de strings com acentos e a sobra física de registros reutilizados. Esses detalhes podem alterar o binário e o checksum. |
| 17 — quebras de linha | A descrição manda pular uma linha ao fim de cada elemento da lista. O exemplo agrupa três vizinhos de Frankfurt na mesma linha. Confira também espaços finais, caixa dos textos, nulos e a saída de vértices isolados. |
| 17 a 20 — arestas fora do caso usual | Velocidade ou unidade nula, ponta sem PoP ativo, laços e pares recíprocos precisam de tratamento consistente. O enunciado não detalha como cada um desses casos afeta o grafo e suas consultas. |
| 18 — significado de contagem | A p. 9 pede todos os ciclos simples. Uma saída oficial para um quadrado com diagonal distingue esse requisito da contagem de ciclos fundamentais. |
| 19 — fluxo indefinido | A p. 11 mostra `-1` sem unidade. Os casos de origem igual ao destino, ID ausente e caminho sem capacidade positiva precisam ser conferidos. |
| 20 — sintaxe e percurso | A p. 12 recebe dois binários ordenados e dois nomes de campo. O exemplo da p. 13 usa `20 topologiaRede.bin 1`. A tabela de comandos segue a definição da p. 12. Confira a exigência de um caminho globalmente simples e o formato da linha que termina em `-1`. |

## Casos para estudar e validar

| Código | Caso | Resultado a verificar |
|---|---|---|
| 15 | Dois tipos de binário, chaves empatadas, nulos, registro ativo com sobra e registro removido | Ordem pelas prioridades de `m`, nulos no fim de cada chave, saída com registros ativos, cabeçalho coerente, origem intacta e checksum da saída. |
| 16 | Um ID com duas conexões, outro sem correspondência e um grupo no fim do arquivo | Uma linha com duas triplas em ordem, sem linha para o ID não casado. Uma junção integralmente vazia devolve a mensagem prevista. |
| 17 | Enlace único em um sentido no arquivo, pesos `1 G` e `200 M`, vértice isolado | Adjacências nos dois sentidos, ordem crescente e conversões para `1024 Mbps` e `200 Mbps`. |
| 18 | Árvore, dois triângulos desconexos e quadrado com diagonal | Respectivamente `0`, `2` e `3` pela definição de ciclos simples. |
| 19 | Caminhos independentes de capacidades 3 e 4 Mbps, seguidos por consulta repetida e outra entre componentes distintos | `7 Mbps` nas duas primeiras consultas e `-1` na última, conforme o caso sem resultado do exemplo. |
| 20 | Arestas `1–2 = 2`, `2–4 = 3`, `1–4 = 1`, em Mbps. Consulta de 1 a 4 via 2 | `5 Mbps`. Ao isolar a parada, a consulta fica sem resultado. |

Nos CSVs do repositório, foram conferidos **108 PoPs** e **125 conexões**.
Os PoPs 40, 80 e 100 são isolados e os pares de conexão
aparecem em apenas um sentido. Há 111 enlaces em G e 14 em M. Na amostra do
repositório, `22–29` tem `3 G` (3072 Mbps pela regra da p. 7) e
`57–58` tem um enlace direto de `301 M`. Já o PDF exemplifica `3 Mbps`
para o primeiro e um fluxo de `78 Mbps` para o segundo. Use os arquivos
exatos de cada caso oficial ao comparar resultados.

[PoPs.csv](../Source/PoPs.csv), [conectaPoPs.csv](../Source/conectaPoPs.csv),
[enunciado, pp. 7–11](SCC0607022026trabalho03.pdf#page=7).

## Integração com o repositório e entrega

- O [despacho atual](../Source/main.c) vai até a funcionalidade 14. As 1 e 8
  geram os dois binários de origem. `auxiliar_1` e `auxiliar_2` concentram
  os codecs binários de [conectaPoPs](../Headers/auxiliares/auxiliar_1.h) e
  [PoPs](../Headers/auxiliares/auxiliar_2.h), respectivamente. Os helpers de
  PoPs usam offsets absolutos, com liberação de nomes alocados
  por `destroi_dados_pops`. As entradas 15 a 20 pedem argumentos diferentes
  das operações anteriores e compartilham a construção do grafo em 17 a 20.
- A ordenação produz novos RRNs ou offsets. Um índice primário do TP2
  refere-se ao arquivo de dados para o qual foi criado. Os comandos do TP3
  recebem os binários de dados e os campos de junção.
- O ZIP para o run.codes inclui fontes C documentados e `makefile`. O
  alvo `all` compila `programaTrab` e `run` o executa. As receitas usam
  tabulação. O código de matrícula indicado é `31DA`.
  O PDF pede nome e NUSP dos integrantes
  no início do código, escrita campo a campo com `stdio.h`, documentação
  de rotinas, variáveis e blocos, organização e modularização. A p. 17
  prevê desconto rigoroso pelo uso de IA na implementação.
- O vídeo vai ao e-disciplinas, tem até 7 minutos e apresenta as
  funcionalidades e suas implementações. Todos os integrantes participam,
  com tempos de fala semelhantes e webcam. O prazo consta na página da
  disciplina. O PDF informa uma divisão de 70% dos casos no run.codes e 30%
  na correção.

[Enunciado, pp. 13–18](SCC0607022026trabalho03.pdf#page=13).
