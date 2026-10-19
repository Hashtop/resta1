/*
Heitor de Oliveira Fonseca - CEFET-MG
    27/10/2026 - Trabalho 1
*/


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <ncurses.h>
#include <unistd.h>
#include "globals.h"


typedef struct Vector2 {
    int x, y; // x é posição das linhas e y, das colunas.
} Vector2;

typedef struct Tabuleiro {
    int **posicoes;
    Vector2 tam;
    Vector2 posSelecao; // será sempre inicializado como x = 0 e y = 2 (primeira peça).
} Tabuleiro;


#define GANHOU 0
#define PERDEU 1
#define REINICIAR 2
#define SAIR 3
#define CONTINUANDO -1

#define TAM_RESTA1 7

const char *NOME_SAVE = "save.tab";


int modoSimples = 0;


/*
    Gera e retorna o tabuleiro (uma matriz 7x7 de inteiros).
*/
Tabuleiro* gerarTabuleiro() {
    Tabuleiro* tabuleiro = malloc(sizeof(Tabuleiro));

    tabuleiro->posicoes = malloc(sizeof(int*) * TAM_RESTA1);
    for (int i = 0; i < TAM_RESTA1; i++) tabuleiro->posicoes[i] = malloc(sizeof(int) * TAM_RESTA1);

    tabuleiro->tam.x = TAM_RESTA1;
    tabuleiro->tam.y = TAM_RESTA1;

    tabuleiro->posSelecao.x = 0; 
    tabuleiro->posSelecao.y = 2;

    /*
    Preenche o tabuleiro com a seguinte configuração padrão.
    */
    int padraoResta1[TAM_RESTA1][TAM_RESTA1] = {{0, 0, 1, 1, 1, 0, 0},
                                                {0, 0, 1, 1, 1, 0, 0},
                                                {1, 1, 1, 1, 1, 1, 1},
                                                {1, 1, 1, 0, 1, 1, 1},
                                                {1, 1, 1, 1, 1, 1, 1},
                                                {0, 0, 1, 1, 1, 0, 0},
                                                {0, 0, 1, 1, 1, 0, 0}};

    for (int i = 0; i < TAM_RESTA1; i++)
        for (int j = 0; j < TAM_RESTA1; j++)
            tabuleiro->posicoes[i][j] = padraoResta1[i][j];

    return tabuleiro;
}



void imprimirTabuleiro(Tabuleiro *tabuleiro) {
    init_pair(1, COLOR_RED, COLOR_GREEN);
 
    init_pair(2, COLOR_GREEN, COLOR_WHITE); // changed id of pair

    for (int i = 0; i < tabuleiro->tam.x; i++) {
        for (int j = 0; j < tabuleiro->tam.y; j++) {
            // Se for imprimir a seleção atual, trocar de cor.
            if (tabuleiro->posSelecao.x == i && tabuleiro->posSelecao.y == j) {
                attron(COLOR_PAIR(2));
                printw("|");
                attroff(COLOR_PAIR(2));

                attron(COLOR_PAIR(1));
                if (tabuleiro->posicoes[i][j] == 1) printw("O");
                else if (tabuleiro->posicoes[i][j] == -1) printw("X");
                else printw(" ");
                attroff(COLOR_PAIR(1));

                if (j == (tabuleiro->tam.y - 1)) {
                    attron(COLOR_PAIR(2));
                    printw("|");
                    attroff(COLOR_PAIR(2));
                }
            }
            else {
                attron(COLOR_PAIR(2));

                if (tabuleiro->posicoes[i][j] == 1) printw("|O");
                else if (tabuleiro->posicoes[i][j] == -1) printw("|X");
                else printw("| ");

                if (j == (tabuleiro->tam.y - 1)) printw("|");

                attroff(COLOR_PAIR(2));
            }
        }
        printw("\n");
    }
}


/*
    Destrói o objeto tabuleiro, por dentro e por fora.
*/
void destruirTabuleiro(Tabuleiro *tabuleiro) {
    for (int i = 0; i < tabuleiro->tam.x; i++) {
        free(tabuleiro->posicoes[i]);
    }

    free(tabuleiro);
}


/*
    Checa se há uma peça em coordenadasPeca e retorna 1, caso haja.
*/
int checarPeca(Tabuleiro *tabuleiro, Vector2 coordenadasPeca) {
    // Impede que uma peça fora do tabuleiro seja checada (contém segmentation faults).
    if (coordenadasPeca.x < 0 || coordenadasPeca.x >= tabuleiro->tam.x) return 0;
    else if (coordenadasPeca.y < 0 || coordenadasPeca.y >= tabuleiro->tam.y) return 0;

    if (tabuleiro->posicoes[coordenadasPeca.x][coordenadasPeca.y] == 1) return 1;
    else return 0;
}


/*
    Checa se há ao menos uma peça (e uma casa vazia para ela se movimentar) ao redor da peça em coordenadasPeca
    e retorna 1, caso haja.
*/
int checarPecasAdjacentes(Tabuleiro *tabuleiro, Vector2 coordenadasPeca) {
    // Só verifica os lados se eles não ultrapassarem os limites da matriz.
    if (coordenadasPeca.x > 1  && coordenadasPeca.x < tabuleiro->tam.x-1)
        if (tabuleiro->posicoes[coordenadasPeca.x - 1][coordenadasPeca.y] == 1 && tabuleiro->posicoes[coordenadasPeca.x + 1][coordenadasPeca.y] == 0) return 1;
    if (coordenadasPeca.x < tabuleiro->tam.x-1 && coordenadasPeca.x > 0)
        if (tabuleiro->posicoes[coordenadasPeca.x + 1][coordenadasPeca.y] == 1 && tabuleiro->posicoes[coordenadasPeca.x - 1][coordenadasPeca.y] == 0) return 1;
    if (coordenadasPeca.y > 1 && coordenadasPeca.y < tabuleiro->tam.y-1)
        if (tabuleiro->posicoes[coordenadasPeca.x][coordenadasPeca.y - 1] == 1 && tabuleiro->posicoes[coordenadasPeca.x][coordenadasPeca.y + 1] == 0) return 1;
    if (coordenadasPeca.y < tabuleiro->tam.y-1 && coordenadasPeca.y > 0)
        if (tabuleiro->posicoes[coordenadasPeca.x][coordenadasPeca.y + 1] == 1  && tabuleiro->posicoes[coordenadasPeca.x][coordenadasPeca.y - 1] == 0) return 1;

    return 0;
}


/*
    Troca a posSelecao pra próxima peça seguindo na direção da tecla apertada.

    Se não houver, ele mantém a mesma posição e retorna 0 (falhou em encontrar próximo alvo).
*/
int escolherPeca(Tabuleiro *tabuleiro, int tecla) {
    Vector2 selecaoAtual = tabuleiro->posSelecao;
    
    int conseguiu = 1; // Se for 0, falhou em encontrar a peca na nova localização.
    switch (tecla) {
        case '\t': // Caso especial do TAB (ativa MODO SIMPLES).
            modoSimples = (1 - modoSimples); // troca de 0 pra 1 e vice-versa.

            break;
        case '\n': // Caso especial de Enter.
            if (!checarPeca(tabuleiro, selecaoAtual)) conseguiu = 0;
            else return '\n';

            break;
        case ' ': // Caso especial do Espaço.
            if (!checarPeca(tabuleiro, selecaoAtual)) conseguiu = 0;
            else return ' ';

            break;
        case 'r': // Caso especial r.
            return 'r';
            
            break;
        /*
        TRATAMENTO PARA WASD.
        */
        case 'w':
            escolherPeca(tabuleiro, KEY_UP);
            break;
        case 's':
            escolherPeca(tabuleiro, KEY_DOWN);
            break;
        case 'a':
            escolherPeca(tabuleiro, KEY_LEFT);
            break;
        case 'd':
            escolherPeca(tabuleiro, KEY_RIGHT);
            break;
        /*
        TRATAMENTO SETINHAS
        */
        case KEY_UP:
            // Se estiver na parte de cima do tabuleiro (ou fora dele), tenta escolher partindo do local inverso.
            if (tabuleiro->posSelecao.x <= 0) {
                tabuleiro->posSelecao.x = tabuleiro->tam.x;
                
                if (!escolherPeca(tabuleiro, KEY_UP)) {
                    conseguiu = 0;
                    tabuleiro->posSelecao = selecaoAtual;
                }

                break;
            }
            else if (modoSimples && tabuleiro->posSelecao.x > 0) {
                tabuleiro->posSelecao.x--;
                break; // Finaliza antes de chegar na seção complexa.
            }
            
            // Procura uma peça diretamente acima da seleção atual.
            do {
                tabuleiro->posSelecao.x--;
            } while (tabuleiro->posSelecao.x > 0 && !checarPeca(tabuleiro, tabuleiro->posSelecao));

            // Caso já não tenha achado, prossegue com a procura de peças nas diagonais.
            if (!checarPeca(tabuleiro, tabuleiro->posSelecao)) {
                tabuleiro->posSelecao = selecaoAtual; // Restaura a posição do cursor.
                
                do {
                    tabuleiro->posSelecao.x--;
                    
                    int dirChecagem = -1;
                    while (!checarPeca(tabuleiro, tabuleiro->posSelecao)) {
                        if (tabuleiro->posSelecao.y <= 0 && dirChecagem == -1) {
                            dirChecagem = 1;
                            tabuleiro->posSelecao.y = selecaoAtual.y;
                        }
                        else if (tabuleiro->posSelecao.y >= tabuleiro->tam.y-1 && dirChecagem == 1) {
                            tabuleiro->posSelecao.y = selecaoAtual.y;
                            break;
                        }

                        tabuleiro->posSelecao.y += dirChecagem;
                    }
                } while (tabuleiro->posSelecao.x > 0 && !checarPeca(tabuleiro, tabuleiro->posSelecao));
                
                // Contingência.
                if (tabuleiro->posSelecao.x == 0 && !checarPeca(tabuleiro, tabuleiro->posSelecao)) {
                    conseguiu = 0;
                    tabuleiro->posSelecao = selecaoAtual;
                }
            }

            break;
        case KEY_DOWN:
            // Se estiver na parte de baixo do tabuleiro (ou fora dele), tenta escolher partindo do local inverso.
            if (tabuleiro->posSelecao.x >= tabuleiro->tam.x-1) {
                tabuleiro->posSelecao.x = -1;
                
                if (!escolherPeca(tabuleiro, KEY_DOWN)) {
                    conseguiu = 0;
                    tabuleiro->posSelecao = selecaoAtual;
                }

                break;
            }
            else if (modoSimples && tabuleiro->posSelecao.x < tabuleiro->tam.x - 1) {
                tabuleiro->posSelecao.x++;
                break; // Finaliza antes de chegar na seção complexa.
            }
            
            // Procura uma peça diretamente abaixo da seleção atual.
            do {
                tabuleiro->posSelecao.x++;
            } while (tabuleiro->posSelecao.x < tabuleiro->tam.x-1 && !checarPeca(tabuleiro, tabuleiro->posSelecao));

            // Caso já não tenha achado, prossegue com a procura de peças nas diagonais.
            if (!checarPeca(tabuleiro, tabuleiro->posSelecao)) {
                tabuleiro->posSelecao = selecaoAtual; // Restaura a posição do cursor.
                
                do {
                    tabuleiro->posSelecao.x++;
                    
                    int dirChecagem = -1;
                    while (!checarPeca(tabuleiro, tabuleiro->posSelecao)) {
                        if (tabuleiro->posSelecao.y <= 0 && dirChecagem == -1) {
                            dirChecagem = 1;
                            tabuleiro->posSelecao.y = selecaoAtual.y;
                        }
                        else if (tabuleiro->posSelecao.y >= tabuleiro->tam.y-1 && dirChecagem == 1) {
                            tabuleiro->posSelecao.y = selecaoAtual.y;
                            break;
                        }

                        tabuleiro->posSelecao.y += dirChecagem;
                    }
                } while (tabuleiro->posSelecao.x < tabuleiro->tam.x-1 && !checarPeca(tabuleiro, tabuleiro->posSelecao));
                
                // Contingência.
                if (tabuleiro->posSelecao.x == tabuleiro->tam.x-1 && !checarPeca(tabuleiro, tabuleiro->posSelecao)) {
                    conseguiu = 0;
                    tabuleiro->posSelecao = selecaoAtual;
                }
            }

            break;
        case KEY_LEFT:
            // Se estiver na esquerda do tabuleiro (ou fora dele), tenta escolher partindo do local inverso.
            if (tabuleiro->posSelecao.y <= 0) {
                tabuleiro->posSelecao.y = tabuleiro->tam.y;
                
                if (!escolherPeca(tabuleiro, KEY_LEFT)) {
                    conseguiu = 0;
                    tabuleiro->posSelecao = selecaoAtual;
                }

                break;
            }
            else if (modoSimples && tabuleiro->posSelecao.y > 0) {
                tabuleiro->posSelecao.y--;
                break; // Finaliza antes de chegar na seção complexa.
            }
            
            // Procura uma peça diretamente à esquerda da seleção atual.
            do {
                tabuleiro->posSelecao.y--;
            } while (tabuleiro->posSelecao.y > 0 && !checarPeca(tabuleiro, tabuleiro->posSelecao));

            // Caso já não tenha achado, prossegue com a procura de peças nas diagonais.
            if (!checarPeca(tabuleiro, tabuleiro->posSelecao)) {
                tabuleiro->posSelecao = selecaoAtual; // Restaura a posição do cursor.
                
                do {
                    tabuleiro->posSelecao.y--;
                    
                    int dirChecagem = -1;
                    while (!checarPeca(tabuleiro, tabuleiro->posSelecao)) {
                        if (tabuleiro->posSelecao.x <= 0 && dirChecagem == -1) {
                            dirChecagem = 1;
                            tabuleiro->posSelecao.x = selecaoAtual.x;
                        }
                        else if (tabuleiro->posSelecao.x >= tabuleiro->tam.x-1 && dirChecagem == 1) {
                            tabuleiro->posSelecao.x = selecaoAtual.x;
                            break;
                        }

                        tabuleiro->posSelecao.x += dirChecagem;
                    }
                } while (tabuleiro->posSelecao.y > 0 && !checarPeca(tabuleiro, tabuleiro->posSelecao));
                
                // Contingência.
                if (tabuleiro->posSelecao.y == 0 && !checarPeca(tabuleiro, tabuleiro->posSelecao)) {
                    conseguiu = 0;
                    tabuleiro->posSelecao = selecaoAtual;
                }
            }

            break;
        case KEY_RIGHT:
            // Se estiver na direita do tabuleiro (ou fora dele), tenta escolher partindo do local inverso.
            if (tabuleiro->posSelecao.y >= tabuleiro->tam.y-1) {
                tabuleiro->posSelecao.y = -1;
                
                if (!escolherPeca(tabuleiro, KEY_RIGHT)) {
                    conseguiu = 0;
                    tabuleiro->posSelecao = selecaoAtual;
                }

                break;
            }
            else if (modoSimples && tabuleiro->posSelecao.y < tabuleiro->tam.y-1) {
                tabuleiro->posSelecao.y++;
                break; // Finaliza antes de chegar na seção complexa.
            }
            
            // Procura uma peça diretamente à direita da seleção atual.
            do {
                tabuleiro->posSelecao.y++;
            } while (tabuleiro->posSelecao.y < tabuleiro->tam.y-1 && !checarPeca(tabuleiro, tabuleiro->posSelecao));

            // Caso já não tenha achado, prossegue com a procura de peças nas diagonais.
            if (!checarPeca(tabuleiro, tabuleiro->posSelecao)) {
                tabuleiro->posSelecao = selecaoAtual; // Restaura a posição do cursor.
                
                do {
                    tabuleiro->posSelecao.y++;
                    
                    int dirChecagem = -1;
                    while (!checarPeca(tabuleiro, tabuleiro->posSelecao)) {
                        if (tabuleiro->posSelecao.x <= 0 && dirChecagem == -1) {
                            dirChecagem = 1;
                            tabuleiro->posSelecao.x = selecaoAtual.x;
                        }
                        else if (tabuleiro->posSelecao.x >= tabuleiro->tam.x-1 && dirChecagem == 1) {
                            tabuleiro->posSelecao.x = selecaoAtual.x;
                            break;
                        }

                        tabuleiro->posSelecao.x += dirChecagem;
                    }
                } while (tabuleiro->posSelecao.y < tabuleiro->tam.y-1 && !checarPeca(tabuleiro, tabuleiro->posSelecao));
                
                // Contingência.
                if (tabuleiro->posSelecao.y == tabuleiro->tam.y-1 && !checarPeca(tabuleiro, tabuleiro->posSelecao)) {
                    conseguiu = 0;
                    tabuleiro->posSelecao = selecaoAtual;
                }
            }

            break; 
    }

    if (conseguiu == 0) {
        tabuleiro->posSelecao = selecaoAtual;
    }

    return conseguiu; // Se chegou até aqui, ou achou ou não achou.
}


/*
    Faz a peça tentar comer a proxima peça adjacente, caso haja alguma na direção desejada.
*/
int movimentarPeca(Tabuleiro *tabuleiro, int direcao) {
    int conseguiu = 1; // Se for 0, falhou em mover a peca.
    switch (direcao) {
        /*
        TRATAMENTO PARA WASD.
        */
        case 'w':
            movimentarPeca(tabuleiro, KEY_UP);
            break;
        case 's':
            movimentarPeca(tabuleiro, KEY_DOWN);
            break;
        case 'a':
            movimentarPeca(tabuleiro, KEY_LEFT);
            break;
        case 'd':
            movimentarPeca(tabuleiro, KEY_RIGHT);
            break;
        /*
        SETINHAS
        */
        case KEY_UP:
            // Se não estiver no topo ou na linha logo abaixo, ele tenta comer.
            if (tabuleiro->posSelecao.x > 1) {
                Vector2 posAtual = tabuleiro->posSelecao;

                // Único caso válido de movimento, peça adjacente acima e vazio um quadrado depois.
                if (tabuleiro->posicoes[posAtual.x - 1][posAtual.y] == 1 && tabuleiro->posicoes[posAtual.x - 2][posAtual.y] == 0) {
                    tabuleiro->posicoes[posAtual.x][posAtual.y] = 0;
                    tabuleiro->posicoes[posAtual.x - 1][posAtual.y] = 0;
                    tabuleiro->posicoes[posAtual.x - 2][posAtual.y] = 1;
                    tabuleiro->posSelecao.x -= 2;
                }
                else conseguiu = 0;
            }
            else conseguiu = 0;

            break;
        case KEY_DOWN:
            // Se não estiver na parte debaixo ou na linha logo acima, ele tenta comer.
            if (tabuleiro->posSelecao.x < tabuleiro->tam.x - 2) {
                Vector2 posAtual = tabuleiro->posSelecao;

                // Único caso válido de movimento, peça adjacente abaixo e vazio um quadrado depois.
                if (tabuleiro->posicoes[posAtual.x + 1][posAtual.y] == 1 && tabuleiro->posicoes[posAtual.x + 2][posAtual.y] == 0) {
                    tabuleiro->posicoes[posAtual.x][posAtual.y] = 0;
                    tabuleiro->posicoes[posAtual.x + 1][posAtual.y] = 0;
                    tabuleiro->posicoes[posAtual.x + 2][posAtual.y] = 1;
                    tabuleiro->posSelecao.x += 2;
                }
                else conseguiu = 0;
            }
            else conseguiu = 0;

            break;
        case KEY_LEFT:
            // Se não estiver totalmente na esquerda ou na coluna logo à direita, ele tenta comer.
            if (tabuleiro->posSelecao.y > 1) {
                Vector2 posAtual = tabuleiro->posSelecao;

                // Único caso válido de movimento, peça adjacente à esquerda e vazio um quadrado depois.
                if (tabuleiro->posicoes[posAtual.x][posAtual.y - 1] == 1 && tabuleiro->posicoes[posAtual.x][posAtual.y - 2] == 0) {
                    tabuleiro->posicoes[posAtual.x][posAtual.y] = 0;
                    tabuleiro->posicoes[posAtual.x][posAtual.y - 1] = 0;
                    tabuleiro->posicoes[posAtual.x][posAtual.y - 2] = 1;
                    tabuleiro->posSelecao.y -= 2;
                }
                else conseguiu = 0;
            }
            else conseguiu = 0;

            break;
        case KEY_RIGHT:
            // Se não estiver totalmente na direita ou na coluna logo à esquerda, ele tenta comer.
            if (tabuleiro->posSelecao.y < tabuleiro->tam.y - 2) {
                Vector2 posAtual = tabuleiro->posSelecao;

                // Único caso válido de movimento, peça adjacente à direita e vazio um quadrado depois.
                if (tabuleiro->posicoes[posAtual.x][posAtual.y + 1] == 1 && tabuleiro->posicoes[posAtual.x][posAtual.y + 2] == 0) {
                    tabuleiro->posicoes[posAtual.x][posAtual.y] = 0;
                    tabuleiro->posicoes[posAtual.x][posAtual.y + 1] = 0;
                    tabuleiro->posicoes[posAtual.x][posAtual.y + 2] = 1;
                    tabuleiro->posSelecao.y += 2;
                }
                else conseguiu = 0;
            }
            else conseguiu = 0;

            break; 
    }

    return conseguiu;
}


/*
    Assumindo inicialmente que não há nenhum,
    verifica se há pelo menos um único movimento válido possível no tabuleiro. 

    Se não houver, retorna PERDEU.
    Se houver, CONTINUANDO.
    Se restou apenas uma peça, GANHOU.
*/
int checarPossibilidades(Tabuleiro *tabuleiro) {
    int contagemPecas = 0;
    for (int i = 0; i < tabuleiro->tam.x; i++) {
        for (int j = 0; j < tabuleiro->tam.y; j++) {
            Vector2 posAtual = {i, j};
            if (checarPeca(tabuleiro, posAtual)) {
                contagemPecas++;

                if (checarPecasAdjacentes(tabuleiro, posAtual)) return CONTINUANDO;
            }
        }
    }

    if (contagemPecas > 1) return PERDEU;
    else if (contagemPecas == 1) return GANHOU;
}


/*
    Inicia o jogo com o tabuleiro dado a ele.
*/
int iniciarJogo(Tabuleiro *tabuleiro) {
    int estadoDoJogo = CONTINUANDO;

    noecho();
    while (estadoDoJogo != GANHOU && estadoDoJogo != PERDEU) {
        // Faz o usuário escolher a peça que vai mover até ser válido.
        int escolha, resultadoEscolha;
        do {
            clear();
            imprimirTabuleiro(tabuleiro);
            
            printw("\nCoordenadas: x = %d, y = %d.\n", tabuleiro->posSelecao.x, tabuleiro->posSelecao.y);
            
            printw("\nWASD ou SETINHAS - MOVIMENTAR CURSOR.\tTAB - ");
            if (!modoSimples) printw("MODO DE SELEÇÃO SIMPLES.");
            else printw("MODO DE SELEÇÃO MAGNÉTICA.");

            printw("\nENTER ou ESPAÇO - ESCOLHER PEÇA.\n\n");

            printw("R - REINICIAR.\n\n");

            printw("Q - VOLTAR PARA O MENU.\n");
            escolha = getch();

            if (escolha == 'q') return SAIR;

            if (resultadoEscolha == 'r') return REINICIAR;

            resultadoEscolha = escolherPeca(tabuleiro, escolha);

        } while (resultadoEscolha != '\n' && resultadoEscolha != ' ');

        clear();
        imprimirTabuleiro(tabuleiro);
        printw("\nCoordenadas: x = %d, y = %d.\n", tabuleiro->posSelecao.x, tabuleiro->posSelecao.y);

        int movimento;
        printw("\nMOVIMENTE A PEÇA COM WASD ou SETINHAS.\n");
        printw("QUALQUER OUTRA TECLA CANCELA O MOVIMENTO.");
        do {
            movimento = getch();
            movimento = movimentarPeca(tabuleiro, movimento);
        } while(!movimento);

        estadoDoJogo = checarPossibilidades(tabuleiro);
    }

    clear();
    imprimirTabuleiro(tabuleiro);

    return estadoDoJogo;
}


/*
    Salva o estado atual do tabuleiro num arquivo.

    Linha 1: tamanho do tabuleiro (x y).
    Linha 2: posição atual do cursor (x y).
    Resto: matriz de tam.x por tam.y do tabuleiro.
*/
void salvar(Tabuleiro *tabuleiro, char *nomeArquivo) {
    FILE *save = fopen(nomeArquivo, "w");

    if (save) {
        fprintf(save, "%d %d\n", tabuleiro->tam.x, tabuleiro->tam.y);
        fprintf(save, "%d %d\n", tabuleiro->posSelecao.x, tabuleiro->posSelecao.y);

        for (int i = 0; i < tabuleiro->tam.x; i++) {
            for (int j = 0; j < tabuleiro->tam.y; j++) {
                fprintf(save, "%d", tabuleiro->posicoes[i][j]);
                if (j < tabuleiro->tam.y-1) fprintf(save, " ");
            }
            fprintf(save, "\n");
        }

        fclose(save);
    }
}


int lerIntArquivo(FILE *arquivo) {
    int charLido, numChars = 0;
    char *cadeia = (char*) malloc(sizeof(char));
    do {
        charLido = fgetc(arquivo);

        if (charLido != ' ' && charLido != '\n') {
            cadeia = (char*) realloc(cadeia, sizeof(char) * (numChars + 1));
            cadeia[numChars] = charLido;

            numChars++;
        }
    } while (charLido != ' ' && charLido != '\n');

    int resultante = atoi(cadeia);

    free(cadeia);

    return resultante;
}


/*
    Carrega um arquivo de tabuleiro.
    Sua estrutura deve ser:
    tamx tamy
    posx posy
    TABULEIRO SEPARADO POR ESPAÇOS E QUEBRAS DE LINHA
    (linha vazia)
*/
Tabuleiro* carregar(char *nomeArquivo) {
    FILE *arquivo = fopen(nomeArquivo, "r");

    if (arquivo) {
        Tabuleiro *tabuleiro = malloc(sizeof(Tabuleiro));
        
        Vector2 tam = {lerIntArquivo(arquivo), lerIntArquivo(arquivo)};
        Vector2 posSelecao = {lerIntArquivo(arquivo), lerIntArquivo(arquivo)};

        tabuleiro->tam = tam;
        tabuleiro->posSelecao = posSelecao;

        // Aloca o espaço necessário para o tabuleiro.
        tabuleiro->posicoes = malloc(sizeof(int*) * tabuleiro->tam.x);
        for (int i = 0; i < tabuleiro->tam.x; i++) {
            tabuleiro->posicoes[i] = malloc(sizeof(int) * tabuleiro->tam.y);
        }

        // Lê os valores das posições do tabuleiro pelo arquivo.
        for (int i = 0; i < tabuleiro->tam.x; i++) {
            for (int j = 0; j < tabuleiro->tam.y; j++) {
                int posAtual = lerIntArquivo(arquivo);

                tabuleiro->posicoes[i][j] = posAtual;
            }
        }

        fclose(arquivo);

        return tabuleiro;
    }

    return gerarTabuleiro(); // Se não funcionou, ele só gera o tabuleiro genérico.
}


void jogar(char *nomeArquivo) {
    int estadoFinal = REINICIAR;

    while (estadoFinal == REINICIAR) {
        Tabuleiro *tabuleiro;
        
        if (!nomeArquivo) tabuleiro = gerarTabuleiro();
        else {
            tabuleiro = carregar(nomeArquivo);
        }

        estadoFinal = iniciarJogo(tabuleiro);

        if (estadoFinal == GANHOU)
            printw("\nVOCÊ CONSEGUIU!\n\nRESTA UM e UM APENAS!!!!");
        else if (estadoFinal == PERDEU)
            printw("\nVocê perdeu. Acabaram as possibilidades de movimento.");
        else if (estadoFinal == REINICIAR) {
            clear();
            printw("REINICIANDO...");
        }
        else if (estadoFinal == SAIR) {
            clear();
            salvar(tabuleiro, (char*) NOME_SAVE);
            printw("SALVANDO E VOLTANDO AO MENU...");
        }
        
        destruirTabuleiro(tabuleiro);
        
        refresh(); sleep(1);

        printw("\n\nAPERTE QUALQUER TECLA."); getch();
    }
}