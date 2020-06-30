cmake_minimum_required(VERSION 3.9)
project(login_server)

find_package(PkgConfig REQUIRED)
find_package(cpprestsdk REQUIRED)

pkg_check_modules(LIBPQXX REQUIRED libpqxx)
pkg_check_modules(OPENSSL REQUIRED openssl)

add_executable(login_server main.cpp)

target_include_directories(login_server PUBLIC
  ${LIBPQXX_INCLUDE_DIRS}
  ${OPENSSL_INCLUDE_DIRS}
)
target_compile_options(login_server PUBLIC
  ${LIBPQXX_CFLAGS_OTHER}
  ${OPENSSL_CFLAGS_OTHER}
)
target_link_libraries(login_server PRIVATE
  cpprestsdk::cpprest
  ${LIBPQXX_LIBRARIES}
  ${OPENSSL_LIBRARIES}
)
