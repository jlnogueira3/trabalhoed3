# Trabalho Prático 3 — referências dos materiais

Os 28 anexos foram consultados. As páginas abaixo são as posições no PDF,
contadas a partir da capa. Para o PowerPoint, a numeração é a dos slides.
O [guia do TP3](TRABALHO3.md) reúne o contrato das operações 15 a 20 e
os casos para validação. Os nomes dos anexos externos permitem localizá-los
no material da disciplina.

## Enunciado

| Anexo | Onde consultar | O que extrair |
|---|---|---|
| [SCC0607022026trabalho03.pdf](SCC0607022026trabalho03.pdf) | pp. 3–4 e 5–7 | Ordenação interna por múltiplas chaves com nulos por último. Junção por igualdade em dois fluxos ordenados com agrupamento das conexões por PoP. |
| Mesmo enunciado | pp. 7–13 | Grafo ponderado não direcionado, listas ordenadas, regra de conversão, ciclos simples, fluxo máximo e caminho mínimo com parada. |
| Mesmo enunciado | pp. 13–18 | Restrições sobre o layout binário, escrita campo a campo, critérios de correção e entregas. |

## Ordenação, junção e arquivos

| Anexo | Onde consultar | Aplicação ao TP3 |
|---|---|---|
| `SCC0607022026ordenacaoCosequencial.pptx.pdf` | pp. 3–5, 14–17 e 18–23 | **15:** leitura sequencial, ordenação em RAM e escrita sequencial. **16:** matching avança a menor chave e termina quando um dos fluxos acaba. A junção do TP3 consome todos os registros de conexão do mesmo ID. A parte de ordenação externa e intercalação de vários arquivos é contexto. |
| `MatheusCR-ArquivosBinariosEmC.pdf` | pp. 3–10 e 15–18 | Modos de `fopen`, posicionamento, retorno de `fread` e `fwrite`, padding de structs e inspeção dos bytes. Em 15, o arquivo novo precisa manter o layout campo a campo. |
| `SCC0215012022organizaRegistros.pdf` | pp. 3–10 | Distingue registro fixo com acesso por RRN de registro variável com indicador de tamanho. Fundamenta os dois percursos de leitura em 15–17. |
| `SCC0215012022organizaCampos.pdf` | pp. 2–5 e 9–11 | Campos fixos, variáveis e delimitadores. Em PoPs, `siglaPais` ocupa dois bytes e os nomes são delimitados por `\|` segundo o TP2. |
| `SCC0215012022organizaHibrido.pdf` | pp. 2–5 | Fundamenta a combinação de metadados fixos e nomes variáveis no mesmo registro PoPs. O arquivo da aula usa outra codificação. |
| `SCC0215012022reusoDinamicoVariavel.pdf` | pp. 2, 8–11 e 14–15 | Lista de espaços por byte offset e tamanho, Worst Fit e fragmentação interna. O tamanho físico guia a leitura mesmo quando há sobra no registro ativo. |
| `SCC0215012026reusoDinamicoFixo.pdf` | pp. 2–5 e 11–12 | Pilha de removidos por RRN em registros fixos. A leitura deve pular o espaço do removido e continuar no próximo registro. |
| `SCC0215012026reusoEstatico.pdf` | pp. 2–3 e 10–12 | Reconstrução do arquivo com descarte dos removidos. É o análogo da saída de 15, que também é ordenada e preserva o arquivo de origem. |
| `SCC02152026armazenamentoSecundario.pdf` | pp. 5–6 e 8–14 | Acesso sequencial reduz reposicionamentos. Posição corrente e organização em cabeçalho e registros ajudam a entender o percurso binário. |

Os slides de matching exemplificam uma interseção de chaves. No TP3,
`idPoPs` é único em PoPs e pode se repetir em conectaPoPs. Cada valor
casado pode, portanto, contribuir com várias triplas para a mesma linha.
No arquivo ordenado de PoPs, a sequência de bytes pode mudar durante a
reconstrução, e o índice do TP2 referencia os offsets do arquivo de origem.

## Grafos

| Anexo | Onde consultar | Aplicação ao TP3 |
|---|---|---|
| `Aula01_grafos_conceitos.pdf` | pp. 21–31 | Caminho, ciclo, componente conexa, árvore e floresta. Útil para distinguir isolamento, ausência de caminho e detecção de ciclos. |
| `Aula02_grafos_TAD.pdf` | pp. 17–27 e 30–31 | Vetor de listas, armazenamento de vizinhos e pesos, custo em grafos esparsos e representação de vizinhos ordenados. É a fonte direta para 17. |
| `Aula03_grafos_BuscaTerca01.pdf` | pp. 4–7, 25, 27–31, 49 e 65 | BFS com fila e DFS com pilha, marcação, arestas de árvore e retorno. A BFS é base para buscar alcançabilidade. A DFS sustenta a identificação de ciclos e componentes. |
| `Aula05_grafos_aplicacoesTerca08.pdf` | pp. 2–10 | Teste de grafo cíclico por aresta de retorno. Para 18, estude a diferença entre detectar um ciclo e contar todos os ciclos simples. A ordenação topológica a partir da p. 11 é para DAGs. |
| `SCC0607022022GrafosCaminhoMaisCurto.pdf` | pp. 2–3, 25–28 e 53–61 | BFS minimiza saltos com pesos iguais. Dijkstra trabalha com soma das arestas e dá distância infinita aos não alcançáveis. Vetor de predecessores e heap são tópicos opcionais para 20, cuja saída pede apenas a distância. |
| `transp-cap7.dvi.pdf` | §§ 7.2.2–7.2.3, 7.3–7.5 e 7.9 | Ziviani, capítulo 7. Listas por ponteiros ou arranjos, liberação da estrutura, DFS/BFS, teste de aciclicidade e relaxamento em Dijkstra. Os exemplos de TAD usam numeração interna de vértices. |
| `aula06_grafos_mstQuinta.pdf` | pp. 2–4, 24–28 e 47–48 | Prim e Kruskal fornecem contexto sobre árvores geradoras e seleção de arestas sem ciclos. Uma árvore geradora serve de referência para o número de ciclos independentes, enquanto a consulta 18 pede ciclos simples da topologia inteira. |

O enunciado pede fluxo máximo na funcionalidade 19. O material de grafos
fornecido aborda BFS, Dijkstra e árvores geradoras. A busca em largura é
o componente aproveitável para estudar Edmonds–Karp, que usa capacidades
residuais. A teoria específica de fluxo máximo fica como estudo complementar.

## Índices em árvore-B

Os comandos do TP3 recebem binários de dados ordenados e consultam um
grafo em RAM. As aulas de árvore-B dão contexto sobre índices em memória
secundária e o custo de ler páginas.

| Anexo | Onde consultar | O que extrair |
|---|---|---|
| `SCC05032026arvoreBintroducao.pdf` | pp. 2–6 | Hierarquia de páginas, chaves separadoras, descendentes e ordem. |
| `SCC0215202201arvoreBdefinicaoFormal.pdf` | pp. 2–5 e 10–16 | Limites de chaves e filhos, requisitos de ocupação, níveis e relação entre ordem, tamanho da página e altura. |
| `SCC05032026arvoreBpesquisa.pdf` | pp. 2–8 | Descida a partir da raiz pelo intervalo de chaves. As referências aos nós são RRNs de páginas e `-1` encerra uma busca malsucedida. |
| `SCC05032026arvoreBinsercao.pptx` | slides 2–11 | Inserção na folha, divisão de nós cheios, promoção da chave e criação de nova raiz quando a divisão chega ao topo. |
| `SCC0215202201algoritmos.pdf` | pp. 2–9 e 10–34 | Layout didático da página com número de chaves e RRNs dos filhos. Busca e inserção trabalham dentro de cada página e entre páginas. |
| `SCC0215202201arvoreBremocao.pdf` | pp. 2–9, 10–13 e 14–26 | Remoção, redistribuição entre irmãos, fusão e possível redução de altura. O descarte de registros removidos na 15 é uma reconstrução de arquivo de dados. |

## Armazenamento e pesquisa complementar

| Anexo | Onde consultar | O que extrair |
|---|---|---|
| `socm-17.pdf` | cap. 17, §§ 17.1–17.3 | Paginação em disco, faltas de página e substituição de páginas pelo sistema operacional. Contexto para localidade e custo de I/O. |
| `Memórias Flash.pdf` | pp. 4–7 | Leitura por página, apagamento por bloco, custo assimétrico e FTL. Contexto para o meio físico de armazenamento. |
| `Novas perspectivas no sistema de gerenciamento de dados.pdf` | pp. 18–30 e 37 | Operações em NAND, atualização fora do lugar, coleta de lixo, nivelamento de desgaste e um exemplo de indexação ciente da memória flash. |
| `artigo2.pdf` (*Spatial Indexing on Flash-Based Solid State Drives*) | pp. 1–4 | Motivação do eFIND e uso de buffers para lidar com a assimetria de leitura e escrita em índices espaciais. |
| `Porting disk-based spatial index structures to flash-based solid state drives.pdf` | pp. 1–3 e 41–42 | Avaliação da adaptação de R-tree, R*-tree, Hilbert R-tree e xBR+-tree a SSD usando eFIND, com resultados dependentes da carga de trabalho. |

Essas referências explicam por que padrões de acesso importam. No TP3, o
aproveitamento prático é ler e gravar sequencialmente quando a operação
permite. As estruturas de SSD e os índices espaciais pertencem ao
contexto de armazenamento.
