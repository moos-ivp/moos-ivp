cmake_minimum_required(VERSION 3.5)

project(IVPPackageConsumer LANGUAGES CXX)

find_package(IVP CONFIG REQUIRED)

add_executable(ivp_package_consumer main.cpp)
target_link_libraries(ivp_package_consumer PRIVATE
  IVP::apputil
  IVP::geometry)
