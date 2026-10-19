# resta1
Uma implementação **(NÃO-VIBECODADA e ROBUSTA)** do jogo Resta Um em C (com NCurses/Linux).

Funciona principalmente utilizando estruturas, passagem por referência e alocação dinâmica de memória.

A implementação é bastante modular e pouco engessada, de forma que tabuleiros customizados (de tamanhos diferentes, não-quadrados, pequenos, grandes) já são suportados pela alteração dos arquivos de save. Resta apenas a codificação de um editor de níveis, de forma a facilitar esse processo, mas já é possível, pela linha de comando, direcionar o programa ao arquivo de tabuleiro `.tab` que deseja jogar.

## Como compilar
Para compilar é necessário ter o `gcc` e `ncurses` instalados, abrir o terminal na mesma pasta do arquivo `comp.sh` e rodá-lo com o comando `bash comp.sh`.

## Como rodar
Abrindo o terminal na mesma pasta do binário compilado ou baixado, use o comando  `./trab1`.

#### JOGANDO TABULEIROS CUSTOMIZADOS
Acrescente ao comando `./trab1` o caminho relativo ao binário, ou o caminho absoluto, do arquivo `.tab` que deseja jogar.
Ex: `./trab1 Tabuleiros/tab1.tab` ou `./trab1 "Tabuleiros/tab1.tab"` (aspas são desconsideradas).