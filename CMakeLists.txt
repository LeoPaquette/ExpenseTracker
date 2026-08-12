cmake_minimum_required(VERSION 4.2)
project(ExpenseTracker)

set(CMAKE_CXX_STANDARD 23) # spec says to use at least 17, but 23 is what we're being taught
set(CMAKE_AUTOMOC ON) # qt
set(CMAKE_AUTOUIC ON) # qt
set(CMAKE_AUTORCC ON) # bakes resources/dark.qss into the exe so the theme cant go missing at runtime
find_package(Qt6 REQUIRED COMPONENTS Widgets)

include(FetchContent)
FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz)
FetchContent_MakeAvailable(json)

# Windows app icon. windres compiles the .rc, which points at resources/gangspong.ico.
# Empty on other platforms, so the source list below just expands to nothing there.
if (WIN32)
    set(app_icon_resource_windows "${CMAKE_CURRENT_SOURCE_DIR}/resources/appicon.rc")
endif ()

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
        src/Category.cpp
        src/TransactionManager.cpp
        src/DataManager.cpp
        src/GUI.cpp
        src/GUI.ui
        include/JSON.h
        resources/resources.qrc
        include/Util.h
        src/Util.cpp
        ${app_icon_resource_windows}
)

target_link_libraries(ExpenseTracker Qt6::Widgets nlohmann_json::nlohmann_json)
target_include_directories(ExpenseTracker PRIVATE .)
# moved the Leo's tests from main to its own test run config.
add_executable(ValidatorTester
        include/Transaction.h
        include/Expense.h
        include/Income.h
        include/Category.h
        src/Transaction.cpp
        src/Expense.cpp
        src/Income.cpp
        src/Category.cpp)