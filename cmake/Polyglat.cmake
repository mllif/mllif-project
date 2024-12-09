function(add_polyglat_library target language)
    foreach (SOURCE ${ARGN})
        string(REGEX REPLACE "[.][^.]*$" ".ll" BITCODE "${SOURCE}")
        string(REGEX REPLACE "^${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_BINARY_DIR}" BITCODE "${BITCODE}")
        set(BITCODES ${BITCODES} "${BITCODE}")
    endforeach ()

    set(COMPILER "CMAKE_${language}_COMPILER")
    foreach (arg IN ZIP_LISTS BITCODES ARGN)
        add_custom_command(
                OUTPUT ${arg_0}
                COMMAND ${${COMPILER}}
                -fplugin="${POLYGLAT_PATH}/polyglat-c.so"
                -S -emit-llvm
                $<TARGET_PROPERTY:${target},COMPILE_OPTIONS>
                -c "${arg_1}"
                -o "${arg_0}"
                DEPENDS ${arg_1}
        )
    endforeach ()

    set(TARGET_PSM_FILE "$<TARGET_PROPERTY:${target},LIBRARY_OUTPUT_DIRECTORY>/$<TARGET_PROPERTY:${target},PREFIX>${target}.psm")
    set(ENSEMBLE_BITCODE "$<TARGET_PROPERTY:${target},BINARY_DIR>/$<TARGET_PROPERTY:${target},PREFIX>${target}.ll")
    add_custom_target(${target}
            COMMAND llvm-link ${BITCODES} -o "${ENSEMBLE_BITCODE}"
            COMMAND opt
            -load-pass-plugin="${POLYGLAT_PATH}/polyglat-ir.so"
            --passes=polyglat
            --polyglat-output="${TARGET_PSM_FILE}"
            "${ENSEMBLE_BITCODE}"
            -o "${ENSEMBLE_BITCODE}"
            COMMAND clang -shared $<TARGET_PROPERTY:${target},LINK_OPTIONS> "${ENSEMBLE_BITCODE}"
            -o "$<TARGET_PROPERTY:${target},LIBRARY_OUTPUT_DIRECTORY>/$<TARGET_PROPERTY:${target},PREFIX>${target}$<TARGET_PROPERTY:${target},SUFFIX>"
            DEPENDS ${BITCODES}
    )
    set_target_properties(${target} PROPERTIES
            LIBRARY_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
            TARGET_PSM_FILE "${TARGET_PSM_FILE}"
            PREFIX "${CMAKE_SHARED_LIBRARY_PREFIX}"
            SUFFIX "${CMAKE_SHARED_LIBRARY_SUFFIX}"
    )
endfunction()
