///
/// @addtogroup LibDCPU-LD-Policy
/// @{
///
/// @file
/// @brief Provides typedefs for executing policy scripts.
/// @author James Rhodes
///

#ifndef __DCPU_LD_POLICY_EXEC_H
#define __DCPU_LD_POLICY_EXEC_H

///
/// @brief Function signature for when the policy executor calls offset().
///
/// When this is called back, the handler should offset all of the code in
/// the linker image such that the if the code was loaded into memory at
/// the specified position, it would execute correctly.
///
/// @param userdata The userdata associated with this state.
/// @param position The position the code should be offset for.
///
typedef void (*policy_call_offset_t)(void* userdata, int position);

///
/// @brief Function signature for when the policy executor calls write().
///
/// When this is called back, the handler should output the specified words
/// to the output file without modification.
///
/// @param userdata The userdata associated with this state.
/// @param words The list of uint16_t words to be written.
///
typedef void (*policy_call_write_t)(void* userdata, list_t words);

///
/// @brief Function signature for when the policy executor calls total(table).
///
/// When this is called back, the handler should return the total number of
/// words in the specified table.
///
/// @param userdata The userdata associated with this state.
/// @param table The table identified by TABLE_ADJUSTMENT, TABLE_REQUIRED, etc.
/// @return The total number of entries in the table.
///
typedef int (*policy_call_total_table_t)(void* userdata, int table);

///
/// @brief Function signature for when the policy executor calls total(section).
///
/// When this is called back, the handler should return the total number of
/// words in the specified section.  If the section does not exist, this function
/// should return 0.
///
/// @note If the section has been previously outputted with .OUTPUT, it will
///       not exist when this function is called (it will have already been
///       flattened into the output).
///
/// @param userdata The userdata associated with this state.
/// @param section The name of the section to calculate the total number of words for.
/// @return The total number of entries in the section.
///
typedef int (*policy_call_total_section_t)(void* userdata, bstring section);

///
/// @brief Function signature for when the policy executor calls field().
///
/// When this is called back, the handler should retrieve the words (uint16_t)
/// associated with the specified field data.  The label_text field should not
/// be packed together; each character should be an individual word.
///
/// @param userdata The userdata associated with this state.
/// @param table The table to retrieve the field data from, identified by TABLE_ADJUSTMENT, etc.
/// @param index The index in the table to retrieve field data from.
/// @param field The field identified by FIELD_LABEL_SIZE, etc.
/// @return A list of uint16_t words associated with the field data.
///
typedef list_t (*policy_call_field_t)(void* userdata, int table, int index, int field);

///
/// @brief Function signature for when the policy executor calls code().
///
/// When this is called back, the handler should return all of the words
/// for what would be outputted (i.e. the output linker bin).
///
/// @param userdata The userdata associated with this state.
/// @return A list of uint16_t words that are the code to be outputted.
///
typedef list_t (*policy_call_code_t)(void* userdata);

///
/// @brief Function signature for when the policy executor calls section().
///
/// When this is called back, the handler should return all of the words
/// that are stored in the specified section bin.  If the section bin does
/// not exist, it should return a list with 0 entries.
///
/// @note If the section has been previously outputted with .OUTPUT, it will
///       not exist when this function is called (it will have already been
///       flattened into the output).
///
/// @param userdata The userdata associated with this state.
/// @param section The name of the section to fetch words from.
/// @return The words stored in the section.
///
typedef list_t (*policy_call_section_t)(void* userdata, bstring name);

#endif

///
/// @}
///