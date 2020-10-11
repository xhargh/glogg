
if (APPLE)
    set(OS_BUNDLE MACOSX_BUNDLE)
elseif (WIN32)
    set(OS_BUNDLE WIN32)
endif()

configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/documentation.qrc.in
        ${DOCUMENTATION_RESOURCE}
        @ONLY)

add_custom_command (OUTPUT ${DOCUMENTATION_HTML}
        COMMAND $<TARGET_FILE:maddy> ${DOCUMENTATION_FILE} ${DOCUMENTATION_HTML}
        DEPENDS maddy ${DOCUMENTATION_FILE})

set(MAIN_SOURCES
    #${CMAKE_CURRENT_SOURCE_DIR}/crashhandler.cpp
    #${CMAKE_CURRENT_SOURCE_DIR}/crashhandler.h
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/messagereceiver.h
    ${CMAKE_CURRENT_SOURCE_DIR}/kloggapp.h
    ${CMAKE_CURRENT_SOURCE_DIR}/klogg.qrc
    ${CMAKE_CURRENT_SOURCE_DIR}/qdarkstyle/style.qrc
    ${DOCUMENTATION_RESOURCE}
    ${ICON_FILE})

add_executable(klogg-io ${OS_BUNDLE} ${MAIN_SOURCES})
add_executable(klogg_io_portable ${OS_BUNDLE} ${MAIN_SOURCES})

add_dependencies(ci_build klogg-io klogg_io_portable)

set_target_properties(klogg-io PROPERTIES AUTORCC ON)
set_target_properties(klogg-io PROPERTIES AUTOMOC ON)
set_target_properties(klogg_io_portable PROPERTIES AUTORCC ON)
set_target_properties(klogg_io_portable PROPERTIES AUTOMOC ON)

if (${USE_LTO})
set_property(TARGET klogg-io PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
set_property(TARGET klogg_io_portable PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

set(MAIN_LIBS ui cli11 singleapp)
if (NOT APPLE)
    if (TBB_BUILD_TBBMALLOC)
        LIST(APPEND MAIN_LIBS klogg_tbbmalloc_proxy)
        target_compile_definitions(klogg-io PUBLIC -DKLOGG_USE_TBB_MALLOC)
        target_compile_definitions(klogg_io_portable PUBLIC -DKLOGG_USE_TBB_MALLOC)
    endif()
endif()

target_link_libraries(klogg-io PUBLIC ${MAIN_LIBS})
target_link_libraries(klogg_io_portable PUBLIC ${MAIN_LIBS})

target_compile_definitions(klogg_io_portable PUBLIC -DKLOGG_PORTABLE)

if (NOT WIN32)

    add_custom_command(TARGET klogg-io POST_BUILD
        COMMAND ${SYMS_TOOLS}/dump_syms $<TARGET_FILE:klogg-io> > ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/klogg-io.sym
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Generating symbols for $<TARGET_FILE:klogg-io>"
    )

    add_custom_command(TARGET klogg_io_portable POST_BUILD
        COMMAND ${SYMS_TOOLS}/dump_syms $<TARGET_FILE:klogg_io_portable> > ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/klogg_io_portable.sym
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Generating symbols for $<TARGET_FILE:klogg_io_portable>"
    )

    if (APPLE)
        find_program (DSYMUTIL dsymutil)
        add_custom_command(TARGET klogg-io POST_BUILD
            COMMAND ${DSYMUTIL} --out=${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/klogg-io.dSym $<TARGET_FILE:klogg-io>
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            COMMENT "Generating symbols for $<TARGET_FILE:klogg-io>"
        )

        add_custom_command(TARGET klogg_portable POST_BUILD
            COMMAND ${DSYMUTIL} --out=${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/klogg_io_portable.dSym $<TARGET_FILE:klogg_io_portable>
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            COMMENT "Generating symbols for $<TARGET_FILE:klogg_io_portable>"
        )

    endif()

    #add_custom_command(TARGET klogg POST_BUILD
    #    COMMAND cmake -E copy $<TARGET_FILE:crashpad_handler> $<TARGET_FILE_DIR:klogg>/klogg_crashpad_handler
    #    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    #    COMMENT "Copy crash_handler"
    #)

    #add_custom_command(TARGET klogg POST_BUILD
    #    COMMAND cmake -E copy  ${SYMS_TOOLS}/minidump_dump $<TARGET_FILE_DIR:klogg>/klogg_minidump_dump
    #    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    #    COMMENT "Copy stackwalk"
    #)
else()
    #add_custom_command(TARGET klogg POST_BUILD
    #    COMMAND cmake -E copy $<TARGET_FILE:crashpad_handler> $<TARGET_FILE_DIR:klogg>/klogg_crashpad_handler.exe
    #    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    #    COMMENT "Copy crash_handler"
    #)

    #if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    #    add_custom_command(TARGET klogg POST_BUILD
    #        COMMAND cmake -E copy  ${SYMS_TOOLS}/minidump_dump_x64.exe $<TARGET_FILE_DIR:klogg>/klogg_minidump_dump.exe
    #        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    #        COMMENT "Copy stackwalk"
    #    )
    #else()
    #    add_custom_command(TARGET klogg POST_BUILD
    #        COMMAND cmake -E copy  ${SYMS_TOOLS}/minidump_dump_x86.exe $<TARGET_FILE_DIR:klogg>/klogg_minidump_dump.exe
    #        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    #        COMMENT "Copy stackwalk"
    #    )
    #endif()
endif()




if (WIN32)

    target_sources(klogg-io PRIVATE ${ProductVersionResourceFiles})
    target_sources(klogg_io_portable PRIVATE ${ProductVersionResourceFiles})

elseif (APPLE)
    set_source_files_properties(${ICON_FILE} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
    set_source_files_properties(${NOTICE_FILE} PROPERTIES MACOSX_PACKAGE_LOCATION SharedSupport)
    set_source_files_properties(${COPYING_FILE} PROPERTIES MACOSX_PACKAGE_LOCATION SharedSupport)

    set_source_files_properties(${README_FILE} PROPERTIES MACOSX_PACKAGE_LOCATION SharedSupport)
    set_source_files_properties(${DOCUMENTATION_FILE} PROPERTIES MACOSX_PACKAGE_LOCATION SharedSupport)
    set_source_files_properties(${DOCUMENTATION_HTML} PROPERTIES MACOSX_PACKAGE_LOCATION SharedSupport)

    set(MACOSX_BUNDLE_BUNDLE_DISPLAY_NAME ${PROJECT_DESCRIPTION})
    set(MACOSX_BUNDLE_BUNDLE_NAME ${PROJECT_NAME})
    set(MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION})
    set(MACOSX_BUNDLE_LONG_VERSION_STRING ${PROJECT_VERSION})
    set(MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}")
    set(MACOSX_BUNDLE_COPYRIGHT ${COPYRIGHT})
    set(MACOSX_BUNDLE_GUI_IDENTIFIER ${IDENTIFIER})
    set(MACOSX_BUNDLE_ICON_FILE ${ICON_NAME})

endif()



install(TARGETS
    klogg-io
    DESTINATION bin
    BUNDLE DESTINATION .
)

#if(APPLE)
#    install(PROGRAMS
#        $<TARGET_FILE_DIR:klogg>/klogg_minidump_dump
#        $<TARGET_FILE_DIR:klogg>/klogg_crashpad_handler
#        DESTINATION $<TARGET_BUNDLE_CONTENT_DIR:klogg>/MacOS
#    )
#elseif(NOT WIN32)
#    install(PROGRAMS
#        $<TARGET_FILE_DIR:klogg>/klogg_minidump_dump
#        $<TARGET_FILE_DIR:klogg>/klogg_crashpad_handler
#        DESTINATION bin
#    )
#endif()

