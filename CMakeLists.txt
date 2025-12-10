cmake_minimum_required(VERSION 3.24)
project(CampusCardSystem VERSION 1.0.0 LANGUAGES CXX)

# ============================================================================
# C++20 标准配置
# ============================================================================
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
# 生成 compile_commands.json 供 clangd 使用
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

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
# 源文件收集 - MVC架构
# ============================================================================

# Model层 - 实体类
set(MODEL_ENTITIES_SOURCES
    src/model/entities/User.cpp
    src/model/entities/Card.cpp
    src/model/entities/Record.cpp
)

set(MODEL_ENTITIES_HEADERS
    src/model/entities/User.h
    src/model/entities/Card.h
    src/model/entities/Record.h
)

# Model层 - 数据访问层
set(MODEL_REPOSITORIES_SOURCES
    src/model/repositories/StorageManager.cpp
)

set(MODEL_REPOSITORIES_HEADERS
    src/model/repositories/StorageManager.h
)

# Model层 - 服务层
set(MODEL_SERVICES_SOURCES
    src/model/services/CardService.cpp
    src/model/services/RecordService.cpp
    src/model/services/AuthService.cpp
)

set(MODEL_SERVICES_HEADERS
    src/model/services/CardService.h
    src/model/services/RecordService.h
    src/model/services/AuthService.h
)

# Model层 - 类型定义
set(MODEL_HEADERS
    src/model/Types.h
)

# Controller层
set(CONTROLLER_SOURCES
    src/controller/AuthController.cpp
    src/controller/CardController.cpp
    src/controller/RecordController.cpp
    src/controller/MainController.cpp
)

set(CONTROLLER_HEADERS
    src/controller/AuthController.h
    src/controller/CardController.h
    src/controller/RecordController.h
    src/controller/MainController.h
)

# View层 - 对话框
set(VIEW_DIALOGS_SOURCES
    src/view/dialogs/LoginDialog.cpp
    src/view/dialogs/RegisterDialog.cpp
    src/view/dialogs/RechargeDialog.cpp
)

set(VIEW_DIALOGS_HEADERS
    src/view/dialogs/LoginDialog.h
    src/view/dialogs/RegisterDialog.h
    src/view/dialogs/RechargeDialog.h
)

# View层 - 面板
set(VIEW_PANELS_SOURCES
    src/view/panels/AdminPanel.cpp
    src/view/panels/StudentPanel.cpp
)

set(VIEW_PANELS_HEADERS
    src/view/panels/AdminPanel.h
    src/view/panels/StudentPanel.h
)

# View层 - 组件
set(VIEW_WIDGETS_SOURCES
    src/view/widgets/RecordTableWidget.cpp
    src/view/widgets/StatisticsWidget.cpp
)

set(VIEW_WIDGETS_HEADERS
    src/view/widgets/RecordTableWidget.h
    src/view/widgets/StatisticsWidget.h
)

# View层 - 主窗口
set(VIEW_SOURCES
    src/view/MainWindow.cpp
)

set(VIEW_HEADERS
    src/view/MainWindow.h
)

set(RESOURCES
    resources/resources.qrc
)

# ============================================================================
# 可执行文件
# ============================================================================
add_executable(${PROJECT_NAME}
    src/main.cpp
    # Model层
    ${MODEL_ENTITIES_SOURCES}
    ${MODEL_ENTITIES_HEADERS}
    ${MODEL_REPOSITORIES_SOURCES}
    ${MODEL_REPOSITORIES_HEADERS}
    ${MODEL_SERVICES_SOURCES}
    ${MODEL_SERVICES_HEADERS}
    ${MODEL_HEADERS}
    # Controller层
    ${CONTROLLER_SOURCES}
    ${CONTROLLER_HEADERS}
    # View层
    ${VIEW_DIALOGS_SOURCES}
    ${VIEW_DIALOGS_HEADERS}
    ${VIEW_PANELS_SOURCES}
    ${VIEW_PANELS_HEADERS}
    ${VIEW_WIDGETS_SOURCES}
    ${VIEW_WIDGETS_HEADERS}
    ${VIEW_SOURCES}
    ${VIEW_HEADERS}
    # 资源
    ${RESOURCES}
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/model
    ${CMAKE_CURRENT_SOURCE_DIR}/src/model/entities
    ${CMAKE_CURRENT_SOURCE_DIR}/src/model/repositories
    ${CMAKE_CURRENT_SOURCE_DIR}/src/model/services
    ${CMAKE_CURRENT_SOURCE_DIR}/src/controller
    ${CMAKE_CURRENT_SOURCE_DIR}/src/view
    ${CMAKE_CURRENT_SOURCE_DIR}/src/view/dialogs
    ${CMAKE_CURRENT_SOURCE_DIR}/src/view/panels
    ${CMAKE_CURRENT_SOURCE_DIR}/src/view/widgets
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

# ============================================================================
# ElaWidgetTools DLL 自动拷贝
# ============================================================================
if(WIN32)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:ElaWidgetTools>
        $<TARGET_FILE_DIR:${PROJECT_NAME}>
        COMMENT "Copying ElaWidgetTools DLL to output directory"
    )
endif()

# ============================================================================
# clangd 支持：在项目根目录创建 compile_commands.json 符号链接
# ============================================================================
if(CMAKE_EXPORT_COMPILE_COMMANDS)
    # 在配置阶段创建符号链接或拷贝
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E create_symlink
        ${CMAKE_BINARY_DIR}/compile_commands.json
        ${CMAKE_SOURCE_DIR}/compile_commands.json
        RESULT_VARIABLE _symlink_result
    )
    if(NOT _symlink_result EQUAL 0)
        # 如果符号链接失败（Windows无权限），则使用拷贝
        add_custom_target(copy_compile_commands ALL
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_BINARY_DIR}/compile_commands.json
            ${CMAKE_SOURCE_DIR}/compile_commands.json
            COMMENT "Copying compile_commands.json to source directory for clangd"
        )
    endif()
endif()

# ============================================================================
# 单元测试（可选）
# ============================================================================
option(BUILD_TESTS "Build unit tests" ON)
if(BUILD_TESTS)
    add_subdirectory(tests)
endif()
