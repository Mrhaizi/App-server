cmake_minimum_required(VERSION 3.5)


project(MessagePackage)

# 收集源文件
file(GLOB_RECURSE SRC src/*.cpp )

# 添加库
add_library(${PROJECT_NAME} STATIC ${SRC})

# 包含头文件路径
target_include_directories(${PROJECT_NAME} PUBLIC include)
if(WIN32)
elseif(UNIX)
    target_link_libraries(${PROJECT_NAME} PUBLIC zues)
endif()
