cmake_minimum_required(VERSION 2.8.11)
project(lxqt-leave)

set(CPP_FILES
    main.cpp
)


# Translations **********************************
include(LXQtTranslate)
lxqt_translate_desktop(DESKTOP_FILES
    SOURCES
    resources/lxqt-hibernate.desktop.in
    resources/lxqt-lockscreen.desktop.in
    resources/lxqt-logout.desktop.in
    resources/lxqt-reboot.desktop.in
    resources/lxqt-shutdown.desktop.in
    resources/lxqt-suspend.desktop.in
)
#************************************************


add_executable(lxqt-leave
    ${CPP_FILES}
    ${DESKTOP_FILES}
)

target_link_libraries(lxqt-leave
    ${LXQT_LIBRARIES}
    ${LXQT_LIBRARY_NAME}
)

install(TARGETS lxqt-leave RUNTIME DESTINATION bin)
install(FILES ${DESKTOP_FILES} DESTINATION share/applications)

