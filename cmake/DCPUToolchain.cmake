# Check to ensure that the appropriate tool path
# has been defined.
if(NOT DEFINED DCPUTOOLCHAIN)
  # The developer must define the path to the DCPUTOOLCHAIN
  # binaries.
  message(FATAL_ERROR "You must define DCPUTOOLCHAIN as the path to toolchain binaries.")
else(NOT DEFINED DCPUTOOLCHAIN)
  # Reconfigure the variable to point to the correct location.
  set(DCPUTOOLCHAIN "${CMAKE_CURRENT_LIST_DIR}/${DCPUTOOLCHAIN}")
endif(NOT DEFINED DCPUTOOLCHAIN)

# Reset flag options
set(DTCCFLAGS "")
set(DTASMFLAGS "-i")

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
	separate_arguments(listargs UNIX_COMMAND "${toolargs}")
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
					message("          command: ${DCPUTOOLCHAIN}/${tool}${CMAKE_EXECUTABLE_SUFFIX} ${toolargs} -o \"${fpth}/${fbse}.oa\" \"${i}\"")
					message("          command: ${DCPUTOOLCHAIN}/dtasm${CMAKE_EXECUTABLE_SUFFIX} -i -o \"${fpth}/${fbse}.o\" -s \"${fpth}/${fbse}.os\" \"${fpth}/${fbse}.oa\"")
					message("          depends: ${tool} dtasm ${i}")
				endif(DEFINED VERY_VERBOSE)
				add_custom_command(
					OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/${fpth}/${fbse}.oa
					COMMAND ${DCPUTOOLCHAIN}/${tool}${CMAKE_EXECUTABLE_SUFFIX}
						ARGS ${listargs} -o "${CMAKE_CURRENT_SOURCE_DIR}/${fpth}/${fbse}.oa" "${CMAKE_CURRENT_SOURCE_DIR}/${i}"
					DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/${i}"
					COMMENT "${verb} ${fbse}${fext} with DCPU-Toolchain...")
				separate_arguments(dtasmlist UNIX_COMMAND "${DTASMFLAGS}")
				add_custom_command(
					OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/${fpth}/${fbse}.o
					COMMAND ${DCPUTOOLCHAIN}/dtasm${CMAKE_EXECUTABLE_SUFFIX}
						ARGS ${dtasmlist} -o "${CMAKE_CURRENT_SOURCE_DIR}/${fpth}/${fbse}.o" -s "${CMAKE_CURRENT_SOURCE_DIR}/${fpth}/${fbse}.os" "${CMAKE_CURRENT_SOURCE_DIR}/${fpth}/${fbse}.oa"
					DEPENDS ${tool} "${CMAKE_CURRENT_SOURCE_DIR}/${i}" "${CMAKE_CURRENT_SOURCE_DIR}/${fpth}/${fbse}.oa"
					COMMENT "Assembling ${fpth}/${fbse}.oa with DCPU-Toolchain...")
				if("${tout}" STREQUAL "")
					set(tout "${CMAKE_CURRENT_SOURCE_DIR}/${fpth}/${fbse}.o")
				else("${tout}" STREQUAL "")
					list(APPEND tout "${CMAKE_CURRENT_SOURCE_DIR}/${fpth}/${fbse}.o")
				endif("${tout}" STREQUAL "")
				set(${outvar} "${${outvar}} ${tout}" PARENT_SCOPE)
			else(${runasm} STREQUAL "true")
				if(DEFINED VERY_VERBOSE)
					message("       .. adding command")
					message("          output: ${fpth}/${fbse}.o")
					message("          command: ${DCPUTOOLCHAIN}/${tool}${CMAKE_EXECUTABLE_SUFFIX} ${toolargs} -o \"${fpth}/${fbse}.o\" -s \"${fpth}/${fbse}.os\" \"${i}\"")
					message("          depends: ${tool}")
				endif(DEFINED VERY_VERBOSE)
				add_custom_command(
					OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/${fpth}/${fbse}.o
					COMMAND ${DCPUTOOLCHAIN}/${tool}${CMAKE_EXECUTABLE_SUFFIX}
						ARGS ${listargs} -o "${CMAKE_CURRENT_SOURCE_DIR}/${fpth}/${fbse}.o" -s "${CMAKE_CURRENT_SOURCE_DIR}/${fpth}/${fbse}.os" "${CMAKE_CURRENT_SOURCE_DIR}/${i}"
					DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/${i}"
					COMMENT "${verb} ${fbse}${fext} with DCPU-Toolchain...")
				if("${tout}" STREQUAL "")
					set(tout "${CMAKE_CURRENT_SOURCE_DIR}/${fpth}/${fbse}.o")
				else("${tout}" STREQUAL "")
					list(APPEND tout "${CMAKE_CURRENT_SOURCE_DIR}/${fpth}/${fbse}.o")
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
	string(STRIP "${DTASMFLAGS}" DTASMFLAGS)
	string(STRIP "${DTCCFLAGS}" DTCCFLAGS)
	string(STRIP "${DTLDFLAGS}" DTLDFLAGS)
	define_dcpu_tool(dtasm "-i -I \"${CMAKE_CURRENT_SOURCE_DIR}/src/include\"" "Assembling" "${ARGN}" "false" tasmoutputs ".dasm" ".dasm16")
	define_dcpu_tool(dtcc "${DTCCFLAGS}" "Compiling" "${ARGN}" "true" tcoutputs ".c")

	# Define the linker.
	string(STRIP "${tasmoutputs}" tasmoutputs)
	string(STRIP "${tcoutputs}" tcoutputs)
	separate_arguments(dtldlist UNIX_COMMAND "${DTLDFLAGS}")
	add_custom_command(
		OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${target}.dcpu16"
		COMMAND ${DCPUTOOLCHAIN}/dtld${CMAKE_EXECUTABLE_SUFFIX}
			ARGS ${dtldlist} -s "${CMAKE_CURRENT_BINARY_DIR}/${target}.dsym16" --symbol-extension="os" -o "${CMAKE_CURRENT_BINARY_DIR}/${target}.dcpu16" ${tasmoutputs} ${tcoutputs}
		DEPENDS ${tasmoutputs} ${tcoutputs}
		COMMENT "Linking ${target}.dcpu16 as image with DCPU-Toolchain...")

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
	list(APPEND params "${CMAKE_CURRENT_BINARY_DIR}/${target}.dcpu16")
	add_custom_target(${target} ALL ${params})
endfunction(add_dcpu_image target)

