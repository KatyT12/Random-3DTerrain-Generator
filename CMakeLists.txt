cmake_minimum_required(VERSION 3.5.1)
SET(CMAKE_CXX_FLAGS "-std=c++17")

project(main)  


ADD_LIBRARY(LibsModule main.cpp)

set(CMAKE_EXECUTABLE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_DIR}/..")

message("-- CMAKE_SYSTEM_INFO_FILE: ${CMAKE_SYSTEM_INFO_FILE}")
message("-- CMAKE_SYSTEM_NAME:      ${CMAKE_SYSTEM_NAME}")
message("-- CMAKE_SYSTEM_PROCESSOR: ${CMAKE_SYSTEM_PROCESSOR}")
message("-- CMAKE_SYSTEM:           ${CMAKE_SYSTEM}")

string (REGEX MATCH "\\.el[1-9]" os_version_suffix ${CMAKE_SYSTEM})
message("-- os_version_suffix:      ${os_version_suffix}")



if(EXISTS "/etc/arch-release")
    set(glfwLib "glfw3")
    add_compile_definitions(ARCH=1 WITH_ARCH)
else()
    set(glfwLib "glfw")
    add_compile_definitions(UBUNTU=1 WITH_UBUNTU)
endif()

target_link_libraries(LibsModule pthread)
target_link_libraries(LibsModule ${glfwLib})
target_link_libraries(LibsModule X11)
target_link_libraries(LibsModule GL)
target_link_libraries(LibsModule GLEW)
target_link_libraries(LibsModule assimp)

if(CMAKE_SYSTEM MATCHES "MANJARO" OR CMAKE_SYSTEM MATCHES "ARCH")
    target_link_libraries(LibsModule Xi)
    target_link_libraries(LibsModule Xrandr)
    target_link_libraries(LibsModule Xxf86vm)
    target_link_libraries(LibsModule dl)
    target_link_libraries(LibsModule Xinerama)
    target_link_libraries(LibsModule Xcursor)
endif()

target_link_libraries(LibsModule jsoncpp)


ADD_EXECUTABLE(main main.cpp include/camera.cpp  include/shader.cpp include/Texture.cpp  vender/stb_image/stb_image.cpp include/random_terrain.cpp include/renderer.cpp include/Water.cpp)


target_link_libraries(main LibsModule)    