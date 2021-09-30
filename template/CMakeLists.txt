add_executable(digitalcurling3_client_examples__template
    template.cpp
)

target_link_libraries(digitalcurling3_client_examples__template
  PRIVATE
    digitalcurling3::digitalcurling3
    Boost::headers
    Boost::date_time
    Boost::regex
    Threads::Threads
)
