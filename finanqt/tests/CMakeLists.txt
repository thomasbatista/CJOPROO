add_executable(tst_Money
    tst_Money.cpp
)

target_link_libraries(tst_Money PRIVATE
    FinanQtCore
    Qt6::Core
    Qt6::Test
)

add_test(NAME tst_Money COMMAND tst_Money)
