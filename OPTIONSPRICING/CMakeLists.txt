cmake_minimum_required(VERSION 3.20)
project(OptionsPricing LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(optionspricing
    main.cpp
    option.cpp
    math_utils.cpp
    pricing/closed_form.cpp
    pricing/simulation.cpp
    pricing/greeks.cpp
)
