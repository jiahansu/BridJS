/*

 Package: dyncall
 Library: dyncallback
 File: dyncallback/dyncall_callback_mips.h
 Description: Callback - Header for MIPS
 License:

   Copyright (c) 2013-2015 Daniel Adler <dadler@uni-goettingen.de>,
                           Tassilo Philipp <tphilipp@potion-studios.com>

   Permission to use, copy, modify, and distribute this software for any
   purpose with or without fee is hereby granted, provided that the above
   copyright notice and this permission notice appear in all copies.

   THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
   WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
   MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
   ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
   WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
   ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
   OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

*/


#ifndef DYNCALL_CALLBACK_MIPS_H
#define DYNCALL_CALLBACK_MIPS_H

#include "dyncall_callback.h"

#include "dyncall_thunk.h"
#include "dyncall_args_mips.h"

struct DCCallback
{
  DCThunk            thunk;         /* offset  0, size 20 */
  DCCallbackHandler* handler;       /* offset 20, size  4 */
  void*              userdata;      /* offset 24, size  4 */
};

#endif /* DYNCALL_CALLBACK_MIPS_H */

