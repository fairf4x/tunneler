cmake_minimum_required(VERSION 3.1...3.27)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

set(CMAKE_BUILD_TYPE Debug)

project(
  Tunneler
  VERSION 1.1
  LANGUAGES CXX)

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)

include_directories(include ${SDL2_INCLUDE_DIRS})

add_subdirectory(src)
add_library(Tunneler_modules
	${Tunneler_SOURCE_DIR}/src/game.cc
	${Tunneler_SOURCE_DIR}/src/bullet.cc
	${Tunneler_SOURCE_DIR}/src/camera.cc
	${Tunneler_SOURCE_DIR}/src/collisions.cc
	${Tunneler_SOURCE_DIR}/src/commondef.cc
	${Tunneler_SOURCE_DIR}/src/painting.cc
	${Tunneler_SOURCE_DIR}/src/player.cc
	${Tunneler_SOURCE_DIR}/src/statusbar.cc
	${Tunneler_SOURCE_DIR}/src/structure.cc
	${Tunneler_SOURCE_DIR}/src/tank.cc
	${Tunneler_SOURCE_DIR}/src/tmap.cc
	)

add_executable(tunneler ${Tunneler_SOURCE_DIR}/src/tunneler.cc)
target_link_libraries(tunneler Tunneler_modules ${SDL2_LIBRARIES} ${SDL2_IMAGE_LIBRARIES})

add_library(server_modules
	${Tunneler_SOURCE_DIR}/src/commondef.cc
	${Tunneler_SOURCE_DIR}/src/initvect.cc
	${Tunneler_SOURCE_DIR}/src/mirror.cc
	)

add_executable(server ${Tunneler_SOURCE_DIR}/src/server.cc)
target_link_libraries(server server_modules)

