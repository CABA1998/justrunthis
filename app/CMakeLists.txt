add_executable(justrunthis WIN32
    main.cpp
)

target_link_libraries(justrunthis PRIVATE windows)

if(WIN32)
    find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${Qt6_DIR}/../../../bin")
    if(WINDEPLOYQT_EXECUTABLE)
        add_custom_command(TARGET justrunthis POST_BUILD
            COMMAND "${WINDEPLOYQT_EXECUTABLE}"
                    --no-translations
                    --no-system-d3d-compiler
                    --no-opengl-sw
                    "$<TARGET_FILE:justrunthis>"
            COMMENT "Deploying Qt runtime next to justrunthis.exe"
        )
    endif()
endif()
