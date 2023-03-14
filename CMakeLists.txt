cmake_minimum_required(VERSION 3.16)
set(CMAKE_CXX_STANDARD 11)

project(nima)
#引入头文件
include_directories("include")
# 指定为C++11 版本


#配置自动根据当前是 32 位还是 64 位程序 , 确定静态库的配置目录
if(CMAKE_CL_64)
    set(platform x64)
else()
    set(platform x86)
endif()
#配置静态库 , 用于引导如何链接动态库和静态库
link_directories("lib/${platform}")

#处理 “timespec”:“struct” 类型重定义 报错信息
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DHAVE_STRUCT_TIMESPEC")




add_executable(aa main.cpp  fixed_thread_pool.cpp)

