set(C_FILES
    lxqtbacklight_backend.c
)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
)
add_executable(lxqt-backlight_backend
    ${C_FILES}
)

install(TARGETS lxqt-backlight_backend RUNTIME DESTINATION bin)
