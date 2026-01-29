#バージョン指定
cmake_minimum_required(VERSION 3.23)

#ソースコード
file(GLOB_RECURSE LIBS_SOURCE
  ${CMAKE_CURRENT_SOURCE_DIR}/rg/adaptor/*.h
  ${CMAKE_CURRENT_SOURCE_DIR}/rg/adaptor/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/rg/adaptor/*.inl
)

#C++バージョン
enable_language(CXX)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#ライブラリ追加
add_library(adaptor STATIC ${LIBS_SOURCE})

#Platform Macros
if(WIN32)
    target_compile_definitions(adaptor PUBLIC _PLATFORM_WINDOWS)
elseif(PS5)
    target_compile_definitions(adaptor PUBLIC _PLATFORM_PS5)
endif()

#インクルードディレクトリ
target_include_directories(adaptor PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/rg/adaptor/src)
target_include_directories(adaptor PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/rg/adaptor/include)
target_include_directories(adaptor PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/rg/common)
target_include_directories(adaptor PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/ykl/core/include)

#フィルタ構成
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/rg/adaptor FILES ${LIBS_SOURCE})

#コンパイルオプション
target_compile_options(adaptor PUBLIC -W3)
