cmake_minimum_required(VERSION 2.8)
#set(CMAKE_VERBOSE_MAKEFILE ON)
project(collections)

# Options
option(DEBUG "Enable/Disable debug library" ON)
option(SHARED "Enable/Disable the shared library version" ON)
option(IMAGE "Enable/Disable image support" OFF)
option(PYPLUGIN "Enable/Disable python plugins" OFF)
option(JAVAPLUGIN "Enable/Disable java plugins" OFF)

include_directories(include)

if(CMAKE_C_COMPILER_VERSION VERSION_GREATER 5)
    add_definitions(-fgnu89-inline)
endif()

if(DEBUG)
    set(CMAKE_BUILD_TYPE Debug)
else(DEBUG)
    set(CMAKE_BUILD_TYPE Release)
endif(DEBUG)

add_definitions("-Wall -Wextra -fPIC")
add_definitions("-DLIBCOLLECTIONS_COMPILE -D_GNU_SOURCE -DPLUGIN_ELF")

execute_process(COMMAND uname -o
    COMMAND tr '[:lower:]' '[:upper:]'
    COMMAND tr '/' '_'
    COMMAND tr -d '\n'
    OUTPUT_VARIABLE OS)

add_definitions("-D${OS}")

file(GLOB CORE "src/core/*.c")
file(GLOB ADT "src/adt/*.c")
file(GLOB CHAT "src/chat/*.c")
file(GLOB FILES "src/files/*.c")
file(GLOB IO "src/io/*.c")
file(GLOB TYPES "src/types/*.c")
file(GLOB PLUGIN "src/plugin/*.c")

set(SOURCE
    ${CORE}
    ${ADT}
    ${CHAT}
    ${FILES}
    ${IO}
    ${TYPES}
    ${PLUGIN})

if(IMAGE)
    include_directories("/usr/include/freetype2")
    add_definitions("-DCL_USE_IMAGEAPI")
    file(GLOB IMAGE "src/image/*.c")
    list(APPEND SOURCE ${IMAGE})
endif(IMAGE)

if(PYPLUGIN)
    include_directories("/usr/include/python2.7")
    add_definitions("-DPLUGIN_PYTHON")
else(PYPLUGIN)
    list(REMOVE_ITEM SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/src/plugin/dl_python.c")
endif(PYPLUGIN)

if(JAVAPLUGIN)
    #LIBDIR += -L/usr/lib/jvm/default-java/jre/lib/$(ARCH)/server/
    include_directories("/usr/lib/jvm/default-java/include")
    include_directories("/usr/lib/jvm/default-java/include/linux")
    add_definitions("-DPLUGIN_JAVA")
else(JAVAPLUGIN)
    list(REMOVE_ITEM SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/src/plugin/dl_java.c")
endif(JAVAPLUGIN)

set(VERSION_SCRIPT
    ${CMAKE_CURRENT_SOURCE_DIR}/misc/lib${PROJECT_NAME}.sym)

set(LIBRARY_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_NAME}.h)

execute_process(COMMAND grep MAJOR_VERSION ${LIBRARY_HEADER}
    COMMAND awk "{print $4}"
    COMMAND tr "\n" " "
    COMMAND sed "s/ //"
    OUTPUT_VARIABLE MAJOR_VERSION)

execute_process(COMMAND grep MINOR_VERSION ${LIBRARY_HEADER}
    COMMAND awk "{print $4}"
    COMMAND tr "\n" " "
    COMMAND sed "s/ //"
    OUTPUT_VARIABLE MINOR_VERSION)

execute_process(COMMAND grep RELEASE ${LIBRARY_HEADER}
    COMMAND awk "{print $4}"
    COMMAND tr "\n" " "
    COMMAND sed "s/ //"
    OUTPUT_VARIABLE RELEASE)

set(DESTINATION_BIN_DIR "/usr/local/lib")
set(DESTINATION_HEADER_DIR "/usr/local/include")

link_directories(${DESTINATION_BIN_DIR})

if(SHARED)
    add_library(${PROJECT_NAME} SHARED ${SOURCE})
    target_link_libraries(${PROJECT_NAME} pthread m rt dl magic ${IMAGELIBS})

    # Additional libraries
    if(IMAGE)
        target_link_libraries(${PROJECT_NAME} opencv_core opencv_imgproc
                              opencv_highgui freetype jpeg png tiff swscale jasper)
    endif(IMAGE)

    if(PYPLUGIN)
        target_link_libraries(${PROJECT_NAME} python2.7)
    endif(PYPLUGIN)

    if(JAVAPLUGIN)
        target_link_libraries(${PROJECT_NAME} jvm)
    endif(JAVAPLUGIN)

    if(NOT ${OS} STREQUAL "GNU_LINUX")
        target_link_libraries(${PROJECT_NAME} intl)
    endif()

    set_target_properties(${PROJECT_NAME} PROPERTIES
                          LINK_FLAGS "-Wl,-soname,${PROJECT_NAME}.so,--version-script,${VERSION_SCRIPT}")

    set_target_properties(${PROJECT_NAME} PROPERTIES
                          SUFFIX .so.${MAJOR_VERSION}.${MINOR_VERSION}.${RELEASE})
else(SHARED)
    add_library(${PROJECT_NAME} STATIC ${SOURCE})
endif(SHARED)

install(TARGETS ${PROJECT_NAME} DESTINATION ${DESTINATION_BIN_DIR})
install(FILES ${LIBRARY_HEADER} DESTINATION ${DESTINATION_HEADER_DIR}/${PROJECT_NAME})
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/api DESTINATION ${DESTINATION_HEADER_DIR}/${PROJECT_NAME})

# If we're dealing with a shared version, we create its symbolic link
if(SHARED)
    set(RELEASE_NAME lib${PROJECT_NAME}.so.${MAJOR_VERSION}.${MINOR_VERSION}.${RELEASE})
    set(LINK_NAME lib${PROJECT_NAME}.so)
    install(CODE "execute_process(COMMAND ln -sf ${DESTINATION_BIN_DIR}/${RELEASE_NAME} ${DESTINATION_BIN_DIR}/${LINK_NAME})")
endif(SHARED)

