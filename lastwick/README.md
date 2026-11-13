# Last Wick

**Last Wick** e um jogo bidimensional de sobrevivencia com perspectiva superior (top-down), desenvolvido em C++17 utilizando a biblioteca grafica raylib 5.5, como projeto academico para a disciplina de Programacao Orientada a Objetos.

## Descricao e Conceito

Em uma regiao ficticia envolta por brumas e nevoa perene, o ultimo guardiao de uma torre ancestral carrega a unica lanterna funcional e precisa manter seu pavio aceso durante tres noites sucessivas (90 segundos cada).

O alcance visual e a seguranca do guardiao dependem diretamente do combustivel (oleo) da lanterna, que queima gradativamente. Fora da iluminacao, sombras se deslocam com agilidade crescente. Ao penetrarem o halo de luz, as entidades sombrias sofrem exposicao luminosa progressiva ate se dissiparem. Em momentos criticos, o guardiao pode liberar um Clarao — uma detonacao concentrada de luz que afasta e consome as sombras ao redor ao custo de uma fracao de combustivel e sujeito a um tempo de recarga. O jogador inicia com 3 vidas.

## Controles

- **W, A, S, D** ou **Setas do Teclado**: Movimentacao suave do guardiao (com normalizacao de diagonais)
- **Barra de Espaco**: Acionamento do Clarao da lanterna (consome combustivel e possui recarga)
- **ESC**: Retorno ao menu principal ou saida da aplicacao
- **G**: Atalho de demonstracao para transicao rapida a tela de fim de jogo

## Arquitetura de Software e Orientacao a Objetos

A estrutura do projeto foi concebida sob principios rigorosos de Orientacao a Objetos:

- **Abstracao e Polimorfismo de Entidades**: A classe abstrata `Entity` estabelece a interface comum (`update` e `draw`) e o gerenciamento de transformacao espacial. As classes `Player`, `Enemy` (abstrata) e `Pickup` (abstrata) estendem essa hierarquia basica.
- **Composicao**: O `Player` compoe uma instancia dedicada de `Lantern`, responsavel pelo calculo dinamicamente parametrizado do raio luminoso, queima de oleo e logica do clarao.
- **Padrao de Projeto State (Cenas)**: A interface `Scene` define o contrato para telas e estados do jogo. O `SceneManager` atua como contexto, intermediando transicoes entre `MenuScene`, `GameScene` e `GameOverScene` sem acoplamento direto.
- **Gerenciamento Seguro de Recursos (RAII)**: Encapsulamento dos recursos do raylib (como `RenderTexture2D`, fontes e subsistema de audio) em classes RAII que garantem alocacao estrita e descarregamento automatico de VRAM/RAM.
- **Iluminacao Dinamica 2D**: Criacao de mascara de escuridao renderizada em uma `RenderTexture2D` com degradê suave e combinada via multiplicacao cromatica (`BLEND_MULTIPLIED`).

## Estrutura de Diretorios

```text
lastwick/
├── CMakeLists.txt        # Configuracao de compilacao com FetchContent do raylib 5.5
├── README.md             # Documentacao do projeto e instrucoes de execucao
├── .clang-format         # Padronizacao de formatacao de codigo-fonte
├── assets/               # Recursos de midia externos do jogo
└── src/
    ├── main.cpp          # Ponto de entrada, configuracao da janela e laco principal
    ├── core/             # Nucleo da aplicacao, gerenciador de cenas e recursos RAII
    │   ├── Scene.h
    │   ├── SceneManager.h
    │   ├── SceneManager.cpp
    │   └── RaylibResource.h
    ├── entities/         # Hierarquia de entidades do jogo e componentes
    │   ├── Entity.h
    │   ├── Entity.cpp
    │   ├── Lantern.h
    │   ├── Lantern.cpp
    │   ├── Player.h
    │   ├── Player.cpp
    │   ├── Enemy.h
    │   └── Pickup.h
    └── scenes/           # Telas e estados do jogo
        ├── MenuScene.h
        ├── MenuScene.cpp
        ├── GameScene.h
        ├── GameScene.cpp
        ├── GameOverScene.h
        └── GameOverScene.cpp
```

## Como Compilar

### Requisitos

- Compilador C++ com suporte ao padrao C++17 (GCC/MinGW-w64 11+ ou Clang 13+)
- CMake 3.16 ou superior
- Git (utilizado pelo CMake para baixar o raylib 5.5 via FetchContent)

### Compilacao pelo Qt Creator (Kit MinGW 64)

1. Abra o Qt Creator.
2. Selecione **File -> Open File or Project...** e aponte para o arquivo `CMakeLists.txt` na raiz deste projeto.
3. Escolha o kit configurado **MinGW 64-bit** (por exemplo, `mingw1310_64`).
4. Clique em **Configure Project**.
5. Aguarde o download automatico do raylib 5.5 via FetchContent e a geracao dos arquivos de projeto.
6. Pressione `Ctrl + B` para compilar ou `Ctrl + R` para executar diretamente.

### Compilacao via Linha de Comando

```bash
# Configuracao do projeto gerando os arquivos de construcao
cmake -B build -DCMAKE_BUILD_TYPE=Release

# Compilacao do executavel
cmake --build build --config Release
```

O binario gerado estara disponivel em `build/LastWick.exe` (ou `build/LastWick` em ambientes Unix).
