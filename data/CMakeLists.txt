# Data files to distribute.
file(GLOB DATA_FILES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
    "shaders/*.glsl*"
    "kernels/*.cl"
)

foreach(data_file ${DATA_FILES})
    add_custom_command(
        OUTPUT "${SVR_DATA_PREFIX}/${data_file}"
        COMMAND cmake -E copy
            "${CMAKE_CURRENT_SOURCE_DIR}/${data_file}"
            "${SVR_DATA_PREFIX}/${data_file}"
        DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/${data_file}"
    )
    list(APPEND data_file_dest "${SVR_DATA_PREFIX}/${data_file}")
endforeach()
add_custom_target(CopyDataFiles ALL DEPENDS ${data_file_dest})

