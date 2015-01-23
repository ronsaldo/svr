file(GLOB DCM2FITS_HEADERS
      "include/*.hpp")

file(GLOB DCM2FITS_SOURCES
      "source/*.hpp" 
      "source/*.cpp")

source_group("Headers" FILES ${DCM2FITS_HEADERS})
source_group("Sources" FILES ${DCM2FITS_SOURCES})

add_executable(dcm2fits ${DCM2FITS_HEADERS} ${DCM2FITS_SOURCES})
target_link_libraries(dcm2fits SVRCore ${SVR_DEP_LIBS} ${DCMIMAGE_LIB} ${DCMIMGLE_LIB} ${DCMDATA_LIB} ${OFSTD_LIB})
set_property(TARGET dcm2fits PROPERTY FOLDER "executables")

install (TARGETS dcm2fits
         RUNTIME DESTINATION ${PROJECT_BINARY_DIR}/bin)
