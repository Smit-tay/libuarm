add_executable(basic_move basic_move.cc)
target_link_libraries(basic_move PRIVATE uarm)

add_executable(teach_record teach_record.cc)
target_link_libraries(teach_record PRIVATE uarm)
