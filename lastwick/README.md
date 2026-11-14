# Last Wick

**Last Wick** e um jogo bidimensional de sobrevivencia com perspectiva superior (top-down), desenvolvido em C++17 utilizando a biblioteca grafica raylib 5.5, como projeto academico para a disciplina de Programacao Orientada a Objetos.

## Descricao e Conceito

Em uma regiao ficticia envolta por brumas e nevoa perene, o ultimo guardiao de uma torre ancestral carrega a unica lanterna funcional e precisa manter seu pavio aceso durante tres noites sucessivas (90 segundos de sobrevivencia por noite, com dificuldade progressiva).

O alcance visual e a seguranca do guardiao dependem diretamente do combustivel (oleo) da lanterna, que queima gradativamente. Fora da iluminacao, sombras se deslocam com agilidade multiplicada por 1.6. Ao penetrarem o halo de luz, as entidades sombrias sofrem exposicao luminosa progressiva ate se dissiparem. Em momentos criticos, o guardiao pode liberar um Clarao — uma detonacao concentrada de luz que atinge 1.8x o raio da lanterna e aplica exposicao extra a todos os inimigos no raio, consumindo 25 de combustivel com recarga de 6 segundos. O jogador inicia com 3 vidas e recebe uma janela de invulnerabilidade de 1.5 segundos ao sofrer dano.

## Variedade de Inimigos

- **Crawler**: Criatura insetoide sombria que persegue o guardiao em linha reta (velocidade 110, morre com 1.0 s de exposicao a luz, causa 1 ponto de dano de contato).
- **Wisp**: Espectro agil que avanca em zigue-zague senoidal (velocidade 170, dissipa-se com 0.5 s de exposicao, causa 1 ponto de dano de contato).
- **Giant**: Colosso resistente e macico (velocidade 60, raio amplo de colisao, requer 3.0 s de exposicao, causa 2 pontos de dano de contato).

Os corpos das criaturas permanecem ocultos sob a mascara de escuridao, revelando-se apenas sob a luz da lanterna, enquanto seus olhos avermelhados brilham diretamente atraves das sombras para sinalizar a aproximacao.

## Itens Coletaveis (Pickups)

Gerados aleatoriamente pelo `PickupSpawner` (Factory Method) a cada 8 a 12 segundos, com permanencia de 15 segundos no chao e limite de 3 itens simultaneos. Possuem brilho pulsante proprio visivel atraves da escuridao:

- **OilFlask**: Frasco ancestral que restaura +35 de combustivel na lanterna (limitado ao maximo de 100).
- **Amulet**: Amuleto protetor que concede escudo mistico de 6 segundos, ignorando qualquer dano de contato.
- **Boots**: Botas aladas que amplificam a velocidade de locomocao em x1.4 por 6 segundos.

## Progressao de Noites e Pontuacao

- **RunState**: Estado persistente mantido entre as noites (pontuacao, vidas, noite atual e inimigos derrotados).
- **NightIntroScene**: Exibe "Noite N" por 3 segundos antes do inicio de cada noite.
- **Finalizacao da Noite**: Ao esgotar os 90 segundos de sobrevivencia, o jogador recebe +100 pontos de bonificacao, mantem vidas e pontuacao, restaura o combustivel para 100 e avanca para a noite seguinte.
- **Vitoria Total**: Ao sobreviver a 3ª noite, concede um bonus adicional de +50 pontos por vida restante e transita para a tela de vitoria.
- **Recorde Persistente (HighScoreStore)**: Leitura e escrita tolerante a falhas do recorde no arquivo `highscore.txt`.

## Sistema de Audio Procedural (AudioManager)

Sintese de audio inteiramente realizada em codigo C++ utilizando buffers de ondas senoidais, quadradas, arpejos e ruido modulado via `Wave` e `LoadSoundFromWave` da raylib, sem dependencia de arquivos de midia externos:

- **Inimigo Derrotado**: Dissipacao de frequencia descendente com ruido.
- **Jogador Atingido**: Impacto percussivo de onda quadrada.
- **Clarao**: Detonacao luminosa ascendente.
- **Item Coletado**: Arpejo de sino cristalino.
- **Amanhecer**: Acorde maior triunfal de conclusao de noite.
- **Derrota**: Tom sombrio menor de encerramento da chama.

## Controles

- **W, A, S, D** ou **Setas do Teclado**: Movimentacao suave do guardiao (com normalizacao de diagonais)
- **Barra de Espaco**: Acionamento do Clarao da lanterna (consome 25 de oleo e possui recarga de 6 s)
- **P** ou **ESC**: Alterna a tela de pausa com congelamento total da simulacao
- **M (no menu de pausa)**: Retorna ao Menu Principal
- **ENTER**: Avanca introducoes e confirma opcoes
- **R (na tela final)**: Reinicia uma nova partida a partir da Noite 1

## Arquitetura de Software e Orientacao a Objetos

- **Abstracao e Polimorfismo de Entidades**: Classes abstratas `Entity`, `Enemy` e `Pickup` estendidas polimorficamente por `Player`, `Crawler`, `Wisp`, `Giant`, `OilFlask`, `Amulet` e `Boots`.
- **Factory Method**: Implementado em `EnemySpawner::createEnemy` e `PickupSpawner::createPickup`.
- **Padrao Observer**: `EventBus` centralizado com inscricoes desacopladas de `HUD` e `AudioManager`.
- **Padrao State**: Gerenciador `SceneManager` intermediando `MenuScene`, `NightIntroScene`, `GameScene` e `GameOverScene`.
- **Composicao**: `Player` compoe uma instancia dedicada de `Lantern`.
- **Centralizacao de Parametros (GameConfig)**: Eliminacao de numeros magicos com constantes `constexpr`.
- **Gerenciamento Seguro de Recursos (RAII)**: Envelopamento de texturas, recursos sonoros e audio.

## Estrutura de Diretorios

```text
lastwick/
├── CMakeLists.txt        # Configuracao de compilacao com FetchContent do raylib 5.5
├── README.md             # Documentacao do projeto e instrucoes de execucao
├── .clang-format         # Padronizacao de formatacao de codigo-fonte
├── assets/               # Recursos de midia externos do jogo
└── src/
    ├── main.cpp          # Ponto de entrada, configuracao da janela e audio
    ├── core/             # Nucleo da aplicacao, configuracoes, eventos, audio e cenas
    │   ├── AudioManager.h
    │   ├── AudioManager.cpp
    │   ├── EventBus.h
    │   ├── EventBus.cpp
    │   ├── GameConfig.h
    │   ├── HighScoreStore.h
    │   ├── HighScoreStore.cpp
    │   ├── RaylibResource.h
    │   ├── RunState.h
    │   ├── Scene.h
    │   ├── SceneManager.h
    │   └── SceneManager.cpp
    ├── entities/         # Entidades, componentes, inimigos e itens
    │   ├── Amulet.h
    │   ├── Amulet.cpp
    │   ├── Boots.h
    │   ├── Boots.cpp
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
    │   ├── OilFlask.h
    │   ├── OilFlask.cpp
    │   ├── Pickup.h
    │   ├── Pickup.cpp
    │   ├── PickupSpawner.h
    │   ├── PickupSpawner.cpp
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
        ├── MenuScene.cpp
        ├── NightIntroScene.h
        └── NightIntroScene.cpp
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
