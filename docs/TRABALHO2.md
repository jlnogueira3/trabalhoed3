# Trabalho 2 — funcionalidades 11 a 14

As anotações de aula e a análise recebida sobre o enunciado e os slides estão em
[Referências do trabalho 2](REFERENCIAS_TRABALHO2.md), incluindo evidências,
interpretações e pontos ainda ambíguos.

Esta parte manipula o arquivo de dados **PoPs** e o índice primário por `idPoPs`.
As funcionalidades 1 a 7 continuam destinadas ao arquivo **conectaPoPs** do
trabalho 1. Os dois formatos de dados são diferentes.

## Funcionalidades implementadas

| Código | Operação |
|---|---|
| 11 | Busca com todos os critérios informados; usa o índice quando há `idPoPs`. |
| 12 | Remoção lógica, inclusão na lista de espaços livres e remoção da chave do índice. |
| 13 | Inserção com Worst Fit; quando não há espaço suficiente, grava no final. |
| 14 | Atualização no próprio espaço ou remoção e reinserção quando o registro cresce. |

As quatro recebem `codigo arquivoDados.bin arquivoIndice.bin n`, seguido das
`n` solicitações. As funcionalidades 12 a 14 mostram, ao final, os resultados
de `BinarioNaTela` para dados e índice, nessa ordem, depois de fechar os arquivos.

## Organização

- `Headers/pops.h`: estruturas exclusivas do trabalho 2 e constantes do formato.
- `Headers/auxiliares/auxiliar_2.h`: interface compartilhada pelas operações.
- `Source/auxiliares/auxiliar_2.c`: leitura e escrita binária, busca, índice e espaços livres.
- `Source/auxiliares/pops_entrada.c`: leitura das entradas, strings, nulos e validação de campos.
- `Source/implementadas/funcionalidade11.c` a `funcionalidade14.c`: sequência de cada operação.
- `Source/PoPs.csv`: CSV fornecido, usado nos testes. A primeira coluna sem nome é ignorada pelos testes.
- `tests/test_pops.py`: testes independentes de integração, com arquivos temporários.

O Makefile já inclui os novos arquivos pelo padrão de diretórios. O CMake também
lista os novos fontes. Não há chamadas novas a `ftell` nem busca binária.
A função `BinarioNaTela` fornecida pela disciplina permanece inalterada.

## Contrato para integrar as funcionalidades 8, 9 e 10

Essas três funcionalidades ainda não estão implementadas. As operações 11 a 14
recebem arquivos binários previamente criados; não convertem CSV automaticamente.
Quem implementar 8 a 10 deve usar o mesmo contrato abaixo.

### Cabeçalho de dados: 25 bytes

| Offset | Campo | Bytes |
|---|---|---:|
| 0 | `status` | 1 |
| 1 | `topoLista` | 8 |
| 9 | `proxByteOffset` | 8 |
| 17 | `nroRegRem` | 4 |
| 21 | `nroPoPs` | 4 |

Os offsets são absolutos. Depois de escrever o cabeçalho, o próximo offset de
um arquivo vazio é **25**. O enunciado menciona inicialização em zero; aqui,
25 representa a posição efetiva após a gravação dos 25 bytes do cabeçalho.
Não se deve entregar para 11 a 14 um cabeçalho finalizado com `proxByteOffset = 0`.

`nroPoPs` é interpretado como quantidade de registros ativos, com IDs únicos.
O cabeçalho vazio tem `topoLista = -1`, `nroRegRem = 0` e `nroPoPs = 0`.

### Registro de dados: tamanho variável

| Offset relativo | Campo | Bytes |
|---|---|---:|
| 0 | `removido` | 1 |
| 1 | `tamanhoRegistro` | 4 |
| 5 | `encadeamentoLista` | 8 |
| 13 | `idPoPs` | 4 |
| 17 | `siglaPais` | 2 |
| 19 | `nomePoPs`, seguido de `\|` | variável |
| após o nome | `nomePais`, seguido de `\|` | variável |
| após os campos | sobra preenchida com `$`, se houver | variável |

**Convenção adotada:** `tamanhoRegistro` contém o tamanho físico total,
incluindo os campos de controle. A redação do enunciado favorece essa leitura,
mas a análise dos materiais não encontrou uma fórmula explícita para PoPs.

- Registro novo: `21 + bytes(nomePoPs) + bytes(nomePais)`.
- Próximo registro: `offsetAtual + tamanhoRegistro`.
- Exemplo: nomes `NATAL` e `BRAZIL` ocupam um registro novo de 32 bytes.
  Começando no offset 25, o próximo começa em 57.
- Se esse conteúdo ocupar um espaço reutilizado de 40 bytes, o tamanho permanece
  40 e a sobra de 8 bytes recebe `$`.
- `siglaPais` nula ocupa `$$`. Nomes nulos são representados somente por `|`.
- Strings não levam `\0` ao disco. A escrita é feita campo a campo.

### Índice

O cabeçalho tem apenas `status`, de 1 byte. Cada entrada tem `idPoPs` de 4 bytes
e `byteoffset` de 8 bytes, totalizando 12 bytes, sem preenchimento entre campos.
O índice contém somente registros ativos, em ordem crescente de ID.

O índice é carregado em memória. Antes de uma busca, todos os critérios são
examinados para identificar `idPoPs`, mesmo quando ele não é o primeiro campo.
Quando presente, usa-se busca linear no índice em memória, acessando diretamente
o offset encontrado e conferindo os demais critérios. Sem ID, a busca é sequencial
no arquivo de dados. Registros removidos têm somente o controle lido; o restante
é pulado por posicionamento no arquivo.

As alterações atualizam o índice em memória a cada registro. Ao finalizar o lote,
`qsort` ordena as entradas e o arquivo é regravado, eliminando entradas antigas.
Os vetores crescem em blocos fixos de 64 elementos; strings, em blocos de 64 bytes.
O valor 64 é uma escolha local, não uma exigência confirmada dos slides.

### Worst Fit e atualização

A lista não precisa estar ordenada. A remoção inclui o registro no início da
lista; a inserção percorre a lista inteira e escolhe o maior espaço suficiente.
O espaço escolhido é retirado da lista atualizando o topo ou o predecessor.
A sobra permanece no mesmo registro, sem divisão em um novo espaço livre.

**Desempate adotado:** o primeiro maior espaço encontrado na lista. Essa é uma
escolha determinística de implementação, pois os materiais analisados não
definem o desempate. Com inserção de removidos no início, isso favorece o mais
recentemente removido quando os tamanhos são iguais.

Uma atualização compara o conteúdo novo com todo o espaço físico atual,
incluindo sobra anterior. Quando cabe, mantém o offset e o tamanho físico.
Quando não cabe, remove e reinsere por Worst Fit, corrigindo o índice.

Cada solicitação coleta seus offsets antes de começar a alterar registros,
evitando reprocessar um registro realocado. A solicitação seguinte faz uma nova
busca sobre o estado atualizado. A ausência de resultados não interrompe o lote.

### Consistência e entradas inválidas

O status é verificado antes de ler o restante do cabeçalho. Consultas usam `rb`;
alterações usam `r+b`; a regravação final do índice usa `wb`.
Antes de modificar os registros, ambos os arquivos são marcados com status `0`.
Os retornos de escrita e de descarga dos buffers são conferidos antes de marcar
os arquivos como consistentes.

IDs nulos ou duplicados são rejeitados com `Falha no processamento do arquivo.`.
Essa resposta para tentativas de duplicação é uma escolha local, pois a análise
não encontrou um comportamento específico exigido para esse caso. Uma falha
depois de iniciar alterações mantém o arquivo de dados inconsistente; não há
rollback das alterações já realizadas. Trabalhe com cópias ao testar entradas inválidas.

## Compilação e testes

Compile normalmente com `make` ou com o comando equivalente do README.
Depois, execute os testes usando Python 3:

```sh
python tests/test_pops.py --program ./programaTrab
```

No Windows, use o caminho do executável `.exe`.
Os testes geram os binários de entrada diretamente, pois 8 e 9 ainda não existem.
Eles cobrem o CSV fornecido, buscas indexadas e sequenciais, critérios combinados,
nulos, ausência de resultados, strings longas, crescimento dos vetores, remoções,
Worst Fit em lista não ordenada, empate, atualização no lugar, realocação,
alteração de ID e arquivos inconsistentes ou truncados.

Além da saída, os testes verificam os bytes de preenchimento, limites dos registros,
contadores, lista de removidos e correspondência exata entre índice e dados.
Eles validam o contrato adotado aqui; não substituem os casos oficiais do run.codes
nem confirmam as convenções que os materiais deixaram ambíguas.
