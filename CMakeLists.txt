cmake_minimum_required(VERSION 4.2)
project(ExpenseTracker)

set(CMAKE_CXX_STANDARD 23) # spec says to use at least 17, but 23 is what we're being taught
set(CMAKE_AUTOMOC ON)
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
        src/Transaction.cpp
        src/Expense.cpp
        src/Income.cpp
        src/TransactionManager.cpp
        src/DataManager.cpp
        src/GUI.cpp)

target_link_libraries(ExpenseTracker Qt6::Widgets)