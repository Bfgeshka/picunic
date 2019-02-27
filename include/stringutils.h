#ifndef STRINGUTILS_H_GUARD_
#define STRINGUTILS_H_GUARD_

/* BSD 3-Clause License
 *
 * Copyright (c) 2018-2019, OMG Eto Zhe BFG
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * * Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above
 *   copyright notice, this list of conditions and the following disclaimer
 *   in the documentation and/or other materials provided with the
 *   distribution.
 *
 * * Neither the name of the  nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/* * About * *
 * c+h library for wrapping strings.
 * Suited for:
 * 	function arguments, no additional var for buffer or length;
 * 	replacing sprintf() and strcat();
 * 	read-once-use-ofter usage,no need for strlen();
 *
 * Heap allocated, call for free_string() when you have finished with it.
 * Beware, there is no reallocation inside stringset() and stringcat(),
 * take care of fitting your string yourself.
 * */

/* Macros */
#include <stdlib.h>

/* Typedef */
typedef struct string
{
	char * s;
	size_t length;
	size_t bytes;
} string;

/* Protos */
string * construct_string ( size_t size );
string * stringcopy ( string * src );
void newstring ( string * dst, size_t src );
void stringset ( string * dst, const char * fmt, ... );
void stringcat ( string * dst, const char * fmt, ... );
void free_string ( string * str );

#endif
