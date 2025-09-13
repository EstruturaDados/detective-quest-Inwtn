#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define TAM_HASH 10
#define MAX_NOME 50

// Nó da árvore binária de busca para pistas.
typedef struct NoPista {
    char pista[MAX_NOME];
    struct NoPista *esq;
    struct NoPista *dir;
} NoPista;

// Nó da lista encadeada para colisão na tabela hash
typedef struct NoHash {
    char pista[MAX_NOME];
    char suspeito[MAX_NOME];
    struct NoHash *prox;
} NoHash;

// Tabela hash: array de ponteiros para listas encadeadas
NoHash* tabelaHash[TAM_HASH];

// Raiz da BST de pistas
NoPista* raizPistas = NULL;

// Função hash simples: soma dos caracteres mod TAM_HASH
unsigned int funcaoHash(const char* chave) {
    unsigned int soma = 0;
    for (int i = 0; chave[i] != '\0'; i++) {
        soma += (unsigned char)chave[i];
    }
    return soma % TAM_HASH;
}

// Inserir pista na BST
NoPista* inserirPista(NoPista* raiz, const char* pista) {
    if (raiz == NULL) {
        NoPista* novo = (NoPista*)malloc(sizeof(NoPista));
        if (!novo) {
            printf("Erro de alocacao!\n");
            exit(1);
        }
        strcpy(novo->pista, pista);
        novo->esq = novo->dir = NULL;
        return novo;
    }
    int cmp = strcmp(pista, raiz->pista);
    if (cmp < 0) {
        raiz->esq = inserirPista(raiz->esq, pista);
    } else if (cmp > 0) {
        raiz->dir = inserirPista(raiz->dir, pista);
    }
    // Se igual, não insere duplicado
    return raiz;
}

// Busca na BST (retorna true se encontrada)
bool buscarPista(NoPista* raiz, const char* pista) {
    if (raiz == NULL) return false;
    int cmp = strcmp(pista, raiz->pista);
    if (cmp == 0) return true;
    else if (cmp < 0) return buscarPista(raiz->esq, pista);
    else return buscarPista(raiz->dir, pista);
}

// Em ordem: imprime todas as pistas da BST
void imprimirPistas(NoPista* raiz) {
    if (raiz == NULL) return;
    imprimirPistas(raiz->esq);
    printf("- %s\n", raiz->pista);
    imprimirPistas(raiz->dir);
}

// Inserir na tabela hash (encadeamento)
void inserirNaHash(const char* pista, const char* suspeito) {
    unsigned int indice = funcaoHash(pista);
    NoHash* novo = (NoHash*)malloc(sizeof(NoHash));
    if (!novo) {
        printf("Erro de alocacao!\n");
        exit(1);
    }
    strcpy(novo->pista, pista);
    strcpy(novo->suspeito, suspeito);
    novo->prox = tabelaHash[indice];
    tabelaHash[indice] = novo;
}

// Imprime todas as associações pista → suspeito
void imprimirHash() {
    printf("\nAssociacoes pista -> suspeito:\n");
    for (int i = 0; i < TAM_HASH; i++) {
        NoHash* atual = tabelaHash[i];
        if (atual != NULL) {
            printf("Indice %d:\n", i);
            while (atual != NULL) {
                printf("  Pista: %-20s | Suspeito: %s\n", atual->pista, atual->suspeito);
                atual = atual->prox;
            }
        }
    }
}

// Conta quantas pistas cada suspeito tem na tabela hash
typedef struct {
    char suspeito[MAX_NOME];
    int contagem;
} ContagemSuspeito;

int contarSuspeitos(ContagemSuspeito* vetor, int tamanho, const char* suspeito) {
    for (int i = 0; i < tamanho; i++) {
        if (strcmp(vetor[i].suspeito, suspeito) == 0) {
            return i;
        }
    }
    return -1;
}

// Encontra o suspeito mais citado
void suspeitoMaisCitado() {
    ContagemSuspeito suspeitos[100];
    int totalSuspeitos = 0;

    // Percorre toda a tabela hash para contar
    for (int i = 0; i < TAM_HASH; i++) {
        NoHash* atual = tabelaHash[i];
        while (atual != NULL) {
            int pos = contarSuspeitos(suspeitos, totalSuspeitos, atual->suspeito);
            if (pos == -1) {
                // Novo suspeito
                strcpy(suspeitos[totalSuspeitos].suspeito, atual->suspeito);
                suspeitos[totalSuspeitos].contagem = 1;
                totalSuspeitos++;
            } else {
                suspeitos[pos].contagem++;
            }
            atual = atual->prox;
        }
    }

    if (totalSuspeitos == 0) {
        printf("Nenhum suspeito encontrado.\n");
        return;
    }

    // Encontrar o com maior contagem
    int maxIndex = 0;
    for (int i = 1; i < totalSuspeitos; i++) {
        if (suspeitos[i].contagem > suspeitos[maxIndex].contagem) {
            maxIndex = i;
        }
    }

    printf("\nSuspeito mais citado: %s com %d pistas associadas.\n",
           suspeitos[maxIndex].suspeito, suspeitos[maxIndex].contagem);
}

// Função para liberar memória da BST
void liberarBST(NoPista* raiz) {
    if (raiz == NULL) return;
    liberarBST(raiz->esq);
    liberarBST(raiz->dir);
    free(raiz);
}

// Função para liberar memória da tabela hash
void liberarHash() {
    for (int i = 0; i < TAM_HASH; i++) {
        NoHash* atual = tabelaHash[i];
        while (atual != NULL) {
            NoHash* temp = atual;
            atual = atual->prox;
            free(temp);
        }
        tabelaHash[i] = NULL;
    }
}

// Menu interativo
void menu() {
    int opcao;
    char pista[MAX_NOME];
    char suspeito[MAX_NOME];

    do {
        printf("\n=== Detective Quest - Nivel Mestre ===\n");
        printf("1 - Inserir nova pista e suspeito\n");
        printf("2 - Buscar pista\n");
        printf("3 - Listar todas as pistas\n");
        printf("4 - Mostrar associacoes pista -> suspeito\n");
        printf("5 - Mostrar suspeito mais citado\n");
        printf("0 - Sair\n");
        printf("Escolha uma opcao: ");
        scanf("%d", &opcao);
        getchar(); // limpar buffer

        switch (opcao) {
            case 1:
                printf("Digite o nome da pista: ");
                fgets(pista, sizeof(pista), stdin);
                pista[strcspn(pista, "\n")] = '\0';

                printf("Digite o nome do suspeito associado: ");
                fgets(suspeito, sizeof(suspeito), stdin);
                suspeito[strcspn(suspeito, "\n")] = '\0';

                // Inserir na BST se não existir
                if (!buscarPista(raizPistas, pista)) {
                    raizPistas = inserirPista(raizPistas, pista);
                    inserirNaHash(pista, suspeito);
                    printf("Pista e suspeito inseridos com sucesso.\n");
                } else {
                    printf("Pista ja existe. Associando suspeito na tabela hash.\n");
                    inserirNaHash(pista, suspeito);
                }
                break;

            case 2:
                printf("Digite o nome da pista para buscar: ");
                fgets(pista, sizeof(pista), stdin);
                pista[strcspn(pista, "\n")] = '\0';

                if (buscarPista(raizPistas, pista)) {
                    printf("Pista '%s' encontrada na BST.\n", pista);
                } else {
                    printf("Pista '%s' nao encontrada.\n", pista);
                }
                break;

            case 3:
                printf("Listando todas as pistas em ordem alfabetica:\n");
                imprimirPistas(raizPistas);
                break;

            case 4:
                imprimirHash();
                break;

            case 5:
                suspeitoMaisCitado();
                break;

            case 0:
                printf("Encerrando o jogo. Ate logo!\n");
                break;

            default:
                printf("Opcao invalida! Tente novamente.\n");
        }
    } while (opcao != 0);
}

int main() {
    // Inicializa tabela hash com NULL
    for (int i = 0; i < TAM_HASH; i++) {
        tabelaHash[i] = NULL;
    }

    printf("Bem-vindo ao Detective Quest - Nivel Mestre\n");

    menu();

    // Liberar memória antes de sair
    liberarBST(raizPistas);
    liberarHash();

    return 0;
}
