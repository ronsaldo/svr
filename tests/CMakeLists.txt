file(GLOB SVRTests_SOURCES
      "source/*.cpp"
)

source_group("Sources" FILES ${SVRTests_SOURCES})

add_executable(SVRTests ${SVRTests_SOURCES})
target_link_libraries(SVRTests SVRCore ${UNITTESTMM_LIBRARIES})
set_property(TARGET SVRTests PROPERTY FOLDER "executables")

install (TARGETS SVRTests
         RUNTIME DESTINATION ${PROJECT_BINARY_DIR}/bin)
