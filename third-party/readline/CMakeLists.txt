cmake_minimum_required(VERSION 2.8.5)

if(MSVC)
	set(readline_LIBRARY "${toolchain_SOURCE_DIR}/third-party/readline/readline.lib
	${toolchain_SOURCE_DIR}/third-party/readline/history.lib" PARENT_SCOPE)
	set(readline_SOURCE_DIR ${toolchain_SOURCE_DIR}/third-party/readline/include PARENT_SCOPE)
else()
	find_package(Readline REQUIRED)
	set(readline_LIBRARY ${Readline_LIBRARY} PARENT_SCOPE)
	set(readline_SOURCE_DIR ${Readline_INCLUDE_DIR} PARENT_SCOPE)
endif()
