cmake_minimum_required(VERSION 3.5)
project(App-server)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# set(CMAKE_AUTOUIC ON)
# set(CMAKE_AUTOMOC ON)
# set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# 设置生成器平台为x64
if(NOT DEFINED CMAKE_GENERATOR_PLATFORM)
    set(CMAKE_GENERATOR_PLATFORM x64)
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_INIT} -fexec-charset=utf-8")

if (WIN32)
    message("windows system")
    set(MYSQL_CONNECTOR_CPP_DIR "C:/Program Files/MySQL/mysql-connector-c++-9.0.0-winx64")
    # set(CMAKE_PREFIX_PATH  "C:/Program Files/MySQL/mysql-connector-c++-9.0.0-winx64/lib/cmake")
    find_package(Qt5 REQUIRED COMPONENTS Widgets Gui Core Network)
    find_package(mysql-concpp REQUIRED)
    link_directories(${MYSQL_CONNECTOR_CPP_DIR}/lib64/vs14)
elseif(UNIX)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
    message("linux system")
    set(CMAKE_PREFIX_PATH "/opt/Qt/5.12.12/gcc_64/lib/cmake/Qt5")
    find_package(mysql-concpp REQUIRED)
    find_package(Qt5 REQUIRED COMPONENTS Core Widgets Network)
    include_directories(${Qt5Core_INCLUDE_DIRS})
endif()

add_subdirectory(ZuesFramework)
add_subdirectory(src/Message)
add_subdirectory(src/MysqlInteractor)
add_subdirectory(src/ClientHandler)
add_subdirectory(src/CommunicatorServer)
add_subdirectory(src/MyTcpServer)
add_definitions(-Wno-builtin-macro-redefined)

add_executable(${PROJECT_NAME} src/main.cpp)
# foreach(lib IN LISTS MY_LIBRARIES)
#     message(${lib})
#     redefine_file_macro(${lib})
# endforeach()

redefine_file_macro(${PROJECT_NAME})

if(WIN32)
    target_include_directories(${PROJECT_NAME} PUBLIC ${MYSQL_CONNECTOR_CPP_DIR}/include)
    target_link_libraries(${PROJECT_NAME} PUBLIC 
                                        Qt5::Widgets
                                        Qt5::Gui
                                        Qt5::Core)
    #合并后的自定义命令
    add_custom_command(TARGET App POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:Qt5::Core>
        $<TARGET_FILE:Qt5::Widgets>
        $<TARGET_FILE:Qt5::Gui>
        $<TARGET_FILE_DIR:App>
        COMMAND ${CMAKE_COMMAND} -E make_directory
        $<TARGET_FILE_DIR:App>/platforms
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "D:/Qt/5.15.0/msvc2019_64/plugins/platforms"
        $<TARGET_FILE_DIR:App>/platforms
    )

elseif(UNIX)
    target_link_libraries(${PROJECT_NAME} PUBLIC 
                                        Qt5::Widgets
                                        Qt5::Core
                                        Qt5::Gui
                                        Qt5::Network
                                        CommunicatorServer
                                        zues
                                        MysqlInteractor
                                        MyTcpServer)
endif()
