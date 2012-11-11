# Define a function for getting the tool path.
function(calculate_tool_path tool variable)
    if(MINGW)
        set(${variable} ${${tool}_BINARY_DIR}/${tool}${CMAKE_EXECUTABLE_SUFFIX} PARENT_SCOPE)
    else()
        set(${variable} ${tool} PARENT_SCOPE)
    endif()
endfunction()

# Define a general function that allows us to
# define tools and their handling of files.
function(define_dcpu_tool tool toolargs verb files runasm outvar)
    set(tout "")
    foreach(arg ${ARGN})
        set(UARGN "${UARGN} ${arg}")
    endforeach(arg ${ARGN})
    foreach(i ${files})
        get_filename_component(fext "${i}" EXT)
        get_filename_component(fpth "${i}" PATH)
        get_filename_component(fbse "${i}" NAME_WE)
        string(TOLOWER "${fext}" fextc)
        set(extension_matches false)
        foreach(e ${ARGN})
            if("${fext}" STREQUAL "${e}")
                set(extension_matches true)
            endif("${fext}" STREQUAL "${e}")
        endforeach(e ${ARGN})
        if(${extension_matches} STREQUAL "true")
            set(asmwarningsilence -Wno-expression-not-adjusted)
            set(cwarningsilence "")
            if(${tool} STREQUAL "dtasm")
                set(toolwarningsilence ${asmwarningsilence})
            else(${tool} STREQUAL "dtcc")
                set(toolwarningsilence ${cwarningsilence})
            endif()
            if(${runasm} STREQUAL "true")
                calculate_tool_path(${tool} main_path)
                calculate_tool_path(dtasm asm_path)
                add_custom_command(
                    OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/${fpth}/${fbse}.oa
                    COMMAND ${main_path}
                        ARGS ${toolargs} ${toolwarningsilence} -I "${CMAKE_CURRENT_SOURCE_DIR}/include" -o "${CMAKE_CURRENT_BINARY_DIR}/${fpth}/${fbse}.oa" "${CMAKE_CURRENT_SOURCE_DIR}/${i}"
                    DEPENDS ${tool} dtasm "${CMAKE_CURRENT_SOURCE_DIR}/${i}"
                    COMMENT "${verb} ${fbse}${fext} with DCPU-Toolchain...")
                add_custom_command(
                    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${fpth}/${fbse}.o
                    COMMAND ${asm_path}
                        ARGS ${asmwarningsilence} -o "${CMAKE_CURRENT_BINARY_DIR}/${fpth}/${fbse}.o" -s "${CMAKE_CURRENT_BINARY_DIR}/${fpth}/${fbse}.os" "${CMAKE_CURRENT_BINARY_DIR}/${fpth}/${fbse}.oa"
                    DEPENDS ${tool} dtasm "${CMAKE_CURRENT_SOURCE_DIR}/${i}" "${CMAKE_CURRENT_BINARY_DIR}/${fpth}/${fbse}.oa"
                    COMMENT "Assembling ${fpth}/${fbse}.oa with DCPU-Toolchain...")
                if("${tout}" STREQUAL "")
                    set(tout "${CMAKE_CURRENT_BINARY_DIR}/${fpth}/${fbse}.o")
                else("${tout}" STREQUAL "")
                    list(APPEND tout "${CMAKE_CURRENT_BINARY_DIR}/${fpth}/${fbse}.o")
                endif("${tout}" STREQUAL "")
                set(${outvar} "${${outvar}} ${tout}" PARENT_SCOPE)
            else(${runasm} STREQUAL "true")
                calculate_tool_path(${tool} main_path)
                add_custom_command(
                    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${fpth}/${fbse}.o
                    COMMAND ${main_path}
                        ARGS ${toolargs} ${toolwarningsilence} -s "${CMAKE_CURRENT_BINARY_DIR}/${fpth}/${fbse}.os" -o "${CMAKE_CURRENT_BINARY_DIR}/${fpth}/${fbse}.o" "${CMAKE_CURRENT_SOURCE_DIR}/${i}"
                    DEPENDS ${tool} "${CMAKE_CURRENT_SOURCE_DIR}/${i}"
                    COMMENT "${verb} ${fbse}${fext} with DCPU-Toolchain...")
                if("${tout}" STREQUAL "")
                    set(tout "${CMAKE_CURRENT_BINARY_DIR}/${fpth}/${fbse}.o")
                else("${tout}" STREQUAL "")
                    list(APPEND tout "${CMAKE_CURRENT_BINARY_DIR}/${fpth}/${fbse}.o")
                endif("${tout}" STREQUAL "")
                set(${outvar} "${${outvar}} ${tout}" PARENT_SCOPE)
            endif(${runasm} STREQUAL "true")
        endif(${extension_matches} STREQUAL "true")
    endforeach(i ${files})
endfunction(define_dcpu_tool tool toolargs files outvar)

# Define function to specify a standard library target.
function(add_dcpu_image target)
    set(toutputs "")    

    # Define each of the tools using the files list.
    define_dcpu_tool(dtasm "" "Assembling" "${ARGN}" "false" tasmoutputs ".dasm" ".dasm16")
    define_dcpu_tool(dtcc "" "Compiling" "${ARGN}" "true" tcoutputs ".c")

    # Define the linker.
    string(STRIP "${tasmoutputs}" tasmoutputs)
    string(STRIP "${tcoutputs}" tcoutputs)
    calculate_tool_path(dtld main_path)
    add_custom_command(
        OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${target}/${target}.dkrn16"
        COMMAND ${main_path}
        ARGS -s "${CMAKE_CURRENT_BINARY_DIR}/${target}/${target}.dsym16" -O3 --symbol-extension="os"
            --keep-outputs -l kernel -o "${CMAKE_CURRENT_BINARY_DIR}/${target}/${target}.dkrn16"
            --jumplist "${CMAKE_CURRENT_BINARY_DIR}/${target}/${target}.djmp16" ${tasmoutputs} ${tcoutputs}
        DEPENDS dtld ${tasmoutputs} ${tcoutputs}
        COMMENT "Linking ${target}.dkrn16 as kernel with DCPU-Toolchain...")

    # Define our target.
    foreach(i ${tasmoutputs})
        if(DEFINED params)
            list(APPEND params DEPENDS)
            list(APPEND params ${i})
        else(DEFINED params)
            set(params DEPENDS ${i})
        endif(DEFINED params)
    endforeach(i ${tasmoutputs})
    foreach(i ${tcoutputs})
        if(DEFINED params)
            list(APPEND params DEPENDS)
            list(APPEND params ${i})
        else(DEFINED params)
            set(params DEPENDS ${i})
        endif(DEFINED params)
    endforeach(i ${tcoutputs})
    list(APPEND params DEPENDS)
    list(APPEND params "${CMAKE_CURRENT_BINARY_DIR}/${target}/${target}.dkrn16")
    add_custom_target(${target} ALL ${params})
endfunction(add_dcpu_image target)

