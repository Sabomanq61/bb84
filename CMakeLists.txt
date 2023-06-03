cmake_minimum_required(VERSION 3.5)
set(NAME alice)

project(${NAME})

add_definitions(-DCMAKE_PROJECT_NAME="${NAME}")

set(SOURCE main.cpp alice.cpp alice_impl.cpp )
set(HEADER alice.h alice_impl.h constants.h alice_error.h)
set(LIBS bb84_interface bb84_common thrift boost_system pthread)

add_subdirectory(test)

add_executable(${NAME} ${HEADER} ${SOURCE})
target_link_libraries(${NAME} ${LIBS})

install(TARGETS ${NAME} CONFIGURATIONS Debug RUNTIME DESTINATION ${BB84_ROOT})
