cmake_minimum_required(VERSION 3.24)
project(CampusCardSystem VERSION 1.0.0 LANGUAGES CXX)

# ============================================================================
# C++20 标准配置
# ============================================================================
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_EXPORT_COMMAND ON)

# ============================================================================
# Qt6 配置
# ============================================================================
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Core Gui)

# 获取 Qt6 版本号并设置给子模块使用
# Qt6_VERSION 由 find_package 设置，但组件版本变量可能未设置
set(_detected_qt_version "")
if(Qt6_VERSION)
    set(_detected_qt_version ${Qt6_VERSION})
else()
    # 备用方案：从 qmake 获取版本
    find_program(QMAKE_EXECUTABLE NAMES qmake6 qmake HINTS "${CMAKE_PREFIX_PATH}/bin")
    if(QMAKE_EXECUTABLE)
        execute_process(
            COMMAND ${QMAKE_EXECUTABLE} -query QT_VERSION
            OUTPUT_VARIABLE _detected_qt_version
            OUTPUT_STRIP_TRAILING_WHITESPACE
        )
    endif()
endif()

if(_detected_qt_version)
    # 设置 Qt 版本相关变量供子模块使用
    set(QT_VERSION_MAJOR 6)
    set(Qt6Widgets_VERSION ${_detected_qt_version})
    set(Qt6Gui_VERSION ${_detected_qt_version})
    set(Qt6Core_VERSION ${_detected_qt_version})
    message(STATUS "Qt6 version for ElaWidgetTools: ${_detected_qt_version}")
endif()

# ============================================================================
# ElaWidgetTools 子模块
# ============================================================================
# 禁用示例和 ElaPacketIO
set(BUILD_ELAPACKETIO OFF CACHE BOOL "Disable ElaPacketIO" FORCE)
# 跳过示例目录
set(SKIP_EXAMPLE ON)
add_subdirectory(third_party/ElaWidgetTools/ElaWidgetTools)

# ============================================================================
# 源文件收集
# ============================================================================
set(CORE_SOURCES
    src/core/Card.cpp
    src/core/Record.cpp
    src/core/User.cpp
    src/core/AuthManager.cpp
    src/core/CardManager.cpp
    src/core/RecordManager.cpp
    src/core/StorageManager.cpp
)

set(CORE_HEADERS
    src/core/Card.h
    src/core/Record.h
    src/core/User.h
    src/core/AuthManager.h
    src/core/CardManager.h
    src/core/RecordManager.h
    src/core/StorageManager.h
    src/core/Types.h
)

set(UI_SOURCES
    src/ui/MainWindow.cpp
    src/ui/LoginDialog.cpp
    src/ui/RegisterDialog.cpp
    src/ui/AdminDashboard.cpp
    src/ui/StudentDashboard.cpp
    src/ui/RechargeDialog.cpp
    src/ui/RecordTableWidget.cpp
    src/ui/StatisticsWidget.cpp
)

set(UI_HEADERS
    src/ui/MainWindow.h
    src/ui/LoginDialog.h
    src/ui/RegisterDialog.h
    src/ui/AdminDashboard.h
    src/ui/StudentDashboard.h
    src/ui/RechargeDialog.h
    src/ui/RecordTableWidget.h
    src/ui/StatisticsWidget.h
)

set(RESOURCES
    resources/resources.qrc
)

# ============================================================================
# 可执行文件
# ============================================================================
add_executable(${PROJECT_NAME}
    src/main.cpp
    ${CORE_SOURCES}
    ${CORE_HEADERS}
    ${UI_SOURCES}
    ${UI_HEADERS}
    ${RESOURCES}
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/ElaWidgetTools/ElaWidgetTools
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Widgets
    Qt6::Core
    Qt6::Gui
    ElaWidgetTools
)

# ============================================================================
# Windows 特定配置
# ============================================================================
if(WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()

# ============================================================================
# 数据目录复制
# ============================================================================
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/data $<TARGET_FILE_DIR:${PROJECT_NAME}>/data
)
