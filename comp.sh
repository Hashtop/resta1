nome_build="trab1"

gcc main.c logica_jogo.c -o $nome_build -lm -lncursesw
comp=$?

echo ""

if [ $comp -eq 0 ]; then
    echo "$nome_build compilado. Rode usando \"./$nome_build\""
else
    echo "Compilação de $nome_build falhou. Tente novamente."
fi