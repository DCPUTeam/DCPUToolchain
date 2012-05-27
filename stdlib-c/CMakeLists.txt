cmake_minimum_required (VERSION 2.6)
project(stdlib-c)

# Check to see if we are being built as part of
# DCPU-Toolchain or whether this is being built
# on it's own.
if(NOT DEFINED IS_ROOT)
  # We need to define a CMakeLists.standalone.txt file for
  # this to work; at the moment it simply errors.
  #include(CMakeLists.standalone.txt NO_POLICY_SCOPE)
  message(FATAL_ERROR "The standard library must be built as part of the toolchain.")
endif(NOT DEFINED IS_ROOT)

# Define the working directory for tools.
set(KERNEL_TOOLS ${OUTPUT_DIR})
set(KERNEL_SRC ${ROOT_DIR}/stdlib-c/src)

# Define a general function that allows us to
# define tools and their handling of files.
function(define_dcpu_tool tool toolargs verb files runasm outvar)
	set(tout "")
	foreach(arg ${ARGN})
		set(UARGN "${UARGN} ${arg}")
	endforeach(arg ${ARGN})
	if("${toolargs}" STREQUAL "")
		message("    .. defining tool ${tool} with '${files}'")
	else("${toolargs}" STREQUAL "")
		message("    .. defining tool ${tool} ${toolargs} with '${files}'")
	endif("${toolargs}" STREQUAL "")
	if(DEFINED VERY_VERBOSE)
		message("       recognising ${UARGN}")
	endif(DEFINED VERY_VERBOSE)
	foreach(i ${files})
		get_filename_component(fext "${i}" EXT)
		get_filename_component(fpth "${i}" PATH)
		get_filename_component(fbse "${i}" NAME_WE)
		string(TOLOWER "${fext}" fextc)
		if(DEFINED VERY_VERBOSE)
			message("       .. scanning ${i}")
			message("          extension: ${fext}")
		endif(DEFINED VERY_VERBOSE)
		set(extension_matches false)
		foreach(e ${ARGN})
			if(DEFINED VERY_VERBOSE)
				message("          .. checking ${e}")
			endif(DEFINED VERY_VERBOSE)
			if("${fext}" STREQUAL "${e}")
				if(DEFINED VERY_VERBOSE)
					message("             (extension matches)")
				endif(DEFINED VERY_VERBOSE)
				set(extension_matches true)
			endif("${fext}" STREQUAL "${e}")
		endforeach(e ${ARGN})
		if(${extension_matches} STREQUAL "true")
			if(${runasm} STREQUAL "true")
				if(DEFINED VERY_VERBOSE)
					message("       .. adding command")
					message("          output: ${fpth}/${fbse}.o")
					message("          command: ${KERNEL_TOOLS}/${tool}${CMAKE_EXECUTABLE_SUFFIX} ${toolargs} -o \"${fpth}/${fbse}.oa\" \"${i}\"")
					message("          command: ${KERNEL_TOOLS}/dtasm${CMAKE_EXECUTABLE_SUFFIX} -i -o \"${fpth}/${fbse}.o\" \"${fpth}/${fbse}.oa\"")
					message("          depends: ${tool} dtasm ${i}")
				endif(DEFINED VERY_VERBOSE)
				add_custom_command(
					OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/${fpth}/${fbse}.oa
					COMMAND ${KERNEL_TOOLS}/${tool}${CMAKE_EXECUTABLE_SUFFIX}
						ARGS ${toolargs} -I "${CMAKE_CURRENT_SOURCE_DIR}/include" -o "${CMAKE_CURRENT_SOURCE_DIR}/${fpth}/${fbse}.oa" "${CMAKE_CURRENT_SOURCE_DIR}/${i}"
					DEPENDS ${tool} dtasm "${CMAKE_CURRENT_SOURCE_DIR}/${i}"
					COMMENT "${verb} ${fbse}${fext} with DCPU-Toolchain...")
				add_custom_command(
					OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/${fpth}/${fbse}.o
					COMMAND ${KERNEL_TOOLS}/dtasm${CMAKE_EXECUTABLE_SUFFIX}
						ARGS -i -o "${CMAKE_CURRENT_SOURCE_DIR}/${fpth}/${fbse}.o" "${CMAKE_CURRENT_SOURCE_DIR}/${fpth}/${fbse}.oa"
					DEPENDS ${tool} dtasm "${CMAKE_CURRENT_SOURCE_DIR}/${i}" "${CMAKE_CURRENT_SOURCE_DIR}/${fpth}/${fbse}.oa"
					COMMENT "Assembling ${fpth}/${fbse}.oa with DCPU-Toolchain...")
				if("${tout}" STREQUAL "")
					set(tout "${fpth}/${fbse}.o")
				else("${tout}" STREQUAL "")
					list(APPEND tout "${fpth}/${fbse}.o")
				endif("${tout}" STREQUAL "")
				set(${outvar} "${${outvar}} ${tout}" PARENT_SCOPE)
			else(${runasm} STREQUAL "true")
				if(DEFINED VERY_VERBOSE)
					message("       .. adding command")
					message("          output: ${fpth}/${fbse}.o")
					message("          command: ${KERNEL_TOOLS}/${tool}${CMAKE_EXECUTABLE_SUFFIX} ${toolargs} -o \"${fpth}/${fbse}.o\" \"${i}\"")
					message("          depends: ${tool}")
				endif(DEFINED VERY_VERBOSE)
				add_custom_command(
					OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/${fpth}/${fbse}.o
					COMMAND ${KERNEL_TOOLS}/${tool}${CMAKE_EXECUTABLE_SUFFIX}
						ARGS ${toolargs} -o "${CMAKE_CURRENT_SOURCE_DIR}/${fpth}/${fbse}.o" "${CMAKE_CURRENT_SOURCE_DIR}/${i}"
					DEPENDS ${tool} "${CMAKE_CURRENT_SOURCE_DIR}/${i}"
					COMMENT "${verb} ${fbse}${fext} with DCPU-Toolchain...")
				if("${tout}" STREQUAL "")
					set(tout "${fpth}/${fbse}.o")
				else("${tout}" STREQUAL "")
					list(APPEND tout "${fpth}/${fbse}.o")
				endif("${tout}" STREQUAL "")
				set(${outvar} "${${outvar}} ${tout}" PARENT_SCOPE)
			endif(${runasm} STREQUAL "true")
		endif(${extension_matches} STREQUAL "true")
	endforeach(i ${files})
endfunction(define_dcpu_tool tool toolargs files outvar)

# Define function to specify a standard library target.
function(add_dcpu_image target)
	set(toutputs "")	
	message("  >> ${target}")

	# Define each of the tools using the files list.
	define_dcpu_tool(dtasm "-i" "Assembling" "${ARGN}" "false" tasmoutputs ".dasm" ".dasm16")
	define_dcpu_tool(dtcc "" "Compiling" "${ARGN}" "true" tcoutputs ".c")

	# Define the linker.
	string(STRIP "${tasmoutputs}" tasmoutputs)
	string(STRIP "${tcoutputs}" tcoutputs)
	add_custom_command(
		OUTPUT "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/stdlib.dlib16"
		COMMAND ${KERNEL_TOOLS}/dtld${CMAKE_EXECUTABLE_SUFFIX}
			ARGS -l static -o "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/stdlib.dlib16" ${tasmoutputs} ${tcoutputs}
		DEPENDS dtld ${tasmoutputs} ${tcoutputs}
		COMMENT "Linking stdlib.dlib16 as static library with DCPU-Toolchain...")

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
	list(APPEND params "${OUTPUT_DIR}/stdlib.dlib16")
	add_custom_target(${target} ALL ${params})
endfunction(add_dcpu_image target)

# Define the standard library.
add_dcpu_image(stdlib-c
	src/bootstrap.dasm16
	src/stdlib.c
	src/string.c
	src/math.c
	src/ext/screen.c
)

# We still need to rely on the rest of the DCPU-Toolchain
# suite being having built at this point.
dt_needs_exe(compiler)
dt_needs_exe(assembler)
dt_needs_exe(linker)

# Copy standard library include files.
file(COPY ./include DESTINATION "${ROOT_DIR}/Debug" USE_SOURCE_PERMISSIONS)
