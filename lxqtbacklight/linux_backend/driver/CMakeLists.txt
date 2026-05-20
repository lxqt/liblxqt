set(C_FILES
    lxqtbacklight_backend.c
)

add_executable(lxqt-backlight_backend
    ${C_FILES}
)

install(TARGETS lxqt-backlight_backend RUNTIME DESTINATION bin)
