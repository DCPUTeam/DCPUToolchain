find_path(Readline_INCLUDE_DIR readline/readline.h)
find_library(Readline_LIBRARY NAMES readline)

if (Readline_INCLUDE_DIR AND Readline_LIBRARY)
  set (Readline_FOUND TRUE)
endif ()

if (Readline_INCLUDE_DIR AND EXISTS "${Readline_INCLUDE_DIR}/readline/readline.h")
  file(STRINGS "${Readline_INCLUDE_DIR}/readline/readline.h"
               Readline_MAJOR_VERSION
       REGEX "^#define RL_VERSION_MAJOR.*[0-9]+$")
  file(STRINGS "${Readline_INCLUDE_DIR}/readline/readline.h"
               Readline_MINOR_VERSION
       REGEX "^#define RL_VERSION_MINOR.*[0-9]+$")

  string(REGEX REPLACE "^.*RL_VERSION_MAJOR.*([0-9]+).*$"
                       "\\1"
                       Readline_MAJOR_VERSION
		       "${Readline_MAJOR_VERSION}")
  string(REGEX REPLACE "^.*RL_VERSION_MINOR.*([0-9]+).*$"
                       "\\1"
                       Readline_MINOR_VERSION
		       "${Readline_MINOR_VERSION}")

  set(Readline_VERSION "${Readline_MAJOR_VERSION}.${Readline_MINOR_VERSION}")
endif ()


if (Readline_FOUND)
  if (Readline_FIND_VERSION VERSION_GREATER Readline_VERSION)
    message(FATAL_ERROR "Found version ${Readline_VERSION} of GNU Readline at ${Readline_LIBRARY}, but version ${Readline_FIND_VERSION} required")
  endif ()
  if (NOT Readline_FIND_QUIETLY)
    message(STATUS "Found GNU Readline: ${Readline_VERSION}")
  endif ()
else ()
  if (Readline_FIND_REQUIRED)
    message(FATAL_ERROR "Could not find GNU Readline")
  endif ()
endif ()
