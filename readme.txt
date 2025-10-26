# mEditor - Editor de Texto em Linha de Comando

## Descrição
mEditor é um editor de texto simples e eficiente que funciona diretamente no terminal.
Ele foi projetado com funções simples de edição de texto, como inserir linhas,
apagar linhas, concatenar linhas,exibir o conteúdo do arquivo e salvar alterações.
Além de outras funções como separar uma linha em duas e juntar duas linhas em uma.

## Funcionalidades
- Inserir linhas de texto em posições específicas.
- Apagar linhas de texto.
- Concatenar linhas de texto.
- Exibir o conteúdo do arquivo.
- Salvar alterações no arquivo.
- Separar uma linha em duas.
- Juntar duas linhas em uma.

## Comandos Disponíveis
- i <n> <texto>: insere uma nova linha com o texto especificado na posição n.
- d <n>: apaga a linha na posição n.
- r <n> <texto>: substitui o conteúdo da linha n pelo texto especificado.
- a <n> <texto>: concatena o texto especificado ao final da linha n.
- split <n> <pos>: separa a linha n na posição pos em duas linhas.
- join <n>: junta a linha n com a linha n+1.
- print: exibe o conteúdo atual do arquivo.
- open <nome_arquivo>: abre um arquivo existente para edição.
- save <nome_arquivo>: salva o conteúdo atual num arquivo com o nome especificado.
- quit: sai do editor.

## Como Executar
1. Certifique-se de ter um Compilador C instalado (como gcc).
2. Certifique-se de ter o CMake instalado.
3. Rode os seguintes comandos no terminal:

- Dê build no projeto com o comando:
        cmake --build cmake-build-debug -- -j 4
- Execute o programa com o comando:
        ./cmake-build-debug/projeto-main

## Observação
- Para que o arquivo de texto funcione corretamente, certifique-se de que ele esteja no mesmo diretório do
executável ou forneça o caminho completo ao abrir ou salvar arquivos.