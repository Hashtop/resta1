/*
Heitor de Oliveira Fonseca - CEFET-MG
    27/10/2026 - Trabalho 1

Implementa o jogo Resta 1 no qual, em um tabuleiro 7x7, deve-se comer todas as peças do tabuleiro até restar apenas uma.

A seleção das peças e a movimentação da peça escolhida é controlada com as setas e o estado do jogo atualiza a cada movimento.

No código, a localização dos 1s define a posição das peças. E os zeros, os espaços vazios.
*/


#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <unistd.h> // Pelo sleep
#include <locale.h> // Pela verificação de UTF-8 no terminal, garantindo caracteres especiais.
#include "globals.h"


#define GANHOU 0
#define PERDEU 1
#define REINICIAR 2
#define SAIR 3
#define CONTINUANDO -1


typedef struct Vector2 {
    int x, y;
} Vector2;

typedef struct Tabuleiro {
    int **posicoes;
    Vector2 tam;
    Vector2 posSelecao; // será sempre inicializado como x = 0 e y = 2 (primeira peça).
} Tabuleiro;


Tabuleiro* gerarTabuleiro();
void imprimirTabuleiro(Tabuleiro *tabuleiro);
void destruirTabuleiro(Tabuleiro *tabuleiro);
int movimentarPeca(Tabuleiro *tabuleiro, int direcao);
int checarPeca(Tabuleiro *tabuleiro, Vector2 coordenadasPeca);
int checarPecasAdjacentes(Tabuleiro *tabuleiro, Vector2 coordenadasPeca);
int escolherPeca(Tabuleiro *tabuleiro, int tecla);
int checarPossibilidades(Tabuleiro *tabuleiro);

int iniciarJogo(Tabuleiro *tabuleiro);
void salvar(Tabuleiro *tabuleiro, char *nomeArquivo);
Tabuleiro* carregar(char *nomeArquivo);

void jogar(char *nomeArquivo);


void imprimeMenu() {
    printw("JOGO RESTA UM\n\n");

    printw("1 - Jogar\n");
    printw("2 - Como jogar\n");

    FILE *save = fopen(NOME_SAVE, "r");
    if (save) printw("3 - Continuar jogo\n");

    printw("\n0 - Sair\n\n");
    
    printw("Digite sua escolha: ");
}


void imprimirTutorial() {
     printw("JOGO RESTA UM\n\n");

     printw("TUTORIAL\n");
     printw("Mova as peças de forma a comer as que estão logo ao lado.\n");
     printw("É preciso que, na mesma direção da peça a ser comida,\nhaja um espaço vazio para a peça se depositar!\n");

     printw("\nVence se restar APENAS UMA peça no final.\n\nDivirta-se!");

     printw("\n\n\nAPERTE QUALQUER TECLA PARA VOLTAR AO MENU.");
}


int main(int argc, char **argv)
{
    // Configura o locale para o valor padrão do sistema
    if (setlocale(LC_ALL, "") == NULL) {
        perror("Erro ao configurar locale");
        return 1;
    }

    initscr();
    start_color();

    keypad(stdscr, TRUE);

    int saindo = 0;

    while (!saindo) {
        clear();

        imprimeMenu(); refresh();

        echo();
        int respostaMenu = getch();

        clear();
        switch (respostaMenu) {
            case '1':
                jogar("Tabuleiros/padrao.tab");

                break;

            case '2':
                imprimirTutorial(); refresh();
                
                noecho(); getch();

                break;

            case '3':
                jogar((char*) NOME_SAVE);
                
                break;

            case '0':
                saindo = 1;

                break;

            default:
                printw("ERRO. SELECIONE UMA OPÇÃO VÁLIDA."); refresh();
                sleep(1);

                break;
        }
    }

    printw("EXECUÇÃO FINALIZADA.\n\nAperte qualquer tecla.");

    getch();
    echo();

    endwin();

    return 0;
}