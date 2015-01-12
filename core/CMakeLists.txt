file(GLOB SVRCore_HEADERS
      "include/Leng/*.hpp")

file(GLOB SVRCore_SOURCES
      "source/*.hpp"
      "source/*.cpp")

source_group("Headers" FILES ${SVRCore_HEADERS})
source_group("Sources" FILES ${SVRCore_SOURCES})

add_definitions(-DBUILD_SVR_CORE)
add_library(SVRCore SHARED ${SVRCore_HEADERS} ${SVRCore_SOURCES})
target_link_libraries(SVRCore ${SVR_DEP_LIBS})
set_property(TARGET SVRCore PROPERTY FOLDER "libraries")

install (TARGETS SVRCore
         RUNTIME DESTINATION ${PROJECT_BINARY_DIR}/bin
         LIBRARY DESTINATION ${PROJECT_BINARY_DIR}/lib)
