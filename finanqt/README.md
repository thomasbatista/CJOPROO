# FinanQt: Gestor de Despesas Pessoais

Aplicação desktop para controle financeiro pessoal, desenvolvida em C++17 e Qt 6 (Widgets), com CMake como sistema de compilação. O projeto foi concebido como trabalho final da disciplina de Programação Orientada a Objetos do curso superior de Tecnologia em Análise e Desenvolvimento de Sistemas.

## Visão Geral

O FinanQt permite registrar e acompanhar receitas e despesas (simples, parceladas e recorrentes), organizá-las por categorias, definir orçamentos mensais com alertas visuais e analisar os gastos em um gráfico desenhado com `QPainter`. Os dados são salvos em JSON no diretório de dados do aplicativo no sistema operacional.

## Principais Recursos

- **Gestão de transações:** cadastro, edição e remoção de receitas e despesas, com confirmação na remoção.
- **Tipos especiais de despesa:** parceladas (valor distribuído entre os meses, com a última parcela absorvendo o resto da divisão) e recorrentes (repetidas mensalmente, com data final opcional).
- **Categorias com tipo:** cada categoria é de receita ou de despesa, com cor própria, e novas categorias podem ser criadas pelo diálogo de transação.
- **Controle orçamentário:** limite mensal por categoria de despesa, com barra de progresso e status Normal, Atenção (a partir de 80%) e Estourado, além de alerta na barra de status.
- **Resumo mensal:** receitas, despesas e saldo do mês, com navegação entre meses.
- **Tabela com filtros:** visualização baseada em Model/View, com ordenação por coluna, filtro por categoria e busca por descrição aplicados em conjunto.
- **Gráfico de gastos:** barras horizontais por categoria, desenhadas com `QPainter`, usando a cor de cada categoria.
- **Persistência:** gravação segura em JSON (`QSaveFile`), com versão do formato e dados em `QStandardPaths::AppDataLocation`.
- **Precisão financeira:** valores monetários em centavos inteiros (`qint64`), sem ponto flutuante no armazenamento.

## Arquitetura e Conceitos de POO

1. **Abstração:** separação entre entidades de domínio (`Money`, `Category`, `Transaction`, `Budget`, `Ledger`), persistência (`IRepository`) e interface gráfica.
2. **Encapsulamento:** atributos privados, validação nos construtores e acesso por métodos `const`; o `Ledger` expõe as transações apenas como ponteiros para leitura.
3. **Herança e polimorfismo:** a classe abstrata `Transaction` é especializada por `Income` e `Expense`, e esta por `InstallmentExpense` e `RecurringExpense`, que sobrescrevem `valueInMonth` e `toJson`.
4. **Sobrecarga de operadores:** `Money` implementa operadores aritméticos e relacionais.
5. **Composição:** `Ledger` agrega transações (`std::unique_ptr<Transaction>`), orçamentos e categorias.
6. **Inversão de dependência:** a `MainWindow` depende da interface `IRepository`, e não da implementação `JsonRepository`.
7. **Observer:** notificação de mudanças por signals e slots (`Ledger::changed`).
8. **Model/View:** `TransactionTableModel` herda de `QAbstractTableModel`, e `TransactionFilterProxyModel` herda de `QSortFilterProxyModel` para combinar os filtros.
9. **Tratamento de exceções:** entradas inválidas lançam `ValidationError`.
10. **Tipos fortemente tipados:** `enum class` para `TransactionType`, `CategoryKind` e `Budget::Status`.
11. **Factory Method:** `Transaction::fromJson` cria a subclasse correta a partir do JSON.

## Estrutura do Projeto

```text
finanqt/
├── CMakeLists.txt
├── README.md
├── .gitignore
├── .clang-format
├── src/
│   ├── main.cpp
│   ├── model/          Money, Category, ValidationError, Transaction, Income,
│   │                   Expense, InstallmentExpense, RecurringExpense, Budget, Ledger
│   ├── persistence/    IRepository, JsonRepository
│   └── ui/             MainWindow, TransactionDialog, BudgetDialog,
│                       TransactionTableModel, TransactionFilterProxyModel,
│                       SpendingChartWidget
└── tests/              testes unitários com QtTest (Money, Transaction, Ledger,
                        JsonRepository, TransactionTableModel)
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

## Testes

```bash
cd finanqt/build
ctest --output-on-failure
```

Os testes também podem ser executados pelo Qt Creator, no painel de resultados de testes.

## Informações Acadêmicas

- **Autor:** Thomas Batista Pinto
- **Curso:** Tecnologia em Análise e Desenvolvimento de Sistemas
- **Disciplina:** Programação Orientada a Objetos
- **Instituição:** Instituto Federal de Educação, Ciência e Tecnologia de São Paulo, Campus Campos do Jordão