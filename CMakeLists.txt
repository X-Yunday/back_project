cmake_minimum_required(VERSION 3.10)
project(x_project)

# 指定 C++ 标准
set(CMAKE_CXX_STANDARD 17)

# 查找 MySQL 库
include_directories(/usr/local/mysql/include)
link_directories(/usr/local/mysql/lib)

# 创建可执行文件
add_executable(server
    src/main.cpp
)

# 链接库 - 使用正确的库名称
target_link_libraries(server
    mysqlcppconnx  # 使用实际库名
    pthread
    mysqlclient
)
