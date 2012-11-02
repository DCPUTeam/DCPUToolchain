
ERR_GENERIC                                          generic                                "generic assembling error."
ERR_LABEL_NOT_FOUND                                  label-not-found                        "label '%s' not found."
ERR_OUTPUT_NULL                                      output-null                            "attempted to output NULL operation."
ERR_BRACKETED_REGISTER_UNSUPPORTED                   bracketed-register-unsupported         "can not use register '%s' in brackets."
ERR_NEXTED_REGISTER_UNSUPPORTED                      nexted-register-unsupported            "can not use register '%s' as next value."
ERR_RELOCATION_TABLE_TOO_LARGE                       relocation-table-too-large             "relocation table too large."
ERR_DAT_UNSUPPORTED_PARAMETER                        unsupported-parameter-dat              "unsupported parameter passed to DAT instruction."
ERR_GEN_UNSUPPORTED_PARAMETER                        unsupported-parameter-non-dat          "unsupported parameter passed to non-DAT instruction."
ERR_UNSUPPORTED_KEYWORD                              unsupported-keyword                    "unsupported keyword encountered."
ERR_EXTENSION_TABLE_TOO_LARGE                        extension-table-too-large              "extension table too large."
ERR_EXTENSION_UNKNOWN                                extension-unknown                      "extension %s is not known to the assembler."
ERR_NEXTED_LABEL_UNSUPPORTED                         nexted-label-unsupported               "can not use label '%s' as next value."
ERR_UNKNOWN_OPCODE                                   unrecognized-opcode                    "unrecognized opcode '%s' encountered."
ERR_UNABLE_TO_INCBIN                                 incbin-failure                         "unable to include binary file '%s' inline."
ERR_NOT_GENERATING_INTERMEDIATE_CODE                 not-generating-intermediate-code       "the .EXPORT and .IMPORT directives can not be used unless generating intermediate code."
ERR_OPERATION_NOT_DEFINED_FOR_LABEL_RESOLUTION       label-resolution-operation-undefined   "operation not defined for label resolution."
ERR_LABEL_RESOLUTION_NOT_PERMITTED                   label-resolution-not-permitted         "unable to resolve '%s' as label resolution is not permitted at this time."
ERR_EXPRESSION_NOT_PERMITTED                         expression-not-permitted               "the imported label '%s' may not be used as a component of an expression."
ERR_EXPRESSION_DIVIDE_BY_ZERO                        expression-divide-by-zero              "expression '%s' evaluates to zero while being used as a divisor."
ERR_OUTPUT_BEFORE_SECTION                            output-before-section                  ".OUTPUT is not permitted prior to .SECTION."
ERR_INVALID_PARAMETER_COUNT                          instruction-invalid-parameter-count    "invalid parameter count for instruction."
ERR_KEYWORD_PARAMETER_MISSING                        keyword-parameter-missing              "required keyword parameter for '%s' is missing."
ERR_ONLY_IMPORT_OPTIONAL                             only-import-optional                   "only the .IMPORT directive may be followed by OPTIONAL."

WARN_EXPRESSION_NOT_ADJUSTED                         expression-not-adjusted                "expressions will not be adjusted at link or relocation time. ensure labels are not used as part of expressions."
WARN_USE_RELOCATION_NOT_ORIGIN                       origin-usage                           ".ORIGIN should not be used for relocation as it is not portable between kernels."
WARN_RELATIVE_PC_ADDRESSING                          relative-jump                          "relative adjustments to PC (such as SUB PC, <n>) are not safe due to literal shortening."

ERR_LABEL_AMBIGIOUS_PROVIDED_JUMP                    label-ambigious                        "label '%s' found in both exported and jump tables."
ERR_CAN_NOT_OPEN_FILE                                file-not-open                          "unable to open '%s'."
ERR_CAN_NOT_WRITE_FILE                               file-not-writable                      "unable to write to '%s'."
ERR_LUA                                              lua                                    "lua error was '%s'."
ERR_OBJECT_VERSION_MISMATCH                          object-version-mismatch                "the object file '%s' was written with a different version of the toolchain."
ERR_OPTIMIZER_LOAD_FAILED                            optimizer-load-failed                  "failed to load optimizer module from '%s'."
ERR_INVALID_TARGET_NAME                              invalid-target-name                    "invalid target type, must be 'image', 'static' or 'kernel'."
ERR_BIN_LOAD_FAILED                                  bin-load-failed                        "failed to load bins from the object file '%s'."
ERR_KERNEL_ARGUMENT_NOT_ALLOWED                      kernel-argument-not-allowed            "the -k argument can not be used when building a kernel."
ERR_JUMPLIST_FILE_REQUIRED                           jump-list-required                     "a jumplist is required when linking against a kernel."
ERR_COULD_NOT_CREATE_OPENGL_CONTEXT                  failed-create-opengl-context           "failed to create an OpenGL context (check your graphic drivers).\n"

WARN_SKIPPING_SHORT_LITERALS_TYPE                    short-literals-skipped-type            "skipping short literal compression due to target type."
WARN_SKIPPING_SHORT_LITERALS_REQUEST                 short-literals-skipped-request         "skipping short literal compression on request."
WARN_KERNEL_NOT_PROVIDING_JUMP_LIST                  jump-list-missing                      "kernel did not specify a position to output a jump list with .JUMP."
WARN_KERNEL_JUMP_LIST_NOT_EXPORTED                   jump-list-not-exported                 "the jump list was not exported along with the kernel."

ERR_PP_ASM_LINE_PARAMETERS_INCORRECT                 pp-asm-line-parameters-incorrect       "incorrect parameters for .LINE near %s."
ERR_PP_ASM_ULINE_PARAMETERS_INCORRECT                pp-asm-uline-parameters-incorrect      "incorrect parameters for .ULINE near %s."
ERR_PP_ASM_DEFINE_PARAMETERS_INCORRECT               pp-asm-define-parameters-incorrect     "incorrect parameters for .DEFINE near %s."
ERR_PP_ASM_INCLUDE_PARAMETERS_INCORRECT              pp-asm-include-parameters-incorrect    "incorrect parameters for .INCLUDE near %s."
ERR_PP_ASM_MACRO_PARAMETERS_INCORRECT                pp-asm-macro-parameters-incorrect      "incorrect parameters for .MACRO near %s."
ERR_PP_ASM_IF_PARAMETERS_INCORRECT                   pp-asm-if-parameters-incorrect         "incorrect parameters for .IF near %s."
ERR_PP_ASM_INCLUDE_FILE_NOT_FOUND                    pp-asm-include-file-not-found          "file not found for .INCLUDE parameter near %s."
ERR_PP_ASM_MACRO_TERMINATION_NOT_FOUND               pp-asm-macro-termination-not-found     ".ENDMACRO directive not found before end of file near %s."
ERR_PP_ASM_MACRO_PARAMETERS_EXPECTED                 pp-asm-macro-parameters-expected       "expected parameters after macro name for .MACRO near %s."
ERR_PP_C_LINE_PARAMETERS_INCORRECT                   pp-c-line-parameters-incorrect         "incorrect parameters for #line near %s."
ERR_PP_C_DEFINE_PARAMETERS_INCORRECT                 pp-c-define-parameters-incorrect       "incorrect parameters for #define near %s."
ERR_PP_C_INCLUDE_PARAMETERS_INCORRECT                pp-c-include-parameters-incorrect      "incorrect parameters for #include near %s."
ERR_PP_C_IF_PARAMETERS_INCORRECT                     pp-c-if-parameters-incorrect           "incorrect parameters for #if near %s."
ERR_PP_C_IFDEF_PARAMETERS_INCORRECT                  pp-c-ifdef-parameters-incorrect        "incorrect parameters for #ifdef near %s."
ERR_PP_C_IFNDEF_PARAMETERS_INCORRECT                 pp-c-ifndef-parameters-incorrect       "incorrect parameters for #ifndef near %s."
ERR_PP_C_INCLUDE_FILE_NOT_FOUND                      pp-c-include-file-not-found            "file not found for #include parameter near %s."

ERR_PP_DEFINE_NOT_FOUND                              pp-define-not-found                    "definition referenced in expression not found near %s."
ERR_PP_DEFINE_NOT_EXPRESSION                         pp-define-not-expression               "definition referenced in expression not well formed expression near %s."
ERR_PP_MACRO_MALFORMED                               pp-macro-malformed                     "macro definition malformed near %s."
ERR_PP_EXPECTED_OPEN_BRACKET                         pp-expected-open-bracket               "expected open bracket in usage of macro near %s."
ERR_PP_NO_TERMINATING_BRACKET                        pp-no-terminating-bracket              "terminating bracket not found for macro usage near %s."
ERR_PP_TOO_MANY_PARAMETERS                           pp-too-many-parameters                 "too many parameters for macro usage near %s."
ERR_PP_NOT_ENOUGH_PARAMETERS                         pp-not-enough-parameters               "not enough parameters for macro usage near %s."
ERR_PP_LONG_FORM_MACRO_ONLY                          pp-long-form-macro-only                "short form macro not permitted in this language near %s."
ERR_PP_COMPAT_OPEN_INPUT_FAILED                      pp-compat-open-input-failed            "opening the input file failed in the preprocessor."
ERR_PP_COMPAT_OPEN_TEMP_FAILED                       pp-compat-open-temp-failed             "opening temporary output file failed in the preprocessor."
