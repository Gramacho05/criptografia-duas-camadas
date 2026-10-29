/*
 * Autor: Gabriel Braes Gramacho
 *
 * Criptografia em duas camadas: Cifra de Cesar (SHIFT fixo) +
 * deslocamento pela sequencia numerica (PA, PG, Fibonacci ou Primos).
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>

#define MAX_LETRAS    15
#define ALFABETO      26
#define ARQ_RESULTADO "resultado_criptografia.txt"
#define ARQ_LOG       "log_execucao.txt"

#define PA_A1_PADRAO  1
#define PA_R_PADRAO   3
#define PG_A1_PADRAO  1
#define PG_Q_PADRAO   2

typedef enum { PA = 1, PG = 2, FIBONACCI = 3, PRIMOS = 4 } TipoSequencia;

typedef struct {
    char      palavra[MAX_LETRAS + 1];
    int       shift;
    int       tipo;
    long long a1;
    long long razao;
} Config;

typedef struct {
    int  tipo;
    char cifra[MAX_LETRAS + 1];
    int  distintos;
    int  inalteradas;
    int  repeticoes;
} Metrica;

static void registrar_log(const char *formato, ...)
{
    FILE *arq = fopen(ARQ_LOG, "a");
    if (arq == NULL) {
        printf("Aviso: nao foi possivel abrir %s\n", ARQ_LOG);
        return;
    }

    time_t agora = time(NULL);
    char data[32];
    strftime(data, sizeof data, "%Y-%m-%d %H:%M:%S", localtime(&agora));
    fprintf(arq, "[%s] ", data);

    va_list args;
    va_start(args, formato);
    vfprintf(arq, formato, args);
    va_end(args);

    fputc('\n', arq);
    fclose(arq);
}

static void ler_linha(const char *msg, char *buf, size_t tam)
{
    printf("%s", msg);
    if (fgets(buf, (int)tam, stdin) == NULL) {
        printf("\nEntrada encerrada.\n");
        exit(0);
    }
    char *nl = strchr(buf, '\n');
    if (nl != NULL) {
        *nl = '\0';
    } else {
        int c;
        while ((c = getchar()) != '\n' && c != EOF) { }
    }
    buf[strcspn(buf, "\r")] = '\0';
}

static long long ler_inteiro(const char *msg, long long min, long long max)
{
    char buf[64];
    char *fim;

    while (1) {
        ler_linha(msg, buf, sizeof buf);
        long long v = strtoll(buf, &fim, 10);
        while (*fim == ' ') fim++;
        if (fim != buf && *fim == '\0' && v >= min && v <= max)
            return v;
        printf("  Valor invalido. Digite um numero entre %lld e %lld.\n", min, max);
    }
}

static void ler_palavra(const char *msg, char *dest)
{
    char buf[128];

    while (1) {
        ler_linha(msg, buf, sizeof buf);
        size_t n = strlen(buf);
        int ok = (n >= 1 && n <= MAX_LETRAS);

        for (size_t i = 0; ok && i < n; i++) {
            char c = buf[i];
            if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')))
                ok = 0;
        }

        if (ok) {
            for (size_t i = 0; i < n; i++)
                dest[i] = (buf[i] >= 'A' && buf[i] <= 'Z') ? (char)(buf[i] + 32) : buf[i];
            dest[n] = '\0';
            return;
        }
        printf("  Invalido. Use de 1 a %d letras, sem acentos, espacos ou numeros.\n", MAX_LETRAS);
    }
}

static const char *nome_sequencia(int tipo)
{
    switch (tipo) {
        case PA:        return "Progressao Aritmetica";
        case PG:        return "Progressao Geometrica";
        case FIBONACCI: return "Fibonacci";
        case PRIMOS:    return "Numeros Primos";
        default:        return "Desconhecida";
    }
}

static int eh_primo(long long n)
{
    if (n < 2) return 0;
    for (long long d = 2; d * d <= n; d++)
        if (n % d == 0) return 0;
    return 1;
}

static void gerar_sequencia(int tipo, int n, long long a1, long long razao, long long *seq)
{
    switch (tipo) {
        case PA:                               /* an = a1 + (n-1)*r */
            for (int i = 0; i < n; i++)
                seq[i] = a1 + i * razao;
            break;

        case PG:                               /* an = a1 * q^(n-1) */
            seq[0] = a1;
            for (int i = 1; i < n; i++)
                seq[i] = seq[i - 1] * razao;
            break;

        case FIBONACCI:                        /* Fn = Fn-1 + Fn-2  */
            for (int i = 0; i < n; i++)
                seq[i] = (i < 2) ? 1 : seq[i - 1] + seq[i - 2];
            break;

        case PRIMOS: {
            long long candidato = 2;
            int achados = 0;
            while (achados < n) {
                if (eh_primo(candidato))
                    seq[achados++] = candidato;
                candidato++;
            }
            break;
        }
    }
}

static long long soma_formula(int tipo, int n, const long long *seq, long long razao)
{
    switch (tipo) {
        case PA:        return (seq[0] + seq[n - 1]) * n / 2;
        case PG:        return (seq[n - 1] * razao - seq[0]) / (razao - 1);
        case FIBONACCI: return (n == 1) ? 1 : 2 * seq[n - 1] + seq[n - 2] - 1;
        default:        return -1;
    }
}

static void mostrar_sequencia(const Config *cfg, const long long *seq, int n)
{
    long long soma = 0;

    printf("\nSequencia (%s): ", nome_sequencia(cfg->tipo));
    for (int i = 0; i < n; i++) {
        printf("%lld%s", seq[i], (i < n - 1) ? ", " : "\n");
        soma += seq[i];
    }

    printf("Soma da serie Sn (por laco)   : %lld\n", soma);
    long long f = soma_formula(cfg->tipo, n, seq, cfg->razao);
    if (f >= 0)
        printf("Soma da serie Sn (por formula): %lld\n", f);
    else
        printf("Soma da serie Sn (por formula): nao existe formula geral para primos\n");
}

static char rotacionar(char c, long long d)
{
    int pos = (int)(((c - 'a') + d) % ALFABETO);
    if (pos < 0) pos += ALFABETO;
    return (char)('a' + pos);
}

static void camada1_cesar(const char *entrada, char *saida, int shift, int sentido)
{
    size_t n = strlen(entrada);
    for (size_t i = 0; i < n; i++)
        saida[i] = rotacionar(entrada[i], (long long)sentido * shift);
    saida[n] = '\0';
}

static void camada2_sequencia(const char *entrada, char *saida, const long long *seq, int sentido)
{
    size_t n = strlen(entrada);
    for (size_t i = 0; i < n; i++)
        saida[i] = rotacionar(entrada[i], sentido * seq[i]);
    saida[n] = '\0';
}

static void gravar_resultado(const char *cifra, const Config *cfg, int n)
{
    FILE *arq = fopen(ARQ_RESULTADO, "a");
    if (arq == NULL) {
        printf("Erro: nao foi possivel gravar em %s\n", ARQ_RESULTADO);
        registrar_log("ERRO ao abrir %s", ARQ_RESULTADO);
        return;
    }

    fprintf(arq, "Palavra codificada: %s | SHIFT: %d | Tipo: %d | Letras: %d",
            cifra, cfg->shift, cfg->tipo, n);
    if (cfg->tipo == PA || cfg->tipo == PG)
        fprintf(arq, " | a1: %lld | razao: %lld", cfg->a1, cfg->razao);
    fputc('\n', arq);

    fclose(arq);
    printf("\nResultado gravado em %s\n", ARQ_RESULTADO);
}

static void mostrar_arquivo(const char *nome)
{
    FILE *arq = fopen(nome, "r");
    char linha[256];

    printf("\n--- %s ---\n", nome);
    if (arq == NULL) {
        printf("(arquivo ainda nao existe)\n");
        return;
    }
    while (fgets(linha, sizeof linha, arq) != NULL)
        printf("%s", linha);
    fclose(arq);
}

static void pedir_configuracao(Config *cfg, const char *msg_palavra)
{
    ler_palavra(msg_palavra, cfg->palavra);
    cfg->shift = (int)ler_inteiro("SHIFT fixo (1 a 25): ", 1, 25);

    printf("Sequencia numerica:\n");
    printf("  1 - Progressao Aritmetica (PA)\n");
    printf("  2 - Progressao Geometrica (PG)\n");
    printf("  3 - Fibonacci\n");
    printf("  4 - Numeros Primos\n");
    cfg->tipo = (int)ler_inteiro("Tipo: ", 1, 4);

    cfg->a1 = 1;
    cfg->razao = 0;
    if (cfg->tipo == PA) {
        cfg->a1    = ler_inteiro("  Primeiro termo a1 (0 a 100): ", 0, 100);
        cfg->razao = ler_inteiro("  Razao r (1 a 100): ", 1, 100);
    } else if (cfg->tipo == PG) {
        cfg->a1    = ler_inteiro("  Primeiro termo a1 (1 a 9): ", 1, 9);
        cfg->razao = ler_inteiro("  Razao q (2 a 9): ", 2, 9);
    }
}

static void opcao_criptografar(void)
{
    Config cfg;
    char c1[MAX_LETRAS + 1], c2[MAX_LETRAS + 1];
    long long seq[MAX_LETRAS];

    pedir_configuracao(&cfg, "Palavra secreta (ate 15 letras, sem acentos): ");
    int n = (int)strlen(cfg.palavra);

    gerar_sequencia(cfg.tipo, n, cfg.a1, cfg.razao, seq);
    mostrar_sequencia(&cfg, seq, n);

    camada1_cesar(cfg.palavra, c1, cfg.shift, +1);
    camada2_sequencia(c1, c2, seq, +1);

    printf("\nCamada 1 (Cesar, SHIFT %d): %s\n", cfg.shift, c1);
    printf("Camada 2 (sequencia)      : %s\n\n", c2);

    printf(" Pos | Letra | Sequencia       | SHIFT+Seq       | Cifrada\n");
    printf("-----+-------+-----------------+-----------------+--------\n");
    for (int i = 0; i < n; i++)
        printf(" %3d |   %c   | %-15lld | %-15lld |   %c\n",
               i + 1, cfg.palavra[i], seq[i], cfg.shift + seq[i], c2[i]);

    printf("\nPalavra criptografada: %s\n", c2);

    gravar_resultado(c2, &cfg, n);
    registrar_log("CRIPTOGRAFIA: letras=%d shift=%d sequencia=%s (tipo %d) a1=%lld razao=%lld",
                  n, cfg.shift, nome_sequencia(cfg.tipo), cfg.tipo, cfg.a1, cfg.razao);
}

static void opcao_descriptografar(void)
{
    Config cfg;
    char c1[MAX_LETRAS + 1], original[MAX_LETRAS + 1];
    long long seq[MAX_LETRAS];

    printf("Informe os MESMOS parametros usados na criptografia.\n");
    pedir_configuracao(&cfg, "Palavra criptografada: ");
    int n = (int)strlen(cfg.palavra);

    gerar_sequencia(cfg.tipo, n, cfg.a1, cfg.razao, seq);

    camada2_sequencia(cfg.palavra, c1, seq, -1);
    camada1_cesar(c1, original, cfg.shift, -1);

    printf("\nPalavra original: %s\n", original);
    registrar_log("DESCRIPTOGRAFIA: letras=%d shift=%d sequencia=%s (tipo %d)",
                  n, cfg.shift, nome_sequencia(cfg.tipo), cfg.tipo);
}

static void opcao_comparar(void)
{
    char palavra[MAX_LETRAS + 1];
    long long seq[MAX_LETRAS];
    Metrica m[4];

    ler_palavra("Palavra para analise (ate 15 letras, sem acentos): ", palavra);
    int shift = (int)ler_inteiro("SHIFT fixo (1 a 25): ", 1, 25);
    int n = (int)strlen(palavra);

    printf("\nParametros usados: PA(a1=%d, r=%d)  PG(a1=%d, q=%d)  Fibonacci  Primos\n\n",
           PA_A1_PADRAO, PA_R_PADRAO, PG_A1_PADRAO, PG_Q_PADRAO);
    printf("%-22s | %-15s | Desloc. distintos | Letras iguais | Repeticoes preservadas\n",
           "Sequencia", "Cifrada");
    printf("-----------------------+-----------------+-------------------+---------------+-----------------------\n");

    for (int t = 0; t < 4; t++) {
        int tipo = t + 1;
        long long a1 = (tipo == PA) ? PA_A1_PADRAO : PG_A1_PADRAO;
        long long razao = (tipo == PA) ? PA_R_PADRAO : PG_Q_PADRAO;
        int usado[ALFABETO] = {0};

        gerar_sequencia(tipo, n, a1, razao, seq);

        m[t].tipo = tipo;
        m[t].distintos = 0;
        m[t].inalteradas = 0;
        m[t].repeticoes = 0;

        for (int i = 0; i < n; i++) {
            long long total = shift + seq[i];
            m[t].cifra[i] = rotacionar(palavra[i], total);
            int d = (int)(total % ALFABETO);
            if (!usado[d]) { usado[d] = 1; m[t].distintos++; }
            if (m[t].cifra[i] == palavra[i]) m[t].inalteradas++;
        }
        m[t].cifra[n] = '\0';

        for (int i = 0; i < n; i++)
            for (int j = i + 1; j < n; j++)
                if (palavra[i] == palavra[j] && m[t].cifra[i] == m[t].cifra[j])
                    m[t].repeticoes++;

        printf("%-22s | %-15s | %17d | %13d | %22d\n",
               nome_sequencia(tipo), m[t].cifra, m[t].distintos, m[t].inalteradas, m[t].repeticoes);
    }

    int melhor = 0;
    for (int t = 1; t < 4; t++) {
        if (m[t].distintos > m[melhor].distintos ||
            (m[t].distintos == m[melhor].distintos && m[t].repeticoes < m[melhor].repeticoes) ||
            (m[t].distintos == m[melhor].distintos && m[t].repeticoes == m[melhor].repeticoes &&
             m[t].inalteradas < m[melhor].inalteradas))
            melhor = t;
    }

    printf("\nAvaliacao: para esta palavra, a sequencia que mais espalhou os deslocamentos foi %s.\n",
           nome_sequencia(m[melhor].tipo));
    printf("Obs.: e uma heuristica simples. Sequencias conhecidas (PA, PG, Fibonacci, primos)\n");
    printf("      sao previsiveis, entao nenhuma delas torna a cifra segura de verdade.\n");

    registrar_log("ANALISE: letras=%d shift=%d melhor_sequencia=%s",
                  n, shift, nome_sequencia(m[melhor].tipo));
}

int main(void)
{
    int opcao;

    registrar_log("Programa iniciado");

    do {
        printf("\n===== CRIPTOGRAFIA EM DUAS CAMADAS =====\n");
        printf("1 - Criptografar palavra\n");
        printf("2 - Descriptografar palavra\n");
        printf("3 - Comparar sequencias (analise)\n");
        printf("4 - Exibir arquivos gerados (resultado e log)\n");
        printf("0 - Sair\n");
        opcao = (int)ler_inteiro("Opcao: ", 0, 4);

        switch (opcao) {
            case 1: opcao_criptografar();   break;
            case 2: opcao_descriptografar(); break;
            case 3: opcao_comparar();       break;
            case 4:
                mostrar_arquivo(ARQ_RESULTADO);
                mostrar_arquivo(ARQ_LOG);
                break;
            case 0: printf("Encerrando...\n"); break;
        }
    } while (opcao != 0);

    registrar_log("Programa encerrado");
    return 0;
}
