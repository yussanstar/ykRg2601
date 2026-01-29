#バージョン指定
cmake_minimum_required(VERSION 3.23)

#プロジェクト名、言語指定
project(rg CXX)

#Windows用
add_definitions(-DNOMINMAX)

#ライブラリをインポート
include("${CMAKE_CURRENT_SOURCE_DIR}/ykl/import_libs.cmake")

#C++バージョン
enable_language(CXX)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#フィルタ設定
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

#adaptor
include("${CMAKE_CURRENT_SOURCE_DIR}/rg/adaptor/import_libs.cmake")

#main
file(GLOB_RECURSE MAIN_SOURCE
  "${CMAKE_CURRENT_SOURCE_DIR}/rg/main/*.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/rg/main/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/rg/main/*.inl"
)
add_executable(main ${MAIN_SOURCE})

target_include_directories(main PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/rg/main)
target_include_directories(main PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/rg/common)
target_include_directories(main PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/ykl/core/include)
target_link_libraries(main PUBLIC ykl)
target_link_libraries(main PUBLIC adaptor)
target_compile_options(main PUBLIC -W3)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/rg/main FILES ${MAIN_SOURCE})

if(PS5)
  target_link_libraries(main PUBLIC -lSceAgcDriver_stub_weak)
  target_link_libraries(main PUBLIC -lSceAgc_stub_weak)
  target_link_libraries(main PUBLIC -lSceVideoOut_stub_weak)
  target_link_libraries(main PUBLIC -lSceAgc_debug_nosubmission)
  target_link_libraries(main PUBLIC -lSceAgcCore_debug_nosubmission)
  target_link_libraries(main PUBLIC -lSceAgcGpuAddress_debug_nosubmission)
endif()

#スタートアッププロジェクトの設定
set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT main)

#警告関連
add_definitions(-D_CRT_SECURE_NO_WARNINGS)

#CustomEvent
if(PS5)
  set_target_properties(main PROPERTIES SUFFIX ".elf")
  set_target_properties(main PROPERTIES
    VS_USER_PROPS "${CMAKE_CURRENT_SOURCE_DIR}/config/ps5_config.props"
  )
endif()