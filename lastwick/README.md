# Last Wick

**Last Wick** é um jogo bidimensional de ação e sobrevivência em visão superior (*top-down*), desenvolvido em C++17 com a biblioteca gráfica e de multimídia raylib 5.5. O projeto foi concebido e implementado do zero para a disciplina de Programação Orientada a Objetos.

---

## Informações Acadêmicas

- **Autor:** Thomas Batista Pinto
- **Disciplina:** Programação Orientada a Objetos
- **Instituição:** Instituto Federal de Educação, Ciência e Tecnologia de São Paulo (IFSP), Campus Campos do Jordão

---

## Conceito e Ambientação

Em uma vasta região fictícia eternamente recoberta por uma névoa densa e impenetrável, o último guardião de uma antiga torre solitária carrega consigo a última lanterna acesa da humanidade. Sua missão vital consiste em defender e alimentar o pavio da lanterna por 3 noites consecutivas (cada uma durando 90 segundos), enfrentando hordas crescentes de entidades hostis feitas de trevas e sombras.

O raio luminoso que protege o guardião depende diretamente da quantidade de combustível (óleo) restante na lanterna, que queima gradualmente ao longo do tempo. Quando as criaturas da névoa estão no escuro, sua velocidade é amplificada em 1.6x; ao adentrarem o halo de iluminação do jogador, passam a sofrer exposição luminosa acumulada até serem purificadas e eliminadas.

Em situações críticas de cerco, o guardião pode acionar o **Clarão da Lanterna** (*Flare*), gerando uma onda de choque de luz concentrada que atinge uma área 1.8x maior que o raio habitual da lanterna, aplicando alta carga de exposição a todas as sombras circundantes em troca de 25 unidades de óleo e um tempo de recarga de 6 segundos.

---

## Controles do Jogo

| Comando | Tecla / Ação | Descrição |
| :--- | :--- | :--- |
| **Movimentação** | `W`, `A`, `S`, `D` ou `Setas` | Movimenta o guardião pelo mapa com normalização vetorial e contenção de bordas. |
| **Clarão da Lanterna** | `Barra de Espaço` | Emite explosão de luz (gasta 25 de óleo, recarga de 6 s) e atinge sombras próximas. |
| **Pausar / Retomar** | `P` | Alterna a sobreposição de pausa congelando tempo, recargas e entidades. |
| **Menu Principal** | `M` (na tela de pausa) | Abandona a sessão em andamento e retorna à tela de abertura. |
| **Confirmar / Iniciar** | `ENTER` | Inicia o jogo no menu principal ou avança para a próxima etapa. |
| **Tentar Novamente** | `R` (na tela final) | Reinicia imediatamente uma nova partida completa a partir da Noite 1. |

*Observação:* Para assegurar a estabilidade das sessões e evitar saídas acidentais, a tecla ESC não fecha a aplicação nem interfere na pausa. O encerramento do jogo ocorre exclusivamente pelo botão de fechar da janela gráfica.

---

## Tabela de Inimigos

As sombras que habitam a névoa eterna manifestam-se em três formas com comportamentos e atributos específicos:

| Inimigo | Velocidade (Luz / Escuro) | Exposição para Morte | Dano de Contato | Comportamento e Dinâmica |
| :--- | :--- | :--- | :--- | :--- |
| **Crawler** | 110 px/s / 176 px/s | 1.0 s | 1 ponto | Persegue o jogador em trajetória retilínea direta, avançando em grupo. |
| **Wisp** | 170 px/s / 272 px/s | 0.5 s | 1 ponto | Espectro veloz que se desloca em zigue-zague senoidal perpendicular ao avanço. |
| **Giant** | 60 px/s / 96 px/s | 3.0 s | 2 pontos | Colosso lento e massivo de grande raio; absorve grande quantidade de luz. |

Todos os inimigos se camuflam integralmente sob a camada escura de névoa, tornando seus corpos visíveis apenas quando iluminados pela lanterna. Contudo, seus olhos vermelhos e sinistros brilham mesmo através da escuridão, alertando o guardião da aproximação de perigo.

---

## Tabela de Itens Coletáveis (*Pickups*)

Surgem aleatoriamente pelo mapa através do `PickupSpawner` (intervalo de 8 a 12 segundos, com expiração de 15 segundos e teto de 3 itens simultâneos). Emitem um brilho místico pulsante visível através das trevas:

| Item | Representação Visual | Efeito Aplicado | Duração / Capacidade |
| :--- | :--- | :--- | :--- |
| **OilFlask** | Frasco alquímico de vidro ambarino | Restaura +35 de combustível na lanterna | Imediato (limitado ao teto de 100 de óleo) |
| **Amulet** | Relíquia sagrada em losango ciano | Barreira protetora que ignora todo dano | 6.0 segundos (exibe anel místico ao redor do herói) |
| **Boots** | Botas aladas com asas douradas | Acelera a locomoção do guardião em 1.4x | 6.0 segundos (rastro veloz de movimento) |

---

## Mapeamento de Conceitos de POO

O projeto foi estruturado seguindo rigorosamente os pilares e padrões clássicos de Programação Orientada a Objetos:

| Conceito de POO | Aplicação no Projeto | Arquivos Principais |
| :--- | :--- | :--- |
| **Abstração** | Interfaces e contratos puros definindo o ciclo de vida de entidades e cenas sem acoplamento a implementações concretas. | `src/entities/Entity.h`, `src/entities/Enemy.h`, `src/entities/Pickup.h`, `src/core/Scene.h` |
| **Encapsulamento** | Dados internos e estados críticos protegidos com métodos de acesso controlados e invariantes estritas. | `src/entities/Lantern.h`, `src/entities/Player.h`, `src/core/HighScoreStore.h`, `src/core/EventBus.h` |
| **Herança** | Especialização em hierarquias bem delineadas: entidades concretas herdando de `Entity`; inimigos especializados herdando de `Enemy`; itens herdando de `Pickup`; cenas herdando de `Scene`. | `src/entities/Crawler.h`, `src/entities/Wisp.h`, `src/entities/Giant.h`, `src/entities/OilFlask.h`, `src/scenes/GameScene.h` |
| **Polimorfismo** | Execução dinâmica em tempo de execução via tabelas virtuais para atualização e renderização de coleções heterogêneas de criaturas e itens. | `src/entities/Enemy.h` (`update`, `draw`, `drawEyes`, `contactDamage`), `src/entities/Pickup.h` (`apply`, `draw`) |
| **Composição** | Construção de objetos complexos a partir de subsistemas menores coesos (`Player` compõe `Lantern`; `GameScene` agrega `EnemySpawner`, `PickupSpawner`, `ParticleSystem`, `AudioManager` e `HUD`). | `src/entities/Player.h`, `src/scenes/GameScene.h` |
| **Padrão State** | Transição elegante entre estados de aplicação (Menu, Introdução de Noite, Jogo Ativo e Fim de Jogo) com suporte a *fade in/out*. | `src/core/SceneManager.h`, `src/core/SceneManager.cpp`, `src/core/Scene.h` |
| **Padrão Observer** | Barramento centralizado para comunicação desacoplada entre a simulação e os observadores visuais, estatísticos e sonoros. | `src/core/EventBus.h`, `src/core/EventBus.cpp`, `src/scenes/HUD.h`, `src/core/ParticleSystem.h`, `src/core/AudioManager.h` |
| **Padrão Factory Method** | Instanciação polimórfica desacoplada de inimigos e itens através de métodos de fabricação dedicados. | `src/entities/EnemySpawner.h` (`createEnemy`), `src/entities/PickupSpawner.h` (`createPickup`) |
| **RAII (*Resource Acquisition Is Initialization*)** | Gerenciamento autônomo e determinístico do ciclo de vida de texturas de renderização, amostras de onda e efeitos de áudio. | `src/core/RaylibResource.h` (`RenderTextureWrapper`), `src/core/AudioManager.h` (`SoundWrapper`, `WaveWrapper`) |

---

## Estrutura de Diretórios

```text
lastwick/
├── CMakeLists.txt              # Configuração CMake do projeto, raylib 5.5 e testes
├── README.md                   # Documentação integral do projeto e guia de execução
├── .clang-format               # Regras de estilização e formatação de código C++
├── .gitignore                  # Filtro de arquivos gerados, builds e temporários
├── assets/                     # Pasta reservada para recursos adicionais
├── src/
│   ├── main.cpp                # Ponto de entrada, janela, taxa de quadros e áudio
│   ├── core/                   # Núcleo arquitetural, utilitários, áudio e eventos
│   │   ├── AudioManager.h      # Síntese procedural de ondas sonoras e RAII
│   │   ├── AudioManager.cpp
│   │   ├── EventBus.h          # Barramento de eventos (Padrão Observer)
│   │   ├── EventBus.cpp
│   │   ├── GameConfig.h        # Constantes globais de balanceamento e física
│   │   ├── HighScoreStore.h    # Persistência de recordes com tolerância a falhas
│   │   ├── HighScoreStore.cpp
│   │   ├── ParticleSystem.h    # Sistema de partículas para feedback visual
│   │   ├── ParticleSystem.cpp
│   │   ├── RaylibResource.h    # Encapsulamento RAII de recursos do raylib
│   │   ├── RunState.h          # Estrutura de estado persistente entre noites
│   │   ├── Scene.h             # Interface base abstrata de cena
│   │   ├── SceneManager.h      # Máquina de estados finitos com fade (Padrão State)
│   │   └── SceneManager.cpp
│   ├── entities/               # Entidades lógicas, jogador, inimigos e itens
│   │   ├── Amulet.h            # Item de invulnerabilidade temporária
│   │   ├── Amulet.cpp
│   │   ├── Boots.h             # Item de aceleração de velocidade
│   │   ├── Boots.cpp
│   │   ├── Crawler.h           # Inimigo rastejante de perseguição linear
│   │   ├── Crawler.cpp
│   │   ├── Enemy.h             # Classe abstrata base para entidades hostis
│   │   ├── Enemy.cpp
│   │   ├── EnemySpawner.h      # Gerador e fábrica polimórfica de inimigos
│   │   ├── EnemySpawner.cpp
│   │   ├── Entity.h            # Classe abstrata base do modelo de entidades
│   │   ├── Entity.cpp
│   │   ├── Giant.h             # Inimigo titânico lento e resistente
│   │   ├── Giant.cpp
│   │   ├── Lantern.h           # Componente de iluminação, óleo e clarão
│   │   ├── Lantern.cpp
│   │   ├── OilFlask.h          # Item de recarga de combustível da lanterna
│   │   ├── OilFlask.cpp
│   │   ├── Pickup.h           # Classe abstrata base para itens coletáveis
│   │   ├── Pickup.cpp
│   │   ├── PickupSpawner.h     # Gerador e fábrica polimórfica de itens
│   │   ├── PickupSpawner.cpp
│   │   ├── Player.h            # Guardião controlado pelo usuário
│   │   ├── Player.cpp
│   │   ├── Wisp.h              # Inimigo etéreo veloz com movimento senoidal
│   │   └── Wisp.cpp
│   └── scenes/                 # Telas concretas do jogo e HUD
│       ├── GameOverScene.h     # Tela de resultado (vitória ou derrota) e recorde
│       ├── GameOverScene.cpp
│       ├── GameScene.h         # Cena central de jogabilidade e orquestração
│       ├── GameScene.cpp
│       ├── HUD.h               # Mostrador de vidas, combustível, tempo e pontos
│       ├── HUD.cpp
│       ├── MenuScene.h         # Tela de título e instrução de comandos
│       ├── MenuScene.cpp
│       ├── NightIntroScene.h   # Tela de transição temática entre noites
│       └── NightIntroScene.cpp
└── tests/
    └── test_main.cpp           # Suíte de testes unitários automatizados
```

---

## Nota Técnica: Geração Procedural de Áudio e Gráficos

O jogo adota uma filosofia autossuficiente e determinística em sua apresentação audiovisual:

1. **Gráficos por Primitivas Geométricas e Mistura de Canais:**
   Todo o visual — desde os galhos retorcidos das árvores mortas, o guardião encapuzado, as três criaturas das trevas até os três itens mágicos — é desenhado diretamente em tempo real por primitivas matemáticas da raylib. A iluminação dinâmica emprega uma textura alvo auxiliar (*RenderTexture2D*) na qual é desenhado o gradiente de luz do guardião, multiplicada sobre o cenário físico com o modo `BLEND_MULTIPLIED` e complementada por um brilho aditivo `BLEND_ADD_COLORS`.

2. **Síntese Sonora Procedural sem Arquivos Externos:**
   O `AudioManager` sintetiza todas as amostras de áudio diretamente em memória (alocação de buffers de ondas via `Wave`), modulando frequências senoidais, quadradas e ruído branco pseudoaleatório com decaimento exponencial de envelope. Cada som é convertido em um recurso de áudio da raylib (`Sound`) encapsulado com semântica RAII para desalocação automática, cobrindo:
   - Impacto de dano no guardião;
   - Purificação e eliminação de inimigos;
   - Detonação do clarão da lanterna;
   - Coleta de artefatos com arpejo cintilante;
   - Alvorada triunfal e esgotamento da chama.

---

## Instruções de Compilação e Execução

### Pré-requisitos
- Compilador C++ com suporte integral a C++17 (MinGW-w64 GCC 11+ ou Clang 13+);
- CMake 3.16 ou superior;
- Gerador Ninja (opcional, recomendado para compilação veloz);
- Conexão à internet na primeira execução para download automático do raylib 5.5 via `FetchContent`.

---

### Opção 1: Compilação pelo Qt Creator (Kit MinGW 64)

1. Abra o **Qt Creator**;
2. Vá em **File -> Open File or Project...** (`Ctrl + O`) e selecione o arquivo `CMakeLists.txt` na raiz da pasta `lastwick`;
3. Na tela de configuração de projetos, selecione o kit **MinGW 64-bit** (por exemplo, `Desktop Qt ... MinGW 64-bit` ou `Tools/mingw1310_64`);
4. Clique em **Configure Project** e aguarde a configuração inicial do CMake;
5. Pressione `Ctrl + B` para compilar o projeto ou `Ctrl + R` para executar o jogo diretamente.

---

### Opção 2: Compilação via Linha de Comando (CMake + Ninja ou Make)

Abra o terminal (PowerShell ou Bash) na raiz do projeto `lastwick`:

```bash
# 1. Configurar o diretório de build
cmake -B build -DCMAKE_BUILD_TYPE=Release -G Ninja

# 2. Compilar o executável do jogo e a suíte de testes
cmake --build build --config Release
```

O binário executável será gerado em:
- **Windows:** `build/LastWick.exe`
- **Linux/macOS:** `build/LastWick`

Para iniciar o jogo:
```bash
./build/LastWick.exe
```

---

## Execução dos Testes Automatizados (CTest)

Os testes unitários cobrem de forma independente de janela ou áudio as regras de negócio de `Lantern`, `RunState`, `HighScoreStore` e `EnemySpawner`:

```bash
# Executar a bateria de testes via CTest exibindo a saída detalhada
ctest --test-dir build --output-on-failure
```

Ou alternativamente executando o binário de testes diretamente:
```bash
./build/LastWickTests.exe
```
