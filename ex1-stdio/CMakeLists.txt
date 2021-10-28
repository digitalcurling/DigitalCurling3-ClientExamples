add_executable(digital_curling_ex1_stdio
    ex1-stdio.cpp
)

target_link_libraries(digital_curling_ex1_stdio
  PRIVATE
    digital_curling::digital_curling
    Boost::headers
    Boost::date_time
    Boost::regex
    Threads::Threads
)
