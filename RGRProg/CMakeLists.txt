cmake_minimum_required(VERSION 3.10)
project(Poker_game)

find_package(cmocka REQUIRED)

enable_testing()

add_executable (game_play user_management.c deck_operations.c game_logic.c main.c)

add_executable(tests testing.c test_poker.c)
target_link_libraries(tests cmocka)

add_test(NAME Poker_game COMMAND tests)

set(CTEST_OUTPUT_ON_FAILURE ON)