# FinanQt: Gestor de Despesas Pessoais

Aplicação desktop para controle financeiro pessoal, desenvolvida em C++17 e Qt 6 (Widgets), com CMake como sistema de compilação. O projeto foi concebido como trabalho final da disciplina de Programação Orientada a Objetos do curso superior de Tecnologia em Análise e Desenvolvimento de Sistemas.

## Visão Geral

O FinanQt permite registrar e acompanhar receitas e despesas (simples, parceladas e recorrentes), organizá-las por categorias, definir orçamentos mensais com alertas visuais e analisar os gastos em um gráfico desenhado com `QPainter`. Os dados são salvos em JSON no diretório de dados do aplicativo no sistema operacional.

## Principais Recursos

- **Gestão de transações:** cadastro, edição e remoção de receitas e despesas.
- **Tipos especiais de despesa:** despesas parceladas (valor distribuído entre os meses) e recorrentes (repetidas mensalmente, com data final opcional).
- **Controle orçamentário:** limite mensal por categoria, com status Normal, Atenção (a partir de 80%) e Estourado.
- **Gráfico de gastos:** distribuição dos gastos por categoria em um gráfico de barras desenhado com `QPainter`.
- **Tabela com filtros:** visualização baseada em Model/View (`QAbstractTableModel`), com ordenação e busca em tempo real via `QSortFilterProxyModel`.
- **Persistência:** serialização e desserialização em JSON em `QStandardPaths::AppDataLocation`.
- **Precisão financeira:** valores monetários em centavos inteiros (`qint64`), evitando imprecisão de ponto flutuante.

## Arquitetura e Conceitos de POO

1. **Abstração:** separação entre entidades de domínio (`Money`, `Transaction`, `Budget`), persistência (`IRepository`) e interface gráfica.
2. **Encapsulamento:** atributos privados, validação no construtor e acesso por métodos `const`.
3. **Herança e polimorfismo:** a classe abstrata `Transaction` é especializada por `Income`, `Expense`, `InstallmentExpense` e `RecurringExpense`, que sobrescrevem `valueInMonth` e `toJson`.
4. **Sobrecarga de operadores:** `Money` implementa operadores aritméticos e relacionais.
5. **Composição:** `Ledger` agrega transações (`std::unique_ptr<Transaction>`), orçamentos e categorias.
6. **Inversão de dependência:** a camada superior depende da interface `IRepository`, e não de `JsonRepository`.
7. **Observer:** notificação de mudanças por signals e slots (`Ledger::changed`).
8. **Model/View:** `TransactionTableModel` herda de `QAbstractTableModel`.
9. **Tratamento de exceções:** entradas inválidas lançam `ValidationError`.

## Estrutura do Projeto

```text
finanqt/
├── CMakeLists.txt
├── README.md
├── .gitignore
├── .clang-format
├── resources.qrc
├── assets/
├── src/
│   ├── main.cpp
│   ├── model/
│   ├── persistence/
│   └── ui/
└── tests/
```

## Requisitos

- Compilador C++17 (GCC 11+, Clang 13+ ou MSVC 2019+)
- Qt 6.2 ou superior (módulos `Core`, `Gui`, `Widgets` e `Test`)
- CMake 3.16 ou superior
- Ninja

## Como Compilar e Executar

### Pelo Qt Creator

1. Abra o Qt Creator.
2. Escolha **File > Open File or Project...** e selecione o arquivo `finanqt/CMakeLists.txt`.
3. Selecione um kit do Qt 6 (MinGW ou MSVC) e clique em **Configure Project**.
4. Compile e execute com `Ctrl+R`.

### Pela linha de comando

```bash
cd finanqt
cmake -B build -S . -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

Para executar, use `./build/FinanQt` (Linux e macOS) ou `.\build\FinanQt.exe` (Windows).

## Informações Acadêmicas

- **Autor:** Thomas Batista Pinto
- **Curso:** Tecnologia em Análise e Desenvolvimento de Sistemas
- **Disciplina:** Programação Orientada a Objetos
- **Instituição:** Instituto Federal de Educação, Ciência e Tecnologia de São Paulo, Campus Campos do Jordão