# Criptografia em Duas Camadas — Cifra de César + Sequências Numéricas

**Autor:** Gabriel Braes Gramacho

## 1. Descrição

Programa em C que criptografa uma palavra (até 15 letras, sem acentos) em duas camadas:

| Camada | Técnica | Como funciona |
|---|---|---|
| 1 | Cifra de César | Todas as letras são deslocadas pelo mesmo `SHIFT` |
| 2 | Deslocamento dinâmico | A letra `i` é deslocada também pelo termo `i` de uma sequência numérica (PA, PG, Fibonacci ou primos) |

O deslocamento total da letra `i` é: **`SHIFT + sequencia[i]`** (módulo 26).

O resultado é gravado em `resultado_criptografia.txt` e cada execução gera registros em `log_execucao.txt`.

## 2. Como compilar e executar

O programa usa apenas a biblioteca padrão do C, então qualquer compilador comum serve. Foi testado com `gcc` e `clang` (padrões C99 e C11), sem avisos.

### Linux

```bash
gcc -Wall -Wextra -o criptografia criptografia.c
./criptografia
```

`-Wall -Wextra` mostram avisos e `-o criptografia` define o nome do executável. Para compilar sem avisos extras, basta `gcc -o criptografia criptografia.c`.

### macOS

```bash
xcode-select --install        # só na primeira vez, instala o compilador
clang -Wall -Wextra -o criptografia criptografia.c
./criptografia
```

### Windows

- **Code::Blocks** (versão "with MinGW"): abra o `criptografia.c` e use **Build and run** (F9).
- **Dev-C++**: abra o arquivo e use **Compilar e Executar** (F11).
- **Terminal com MinGW (gcc):**

```bat
gcc -o criptografia.exe criptografia.c
criptografia.exe
```

### Online (sem instalar nada)

Em um compilador online, como o OnlineGDB, escolha a linguagem **C**, cole o código do `criptografia.c` e clique em **Run**. Digite as entradas no terminal que aparece. Esses sites costumam usar o horário UTC, então a hora do log pode ficar diferente do horário local.

### Executar com entradas prontas (Linux/macOS)

Para testar sem digitar, envie as respostas por um pipe. Exemplo: criptografar `noite` com SHIFT 3 e Fibonacci, e sair:

```bash
printf '1\nnoite\n3\n3\n0\n' | ./criptografia
```

### Onde ficam os arquivos gerados

`resultado_criptografia.txt` e `log_execucao.txt` são criados na pasta de onde o programa é executado. Se não encontrar, procure pelo nome desses arquivos.

## 3. Menu do programa

1. **Criptografar** — pede palavra, SHIFT e sequência; mostra a sequência, a soma da série, cada camada e uma tabela letra a letra.
2. **Descriptografar** — desfaz as camadas na ordem inversa (com os mesmos parâmetros).
3. **Comparar sequências** — cifra a mesma palavra com as 4 sequências e compara os resultados (análise e avaliação).
4. **Exibir arquivos gerados** — mostra o resultado e o log.

Tipos de sequência: `1` = PA, `2` = PG, `3` = Fibonacci, `4` = Primos.
PA e PG pedem também o primeiro termo `a1` e a razão; por isso essas duas informações são gravadas no arquivo de resultado (sem elas não dá para decifrar).

## 4. Matemática aplicada

| Sequência | Termo geral usado no código | Soma da série (Sn) |
|---|---|---|
| PA | `aₙ = a₁ + (n−1)·r` | `Sₙ = (a₁ + aₙ)·n / 2` |
| PG | `aₙ = a₁·qⁿ⁻¹` | `Sₙ = a₁·(qⁿ − 1)/(q − 1)` |
| Fibonacci | `Fₙ = Fₙ₋₁ + Fₙ₋₂`, `F₁ = F₂ = 1` | `Sₙ = Fₙ₊₂ − 1` |
| Primos | testa divisores até √n | não existe fórmula geral |

O programa calcula a soma da série de duas formas (laço e fórmula) e mostra as duas para conferência.

Como só importa o resto da divisão por 26, o deslocamento usa `% 26`. Os termos da PG crescem rápido, por isso `a1` e `q` foram limitados a 1–9 e os valores usam `long long`.

## 5. Execução

Entrada: palavra **`noite`**, SHIFT **`3`**, sequência **Fibonacci** (`1, 1, 2, 3, 5`).

Soma da série: `S₅ = 12` (conferida por laço e pela fórmula `F₇ − 1 = 13 − 1`).

| Pos | Letra | Sequência | SHIFT + Seq | Cifrada |
|---|---|---|---|---|
| 1 | n | 1 | 4 | r |
| 2 | o | 1 | 4 | s |
| 3 | i | 2 | 5 | n |
| 4 | t | 3 | 6 | z |
| 5 | e | 5 | 8 | m |

- Camada 1 (César, SHIFT 3): `qrlwh`
- Camada 2 (Fibonacci): `rsnzm`

Saída: **`rsnzm`**. A descriptografia com os mesmos parâmetros devolve `noite`.

Arquivo `resultado_criptografia.txt`:

```
Palavra codificada: rsnzm | SHIFT: 3 | Tipo: 3 | Letras: 5
```

> **Nota técnica:** no exemplo do enunciado (`coracao`, SHIFT 3, Fibonacci), a saída indicada é `fqvdjqb`. Aplicando letra a letra a fórmula `SHIFT + sequência[i]` descrita no enunciado, o resultado é `gswgkle`, que é o valor gerado por este programa e que a descriptografia reverte corretamente para `coracao`.

## 6. Análise e avaliação (opção 3 do menu)

Palavra `noite`, SHIFT 3 (PA: a1=1, r=3; PG: a1=1, q=2):

| Sequência | Cifrada | Deslocamentos distintos | Letras iguais ao original | Repetições preservadas |
|---|---|---|---|---|
| PA | rvsgu | 5 | 0 | 0 |
| PG | rtpex | 5 | 0 | 0 |
| Fibonacci | rsnzm | 4 | 0 | 0 |
| Primos | suqds | 5 | 0 | 0 |

Métricas usadas:

- **Deslocamentos distintos:** quanto mais variados, menos a cifra lembra um César simples.
- **Letras iguais ao original:** letras que "não foram cifradas" (deslocamento múltiplo de 26).
- **Repetições preservadas:** pares de letras iguais no texto original que continuam iguais na cifra. No César puro, uma mesma letra sempre vira a mesma letra, o que facilita a análise de frequência. Como `noite` não tem letras repetidas, essa métrica vale 0 para todas as sequências.

**Avaliação:** o Fibonacci começa com `1, 1`, então as duas primeiras letras (`n` e `o`) recebem o mesmo deslocamento e a variedade cai para 4. PA, PG e primos empataram com 5 deslocamentos distintos; o programa fica com a primeira em caso de empate, por isso aponta a PA. Em uma palavra com letras repetidas (como `coracao`), a métrica de repetições passa a ser mais útil.

**Limitações (pensamento crítico):**

- Todas as sequências são **determinísticas e públicas**. Quem souber o tipo de sequência e o SHIFT decifra tudo.
- Sem saber os parâmetros, ainda é possível atacar por força bruta: só existem 25 SHIFTs × poucas sequências × poucos parâmetros.
- Primos e Fibonacci são famosos, então são os primeiros palpites de um atacante. Uma PA/PG com parâmetros escolhidos pelo próprio autor tende a ser menos previsível.
- Este projeto é didático. Cifras de verdade (AES, RSA) usam matemática e chaves muito mais robustas.

## 7. Taxonomia de Bloom no projeto

| Nível | O que foi feito neste projeto |
|---|---|
| Lembrar | Alfabeto de 26 letras, valores ASCII (`'a'` = 97) e fórmulas de PA, PG, Fibonacci e primos |
| Compreender | Discussão do efeito do SHIFT e da cifra de César; tabela letra a letra mostra o deslocamento total |
| Aplicar | Funções `camada1_cesar`, `camada2_sequencia` e `gerar_sequencia` em C |
| Analisar | Opção 3 compara PA, PG, Fibonacci e primos com métricas objetivas |
| Avaliar | Seção 6: julgamento de qual sequência espalha melhor os deslocamentos e limitações de segurança |
| Criar | Sistema completo e personalizável (palavra, SHIFT, tipo e parâmetros da sequência, descriptografia, log) |

## 8. Estrutura do repositório

```
.
├── criptografia.c                # código-fonte
├── criptografia_codigo.txt       # cópia do código-fonte em texto simples
├── README.md                     # esta documentação
├── resultado_criptografia.txt    # gerado pelo programa
└── log_execucao.txt              # gerado pelo programa
```
