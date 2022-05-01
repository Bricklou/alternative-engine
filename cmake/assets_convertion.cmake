find_program(ASSETS_CONVERTER assets-converter HINTS ${PROJECT_BINARY_DIR})

file(GLOB_RECURSE ASSETS_SOURCE_FILES
        "${PROJECT_SOURCE_DIR}/assets_src/**.png"
        "${PROJECT_SOURCE_DIR}/assets_src/**.jpg"

        "${PROJECT_SOURCE_DIR}/assets_src/**.wav"
        "${PROJECT_SOURCE_DIR}/assets_src/**.mp3"
        "${PROJECT_SOURCE_DIR}/assets_src/**.ogg"
        )

foreach(ASSET ${ASSETS_SOURCE_FILES})
    message(STATUS "BUILDING ASSETS")

    # Get the filename without the extension
    get_filename_component(FILE_NAME ${ASSET} NAME_WE)
    # Set the filename to "<filename>.ale-asset"
    set(OUT_FILENAME "${FILE_NAME}.alte-asset")

    # Get the export folder path "assets"
    set(OUT_FOLDER "${PROJECT_SOURCE_DIR}/assets")

    # Get the subpath inside the asset folder
    file(RELATIVE_PATH REL_FILE_PATH "${PROJECT_SOURCE_DIR}/assets_src" ${ASSET})
    # Now remove filename in the path
    get_filename_component(SUBFOLDERS ${REL_FILE_PATH} DIRECTORY)

    # Merge everything together
    set(OUT_FILEPATH "${OUT_FOLDER}/${SUBFOLDERS}/${OUT_FILENAME}")

    message(STATUS ${ASSET})
    add_custom_command(
            OUTPUT ${OUT_FILEPATH}
            POST_BUILD
            DEPENDS assets-converter ${ASSET}
            COMMAND ${ASSETS_CONVERTER} ${ASSET} ${OUT_FILEPATH}
    )
    list(APPEND ASSETS_BINARY_FILES ${OUT_FILEPATH})
endforeach(ASSET)


add_custom_target(
        Assets
        DEPENDS ${ASSETS_BINARY_FILES})