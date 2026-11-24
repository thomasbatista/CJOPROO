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

Entre a primeira e a segunda noite, e entre a segunda e a terceira noite, o guardião tem acesso a uma seleção de **Melhorias** (*Upgrades*) que alteram permanentemente seus atributos e capacidades até o final da jornada.

---

## Controles do Jogo

| Comando | Tecla / Ação | Descrição |
| :--- | :--- | :--- |
| **Movimentação** | `W`, `A`, `S`, `D` ou `Setas` | Movimenta o guardião pelo mapa com normalização vetorial e contenção de bordas. |
| **Clarão da Lanterna** | `Barra de Espaço` | Emite explosão de luz (gasta 25 de óleo, recarga de 6 s) e atinge sombras próximas. |
| **Pausar / Retomar** | `P` | Alterna a sobreposição de pausa congelando tempo, recargas e entidades. |
| **Alternar Tela Cheia** | `F11` | Alterna entre janela e tela cheia sem bordas com proporção 16:9 (*letterbox*). |
| **Alternar Som** | `M` | Liga e desliga todo o áudio (música e efeitos) em qualquer cena. |
| **Escolher Melhoria** | `1`, `2`, `3` ou `Setas + ENTER` | Seleciona e confirma a dádiva desejada na tela de melhorias entre noites. |
| **Menu Principal** | `Q` (na tela de pausa) | Abandona a sessão em andamento e retorna à tela de abertura. |
| **Confirmar / Iniciar** | `ENTER` | Inicia o jogo no menu principal ou avança para a próxima etapa. |
| **Tentar Novamente** | `R` (na tela final) | Reinicia imediatamente uma nova partida completa a partir da Noite 1. |

*Observação:* Para assegurar a estabilidade das sessões e evitar saídas acidentais, a tecla ESC não fecha a aplicação nem interfere na pausa. O encerramento do jogo ocorre exclusivamente pelo botão de fechar da janela gráfica.

---

## Tabela de Inimigos

As sombras que habitam a névoa eterna manifestam-se em formas com comportamentos e atributos específicos:

| Inimigo | Velocidade (Luz / Escuro) | Exposição para Morte | Dano de Contato | Comportamento e Dinâmica |
| :--- | :--- | :--- | :--- | :--- |
| **Crawler** | 110 px/s / 176 px/s | 1.0 s | 1 ponto | Persegue o jogador em trajetória retilínea direta, avançando em grupo. Concede 10 pontos. |
| **Wisp** | 264 px/s / 422.4 px/s | 0.5 s | 1 ponto | Espectro veloz que se desloca em zigue-zague senoidal perpendicular ao avanço. Concede 10 pontos. |
| **Giant** | 111 px/s / 177.6 px/s | 3.0 s | 2 pontos | Colosso lento e massivo de grande raio; absorve grande quantidade de luz. Concede 10 pontos. |
| **Splitter** | 95 px/s / 152 px/s | 1.5 s | 1 ponto | Sombra losangular que, ao sucumbir à luz via método gancho `onDeath`, divide-se em dois fragmentos menores. Concede 10 pontos. |
| **Fragment** | 150 px/s / 240 px/s | 0.4 s | 1 ponto | Fragmento ágil derivado da divisão do Splitter. Não gera novas entidades ao morrer e concede 5 pontos. |

Todos os inimigos se camuflam integralmente sob a camada escura de névoa, tornando seus corpos visíveis apenas quando iluminados pela lanterna. Contudo, seus olhos vermelhos brilham mesmo através da escuridão, alertando o guardião da aproximação de perigo.

---

## Progressão das Noites por Fase

O `EnemySpawner` consulta uma linha do tempo orientada a dados (`NightConfig`) de acordo com o tempo decorrido da noite em curso:

| Noite | Fase (Tempo Decorrido) | Intervalo de Spawn | Crawler | Wisp | Giant | Splitter |
| :--- | :--- | :--- | :--- | :--- | :--- | :--- |
| **Noite 1** | 0 s – 30 s | 2.5 s | 100% | — | — | — |
| **Noite 1** | 30 s – 60 s | 2.0 s | 70% | 30% | — | — |
| **Noite 1** | 60 s – 90 s | 1.8 s | 50% | 30% | 20% | — |
| **Noite 2** | 0 s – 30 s | 1.8 s | 50% | 35% | — | 15% |
| **Noite 2** | 30 s – 60 s | 1.5 s | 30% | 45% | — | 25% |
| **Noite 2** | 60 s – 90 s | 1.3 s | 25% | 40% | 15% | 20% |
| **Noite 3** | 0 s – 65 s | 1.3 s | 30% | 30% | 15% | 25% |
| **Noite 3** | 65 s – 90 s *(Frenesi)* | 0.7 s | 30% | 30% | 15% | 25% |

---

## Mecânicas de Sobrevivência e Frenesi

- **Penalidade por Ficar Parado:** Se o guardião permanecer imóvel por mais de `1.5 s`, o raio base da lanterna diminui suavemente até `30%` ao longo de `2.0 s` (o clarão não é afetado). Ao voltar a se movimentar, o raio retorna ao normal em `0.5 s`.
- **Frenesi da Terceira Noite:** Nos últimos `25 s` da Noite 3 (a partir de `65 s` decorridos), o intervalo de surgimento cai para `0.7 s` e o evento `FrenzyStarted` é emitido no `EventBus`. O `MusicPlayer` acelera o andamento em `25%` e adiciona uma camada extra de notas mais agudas até o fim da noite.
- **Bônus de Óleo por Eliminação:** Cada criatura derrotada restaura combustível na lanterna (limitado ao máximo de `100`): `+2` unidades para `Crawler`, `Wisp`, `Splitter` e `Fragment`, e `+3` unidades para `Giant`.

---

## Tabela de Itens Coletáveis (*Pickups*)

Surgem aleatoriamente pelo mapa através do `PickupSpawner` (intervalo de 8 a 12 segundos, com expiração de 15 segundos e teto de 3 itens simultâneos). O gerador sorteia os tipos conforme probabilidades ponderadas e impede o surgimento de mais óleo caso já existam 2 ou mais itens em cena (evitando acúmulo excessivo). Emitem brilho místico pulsante visível através das trevas:

| Item | Probabilidade Base | Representação Visual | Efeito Aplicado | Duração / Capacidade |
| :--- | :--- | :--- | :--- | :--- |
| **OilFlask** | 50% (bloqueado se 2+ itens no mapa) | Frasco alquímico de vidro ambarino | Restaura 35 unidades de óleo na lanterna mágica. | Imediato (limitado a 100 de óleo) |
| **Amulet** | 20% (ou 40% se OilFlask bloqueado) | Amuleto circular com gema radiante | Concede escudo místico com invulnerabilidade total a dano. | 6 segundos contínuos |
| **Boots** | 30% (ou 60% se OilFlask bloqueado) | Botas de couro alado com asas etéreas | Aumenta a velocidade de deslocamento do jogador em 1.4x. | 6 segundos contínuos |

---

## Sistema de Melhorias Entre Noites (*Strategy Pattern*)

Ao sobreviver com sucesso ao amanhecer da Noite 1 e da Noite 2, o guardião é conduzido à cena de escolha de melhorias (`UpgradeScene`). Três opções distintas são sorteadas aleatoriamente a partir do repositório `UpgradePool`. Cada melhoria pode ser selecionada até um máximo de 3 vezes por partida, sendo removida do sorteio ao atingir este teto.

| Melhoria | Classe Derivada | Efeito Acumulativo |
| :--- | :--- | :--- |
| **Luz Ampliada** | `WiderLightUpgrade` | +15% no raio máximo de iluminação da lanterna. |
| **Clarão Veloz** | `FasterFlareUpgrade` | -25% no tempo de recarga do clarão da lanterna. |
| **Vida Extra** | `ExtraLifeUpgrade` | +1 vida para o guardião (até o limite de 5 vidas). |
| **Chama Duradoura** | `LongBurnUpgrade` | -20% na taxa de consumo de combustível (óleo). |
| **Agilidade** | `SwiftnessUpgrade` | +10% na velocidade de deslocamento do guardião. |

Os multiplicadores acumulados são armazenados na estrutura `UpgradeModifiers` dentro de `RunState`, que persiste os dados através de todas as noites. As melhorias ativas e seus respectivos níveis podem ser consultados a qualquer momento na tela de pausa (`P`).

---

## Sistema de Dicas na Primeira Noite (*HintSystem*)

Durante os primeiros 25 segundos da Noite 1, o sistema `HintSystem` exibe orientações breves e elegantes com transição suave (*fade in / fade out*) na parte inferior da tela, introduzindo gradualmente ao jogador os conceitos de locomoção, clarão, queima das sombras e coleta de artefatos.

---

## Apresentação e Tela Cheia com Proporção Fixa (*Letterbox*)

Para garantir qualidade visual consistente em monitores de qualquer resolução ou proporção de aspecto (16:9, 16:10, 21:9), toda a cena é renderizada internamente em uma textura lógica de alta precisão de 1280x720 pixels (`RenderTextureWrapper`). Ao alternar para o modo tela cheia via `F11` (`ToggleBorderlessWindowed`), a imagem é escalada mantendo estritamente a proporção original de 16:9, preenchendo as áreas laterais ou verticais excedentes com faixas pretas (*letterbox* / *pillarbox*).

---

## Padrões de Projeto e Arquitetura Orientada a Objetos

O projeto segue rigorosamente os princípios de responsabilidade única (SRP), inversão de dependência (DIP) e orientação a interfaces:

1. **State Pattern:**
   O gerenciamento do fluxo do jogo é estruturado através da classe base abstrata `Scene` e da máquina de estados `SceneManager`. As transições ocorrem de forma suave via *fade* temporal:
   - `MenuScene`: Tela de abertura, instruções de controles e consulta ao recorde histórico.
   - `NightIntroScene`: Transição temática de 3 segundos apresentando o número e contexto da noite.
   - `GameScene`: Simulação do mundo, física, combate, câmera com tremor e iluminação.
   - `UpgradeScene`: Seleção interativa de três dádivas entre as noites com cartões estilizados.
   - `GameOverScene`: Apresentação das estatísticas finais, distinção entre vitória e derrota e persistência de recordes.

2. **Observer Pattern:**
   Implementado pelo barramento desacoplado `EventBus`, permitindo que entidades e subsistemas emitam e recebam eventos tipados (`EnemyKilled`, `PlayerHit`, `FlareUsed`, `PickupCollected`, `DawnReached`, `UpgradeChosen`, etc.) sem acoplamento direto com a interface, sistema de partículas ou gerenciador sonoro.

3. **Strategy Pattern:**
   A hierarquia abstrata `Upgrade` encapsula o algoritmo e aplicação de cada dádiva sobre a estrutura `UpgradeModifiers`, permitindo adicionar novas melhorias sem modificar a lógica interna de `Player` ou `Lantern`.

4. **Factory Method e Gancho Polimórfico (*Template Method*):**
   - `UpgradePool::drawThree`: Sorteia e instancia polimorficamente as melhorias disponíveis.
   - `EnemySpawner::createEnemy`: Instancia as variantes concretas de criaturas hostis.
   - `Enemy::onDeath(std::vector<std::unique_ptr<Enemy>>&)`: Método virtual gancho invocado ao abater uma criatura. A implementação padrão não gera entidades, enquanto a subclasse `Splitter` instancia e adiciona dinamicamente dois novos `Fragment` ao vetor de entidades ativas.
   - `PickupSpawner::createPickup`: Instancia polimorficamente os itens coletáveis.

5. **Gerenciamento Seguro de Recursos (RAII):**
   Encapsulamento de buffers de áudio, fluxos de som contínuos e texturas gráficas em classes com destrutores determinísticos (`AudioManager`, `MusicPlayer`, `RenderTextureWrapper`), eliminando vazamentos de memória.

---

## Estrutura de Diretórios do Projeto

```text
lastwick/
├── CMakeLists.txt              # Configuração CMake e integração via FetchContent
├── README.md                   # Documentação abrangente da arquitetura e compilação
├── .clang-format               # Diretrizes de estilo de código automatizado
├── src/
│   ├── main.cpp                # Ponto de entrada, inicialização de janela e laço principal
│   ├── core/
│   │   ├── AudioManager.h      # Efeitos sonoros sintetizados matematicamente
│   │   ├── AudioManager.cpp
│   │   ├── EventBus.h          # Barramento desacoplado de eventos (Observer)
│   │   ├── EventBus.cpp
│   │   ├── GameConfig.h        # Constantes estáticas centralizadas de balanceamento
│   │   ├── HighScoreStore.h    # Persistência do recorde histórico em disco
│   │   ├── HighScoreStore.cpp
│   │   ├── LaunchOptions.h     # Análise dos argumentos de linha de comando (--demo)
│   │   ├── MusicPlayer.h       # Música ambiente contínua via AudioStream
│   │   ├── MusicPlayer.cpp
│   │   ├── ParticleSystem.h    # Sistema de emissão de partículas por primitivas
│   │   ├── ParticleSystem.cpp
│   │   ├── RaylibResource.h    # Invólucro RAII para alvos RenderTexture2D
│   │   ├── RunState.h          # Estado persistente da partida através das 3 noites
│   │   ├── Scene.h             # Interface base abstrata para o padrão State
│   │   ├── SceneManager.h      # Gerenciador de transições e laço de atualização de cenas
│   │   ├── SceneManager.cpp
│   │   ├── TextUtil.h          # Utilitário de quebra automática de linha com ajuste de fonte
│   │   ├── TextUtil.cpp
│   │   ├── Upgrade.h           # Interface base e subclasses de melhorias (Strategy)
│   │   ├── Upgrade.cpp
│   │   ├── UpgradeModifiers.h  # Estrutura com multiplicadores acumulados de melhorias
│   │   ├── UpgradePool.h       # Repositório e sorteio de melhorias (Factory Method)
│   │   └── UpgradePool.cpp
│   ├── entities/
│   │   ├── Amulet.h            # Item: Escudo de invulnerabilidade temporária
│   │   ├── Amulet.cpp
│   │   ├── Boots.h             # Item: Acelerador de velocidade de locomoção
│   │   ├── Boots.cpp
│   │   ├── Crawler.h           # Inimigo: Perseguidor retilíneo padrão
│   │   ├── Crawler.cpp
│   │   ├── Enemy.h             # Classe base abstrata para criaturas sombrias
│   │   ├── Enemy.cpp
│   │   ├── EnemySpawner.h      # Gerador de inimigos por noite com Factory Method
│   │   ├── EnemySpawner.cpp
│   │   ├── Entity.h            # Classe base abstrata para entidades bidimensionais
│   │   ├── Entity.cpp
│   │   ├── Fragment.h          # Inimigo derivado: fragmento veloz gerado pelo Splitter
│   │   ├── Fragment.cpp
│   │   ├── Giant.h             # Inimigo: Colosso lento de alta resistência à luz
│   │   ├── Giant.cpp
│   │   ├── Lantern.h           # Lanterna mágica com combustível e clarão expansivo
│   │   ├── Lantern.cpp
│   │   ├── OilFlask.h          # Item: Frasco restaurador de óleo combustível
│   │   ├── OilFlask.cpp
│   │   ├── Pickup.h            # Classe base abstrata para itens mágicos coletáveis
│   │   ├── Pickup.cpp
│   │   ├── PickupSpawner.h     # Gerador probabilístico balanceado de itens
│   │   ├── PickupSpawner.cpp
│   │   ├── Player.h            # Guardião da torre com movimentação e lanterna
│   │   ├── Player.cpp
│   │   ├── Splitter.h          # Inimigo: Sombra fragmentada com divisão ao morrer
│   │   ├── Splitter.cpp
│   │   ├── Wisp.h              # Inimigo: Espectro veloz em zigue-zague senoidal
│   │   └── Wisp.cpp
│   └── scenes/
│       ├── GameOverScene.h     # Tela de fim de partida (vitória ou derrota)
│       ├── GameOverScene.cpp
│       ├── GameScene.h         # Cena central de jogabilidade e simulação
│       ├── GameScene.cpp
│       ├── HintSystem.h        # Sistema de dicas contextuais da Noite 1
│       ├── HintSystem.cpp
│       ├── HUD.h               # Mostrador de vidas, combustível, tempo e pontos
│       ├── HUD.cpp
│       ├── MenuScene.h         # Tela de título e instrução de comandos
│       ├── MenuScene.cpp
│       ├── NightIntroScene.h   # Tela de transição temática entre noites
│       ├── NightIntroScene.cpp
│       ├── UpgradeScene.h      # Tela de seleção de melhorias entre as noites
│       └── UpgradeScene.cpp
└── tests/
    └── test_main.cpp           # Suíte de testes unitários automatizados
```

---

## Nota Técnica: Geração Procedural de Áudio e Gráficos

O jogo adota uma filosofia autossuficiente e determinística em sua apresentação audiovisual:

1. **Gráficos por Primitivas Geométricas e Mistura de Canais:**
   Todo o visual — desde os galhos retorcidos das árvores mortas, o guardião encapuzado, as cinco criaturas das trevas até os três itens mágicos e cartões de melhorias — é desenhado diretamente em tempo real por primitivas matemáticas da raylib. A iluminação dinâmica emprega uma textura alvo auxiliar (*RenderTexture2D*) na qual é desenhado o gradiente de luz do guardião, multiplicada sobre o cenário físico com o modo `BLEND_MULTIPLIED` e complementada por um brilho aditivo `BLEND_ADD_COLORS`.

2. **Efeitos e Música Gerados por Código, sem Arquivos Externos:**
   - **Música Ambiente Contínua (`MusicPlayer`):** Sintetizada em tempo real via `AudioStream` da raylib com renovação assíncrona de buffers e fases contínuas, garantindo um loop fluido sem descontinuidades ou cortes audíveis. Combina drones graves senoidais com modulação de baixa frequência (LFO) e batimento sutil, acrescidos de notas esparsas em escala menor com envelope suave. A progressão entre as noites intensifica o andamento, tessitura e densidade harmônica; a pausa do jogo reduz o volume dinamicamente; e o desfecho da jornada é marcado por acordes temáticos de vitória ou derrota.
   - **Efeitos Sonoros (`AudioManager`):** Sintetizados diretamente em memória por matrizes matemáticas (modulação de frequência, envelopes exponenciais de amplitude e ruído branco pseudoaleatório) encapsulados em recursos `Sound` via RAII. Abrangem dano sofrido, eliminação de sombras, clarão da lanterna, coleta de artefatos com sino cintilante, confirmação de melhorias e fanfarras de alvorada ou encerramento da chama.
   - **Controle Global de Silenciamento:** A tecla `M` permite ligar e desligar instantaneamente todo o subsistema sonoro (música e efeitos) em qualquer cena, apresentando um indicador discreto em tela quando em modo mudo. Em ambientes sem dispositivo de áudio disponível, o jogo detecta a ausência e continua em execução estável sem emitir falhas.

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

### Modo de Demonstração (`--demo`)

O argumento `--demo` habilita atalhos de apresentação sem alterar a jogabilidade padrão quando omitido:
- Título da janela alterado para `Last Wick [DEMO]`;
- `F5`, `F6` e `F7` iniciam diretamente as Noites 1, 2 e 3 com um `RunState` novo;
- `F8` liga ou desliga a invulnerabilidade do guardião.

**Pela linha de comando:**
```bash
./build/LastWick.exe --demo
```

**Pelo Qt Creator:**
1. No painel lateral esquerdo, acesse **Projects** (`Ctrl + 5`);
2. Em **Build & Run**, selecione **Run** no kit ativo;
3. No campo **Command line arguments**, informe `--demo`;
4. Execute normalmente com `Ctrl + R`.

---

## Execução dos Testes Automatizados (CTest)

Os testes unitários cobrem de forma independente de janela ou áudio as regras de negócio de `Lantern` (incluindo penalidade por imobilidade), `RunState`, `HighScoreStore`, `EnemySpawner` (linha do tempo por fase, limites temporais, frenesi da noite 3 e bônus de óleo por tipo), `PickupSpawner`, `UpgradePool`, `Upgrade` (multiplicadores de melhorias), método gancho `onDeath` (`Splitter` e `Fragment`) e análise do argumento `--demo` (`LaunchOptions`):

```bash
# Executar a bateria de testes via CTest exibindo a saída detalhada
ctest --test-dir build --output-on-failure
```

Ou alternativamente executando o binário de testes diretamente:
```bash
./build/LastWickTests.exe
```
