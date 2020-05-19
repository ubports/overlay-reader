add_subdirectory(arcsParser)
add_subdirectory(axmlParser)

set(SERVICE_SRC
    main.cpp
    overlay-reader.cpp
)

add_executable(overlay-reader
    ${SERVICE_SRC} 
)

target_link_libraries(overlay-reader
    arcsParser-core
    axmlParser-core
    ${ZIP_LIBRARIES}
    ${Boost_LIBRARIES}
)

install(TARGETS overlay-reader RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
