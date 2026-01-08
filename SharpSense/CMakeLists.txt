cmake_minimum_required(VERSION 3.16)

project(SharpSense VERSION 0.1 LANGUAGES CXX)

# 你已经不用 .ui 文件了，可以关掉 AUTOUIC
set(CMAKE_AUTOUIC OFF)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 找 Qt
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Network)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Network)

# 源码 & 头文件
set(SHARP_SENSE_SOURCES
    src/main.cpp
    src/ui/MainWindow.cpp
    src/ui/MatchHistoryPage.cpp      # 新增：战绩页面
)

set(SHARP_SENSE_HEADERS
    inc/ui/MainWindow.h
    inc/ui/MatchHistoryPage.h        # 新增：战绩页面头
    inc/core/StructType.h            # 新增：数据结构定义
    # 以后这里可以继续加 inc/tools/xxx.h, inc/utils/xxx.h
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(SharpSense
        MANUAL_FINALIZATION
        ${SHARP_SENSE_SOURCES}
        ${SHARP_SENSE_HEADERS}
        inc/core/LOLClientService.h
        src/core/LOLClientService.cpp
    )
else()
    add_executable(SharpSense
        ${SHARP_SENSE_SOURCES}
        ${SHARP_SENSE_HEADERS}
    )
endif()

# 让编译器能找到 inc/ 下的头文件
target_include_directories(SharpSense
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

# 链接 Qt 库

target_link_libraries(SharpSense PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Network
)

# mac / win 打包相关（你原来的逻辑直接沿用）
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.SharpSense)
endif()
set_target_properties(SharpSense PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS SharpSense
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(SharpSense)
endif()
