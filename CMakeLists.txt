cmake_minimum_required(VERSION 3.23)
project(hcp)

set(CMAKE_CXX_STANDARD 17)

add_executable(hcp main.cpp hierarchical_model.cpp hierarchical_model.h readgml.cpp network.h readgml.h indexed_list.cpp indexed_list.h node.h parameters.cpp parameters.h)
find_package(GSL REQUIRED)
target_link_libraries(hcp GSL::gsl GSL::gslcblas)