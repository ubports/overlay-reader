configure_file(
  liboverlay-reader.pc.in liboverlay-reader.pc @ONLY
)

install(
  FILES ${CMAKE_CURRENT_BINARY_DIR}/liboverlay-reader.pc
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
  )
