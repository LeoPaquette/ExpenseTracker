cmake_minimum_required(VERSION 4.2)
project(ExpenseTracker)

set(CMAKE_CXX_STANDARD 14)

add_executable(ExpenseTracker
        src/main.cpp
        include/Transaction.h
        include/Expense.h
        include/Income.h
        include/Category.h
        include/TransactionManager.h
        include/Analytics.h
        include/DataManager.h
        src/Transaction.cpp
        src/Expense.cpp
        src/Income.cpp
        src/TransactionManager.cpp
        src/DataManager.cpp
        src/GUI.cpp)
