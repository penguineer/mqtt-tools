configure_file(
    mqtta-build.h.in
    mqtta-build.h
    @ONLY
)

install(FILES
	mqtta.h
	mosqhelper.h
	DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/mqtt-tools"
)
