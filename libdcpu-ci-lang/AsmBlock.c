/**

	File:		AsmBlock.c

	Project:	DCPU-16 Toolchain
	Component:	LibDCPU-CI-Lang

	Authors:	James Rhodes

	Description:	Defines the AsmBlock class.

**/

#include <assert.h>
#include <stdlib.h>
#include <bstring.h>
#include <class.h>
#include <dcpu.h>
#include "AsmBlock.h"

void AsmBlock_write_char(THIS(AsmBlock), char input)
{
	bconchar(this->m_Assembly, input);
}

void AsmBlock_write_const_size_t(THIS(AsmBlock), const size_t input)
{
	// TODO: Implement writing integers into bstring.
	assert(false /* write_const_size_t not implemented */);
}

void AsmBlock_write_const_char(THIS(AsmBlock), const char input[])
{
	bcatcstr(this->m_Assembly, &input[0]);
}

void AsmBlock_write_stream(THIS(AsmBlock), struct bStream* stream)
{
	while (bsreada(this->m_Assembly, stream, 1) != BSTR_ERR) ;
}

void AsmBlock_write_string(THIS(AsmBlock), bstring input)
{
	bconcat(this->m_Assembly, input);
}

void AsmBlock_write_asmblock(THIS(AsmBlock), AsmBlock* block)
{
	bconcat(this->m_Assembly, block->m_Assembly);
}

AsmBlock* new_AsmBlock()
{
	CLASS_INIT(AsmBlock);
	this->m_Assembly = bfromcstr("");
	this->write_char = &AsmBlock_write_char;
	this->write_const_size_t = &AsmBlock_write_const_size_t;
	this->write_const_char = &AsmBlock_write_const_char;
	this->write_stream = &AsmBlock_write_stream;
	this->write_string = &AsmBlock_write_string;
	this->write_asmblock = &AsmBlock_write_asmblock;
	return this;
}

void delete_AsmBlock(THIS(AsmBlock))
{
	bdestroy(this->m_Assembly);
	free(this);
}
