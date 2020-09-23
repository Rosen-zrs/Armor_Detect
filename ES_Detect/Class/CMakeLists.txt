# 查找当前目录下的所有源文件

# 并将名称保存到 DIR_LIB_SRCS 变量

aux_source_directory(. DIR_LIB_SRCS)

find_package(OpenCV  REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

include_directories(../include)
# include_directories(/usr/local/include/opencv4/opencv2)


# 生成链接库

add_library (Class ${DIR_LIB_SRCS})

target_link_libraries(Class ${OpenCV_LIBS})

