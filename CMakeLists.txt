cmake_minimum_required(VERSION 4.2)
project(ExpenseTracker)

set(CMAKE_CXX_STANDARD 23) # spec says to use at least 17, but 23 is what we're being taught
set(CMAKE_AUTOMOC ON) # qt
set(CMAKE_AUTOUIC ON) # qt
find_package(Qt6 REQUIRED COMPONENTS Widgets)

add_executable(ExpenseTracker
        src/main.cpp
        include/Transaction.h
        include/Expense.h
        include/Income.h
        include/Category.h
        include/TransactionManager.h
        include/AnalyticsEngine.h
        include/DataManager.h
        include/GUI.h
        src/Transaction.cpp
        src/Expense.cpp
        src/Income.cpp
        src/TransactionManager.cpp
        src/DataManager.cpp
        src/GUI.cpp
        src/GUI.ui)

target_link_libraries(ExpenseTracker Qt6::Widgets)
target_include_directories(ExpenseTracker PRIVATE .)

# moved the Leo's tests from main to its own test run config.
add_executable(ValidatorTester
        tests/ValidatorTester.cpp
        include/Transaction.h
        include/Expense.h
        include/Income.h
        src/Transaction.cpp
        src/Expense.cpp
        src/Income.cpp)