/*******************************************************************************
 *
 * Copyright 2014-2015, Imagination Technologies Limited and/or its
 *                      affiliated group companies.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its
 * contributors may be used to endorse or promote products derived from this
 * software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************/

/******************************************************************************
*                 file : $RCSfile: memset.c,v $ 
*               author : $Author Imagination Technologies Ltd
*    date last revised : $
*      current version : $
******************************************************************************/

#include <string.h>
#include "low/_flavour.h"

void *
memset (void *m, int c, size_t n)
{
  char *s = (char *) m;

#ifndef __PREFER_SIZE_OVER_SPEED__
  int blocksize = sizeof (long) << 1;
  unsigned long buffer;
  unsigned long *aligned_addr;
  unsigned int d = c & 0xff;	/* To avoid sign extension, copy C to an
				   unsigned variable.  */
  if (n >= blocksize && !((long)s & (sizeof(long) - 1)))
    {
      /* If we get this far, we know that n is large and s is word-aligned. */
      aligned_addr = (unsigned long *) s;

      /* Store D into each char sized location in BUFFER so that
         we can set large blocks quickly.  */
      buffer = (d << 8) | d;
      buffer |= (buffer << 16);

      if (sizeof (long) == 8)
	buffer |= ((unsigned long long ) buffer << 32);

      /* Unroll the loop.  */
      while (n >= blocksize)
        {
          *aligned_addr++ = buffer;
          *aligned_addr++ = buffer;
          n -= blocksize;
        }

     /* Pick up the remainder with a bytewise loop.  */
      s = (char*)aligned_addr;
    }

#endif /* __PREFER_SIZE_OVER_SPEED__ */

  while (n)
  {
    *s++ = (char) c;
    --n;
  }

  return m;
}
