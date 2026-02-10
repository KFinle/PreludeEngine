cmake_minimum_required(VERSION 3.23)

# Set the C++ version to be C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project(RhythmEngine)

set(THIRD_PARTY_DIR "${PROJECT_SOURCE_DIR}/third_party")

###############################################################################
# Common Project for Setup
# This project exists to provide common definitions and config for all platforms
# Most notably, this includes platform definitions
###############################################################################

add_library(Common INTERFACE)

set(IS_PLATFORM_WINDOWS 0)
set(IS_PLATFORM_APPLE 0)
set(IS_PLATFORM_LINUX 0)

if (WIN32)
	set(IS_PLATFORM_WINDOWS 1)
endif()

if (APPLE)
	set(IS_PLATFORM_APPLE 1)
endif()

if (UNIX AND NOT APPLE)
	set(IS_PLATFORM_LINUX 1)
endif()

target_compile_definitions(Common INTERFACE 
    BUILD_PLATFORM_WINDOWS=${IS_PLATFORM_WINDOWS}
	BUILD_PLATFORM_APPLE=${IS_PLATFORM_APPLE}
	BUILD_PLATFORM_LINUX=${IS_PLATFORM_LINUX}
	GL_SILENCE_DEPRECATION
)

message(STATUS "System Name is ${CMAKE_SYSTEM_NAME}")
message(STATUS "Compiler Name is ${CMAKE_CXX_COMPILER_ID}")

find_package(SDL3 REQUIRED)


###############################################################################
# Engine
###############################################################################

file(GLOB_RECURSE ENGINE_SRC_FILES ${PROJECT_SOURCE_DIR}/src/Engine/*.cpp)
list(REMOVE_ITEM ENGINE_SRC_FILES "${PROJECT_SOURCE_DIR}/src/Engine/RuntimeMain.cpp")

add_library(Engine
	${ENGINE_SRC_FILES}
	${THIRD_PARTY_DIR}/miniaudio/miniaudio.cpp
)

target_include_directories(Engine PUBLIC
	"${PROJECT_SOURCE_DIR}/src"
)

target_include_directories(Engine PRIVATE
	"${THIRD_PARTY_DIR}"
)

target_link_libraries(Engine PUBLIC Common SDL3::SDL3)

###############################################################################
# Rhythm (composition + synth)
###############################################################################

file(GLOB_RECURSE RHYTHM_INC_FILES ${PROJECT_SOURCE_DIR}/src/Rhythm/*.h)
file(GLOB_RECURSE RHYTHM_SRC_FILES ${PROJECT_SOURCE_DIR}/src/Rhythm/*.cpp)

add_library(Rhythm
	${RHYTHM_INC_FILES}
	${RHYTHM_SRC_FILES}
)

target_include_directories(Rhythm PUBLIC
	"${PROJECT_SOURCE_DIR}/src"
	"${PROJECT_SOURCE_DIR}/src/Rhythm"
)

target_link_libraries(Rhythm PUBLIC Common)

###############################################################################
# Gameplay (rules + scoring)
###############################################################################

file(GLOB_RECURSE GAMEPLAY_INC_FILES ${PROJECT_SOURCE_DIR}/src/Gameplay/*.h)
file(GLOB_RECURSE GAMEPLAY_SRC_FILES ${PROJECT_SOURCE_DIR}/src/Gameplay/*.cpp)

add_library(Gameplay
	${GAMEPLAY_INC_FILES}
	${GAMEPLAY_SRC_FILES}
)

target_include_directories(Gameplay PUBLIC
	"${PROJECT_SOURCE_DIR}/src"
	"${PROJECT_SOURCE_DIR}/src/Gameplay"
)

target_link_libraries(Gameplay PUBLIC Rhythm)

###############################################################################
# Game Project
# This configures the game project, where participants will write their code
###############################################################################

file(GLOB_RECURSE GAME_INC_FILES ${PROJECT_SOURCE_DIR}/src/Game/*.h)
file(GLOB_RECURSE GAME_SRC_FILES ${PROJECT_SOURCE_DIR}/src/Game/*.cpp)

set(GAME_EXECUTABLE_TYPE "")
if (WIN32)
	set(GAME_EXECUTABLE_TYPE WIN32)
endif()

add_executable(Game ${GAME_EXECUTABLE_TYPE}
	${GAME_INC_FILES} 
	${GAME_SRC_FILES}
	src/Engine/RuntimeMain.cpp
)

target_include_directories(Game PRIVATE 
	"${PROJECT_SOURCE_DIR}/src"
	"${PROJECT_SOURCE_DIR}/src/Game"
)


target_link_libraries(Game PRIVATE Engine Rhythm Gameplay)

# Add custom command 'run' for makefiles to run the output exe
# This allows us to write 'make run' in the terminal and have it run in the correct directory pointing to data
add_custom_target(run
	DEPENDS Game
	COMMAND "$<TARGET_FILE:Game>"
	WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
)

if (WIN32)
	set_target_properties(Game PROPERTIES
		VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
	)
	set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Game)
endif()
