add_executable(digital_curling_ai_sample__template
    template.cpp
)

find_package(Threads REQUIRED)

target_link_libraries(digital_curling_ai_sample__template
  PRIVATE
    digital_curling::digital_curling
    Boost::headers
    Boost::date_time
    Boost::regex
    Threads::Threads
)
