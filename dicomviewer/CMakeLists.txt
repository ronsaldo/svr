file(GLOB DCMVIEWER_HEADERS
      "include/*.hpp")

file(GLOB DCMVIEWER_SOURCES
      "source/*.hpp" 
      "source/*.cpp")

source_group("Headers" FILES ${DCMVIEWER_HEADERS})
source_group("Sources" FILES ${DCMVIEWER_SOURCES})

add_executable(dcmviewer ${DCMVIEWER_HEADERS} ${DCMVIEWER_SOURCES})
target_link_libraries(dcmviewer SVRCore ${SVR_DEP_LIBS} ${DCMIMAGE_LIB} ${DCMIMGLE_LIB} ${DCMDATA_LIB} ${OFSTD_LIB})
set_property(TARGET dcmviewer PROPERTY FOLDER "executables")

install (TARGETS dcmviewer
         RUNTIME DESTINATION ${PROJECT_BINARY_DIR}/bin)
