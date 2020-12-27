cmake_minimum_required(VERSION 3.5.1)
SET(CMAKE_CXX_FLAGS "-std=c++17")

project(main)  


ADD_LIBRARY(LibsModule main.cpp)

set(CMAKE_EXECUTABLE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_DIR}/..")


target_link_libraries(LibsModule pthread)
target_link_libraries(LibsModule glfw3)
target_link_libraries(LibsModule Xrandr)
target_link_libraries(LibsModule X11)
target_link_libraries(LibsModule GL)
target_link_libraries(LibsModule Xi)
target_link_libraries(LibsModule Xxf86vm)
target_link_libraries(LibsModule dl)
target_link_libraries(LibsModule Xinerama)
target_link_libraries(LibsModule Xcursor)
target_link_libraries(LibsModule GLEW)
target_link_libraries(LibsModule assimp)


target_link_libraries(LibsModule jsoncpp)


ADD_EXECUTABLE(main main.cpp include/camera.cpp  include/shader.cpp include/Texture.cpp  vender/stb_image/stb_image.cpp include/random_terrain.cpp include/renderer.cpp)


target_link_libraries(main LibsModule)    