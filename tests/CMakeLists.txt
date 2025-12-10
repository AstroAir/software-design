# ============================================================================
# 单元测试配置
# ============================================================================

# 启用测试
enable_testing()

# 查找 GTest
find_package(GTest REQUIRED)
include(GoogleTest)

# 查找 Qt Test 模块
find_package(Qt6 REQUIRED COMPONENTS Test)

# 源文件目录
set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)
set(TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR})

# ============================================================================
# 被测试的源文件（使用绝对路径）
# ============================================================================

# Model层 - 实体类源文件
set(TEST_MODEL_ENTITIES_SOURCES
    ${SRC_DIR}/model/entities/User.cpp
    ${SRC_DIR}/model/entities/Card.cpp
    ${SRC_DIR}/model/entities/Record.cpp
)

# Model层 - 数据访问层源文件
set(TEST_MODEL_REPOSITORIES_SOURCES
    ${SRC_DIR}/model/repositories/StorageManager.cpp
)

# Model层 - 服务层源文件
set(TEST_MODEL_SERVICES_SOURCES
    ${SRC_DIR}/model/services/CardService.cpp
    ${SRC_DIR}/model/services/RecordService.cpp
    ${SRC_DIR}/model/services/AuthService.cpp
)

# Controller层源文件
set(TEST_CONTROLLER_SOURCES
    ${SRC_DIR}/controller/AuthController.cpp
    ${SRC_DIR}/controller/CardController.cpp
    ${SRC_DIR}/controller/RecordController.cpp
    ${SRC_DIR}/controller/MainController.cpp
)

# View层源文件
set(TEST_VIEW_SOURCES
    ${SRC_DIR}/view/MainWindow.cpp
    ${SRC_DIR}/view/dialogs/LoginDialog.cpp
    ${SRC_DIR}/view/dialogs/RegisterDialog.cpp
    ${SRC_DIR}/view/dialogs/RechargeDialog.cpp
    ${SRC_DIR}/view/panels/AdminPanel.cpp
    ${SRC_DIR}/view/panels/StudentPanel.cpp
    ${SRC_DIR}/view/widgets/RecordTableWidget.cpp
    ${SRC_DIR}/view/widgets/StatisticsWidget.cpp
)

# ============================================================================
# Model层 - 实体类测试
# ============================================================================
set(MODEL_ENTITIES_TEST_SOURCES
    ${TEST_DIR}/model/entities/UserTest.cpp
    ${TEST_DIR}/model/entities/CardTest.cpp
    ${TEST_DIR}/model/entities/RecordTest.cpp
)

# ============================================================================
# Model层 - 类型定义测试
# ============================================================================
set(MODEL_TYPES_TEST_SOURCES
    ${TEST_DIR}/model/TypesTest.cpp
)

# ============================================================================
# Model层 - 数据访问层测试
# ============================================================================
set(MODEL_REPOSITORIES_TEST_SOURCES
    ${TEST_DIR}/model/repositories/StorageManagerTest.cpp
)

# ============================================================================
# Model层 - 服务层测试
# ============================================================================
set(MODEL_SERVICES_TEST_SOURCES
    ${TEST_DIR}/model/services/CardServiceTest.cpp
    ${TEST_DIR}/model/services/RecordServiceTest.cpp
    ${TEST_DIR}/model/services/AuthServiceTest.cpp
)

# ============================================================================
# Controller层测试
# ============================================================================
set(CONTROLLER_TEST_SOURCES
    ${TEST_DIR}/controller/AuthControllerTest.cpp
    ${TEST_DIR}/controller/CardControllerTest.cpp
    ${TEST_DIR}/controller/RecordControllerTest.cpp
    ${TEST_DIR}/controller/MainControllerTest.cpp
)

# ============================================================================
# View层测试
# ============================================================================
set(VIEW_TEST_SOURCES
    ${TEST_DIR}/view/MainWindowTest.cpp
    ${TEST_DIR}/view/dialogs/LoginDialogTest.cpp
    ${TEST_DIR}/view/dialogs/RegisterDialogTest.cpp
    ${TEST_DIR}/view/dialogs/RechargeDialogTest.cpp
    ${TEST_DIR}/view/panels/AdminPanelTest.cpp
    ${TEST_DIR}/view/panels/StudentPanelTest.cpp
    ${TEST_DIR}/view/widgets/RecordTableWidgetTest.cpp
    ${TEST_DIR}/view/widgets/StatisticsWidgetTest.cpp
)

# ============================================================================
# 所有测试源文件
# ============================================================================
set(ALL_TEST_SOURCES
    ${TEST_DIR}/main.cpp
    ${MODEL_ENTITIES_TEST_SOURCES}
    ${MODEL_TYPES_TEST_SOURCES}
    ${MODEL_REPOSITORIES_TEST_SOURCES}
    ${MODEL_SERVICES_TEST_SOURCES}
    ${CONTROLLER_TEST_SOURCES}
    ${VIEW_TEST_SOURCES}
)

# ============================================================================
# 测试可执行文件
# ============================================================================
add_executable(${PROJECT_NAME}_tests
    ${ALL_TEST_SOURCES}
    # 引入被测试的源文件
    ${TEST_MODEL_ENTITIES_SOURCES}
    ${TEST_MODEL_REPOSITORIES_SOURCES}
    ${TEST_MODEL_SERVICES_SOURCES}
    ${TEST_CONTROLLER_SOURCES}
    ${TEST_VIEW_SOURCES}
)

target_include_directories(${PROJECT_NAME}_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/model
    ${CMAKE_SOURCE_DIR}/src/model/entities
    ${CMAKE_SOURCE_DIR}/src/model/repositories
    ${CMAKE_SOURCE_DIR}/src/model/services
    ${CMAKE_SOURCE_DIR}/src/controller
    ${CMAKE_SOURCE_DIR}/src/view
    ${CMAKE_SOURCE_DIR}/src/view/dialogs
    ${CMAKE_SOURCE_DIR}/src/view/panels
    ${CMAKE_SOURCE_DIR}/src/view/widgets
    ${CMAKE_SOURCE_DIR}/third_party/ElaWidgetTools/ElaWidgetTools
    ${CMAKE_SOURCE_DIR}/tests
)

target_link_libraries(${PROJECT_NAME}_tests PRIVATE
    Qt6::Widgets
    Qt6::Core
    Qt6::Gui
    Qt6::Test
    GTest::gtest
    GTest::gtest_main
    GTest::gmock
    GTest::gmock_main
    ElaWidgetTools
)

# ============================================================================
# 注册测试
# ============================================================================
gtest_discover_tests(${PROJECT_NAME}_tests)

# ============================================================================
# 测试数据目录
# ============================================================================
add_custom_command(TARGET ${PROJECT_NAME}_tests POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:${PROJECT_NAME}_tests>/test_data
    COMMENT "Creating test data directory"
)
