add_executable(tst_Money
    tst_Money.cpp
)
target_link_libraries(tst_Money PRIVATE FinanQtCore Qt6::Core Qt6::Test)
add_test(NAME tst_Money COMMAND tst_Money)

add_executable(tst_Transaction
    tst_Transaction.cpp
)
target_link_libraries(tst_Transaction PRIVATE FinanQtCore Qt6::Core Qt6::Test)
add_test(NAME tst_Transaction COMMAND tst_Transaction)

add_executable(tst_Ledger
    tst_Ledger.cpp
)
target_link_libraries(tst_Ledger PRIVATE FinanQtCore Qt6::Core Qt6::Test)
add_test(NAME tst_Ledger COMMAND tst_Ledger)

add_executable(tst_JsonRepository
    tst_JsonRepository.cpp
)
target_link_libraries(tst_JsonRepository PRIVATE FinanQtCore Qt6::Core Qt6::Test)
add_test(NAME tst_JsonRepository COMMAND tst_JsonRepository)

add_executable(tst_TransactionTableModel
    tst_TransactionTableModel.cpp
)
target_link_libraries(tst_TransactionTableModel PRIVATE
    FinanQtCore
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Test
)
add_test(NAME tst_TransactionTableModel COMMAND tst_TransactionTableModel)
