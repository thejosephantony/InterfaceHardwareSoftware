#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <pthread.h>
#include <stdatomic.h>
#include <ctype.h>

#define MAX_usuarios 1000
#define TABELA_tamanho 10007
#define N_THREADS 8
#define CHAR_QTD 62
#define MIN_senha 2
#define MAX_senha 4

const char cs[] =
"abcdefghijklmnopqrstuvwxyz"
"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789";

typedef struct Usuario {
    char login[64];
    uint8_t hash[8];
    char senha[MAX_senha + 2];
    int encontrada;
    struct Usuario *prox;
} Usuario;

typedef struct {
    int tid;
    int tam;
} ThreadData;

Usuario usuarios[MAX_usuarios];
Usuario *tabela[TABELA_tamanho];

int total_usuarios = 0;
atomic_int total_encontrados = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

static inline uint8_t hrand(uint32_t *ri) {
    *ri = (1103515245u * (*ri)) + 12345u;

    uint32_t x = *ri;

    return (uint8_t)(x ^ (x >> 8) ^ (x >> 16) ^ (x >> 24));
}

void MAO_64(uint8_t hash[8], const char *senha) {
    uint32_t seed = 0;
    uint32_t n = strlen(senha);

    for (uint32_t i = 0; i < 8; i++)
        hash[i] = 0;

    for (uint32_t i = 0; i < n; i++)
        seed = (seed << 8) | ((seed >> 24) ^ (unsigned char)senha[i]);

    uint32_t ri = seed;

    for (uint32_t i = 0; i < 32; i++)
        hash[i & 7] = hash[i & 7] ^ hrand(&ri);
}

uint32_t indice_hash(const uint8_t h[8]) {
    uint32_t v = 0;

    for (int i = 0; i < 8; i++)
        v = v * 31u + h[i];

    return v % TABELA_tamanho;
}

void inserir(Usuario *u) {
    uint32_t p = indice_hash(u->hash);
    u->prox = tabela[p];
    tabela[p] = u;
}

void verificar(const uint8_t h[8], const char *senha) {
    uint32_t p = indice_hash(h);
    Usuario *u = tabela[p];

    while (u != NULL) {
        if (memcmp(u->hash, h, 8) == 0) {
            pthread_mutex_lock(&mutex);

            if (!u->encontrada ||
                strlen(senha) < strlen(u->senha) ||
                (strlen(senha) == strlen(u->senha) && strcmp(senha, u->senha) < 0)) {

                if (!u->encontrada)
                    atomic_fetch_add(&total_encontrados, 1);

                u->encontrada = 1;
                snprintf(u->senha, sizeof(u->senha), "%s", senha);
            }

            pthread_mutex_unlock(&mutex);
        }

        u = u->prox;
    }
}

void gerar(int pos, int tam, char s[MAX_senha + 2]) {
    if (atomic_load(&total_encontrados) == total_usuarios)
        return;

    if (pos == tam) {
        uint8_t h[8];

        s[tam] = '\0';
        MAO_64(h, s);
        verificar(h, s);

        return;
    }

    for (int i = 0; i < CHAR_QTD; i++) {
        s[pos] = cs[i];
        gerar(pos + 1, tam, s);
    }
}

void *worker(void *arg) {
    ThreadData *td = (ThreadData *)arg;
    char s[MAX_senha + 2];

    for (int i = td->tid; i < CHAR_QTD; i += N_THREADS) {
        s[0] = cs[i];
        gerar(1, td->tam, s);
    }

    return NULL;
}

void quebrar_tamanho(int tam) {
    pthread_t th[N_THREADS];
    ThreadData td[N_THREADS];

    for (int i = 0; i < N_THREADS; i++) {
        td[i].tid = i;
        td[i].tam = tam;
        pthread_create(&th[i], NULL, worker, &td[i]);
    }

    for (int i = 0; i < N_THREADS; i++)
        pthread_join(th[i], NULL);
}

int hexval(int c) {
    if (c >= '0' && c <= '9')
        return c - '0';

    if (c >= 'a' && c <= 'f')
        return c - 'a' + 10;

    if (c >= 'A' && c <= 'F')
        return c - 'A' + 10;

    return -1;
}

char *trim(char *s) {
    while (isspace((unsigned char)*s))
        s++;

    if (*s == '\0')
        return s;

    char *fim = s + strlen(s) - 1;

    while (fim > s && isspace((unsigned char)*fim)) {
        *fim = '\0';
        fim--;
    }

    return s;
}

int ler_hash(char *hex, uint8_t h[8]) {
    hex = trim(hex);

    if (strlen(hex) != 16)
        return 0;

    for (int i = 0; i < 16; i++) {
        if (!isxdigit((unsigned char)hex[i]))
            return 0;
    }

    for (int i = 0; i < 8; i++)
        h[i] = (uint8_t)((hexval(hex[2 * i]) << 4) | hexval(hex[2 * i + 1]));

    return 1;
}

int verifica_linha(const char *s) {
    while (isspace((unsigned char)*s))
        s++;

    if (!isdigit((unsigned char)*s))
        return 0;

    while (isdigit((unsigned char)*s))
        s++;

    while (isspace((unsigned char)*s))
        s++;

    return *s == '\0';
}

void carrega_linha(char *linha) {
    linha[strcspn(linha, "\r\n")] = '\0';

    char *l = trim(linha);

    if (*l == '\0')
        return;

    char login[64];
    char hash_txt[128];

    char *p = strchr(l, ':');

    if (p != NULL) {
        *p = '\0';
        snprintf(login, sizeof(login), "%s", trim(l));
        snprintf(hash_txt, sizeof(hash_txt), "%s", trim(p + 1));
    } else {
        if (sscanf(l, "%63s %127s", login, hash_txt) != 2)
            return;
    }

    if (total_usuarios >= MAX_usuarios)
        return;

    if (!ler_hash(hash_txt, usuarios[total_usuarios].hash))
        return;

    snprintf(usuarios[total_usuarios].login,
             sizeof(usuarios[total_usuarios].login),
             "%s",
             login);

    usuarios[total_usuarios].senha[0] = '\0';
    usuarios[total_usuarios].encontrada = 0;
    usuarios[total_usuarios].prox = NULL;

    inserir(&usuarios[total_usuarios]);
    total_usuarios++;
}

int main(void) {
    char entrada[][128] = {
        "3",
        "bruno:40ded38ad4bffcc4",
        "ihs:6b9f088adaa7abe2",
        "facil:37dfe6123bc4f057"
    };

    int qtd_linhas = sizeof(entrada) / sizeof(entrada[0]);
    int primeira = 1;

    for (int i = 0; i < qtd_linhas; i++) {
        if (primeira && verifica_linha(entrada[i])) {
            primeira = 0;
            continue;
        }

        primeira = 0;
        carrega_linha(entrada[i]);
    }

    for (int tam = MIN_senha; tam <= MAX_senha; tam++) {
        quebrar_tamanho(tam);

        if (atomic_load(&total_encontrados) == total_usuarios)
            break;
    }

    for (int i = 0; i < total_usuarios; i++) {
        printf("%s:%s\n",
               usuarios[i].login,
               usuarios[i].encontrada ? usuarios[i].senha : "nao encontrou");
    }

    return 0;
}