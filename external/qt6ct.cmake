cmake_minimum_required(VERSION 3.16.0)
set(PROJECT_NAME scwx-qt6ct)

add_subdirectory(qt6ct/src/qt6ct-common)
set_target_properties(qt6ct-common PROPERTIES PUBLIC_HEADER qt6ct/src/qt6ct-common/qt6ct.h)
target_include_directories( qt6ct-common INTERFACE qt6ct/src )
install(TARGETS qt6ct-common
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/qt6ct
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
