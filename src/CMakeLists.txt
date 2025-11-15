# SPDX-License-Identifier: GPL-3.0-or-later
# SPDX-FileCopyrightText: 2025 J <jean.chalard@gmail.com>

# Target: static library
qt_add_qml_module(manette_static
    STATIC
    URI org.j.manette
    VERSION 1.0
    QML_FILES
        contents/ui/Main.qml
        contents/ui/About.qml
)

qt_add_resources(manette_static "sounds"
    PREFIX "/sounds"
    FILES res/left.ogg res/bottom.ogg res/right.ogg res/top.ogg
)

find_package(PkgConfig REQUIRED)
pkg_check_modules(SYSTEMD REQUIRED libsystemd)

target_sources(manette_static PUBLIC
    app.cpp
    evdevcontroller.h
    evdevcontroller.cpp
)

target_link_libraries(manette_static PUBLIC
    Qt6::Core
    Qt6::Gui
    Qt6::Qml
    Qt6::Quick
    Qt6::QuickControls2
    Qt6::Svg
    KF6::I18n
    KF6::CoreAddons
    KF6::ConfigCore
    KF6::ConfigGui
    ${SYSTEMD_LIBRARIES}
)
target_include_directories(manette_static PUBLIC ${CMAKE_BINARY_DIR})

target_link_libraries(manette_static PUBLIC Qt::Widgets)

kconfig_add_kcfg_files(manette_static GENERATE_MOC manetteconfig.kcfgc)

# Target: main executable
add_executable(manette main.cpp)
target_link_libraries(manette PUBLIC manette_static manette_staticplugin)
install(TARGETS manette ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
