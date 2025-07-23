cmake_minimum_required(VERSION 3.5)
project(GAME_TXT C)

# Define the default build type to Release
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Set compilation flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -Wall")  

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g --coverage")   
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O2")

# Enable testing
include(CTest)
enable_testing()

# Add the library
add_library(game src/game.c src/game_aux.c src/game_ext.c src/queue.c src/game_private.c src/game_tools.c)

# Memory check settings
set(MEMORYCHECK_COMMAND "valgrind")
set(MEMORYCHECK_COMMAND_OPTIONS "--leak-check=full --error-exitcode=1")

# file copy
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/default.txt DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# Add the main executable
add_executable(game_text src/game_text.c)
target_link_libraries(game_text game m)

# Add the random game generator executable
add_executable(game_random src/game_random.c)
target_link_libraries(game_random game m)

# Add the solver game executable
add_executable(game_solve src/game_solve.c)
target_link_libraries(game_solve game m)

# Add test executables
add_executable(game_test_eucer tests/game_test_eucer.c)
target_include_directories(game_test_eucer PUBLIC ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(game_test_eucer game m)

add_executable(game_test_hbotanlioglu tests/game_test_hbotanlioglu.c)
target_include_directories(game_test_hbotanlioglu PUBLIC ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(game_test_hbotanlioglu game m)

add_executable(game_test_ddausse tests/game_test_ddausse.c)
target_include_directories(game_test_ddausse PUBLIC ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(game_test_ddausse game m)

## find SDL2
include(sdl2.cmake)
message(STATUS "SDL2 include dir: ${SDL2_ALL_INC}")
message(STATUS "SDL2 all libraries: ${SDL2_ALL_LIBS}")

include_directories(${SDL2_ALL_INC})
add_executable(game_sdl src/game_sdl.c src/model.c)
target_link_libraries(game_sdl ${SDL2_ALL_LIBS} game m)

## copy useful ressources in the build directory
file(COPY res DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# Add tests for game_test_eucer
add_test(test_eucer_game_new_empty ./game_test_eucer game_new_empty)
add_test(test_eucer_game_new ./game_test_eucer game_new)
add_test(test_eucer_game_copy ./game_test_eucer game_copy)
add_test(test_eucer_game_equal ./game_test_eucer game_equal)
add_test(test_eucer_game_delete ./game_test_eucer game_delete)
add_test(test_eucer_game_set_piece_shape ./game_test_eucer game_set_piece_shape)
add_test(test_eucer_game_new_empty_ext ./game_test_eucer game_new_empty_ext)
add_test(test_eucer_game_nb_rows ./game_test_eucer game_nb_rows)
add_test(test_eucer_game_undo ./game_test_eucer game_undo)
add_test(test_eucer_game_load ./game_test_eucer game_load)

# Add tests for game_test_hbotanlioglu
add_test(test_hbotanlioglu_game_set_piece_orientation ./game_test_hbotanlioglu game_set_piece_orientation)
add_test(test_hbotanlioglu_game_get_piece_shape ./game_test_hbotanlioglu game_get_piece_shape)
add_test(test_hbotanlioglu_game_get_piece_orientation ./game_test_hbotanlioglu game_get_piece_orientation)
add_test(test_hbotanlioglu_game_play_move ./game_test_hbotanlioglu game_play_move)
add_test(test_hbotanlioglu_game_won ./game_test_hbotanlioglu game_won)
add_test(test_hbotanlioglu_game_reset_orientation ./game_test_hbotanlioglu game_reset_orientation)
add_test(test_hbotanlioglu_game_shuffle_orientation ./game_test_hbotanlioglu game_shuffle_orientation)
add_test(test_hbotanlioglu_game_nb_cols ./game_test_hbotanlioglu game_nb_cols)
add_test(test_hbotanlioglu_game_redo ./game_test_hbotanlioglu game_redo)
add_test(test_hbotanlioglu_game_save ./game_test_hbotanlioglu game_save)

# Add tests for game_test_ddausse
add_test(test_ddausse_game_print ./game_test_ddausse game_print)
add_test(test_ddausse_game_default ./game_test_ddausse game_default)
add_test(test_ddausse_game_default_solution ./game_test_ddausse game_default_solution)
add_test(test_ddausse_game_get_ajacent_square ./game_test_ddausse game_get_ajacent_square)
add_test(test_ddausse_game_has_half_edge ./game_test_ddausse game_has_half_edge)
add_test(test_ddausse_game_check_edge ./game_test_ddausse game_check_edge)
add_test(test_ddausse_game_is_well_paired ./game_test_ddausse game_is_well_paired)
add_test(test_ddausse_game_is_connected ./game_test_ddausse game_is_connected)
add_test(test_ddausse_game_is_wrapping ./game_test_ddausse game_is_wrapping)
add_test(test_ddausse_game_new_ext ./game_test_ddausse game_new_ext)


