# Last Wick

**Last Wick** e um jogo bidimensional de sobrevivencia com perspectiva superior (top-down), desenvolvido em C++17 utilizando a biblioteca grafica raylib 5.5, como projeto academico para a disciplina de Programacao Orientada a Objetos.

## Descricao e Conceito

Em uma regiao ficticia envolta por brumas e nevoa perene, o ultimo guardiao de uma torre ancestral carrega a unica lanterna funcional e precisa manter seu pavio aceso durante noites de nevoa densa (90 segundos de sobrevivencia por noite).

O alcance visual e a seguranca do guardiao dependem diretamente do combustivel (oleo) da lanterna, que queima gradativamente. Fora da iluminacao, sombras se deslocam com agilidade multiplicada por 1.6. Ao penetrarem o halo de luz, as entidades sombrias sofrem exposicao luminosa progressiva ate se dissiparem. Em momentos criticos, o guardiao pode liberar um Clarao — uma detonacao concentrada de luz que atinge 1.8x o raio da lanterna e aplica exposicao extra a todos os inimigos no raio, consumindo 25 de combustivel com recarga de 6 segundos. O jogador inicia com 3 vidas e recebe uma janela de invulnerabilidade de 1.5 segundos ao sofrer dano.

## Variedade de Inimigos

- **Crawler**: Criatura insetoide sombria que persegue o guardiao em linha reta (velocidade 110, morre com 1.0 s de exposicao a luz, causa 1 ponto de dano de contato).
- **Wisp**: Espectro agil que avanca em zigue-zague senoidal (velocidade 170, dissipa-se com 0.5 s de exposicao, causa 1 ponto de dano de contato).
- **Giant**: Colosso resistente e macico (velocidade 60, raio amplo de colisao, requer 3.0 s de exposicao, causa 2 pontos de dano de contato).

Os corpos das criaturas permanecem ocultos sob a mascara de escuridao, revelando-se apenas sob a luz da lanterna, enquanto seus olhos avermelhados brilham diretamente atraves das sombras para sinalizar a aproximacao.

## Controles

- **W, A, S, D** ou **Setas do Teclado**: Movimentacao suave do guardiao (com normalizacao de diagonais)
- **Barra de Espaco**: Acionamento do Clarao da lanterna (consome 25 de oleo e possui recarga de 6 s)
- **ESC**: Retorno ao menu principal ou saida da aplicacao
- **G**: Atalho de demonstracao para transicao rapida a tela de fim de jogo

## Arquitetura de Software e Orientacao a Objetos

A estrutura do projeto adota padroes de projeto consolidados:

- **Abstracao e Polimorfismo de Entidades**: A classe abstrata `Entity` estabelece a base para coordenadas e atualizacao. `Enemy` estende `Entity` com metodos virtuais para perseguicao, acumulo de exposicao e dano de contato, servindo de base para `Crawler`, `Wisp` e `Giant`.
- **Factory Method (EnemySpawner)**: Centraliza a logica de instanciacao das criaturas hostis atraves do metodo `createEnemy(EnemyType, Vector2)`, controlando taxas de surgimento fora do campo de visao da camera e respeitando o limite maximo de entidades ativas.
- **Padrao Observer (EventBus)**: Barramento central desacoplado que notifica ouvintes sobre eventos do jogo (`EnemyKilled`, `PlayerHit`, `FlareUsed`, `FuelEmpty`). O componente `HUD` inscreve-se no barramento para contabilizar a pontuacao (10 pontos por baixa).
- **Padrao State (Cenas)**: A interface `Scene` e o `SceneManager` coordenam o fluxo entre `MenuScene`, `GameScene` e `GameOverScene`.
- **Composicao**: O `Player` compoe uma instancia dedicada de `Lantern`, responsavel pela gestao do oleo, raio visual e recarga do clarao.
- **Centralizacao de Parametros (GameConfig)**: Todas as constantes de balanceamento e configuracao estao isoladas em `GameConfig.h` com identificadores `constexpr`.
- **Gerenciamento Seguro de Recursos (RAII)**: Encapsulamento de recursos do raylib (`RenderTextureWrapper`, `AudioDeviceGuard`, `SoundWrapper`, `FontWrapper`) assegurando liberacao automatica de memoria de video e recursos do sistema.

## Estrutura de Diretorios

```text
lastwick/
├── CMakeLists.txt        # Configuracao de compilacao com FetchContent do raylib 5.5
├── README.md             # Documentacao do projeto e instrucoes de execucao
├── .clang-format         # Padronizacao de formatacao de codigo-fonte
├── assets/               # Recursos de midia externos do jogo
└── src/
    ├── main.cpp          # Ponto de entrada, configuracao da janela e laco principal
    ├── core/             # Nucleo da aplicacao, configuracoes, eventos e cenas
    │   ├── EventBus.h
    │   ├── EventBus.cpp
    │   ├── GameConfig.h
    │   ├── RaylibResource.h
    │   ├── Scene.h
    │   ├── SceneManager.h
    │   └── SceneManager.cpp
    ├── entities/         # Entidades, componentes e gerador de inimigos
    │   ├── Crawler.h
    │   ├── Crawler.cpp
    │   ├── Enemy.h
    │   ├── Enemy.cpp
    │   ├── EnemySpawner.h
    │   ├── EnemySpawner.cpp
    │   ├── Entity.h
    │   ├── Entity.cpp
    │   ├── Giant.h
    │   ├── Giant.cpp
    │   ├── Lantern.h
    │   ├── Lantern.cpp
    │   ├── Pickup.h
    │   ├── Player.h
    │   ├── Player.cpp
    │   ├── Wisp.h
    │   └── Wisp.cpp
    └── scenes/           # Telas de jogo e interface
        ├── GameOverScene.h
        ├── GameOverScene.cpp
        ├── GameScene.h
        ├── GameScene.cpp
        ├── HUD.h
        ├── HUD.cpp
        ├── MenuScene.h
        └── MenuScene.cpp
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
