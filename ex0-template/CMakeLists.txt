add_executable(digital_curling_ex0_template
    ex0-template.cpp
)

target_link_libraries(digital_curling_ex0_template
  PRIVATE
    digital_curling::digital_curling
    Boost::headers
    Boost::date_time
    Boost::regex
    Threads::Threads
)
