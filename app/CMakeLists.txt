file(GLOB SVR_HEADERS
      "include/*.hpp")

file(GLOB SVR_SOURCES
      "source/*.hpp" 
      "source/*.cpp")

source_group("Headers" FILES ${SVR_HEADERS})
source_group("Sources" FILES ${SVR_SOURCES})

add_executable(SVR ${SVR_HEADERS} ${SVR_SOURCES})
target_link_libraries(SVR SVRCore ${SVR_DEP_LIBS})
set_property(TARGET SVR PROPERTY FOLDER "executables")

install (TARGETS SVR
         RUNTIME DESTINATION ${PROJECT_BINARY_DIR}/bin)
