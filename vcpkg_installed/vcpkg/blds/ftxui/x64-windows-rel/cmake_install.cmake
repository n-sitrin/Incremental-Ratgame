# Install script for directory: C:/Users/noah.sitrin/source/repos/Incremental Ratgame/vcpkg_installed/vcpkg/blds/ftxui/src/v7.0.0-9aedf7bb2b.clean

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Users/noah.sitrin/source/repos/Incremental Ratgame/vcpkg_installed/vcpkg/pkgs/ftxui_x64-windows")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "OFF")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "C:/Users/noah.sitrin/source/repos/Incremental Ratgame/vcpkg_installed/vcpkg/blds/ftxui/x64-windows-rel/ftxui-screen.lib")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "C:/Users/noah.sitrin/source/repos/Incremental Ratgame/vcpkg_installed/vcpkg/blds/ftxui/x64-windows-rel/ftxui-dom.lib")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "C:/Users/noah.sitrin/source/repos/Incremental Ratgame/vcpkg_installed/vcpkg/blds/ftxui/x64-windows-rel/ftxui-component.lib")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include" TYPE DIRECTORY FILES "C:/Users/noah.sitrin/source/repos/Incremental Ratgame/vcpkg_installed/vcpkg/blds/ftxui/src/v7.0.0-9aedf7bb2b.clean/include/ftxui")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/ftxui/ftxui-targets.cmake")
    file(DIFFERENT _cmake_export_file_changed FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/ftxui/ftxui-targets.cmake"
         "C:/Users/noah.sitrin/source/repos/Incremental Ratgame/vcpkg_installed/vcpkg/blds/ftxui/x64-windows-rel/CMakeFiles/Export/418a5211b54bdfa9fa029e8a3112c4d3/ftxui-targets.cmake")
    if(_cmake_export_file_changed)
      file(GLOB _cmake_old_config_files "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/ftxui/ftxui-targets-*.cmake")
      if(_cmake_old_config_files)
        string(REPLACE ";" ", " _cmake_old_config_files_text "${_cmake_old_config_files}")
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/ftxui/ftxui-targets.cmake\" will be replaced.  Removing files [${_cmake_old_config_files_text}].")
        unset(_cmake_old_config_files_text)
        file(REMOVE ${_cmake_old_config_files})
      endif()
      unset(_cmake_old_config_files)
    endif()
    unset(_cmake_export_file_changed)
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/ftxui" TYPE FILE FILES "C:/Users/noah.sitrin/source/repos/Incremental Ratgame/vcpkg_installed/vcpkg/blds/ftxui/x64-windows-rel/CMakeFiles/Export/418a5211b54bdfa9fa029e8a3112c4d3/ftxui-targets.cmake")
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/ftxui" TYPE FILE FILES "C:/Users/noah.sitrin/source/repos/Incremental Ratgame/vcpkg_installed/vcpkg/blds/ftxui/x64-windows-rel/CMakeFiles/Export/418a5211b54bdfa9fa029e8a3112c4d3/ftxui-targets-release.cmake")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/ftxui" TYPE FILE FILES
    "C:/Users/noah.sitrin/source/repos/Incremental Ratgame/vcpkg_installed/vcpkg/blds/ftxui/x64-windows-rel/cmake/ftxui-config.cmake"
    "C:/Users/noah.sitrin/source/repos/Incremental Ratgame/vcpkg_installed/vcpkg/blds/ftxui/x64-windows-rel/cmake/ftxui-config-version.cmake"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/pkgconfig" TYPE FILE FILES "C:/Users/noah.sitrin/source/repos/Incremental Ratgame/vcpkg_installed/vcpkg/blds/ftxui/x64-windows-rel/ftxui.pc")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("C:/Users/noah.sitrin/source/repos/Incremental Ratgame/vcpkg_installed/vcpkg/blds/ftxui/x64-windows-rel/examples/cmake_install.cmake")
  include("C:/Users/noah.sitrin/source/repos/Incremental Ratgame/vcpkg_installed/vcpkg/blds/ftxui/x64-windows-rel/doc/cmake_install.cmake")

endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
if(CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "C:/Users/noah.sitrin/source/repos/Incremental Ratgame/vcpkg_installed/vcpkg/blds/ftxui/x64-windows-rel/install_local_manifest.txt"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
if(CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_COMPONENT MATCHES "^[a-zA-Z0-9_.+-]+$")
    set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
  else()
    string(MD5 CMAKE_INST_COMP_HASH "${CMAKE_INSTALL_COMPONENT}")
    set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INST_COMP_HASH}.txt")
    unset(CMAKE_INST_COMP_HASH)
  endif()
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "C:/Users/noah.sitrin/source/repos/Incremental Ratgame/vcpkg_installed/vcpkg/blds/ftxui/x64-windows-rel/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
