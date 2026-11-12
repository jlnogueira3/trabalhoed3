# Trabalho 2 — anotações de aula e análise dos materiais

Este arquivo reúne as anotações da imagem enviada pelo usuário e a análise textual recebida sobre o enunciado e os slides. As referências e classificações da análise foram preservadas; sua inclusão aqui não representa uma nova conferência dos documentos originais.

O contrato adotado na implementação está em [TRABALHO2.md](TRABALHO2.md). Uma escolha local de implementação não transforma uma interpretação ou pendência em regra confirmada da disciplina.

## Anotações da imagem

Transcrição das mensagens relacionadas ao trabalho, datadas de 13 de outubro de 2026 na imagem:

1. “nao usar ftell”
2. “nao fazer busca binaria”
3. “usar realloc com numero fixo”
4. “usar Quick sort da biblioteca do c”
5. “pode inserir tudo e depois ordenar”
6. “sem índice com busca binária ou linear no disco”
7. “tem que usar índice para busca com id”
8. “pre tratamento antes de começar a busca” — em resposta à anotação sobre busca com ID.

As outras mensagens visíveis na imagem tratam de assuntos diferentes e não compõem as orientações deste trabalho.

## Como interpretar essas anotações

- **`ftell`:** a orientação do usuário é não usar. A análise recebida registra que a proibição não foi encontrada nos slides; isso não exclui uma orientação oral específica.
- **Busca binária:** a orientação explícita é não fazer. A frase “sem índice com busca binária ou linear no disco” é ambígua e foi mantida literalmente, sem tratá-la como autorização para busca binária.
- **`realloc` com número fixo:** foi interpretado no projeto como crescimento da capacidade em blocos de tamanho fixo. O tamanho de 64 foi uma escolha local, não uma exigência confirmada.
- **“Quick sort da biblioteca”:** foi interpretado como usar `qsort` da biblioteca padrão de C. A anotação não exige escrever um algoritmo próprio de ordenação.
- **Inserir e ordenar depois:** permite acumular as entradas antes de ordenar. O índice final deve estar ordenado por `idPoPs` e refletir os dados atuais.
- **Busca com ID:** identificar se `idPoPs` está entre todos os critérios antes de iniciar a busca; nesse caso, consultar o índice e conferir também os demais critérios. Sem ID, a busca dos dados é sequencial conforme o enunciado.

## Relação com as decisões atuais do projeto

As decisões abaixo estão documentadas em `TRABALHO2.md` e não devem ser confundidas com exigências confirmadas pelos slides:

- `tamanhoRegistro` representa o tamanho físico total, incluindo os campos de controle.
- O arquivo de dados vazio finalizado tem `proxByteOffset = 25`, após o cabeçalho.
- A lista de removidos não é ordenada; novas remoções entram no início.
- Worst Fit percorre a lista inteira e escolhe o maior espaço suficiente. Em empate, fica com o primeiro encontrado.
- O espaço reutilizado conserva seu tamanho físico e a sobra recebe `$`.
- O índice fica em memória, é consultado linearmente e é ordenado com `qsort` antes da regravação final.
- Os vetores e strings crescem em blocos fixos de 64 elementos ou bytes, respectivamente.
- As posições encontradas são coletadas antes de cada solicitação de alteração para evitar reprocessar registros realocados.

## Análise recebida sobre o enunciado e os slides

O texto a seguir foi fornecido pelo usuário. As classificações “confirmado”, “inferido” e “não encontrado” pertencem à análise recebida. As referências indicam páginas dos PDFs ou questões do documento de exercícios, conforme informado naquela análise.

O enunciado confirma a escolha do **maior espaço disponível** e a preservação da sobra como lixo, mas não confirma uma lista ordenada por tamanho nem uma regra de desempate. O diagrama de Worst Fit mostra, inclusive, uma lista **não ordenada**. Também há uma ambiguidade relevante entre iniciar `proxByteOffset` com zero e usá-lo como posição absoluta após um cabeçalho de 25 bytes.

**Atenção para o outro chat:** o enunciado afirma que o uso de ferramentas de inteligência artificial para implementar as funcionalidades e outras funções necessárias ao embasamento do conteúdo acarretará desconto rigoroso na nota. Esta resposta fica restrita à análise solicitada. Enunciado, páginas PDF 24–25. SCC0607022026trabalho02.pdf

## Regras e evidências

As referências a seguir usam a **página do PDF**. Os slides examinados não apresentam uma numeração impressa independente claramente identificável; por isso, não atribuí números de slide diferentes dos números de página.

Abreviações para as fontes:

- **Enunciado:** SCC0607022026trabalho02.pdf
- **Reuso variável:** SCC0215012026reusoDinamicoVariavel.pdf
- **Índice primário:** SCC0215012026indicePrimario.pdf
- **Exercícios de índice:** Índice Simples ou Linear exercicios.docx
- **Arquivos binários em C:** MatheusCR-ArquivosBinariosEmC.pdf

| Questão | Regra encontrada | Arquivo e página | Evidência curta | Situação |
|---|---|---|---|---|
| Conteúdo de `tamanhoRegistro` | A leitura literal favorece o **tamanho total** do registro, mas não há fórmula explícita para esse campo no enunciado. | Enunciado, PDF 4 | Define-o como “número de bytes do registro” e inclui os campos de controle na representação. | **Inferido**, com boa sustentação |
| Excluir os 5 bytes iniciais | O exercício de índice define um indicador que exclui seus próprios 4 bytes, mas esse é outro arquivo e outra especificação. Não estabelece a convenção de PoPs. | Exercícios de índice, questão 4; enunciado, PDF 4 | O exercício diz expressamente “sem contar os 4 bytes”; o enunciado de PoPs não traz essa ressalva. | **Não encontrado** para PoPs |
| Próximo offset | Se `tamanhoRegistro` for o total físico, `próximo = offset_atual + tamanhoRegistro`. Se a interpretação for outra, a fórmula muda. | Enunciado, PDF 4–5 | O registro tem indicador de tamanho e campos delimitados; a fórmula não é apresentada. | **Inferido**, condicionado à convenção |
| Mínimo físico | O layout ocupa pelo menos **21 bytes**: 19 fixos mais dois delimitadores, inclusive com os dois campos variáveis nulos. | Enunciado, PDF 3–5 | Os dois valores variáveis nulos são representados apenas por `\|`. | **Inferido** diretamente do layout |
| Seleção Worst Fit | Escolher o maior espaço removido, desde que comporte o novo registro; se nenhum servir, inserir no final. | Reuso variável, PDF 8–9 e 14–15; enunciado, PDF 17 | “Worst-Fit” escolhe o maior espaço possível. | **Confirmado** |
| Ordem da lista | Não foi encontrada exigência de ordem decrescente. No exemplo, a lista inicial é `24 → 29 → 22` em tamanhos, portanto não está ordenada. | Reuso variável, PDF 7 e 14–15 | O exemplo percorre uma lista e remove dela o espaço de tamanho 29. | **Não encontrado** como obrigação; ordenação é uma opção |
| Empate e posição do recém-removido | Não foi encontrada regra que escolha entre espaços de mesmo tamanho ou determine se o mais recente vem antes. | Reuso variável, PDF 8–15; enunciado, PDF 15–19 | O exemplo mostrado tem tamanhos diferentes. | **Não encontrado** |
| Sobra no reuso | O exemplo preserva a sobra **dentro do registro** como fragmentação interna; o enunciado exige que o lixo remanescente seja `$`. | Reuso variável, PDF 14–15; enunciado, PDF 5 e 17 | O diagrama identifica a sobra como “Fragmentação Interna”. | **Confirmado** para o tratamento ilustrado |
| Indicador ao reutilizar | Preservar o valor que representa toda a área física é coerente com percorrer registros e com a fragmentação interna. O exemplo de Worst Fit usa outro formato, sem demonstrar o indicador ativo de PoPs. | Reuso variável, PDF 15; enunciado, PDF 4–5 | A área sobrante continua dentro do registro. | **Inferido**, dependente da convenção de tamanho |
| Encadeamentos | Ao reutilizar um espaço, ele deixa a lista: atualiza-se o topo ou o apontador do predecessor para o sucessor. O exemplo mostra o predecessor passando a apontar para o sucessor. | Reuso variável, PDF 14–15 | A ligação passa de `0 → 71 → 24` para `0 → 24` no diagrama. | **Confirmado** |
| Remoção lógica | Marcar removido, manter os campos de controle e preencher os bytes dos demais campos com `$`; remover a chave correspondente do índice. | Enunciado, PDF 15 | A exceção ao preenchimento é para os campos de controle. | **Confirmado** |
| Atualização no lugar | Se o registro atualizado couber, regravar no mesmo lugar e deixar eventual lixo `$` no final. | Enunciado, PDF 19 | Compara o tamanho atualizado ao tamanho do registro atual. | **Confirmado** |
| Sobra anterior na atualização | Considerar toda a área física disponível, inclusive fragmentação interna, é coerente com preservar o espaço do registro. O enunciado não usa essa formulação expressamente. | Enunciado, PDF 19; reuso variável, PDF 15 | O lixo permanece dentro do registro. | **Inferido** |
| Crescimento | Se não couber, remover logicamente o registro atual e inserir o atualizado como novo, aplicando Worst Fit. | Enunciado, PDF 19 | A regra de remoção seguida de inserção é expressa. | **Confirmado** |
| Evitar reprocessamento | Coletar previamente os offsets correspondentes é uma estratégia possível; não encontrei exigência ou algoritmo específico para isso. Cada uma das `n` solicitações precisa considerar o estado resultante das anteriores. | Enunciado, PDF 19–20 | A funcionalidade é executada `n` vezes seguidas. | **Não encontrado** como técnica obrigatória |
| Busca com e sem ID | Se `idPoPs` integrar os critérios, usar o índice e verificar todos os critérios; caso contrário, busca sequencial. Não exibir removidos. | Enunciado, PDF 12–13 | A busca deve satisfazer o critério completo, por um ou mais campos. | **Confirmado** |
| Índice em memória | O slide ensina carregar o índice integralmente por varredura sequencial e regravá-lo quando necessário. Não localizei no enunciado uma obrigação explícita de que as funcionalidades 11–14 façam esse carregamento integral. | Índice primário, PDF 13–15 | O slide descreve um índice em memória, geralmente como vetor. | **Confirmado como método ensinado**; obrigatoriedade específica **não encontrada** |
| Busca binária e disco | Os exercícios propõem busca binária em um índice já carregado em memória. O slide menciona que busca binária no disco pode exigir vários acessos. Não encontrei proibição geral de busca binária nos materiais examinados. | Exercícios de índice, questões 1 e 2(c); índice primário, PDF 15 | A lista pede descrever a busca binária em memória. | **Confirmado como opção didática**; escopo da anotação **pendente** |
| Ordenação e regravação | O índice em disco deve estar em ordem crescente de ID e refletir as alterações. Os slides descrevem reescrita a partir da memória quando necessário, sem fixar `qsort` nem o momento exato dentro de um lote. | Enunciado, PDF 6 e 15–20; índice primário, PDF 10, 12–14 | O índice exige reorganização e reescrita quando necessário. | **Confirmado** para o resultado; procedimento específico **não encontrado** |
| `qsort`, `realloc` e blocos de 64 | Não encontrei nos PDFs examinados menções a `qsort`, “quick sort” ou `realloc`, nem uma dimensão de bloco obrigatória. A anotação sobre “número fixo” permanece ambígua. | PDFs fornecidos; anotação de aula do usuário | A pesquisa textual não encontrou esses termos. | **Não encontrado** |
| IDs repetidos | `idPoPs` não pode ser nulo nem repetido. Não encontrei a resposta operacional exigida quando uma inserção ou atualização tenta criar duplicata. | Enunciado, PDF 3 e 5 | O CSV de entrada já garante a propriedade; a regra geral também a declara. | **Confirmado** para unicidade; tratamento da tentativa **não encontrado** |
| `proxByteOffset` inicial | O texto manda iniciá-lo em **0**; ao mesmo tempo, define-o como próximo byte offset disponível e o cabeçalho ocupa bytes 0–24. Logo, o primeiro offset absoluto de dados é **25**. O momento de passar de 0 para 25 não é explicitado. | Enunciado, PDF 2–3 | Inicialização literal e representação gráfica entram em tensão. | **Conflito/ambiguidade** |
| `nroPoPs` | É a quantidade de PoPs diferentes armazenados. Com IDs únicos e removidos fora do conjunto lógico, interpretá-lo como número de registros ativos é coerente; o enunciado não fornece uma tabela de transições. | Enunciado, PDF 3 e 5 | “Quantidade de PoPs diferentes” armazenados. | **Inferido** |
| Status | Para cada arquivo aberto para escrita, marcar `'0'` e, ao finalizar seu uso, `'1'`. Dados e índice possuem seus próprios status. | Enunciado, PDF 2 e 5–6 | A regra aparece nas duas descrições de cabeçalho. | **Confirmado** |

Há ainda uma diferença importante nas anotações: o material **Arquivos binários em C**, PDF 7, explica o uso de `ftell`; ele não estabelece a proibição anotada. Isso não invalida uma orientação oral específica da aula, mas ela não fica confirmada pelo documento.

## Exemplo de registro e próximo offset

Suponha um registro novo, sem sobra, com `nomePoPs = "NATAL"` e `nomePais = "BRAZIL"`, ambos em caracteres de um byte:

| Campo | Bytes |
|---|---:|
| `removido` | 1 |
| `tamanhoRegistro` | 4 |
| `encadeamentoLista` | 8 |
| `idPoPs` | 4 |
| `siglaPais` | 2 |
| `NATAL` e `\|` | 6 |
| `BRAZIL` e `\|` | 7 |
| **Total físico** | **32** |

Pela leitura literal de tamanho **total**, o valor armazenado seria `32`. Começando no offset absoluto `25`, o próximo registro começaria em `25 + 32 = 57`. Se o mesmo registro ocupasse uma área física reutilizada de 40 bytes, a interpretação coerente com preservar a área inteira seria armazenar `40`, preencher os 8 bytes finais com `$` e avançar até `65`. Essa última regra para o valor do indicador é **inferida**, não demonstrada diretamente para PoPs.

O exercício de índice usa explicitamente outro formato que exclui o indicador do tamanho; não se deve transportar automaticamente essa convenção para o trabalho.

## Worst Fit e desempate

Não há evidência suficiente para montar um exemplo que **defina** o desempate. Por exemplo, se a lista contiver dois espaços de 40 bytes, ambos são igualmente maiores; os materiais examinados não determinam qual deles será escolhido, nem se uma remoção recente deve ficar antes ou depois da antiga.

O exemplo efetivamente ilustrado no slide tem espaços de **24, 29 e 22 bytes**, nessa ordem na lista. Para um registro de 18 bytes, escolhe o de 29, atualiza o encadeamento para retirá-lo da lista e mantém a sobra como fragmentação interna. O formato de PoPs acrescenta a exigência de preencher esse lixo com `$`.

## Decisões provisórias

- **Manter como interpretação provisória, não como regra expressa:** `tamanhoRegistro` igual ao tamanho físico total, inclusive os 5 bytes iniciais. A redação do enunciado favorece essa leitura.
- **Manter:** mínimo físico de 21 bytes, deduzido dos campos e delimitadores.
- **Rever como exigência:** lista decrescente. Ela pode ser uma escolha de implementação, mas o exemplo ensinado não está nessa ordem.
- **Pendente:** colocar o removido mais recente antes dos antigos em empate. Não encontrei critério de desempate.
- **Manter:** reaproveitar a área inteira e identificar a sobra com `$`; preservar no indicador o tamanho físico inteiro é a interpretação coerente, ainda que não esteja demonstrada para esse formato específico.
- **Manter como escolha, não como obrigação comprovada:** carregar o índice em memória e usar busca linear. A busca binária em memória aparece nos exercícios, enquanto a anotação oral de proibição tem escopo incerto.
- **Pendente:** `qsort` e blocos de 64. Nem o nome da função nem esse tamanho foram confirmados.
- **Manter como estratégia possível:** coletar os offsets antes de modificar os registros de uma solicitação. Isso não foi encontrado como regra dos slides; a coleta deve ser refeita para cada solicitação, sobre o estado então existente.

Para os contadores, **se** `nroPoPs` for adotado como número de ativos, as transições coerentes são: remoção `nroRegRem + 1`, `nroPoPs − 1`; reutilização `nroRegRem − 1`, `nroPoPs + 1`; inserção no final `nroPoPs + 1` e avanço do próximo offset; realocação como remoção seguida de inserção resulta em variação líquida zero de `nroPoPs`, enquanto `nroRegRem` depende de haver ou não reuso. Essas transições são **inferências**, não uma tabela expressa no enunciado.

## Resumo para encaminhar

> O enunciado define `tamanhoRegistro` como número de bytes do registro e mostra os campos de controle como parte dele; tamanho total é a leitura favorecida, mas falta fórmula explícita. O mínimo físico é 21 bytes. Worst Fit escolhe o maior espaço suficiente; o exemplo da aula usa lista não ordenada, não define desempate e conserva a sobra como fragmentação interna. No trabalho, lixo deve ser `$`. Atualização que cabe permanece no local; se cresce além do espaço, remove e reinsere. Busca com `idPoPs` usa índice e confere os demais critérios; sem ID é sequencial. Índice final deve estar ordenado e refletir IDs e offsets atuais.
>
> Pendências: desempate, escopo da proibição oral de busca binária, significado de `realloc` com número fixo, obrigatoriedade de `qsort`, tratamento de tentativa de ID duplicado e convenção definitiva de `tamanhoRegistro`. O texto manda inicializar `proxByteOffset` em 0, mas o primeiro offset absoluto após o cabeçalho é 25; esclarecer o momento da transição. O enunciado também prevê desconto rigoroso pelo uso de IA na implementação.