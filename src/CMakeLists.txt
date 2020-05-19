add_subdirectory(arcsParser)
add_subdirectory(axmlParser)

add_library(overlay-reader SHARED
    overlay-reader.cpp
    overlay-reader.h
)

target_link_libraries(overlay-reader
    arcsParser-core
    axmlParser-core
    ${ZIP_LIBRARIES}
    ${Boost_LIBRARIES}
)

add_executable(getoverlay
    main.cpp
)

target_link_libraries(getoverlay
    overlay-reader
)

install(TARGETS getoverlay RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
