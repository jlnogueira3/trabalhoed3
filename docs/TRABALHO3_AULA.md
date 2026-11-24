# Trabalho Prático 3 — orientações da aula

Leitura obrigatória para análise, planejamento e revisão das funcionalidades
15 a 20. Este arquivo organiza as anotações do grupo. O
[enunciado do TP3](SCC0607022026trabalho03.pdf) define o contrato de entrada,
saída e formato binário. O [guia do TP3](TRABALHO3.md) reúne os exemplos e as
divergências já encontradas no PDF.

## 15 — Ordenação por múltiplos campos

1. Leia os registros válidos do arquivo binário para um vetor em RAM.
   Separe as rotinas de ordenação de **PoPs** e **conectaPoPs**, pois os
   formatos, os campos e os cabeçalhos são diferentes.
2. Use `qsort_r` e passe a lista de critérios e sua quantidade no
   contexto do comparador. Mantenha esse contexto local à ordenação,
   sem variável global. Para a variante GNU, defina `_GNU_SOURCE` antes
   dos headers de sistema e use `<stdlib.h>`. Confira a disponibilidade
   e a assinatura no ambiente de compilação da disciplina.
3. Compare dois registros pela ordem de prioridade dos critérios. Use
   comparação numérica para IDs e outros campos inteiros, e `strcmp`
   para texto. O nome de cada campo pode virar um identificador inteiro
   para facilitar a seleção do critério. O primeiro resultado diferente
   de zero decide a ordem. Se os valores forem iguais, passe para a
   próxima chave. Quando todas empatarem, o comparador retorna zero.
4. Trate os nulos pela regra do enunciado. Um valor presente vem antes de
   um nulo, e dois nulos empatam naquele critério. A professora informou
   que os nomes de campo recebidos correspondem ao tipo de arquivo.
5. Grave os registros ordenados em **um novo binário**, aberto do zero.
   Preserve o original. Recalcule o cabeçalho do novo arquivo, incluindo
   contadores, próximo RRN ou offset, estado da lista ou pilha e status.
   Mantenha os bytes e as mensagens conforme o enunciado.

Nas anotações, `QSOR_R` e `gnc_source` referem-se a `qsort_r` e
`_GNU_SOURCE`. [Manual do Linux](https://man7.org/linux/man-pages/man3/qsort_r.3.html),
[notas de portabilidade do Gnulib](https://www.gnu.org/software/gnulib/manual/html_node/qsort_005fr.html).

## 16 — Junção pelo ID do PoP

- Abra os dois binários já ordenados e leia um registro corrente de cada
  um. Compare `idPoPs` com `idPoPs`.
- Avance apenas o leitor da menor chave. Quando as chaves coincidirem,
  reúna todos os registros consecutivos de conectaPoPs daquele ID.
  Depois do grupo, avance o leitor de PoPs e mantenha a próxima conexão
  já lida para a comparação seguinte.
- Acompanhe o fim de cada arquivo com um estado do respectivo leitor.
  O laço de junção segue enquanto houver registro corrente válido nos dois
  fluxos. Separe fim normal de erro de leitura. Processe o último grupo
  mesmo quando a leitura seguinte encontrar o fim do arquivo.
- Exiba os campos do PoP e das conexões correspondentes na ordem do
  enunciado. Exiba `NULO` para os campos ausentes. As velocidades e
  unidades são as do registro de entrada nesta funcionalidade.
- A junção percorre os próprios arquivos ordenados. Quando uma estrutura
  de índice auxiliar for necessária no TP3, mantenha-a em RAM.
- Separe o percurso da junção da apresentação. A 16 imprime o resultado
  do percurso compartilhado e a 17 reutiliza o percurso para montar o grafo.

## 17 — Construção do TAD grafo

- Reutilize o percurso de junção da 16 para associar os PoPs às conexões.
  Mantenha todos os PoPs ativos no conjunto de vértices, inclusive aqueles
  sem grupo correspondente na junção.
- Represente a topologia como **grafo não direcionado, ponderado e com
  listas encadeadas de adjacência**. O vetor contém os vértices e as cabeças
  das listas, em ordem crescente de `idPoPs`. As listas ficam em ordem
  crescente de `idPoPsConectado`. Um enlace válido deve aparecer como
  adjacência dos dois vértices.
- É possível ordenar ao inserir ou depois de carregar. Qualquer
  consumidor recebe o grafo com vértices e listas já ordenados.
- Armazene os pesos em **Gbps**. Para valores em Mbps, divida por 1024
  e preserve a parte fracionária. A saída da 17 segue o enunciado em Mbps.
- Modele o grafo como TAD compartilhado por 17 a 20. Mantenha em módulos
  coesos a construção, o acesso às adjacências e a liberação da estrutura.
  Uma `struct` pública é aceitável quando a interface realmente precisar
  expor sua representação. Mantenha no `.h` somente o que os consumidores
  precisam.

## 18 — Quantidade de ciclos

Abra os dois arquivos, carregue o grafo pelo mesmo TAD e execute o algoritmo
de ciclos estudado em aula. Imprima a **quantidade** no formato do enunciado.
Identifique nos slides qual algoritmo atende à contagem pedida. Os slides
consultados mostram detecção de ciclos por DFS. A página 9 pede a contagem
de ciclos simples. Um quadrado com uma diagonal contém três ciclos simples
e dois ciclos independentes. Use esse caso para verificar o que o algoritmo
proposto está contando e registre eventual incompatibilidade para confirmação.

[Enunciado, p. 9](SCC0607022026trabalho03.pdf#page=9),
[materiais de grafos](REFERENCIAS_TRABALHO3.md#grafos).

## 19 — Fluxo máximo

Use os pesos como capacidades de transmissão. A anotação “fort fluxon”
foi interpretada como **Ford–Fulkerson**. A capacidade residual pertence à
consulta e deve começar no estado inicial para cada par de PoPs. Preserve
as capacidades da topologia para as consultas seguintes.

Faça os cálculos com a conversão do enunciado. O grafo usa Gbps, e o valor
exibido usa **Mbps**, multiplicando por 1024. Preserve valores fracionários
durante o cálculo e confira a formatação da linha final na página 11 e
nos exemplos oficiais.

[Enunciado, pp. 7 e 10–11](SCC0607022026trabalho03.pdf#page=7),
[referência de fluxo máximo](https://web.stanford.edu/class/archive/cs/cs161/cs161.1166/lectures/lecture17.pdf).

## 20 — Caminho mínimo com parada obrigatória

Faça **duas aplicações de Dijkstra**, reutilizando a mesma rotina. A primeira
calcula a menor soma de pesos da origem até a parada. A segunda calcula da
parada até o destino. Se os dois trechos existirem, some as distâncias e
converta o resultado de Gbps para **Mbps**. Se algum trecho for inalcançável,
use a resposta de ausência especificada no enunciado.

[Enunciado, pp. 12–13](SCC0607022026trabalho03.pdf#page=12),
[slides de caminho mais curto](REFERENCIAS_TRABALHO3.md#grafos).
