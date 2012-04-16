SET(ASSEMBLER_SRC aerr.c aout.c assem.c imap.c lexer.c main.c parser.c textn.c
    treloc.c pp.c)

add_executable(assembler ${ASSEMBLER_SRC})
target_link_libraries(assembler argtable2)
