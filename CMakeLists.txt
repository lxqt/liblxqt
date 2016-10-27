cmake_minimum_required(VERSION 3.0.2 FATAL_ERROR)

project(liblxqt)

set(QT_MINIMUM_VERSION "5.4.2")
set(QTXDG_MINIMUM_VERSION "2.0.0")
set(LXQTBT_MINIMUM_VERSION "0.1.0")

include(CMakePackageConfigHelpers)
include(GNUInstallDirs) # Standard directories for installation

#-----------------------------------------------------------------------------
# Release is the default build type
#-----------------------------------------------------------------------------
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(LXQT_PKG_CONFIG_DESCRIPTION "Shared library for LXQt applications")

set(PUB_HDRS
    lxqthtmldelegate.h
    lxqtsettings.h
    lxqtplugininfo.h
    lxqtpowermanager.h
    lxqtscreensaver.h
    lxqtapplication.h
    lxqtsingleapplication.h
    lxqttranslator.h
    lxqtprogramfinder.h

    configdialog/lxqtconfigdialog.h
    configdialog/lxqtpageselectwidget.h

    lxqtpower/lxqtpower.h
    lxqtnotification.h
    lxqtautostartentry.h
    lxqtgridlayout.h
    lxqtrotatedwidget.h
    lxqtglobals.h
)

set(PUBLIC_CLASSES
    HtmlDelegate
    Settings
    PluginInfo
    PowerManager
    ScreenSaver
    Application
    SingleApplication
    Translator
    ProgramFinder

    ConfigDialog
    PageSelectWidget

    Power
    Notification
    AutostartEntry
    GridLayout
    RotatedWidget
)

set(SRCS
    lxqthtmldelegate.cpp
    lxqtplugininfo.cpp
    lxqtpowermanager.cpp
    lxqtsettings.cpp
    lxqtscreensaver.cpp
    lxqtapplication.cpp
    lxqtsingleapplication.cpp
    lxqttranslator.cpp
    lxqtprogramfinder.cpp

    configdialog/lxqtconfigdialog.cpp
    configdialog/lxqtpageselectwidget.cpp

    lxqtpower/lxqtpower.cpp
    lxqtpower/lxqtpowerproviders.cpp
    lxqtautostartentry.cpp
    lxqtnotification.cpp
    lxqtgridlayout.cpp
    lxqtrotatedwidget.cpp
)

set(MOCS
    lxqthtmldelegate.h
    lxqtpowermanager.h
    lxqtsettings.h
    lxqtscreensaver.h
    lxqtapplication.h

    configdialog/lxqtconfigdialog.h
    configdialog/lxqtpageselectwidget.h

    lxqtnotification.h
    lxqtnotification_p.h
    lxqtpower/lxqtpower.h
    lxqtpower/lxqtpowerproviders.h
    lxqtgridlayout.h
    lxqtrotatedwidget.h
)

set(FORMS
    configdialog/lxqtconfigdialog.ui
)

option(UPDATE_TRANSLATIONS "Update source translation translations/*.ts files" OFF)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

find_package(lxqt-build-tools ${LXQTBT_MINIMUM_VERSION} REQUIRED)
find_package(X11 REQUIRED QUIET)
find_package(Qt5 ${QT_MINIMUM_VERSION} CONFIG REQUIRED Widgets DBus X11Extras LinguistTools)
find_package(Qt5Xdg ${QTXDG_MINIMUM_VERSION} REQUIRED)
find_package(KF5WindowSystem REQUIRED QUIET)
message(STATUS "Building with Qt ${Qt5Core_VERSION_STRING}")

QT5_ADD_DBUS_INTERFACE(SRCS
    dbus/org.freedesktop.Notifications.xml
    notifications_interface
)

QT5_ADD_DBUS_ADAPTOR(SRCS
    dbus/org.lxqt.SingleApplication.xml
    lxqtsingleapplication.h LXQt::SingleApplication
)


# KF5WindowSystem is missing here. KF5WindowSystem doesn't provide an .pc file.
set(LXQT_PKG_CONFIG_REQUIRES "Qt5Xdg >= ${QTXDG_MINIMUM_VERSION}, Qt5Widgets >= ${QT_MINIMUM_VERSION}, Qt5Xml >= ${QT_MINIMUM_VERSION}, Qt5DBus >= ${QT_MINIMUM_VERSION}, Qt5X11Extras >= ${QT_MINIMUM_VERSION}")


# Standard directories for installation
include(LXQtCompilerSettings NO_POLICY_SCOPE)
include(LXQtCreatePkgConfigFile)
include(LXQtCreatePortableHeaders)
include(LXQtConfigVars)

set(LXQT_INTREE_INCLUDE_DIR "${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/include")
set(LXQT_INTREE_TARGETS_FILE "${CMAKE_BINARY_DIR}/${LXQT_LIBRARY_NAME}-targets.cmake")

set(LXQT_INSTALL_INCLUDE_DIR "${CMAKE_INSTALL_FULL_INCLUDEDIR}/${LXQT_LIBRARY_NAME}")
set(LXQT_INSTALL_CMAKE_DIR "${CMAKE_INSTALL_FULL_DATAROOTDIR}/cmake")


## Translations
include(LXQtTranslateTs)

lxqt_translate_ts(QM_FILES
    UPDATE_TRANSLATIONS
        ${UPDATE_TRANSLATIONS}
    SOURCES
        ${SRCS}
        ${FORMS}
    INSTALL_DIR
        "${LXQT_TRANSLATIONS_DIR}/${PROJECT_NAME}"
    PULL_TRANSLATIONS
        ${PULL_TRANSLATIONS}
    CLEAN_TRANSLATIONS
        ${CLEAN_TRANSLATIONS}
    TRANSLATIONS_REPO
        ${TRANSLATIONS_REPO}
    TRANSLATIONS_REFSPEC
        ${TRANSLATIONS_REFSPEC}
)

message(STATUS "")
message(STATUS "liblxqt version: ${LXQT_VERSION}")
message(STATUS "")


# Copy public headers
foreach(h ${PUB_HDRS})
    get_filename_component(bh ${h} NAME)
    configure_file(${h} "${LXQT_INTREE_INCLUDE_DIR}/LXQt/${bh}" COPYONLY)
endforeach()

# Create the portable headers
lxqt_create_portable_headers(INTREE_PORTABLE_HEADERS
    NAME_PREFIX "lxqt"
    OUTPUT_DIR "${LXQT_INTREE_INCLUDE_DIR}/LXQt"
    HEADER_NAMES ${PUBLIC_CLASSES}
)

#check_portable_headers(H_FILES ${PUB_HDRS} LINKS "${INTREE_PORTABLE_HEADERS}")

#************************************************
# Create in-tree build infrastructure
#************************************************
set(CFG_LXQT_TARGETS_FILE   "${LXQT_INTREE_TARGETS_FILE}")

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/lxqt-config.cmake.in"
    "${CMAKE_BINARY_DIR}/${LXQT_LIBRARY_NAME}-config.cmake"
    INSTALL_DESTINATION "neverland"     # required, altough we don't install it
)

#************************************************
# Create installable build infrastructure
#************************************************
set(CFG_LXQT_TARGETS_FILE "${LXQT_INSTALL_CMAKE_DIR}/${LXQT_LIBRARY_NAME}/${LXQT_LIBRARY_NAME}-targets.cmake")

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/lxqt-config.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/install/${LXQT_LIBRARY_NAME}-config.cmake"
    INSTALL_DESTINATION "${LXQT_INSTALL_CMAKE_DIR}/${LXQT_LIBRARY_NAME}"
)

write_basic_package_version_file(
    "${CMAKE_BINARY_DIR}/${LXQT_LIBRARY_NAME}-config-version.cmake"
    VERSION ${LXQT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/install/${LXQT_LIBRARY_NAME}-config.cmake"
    "${CMAKE_BINARY_DIR}/${LXQT_LIBRARY_NAME}-config-version.cmake"
    DESTINATION "${LXQT_INSTALL_CMAKE_DIR}/${LXQT_LIBRARY_NAME}"
    COMPONENT Devel
)

install(EXPORT
    ${LXQT_LIBRARY_NAME}-targets
    DESTINATION "${CMAKE_INSTALL_DATADIR}/cmake/${LXQT_LIBRARY_NAME}"
    COMPONENT Devel
)

add_library(${LXQT_LIBRARY_NAME}
    SHARED ${PUB_HDRS}
           ${SRCS}
           ${dbus_generated}
           ${UIS} ${MOCS}
           ${QM_FILES}
           ${QRC_CXX_SOURCES}

)

target_link_libraries(${LXQT_LIBRARY_NAME}
    PUBLIC
        KF5::WindowSystem
        Qt5::Widgets
        Qt5::DBus
        Qt5::X11Extras
        Qt5Xdg
    PRIVATE
        ${X11_Xscreensaver_LIB}
        ${X11_X11_LIB}
)

set_target_properties(${LXQT_LIBRARY_NAME} PROPERTIES
    VERSION   ${LXQT_VERSION}
    SOVERSION ${LXQT_MAJOR_VERSION}
)

target_compile_definitions(${LXQT_LIBRARY_NAME}
    PRIVATE
        "LXQT_RELATIVE_SHARE_DIR=\"${LXQT_RELATIVE_SHARE_DIR}\""
        "LXQT_SHARE_DIR=\"${LXQT_SHARE_DIR}\""
        "LXQT_RELATIVE_SHARE_TRANSLATIONS_DIR=\"${LXQT_RELATIVE_TRANSLATIONS_DIR}\""
        "LXQT_SHARE_TRANSLATIONS_DIR=\"${LXQT_TRANSLATIONS_DIR}\""
        "LXQT_GRAPHICS_DIR=\"${LXQT_GRAPHICS_DIR}\""
        "LXQT_ETC_XDG_DIR=\"${LXQT_ETC_XDG_DIR}\""
        "LXQT_DATA_DIR=\"${LXQT_DATA_DIR}\""
        "LXQT_INSTALL_PREFIX=\"${CMAKE_INSTALL_PREFIX}\""
        "LXQT_VERSION=\"${LXQT_VERSION}\""
        "COMPILE_LIBLXQT"
        "$<$<CONFIG:Release>:QT_NO_DEBUG_OUTPUT>"
        "$<$<CONFIG:Release>:QT_NO_WARNING_OUTPUT>"
)

if (WITH_XDG_DIRS_FALLBACK)
    target_compile_definitions(${LXQT_LIBRARY_NAME}
        PRIVATE "WITH_XDG_DIRS_FALLBACK"
    )
endif ()

target_include_directories(${LXQT_LIBRARY_NAME}
    INTERFACE
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${LXQT_LIBRARY_NAME}>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${LXQT_LIBRARY_NAME}/LXQt>"
        "$<BUILD_INTERFACE:${LXQT_INTREE_INCLUDE_DIR}>"
        "$<BUILD_INTERFACE:${LXQT_INTREE_INCLUDE_DIR}/LXQt>"
)

install(TARGETS
    ${LXQT_LIBRARY_NAME}
    DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    EXPORT ${LXQT_LIBRARY_NAME}-targets
    LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    COMPONENT Runtime
)

export(TARGETS ${LXQT_LIBRARY_NAME}
    APPEND FILE "${LXQT_INTREE_TARGETS_FILE}"
    EXPORT_LINK_INTERFACE_LIBRARIES
)

install(FILES
    ${PUB_HDRS}
    DESTINATION "${LXQT_INSTALL_INCLUDE_DIR}/LXQt"
    COMPONENT Devel
)

install(FILES
    ${INTREE_PORTABLE_HEADERS}
    DESTINATION "${LXQT_INSTALL_INCLUDE_DIR}/LXQt"
    COMPONENT Devel
)

#************************************************
# Create and install pkgconfig file
#************************************************
lxqt_create_pkgconfig_file(
    PACKAGE_NAME ${LXQT_LIBRARY_NAME}
    DESCRIPTIVE_NAME ${LXQT_LIBRARY_NAME}
    DESCRIPTION ${LXQT_PKG_CONFIG_DESCRIPTION}
    INCLUDEDIRS ${LXQT_LIBRARY_NAME}
    LIBS ${LXQT_LIBRARY_NAME}
    REQUIRES ${LXQT_PKG_CONFIG_REQUIRES}
    VERSION ${LXQT_VERSION}
    INSTALL
)
#************************************************
