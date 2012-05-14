find_package(Sphinx)

# Skip documentation if Sphinx isn't installed
if(SPHINX_EXECUTABLE)
	if(NOT DEFINED SPHINX_THEME)
		set(SPHINX_THEME default)
	endif()
	
	if(NOT DEFINED SPHINX_THEME_DIR)
		set(SPHINX_THEME_DIR)
	endif()
	
	# configured documentation tools and intermediate build results
	set(BINARY_BUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}/_build")
	
	# Sphinx cache with pickled ReST documents
	set(SPHINX_CACHE_DIR "${CMAKE_CURRENT_BINARY_DIR}/_doctrees")
	
	# HTML output directory
	set(SPHINX_HTML_DIR "${CMAKE_CURRENT_BINARY_DIR}/html")
	
	configure_file(
		"${CMAKE_CURRENT_SOURCE_DIR}/conf.py.in"
		"${BINARY_BUILD_DIR}/conf.py"
		@ONLY)

	if(TOOLCHAIN_ON_CYGWIN)	
		add_custom_target(docs ALL
			${SPHINX_EXECUTABLE}
			-q -b html
			-c "_build"
			-d "_doctrees"
			"."
			"html"
			COMMENT "Building HTML documentation with Sphinx (in Cygwin, update shutil.py if errors)")
	else(TOOLCHAIN_ON_CYGWIN)
		add_custom_target(docs ALL
			${SPHINX_EXECUTABLE}
			-q -b html
			-c "${BINARY_BUILD_DIR}"
			-d "${SPHINX_CACHE_DIR}"
			"${CMAKE_CURRENT_SOURCE_DIR}"
			"${SPHINX_HTML_DIR}"
			COMMENT "Building HTML documentation with Sphinx")
	endif(TOOLCHAIN_ON_CYGWIN)
else(SPHINX_EXECUTABLE)
	message("Unable to find Sphinx, so documentation won't be created.")
endif(SPHINX_EXECUTABLE)
