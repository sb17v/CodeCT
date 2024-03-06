/* -*- C -*- 
   Please see COPYRIGHT AND LICENSE information at the end of this file.

   ----- 

   codecti.h
*/

#ifndef _CODECTI_H
#define _CODECTI_H

#include <assert.h>
#include <malloc.h>
#include <math.h>
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>

#include "codect-config.h"

#ifdef ENABLE_BFD
#include "bfd.h"
#endif

#include "codect-hash.h"
#include "codect-callsites.h"

#define CODECTI_HOSTNAME_LEN_MAX 128

typedef char mpiPi_hostname_t[CODECTI_HOSTNAME_LEN_MAX];

#ifdef ENABLE_BFD
typedef struct SO_INFO
{
  void *lvma;
  void *uvma;
  size_t offset;
  char *fpath;
  bfd *bfd;
} so_info_t;
#endif

typedef struct _codecti_t
{
  char *toolname;
  char *appName;
  char *appFullName;
  FILE *stdout_;
  FILE *stderr_;

  int reportStackDepth;
  int internalStackDepth;
  int fullStackDepth;
  int baseNames;
  int do_lookup;
  int inAPIrtb;
  long text_start;
  int obj_mode;
  int tableSize;
  int use_pc_cache;
#ifdef SO_LOOKUP
  so_info_t **so_info;
  int so_count;
#endif
} codecti_t;

extern codecti_t codecti;

extern int codecti_vmajor;
extern int codecti_vminor;
extern int codecti_vpatch;
extern char *codecti_vdate;
extern char *codecti_vtime;

extern int codect_debug;

extern h_t *callsite_pc_cache;
extern h_t *callsite_src_id_cache;

/* CodeCT internal functions */
extern void codecti_init (char **argv);
extern void codecti_record_callsite (struct callsite_stats **p);
extern void codecti_resolve_callsite (struct callsite_stats *p);
extern void codecti_print_callsite (struct callsite_stats *p);
extern void codecti_free_callsite (struct callsite_stats *p);
extern int  codecti_ht_insert_callsite (struct callsite_stats *p);
extern void codecti_serialize_callsite (struct callsite_stats *p, void **start_p, size_t *len);
extern void codecti_deserialize_callsite (void *start_p, size_t len, struct callsite_stats **p);
extern void codecti_fini ();


#if !defined(UNICOS_mp)

/*  AIX  */
#if defined(AIX)

#if defined(__64BIT__)		/*  64-bit AIX  */
#define ParentFP(jb) ((void *) *(long *) jb[5])
#else /*  32-bit AIX  */
#define ParentFP(jb) ((void *) *(long *) jb[3])
#endif

/*  For both 32-bit and 64-bit AIX  */
#define FramePC(fp) ((void *) *(long *) (((long) fp) + (2 * sizeof (void *))))
#define NextFP(fp) ((void *) *(long *) fp)

/*  IA32 Linux  */
#elif defined(Linux) && defined(IA32)
#define ParentFP(jb) ((void*) jb[0].__jmpbuf[3])
#define FramePC(fp) ((void*)(((void**)fp)[1]))
#define NextFP(fp) ((void*)((void**)fp)[0])

/*  X86_64 Linux  */
#elif defined(Linux) && defined(X86_64)
#define ParentFP(jb) ((void*) jb[0].__jmpbuf[1])
#define FramePC(fp) ((void*)(((void**)fp)[1]))
#define NextFP(fp) ((void*)((void**)fp)[0])

/*  BG/L  */
#elif defined(ppc64)
#define ParentFP(jb) NextFP(((void*)jb[0].__jmpbuf[0]))
#define FramePC(fp) ((void*)(((char**)fp)[1]))
#define NextFP(fp) ((void*)((char**)fp)[0])

/* Catamount */
#elif defined(Catamount) && defined(X86_64)
#define ParentFP(jb) ((void*) jb[0].__jmpbuf[1])
#define FramePC(fp) ((void*)(((void**)fp)[1]))
#define NextFP(fp) ((void*)((void**)fp)[0])

/* MIPS+GCC */
/* We don't use the macros on the mips and instead
 * use gcc's __builtin_return_address(0) intrinsic
 * to get a single level of stack backtrace. The
 * value is saved in saved_ret_addr variable each time
 * setjmp is called.
 */
#elif defined(mips) && defined(__GNUC__)
#define ParentFP(jb) (0)
#define FramePC(fp) (0)
#define NextFP(fp) (0)
extern void *saved_ret_addr;


/*  undefined  */
#else
#if ! (defined(HAVE_LIBUNWIND) || defined(USE_BACKTRACE))

#warning "No stack trace mechanism defined for this platform."
#endif
#define ParentFP(jb) (0)
#define FramePC(fp) (0)
#define NextFP(fp) (0)
#endif

#else /* defined(UNICOS_mp) */

#include <intrinsics.h>
#include <stdint.h>

/*
 * Cray X1
 * Stacks grow downward in the address space.
 */


/*
 * Unlike some of the other platforms, we do not use setjmp to obtain
 * the frame pointer of the current function.  Instead, we use the
 * Cray intrinsic function _read_fp() to get the frame pointer of the 
 * current function.
 */
#define GetFP()    ((void*)(_read_fp()))


/* 
 * Given a frame pointer for a callee, the caller's frame pointer is at the 
 * callee's frame pointer address.
 */
#define NextFP(fp)      ((void*)((void**)fp)[0])


/*
 * Given a frame pointer for a callee, the return address to the caller
 * is two elements in the callee's frame.

 * TODO try masking off the high 32-bits
 */
#define FramePC(fp)     ((void*)(((uint64_t)(((void**)fp)[-2])) & 0xFFFFFFFF))


#endif /* defined(UNICOS_mp) */


#define min(x,y) ((x<y)?(x):(y))
#define max(x,y) ((x>y)?(x):(y))

#include "codect_proto.h" // TODO: We need a prototype file with all the required APIs

#endif

/*
  
  <license>
  
  Copyright (c) 2006, The Regents of the University of California. 
  Produced at the Lawrence Livermore National Laboratory 
  Written by Jeffery Vetter and Christopher Chambreau. 
  UCRL-CODE-223450. 
  All rights reserved. 
   
  This file is part of mpiP.  For details, see http://llnl.github.io/mpiP. 
   
  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are
  met:
   
  * Redistributions of source code must retain the above copyright
  notice, this list of conditions and the disclaimer below.
  
  * Redistributions in binary form must reproduce the above copyright
  notice, this list of conditions and the disclaimer (as noted below) in
  the documentation and/or other materials provided with the
  distribution.
  
  * Neither the name of the UC/LLNL nor the names of its contributors
  may be used to endorse or promote products derived from this software
  without specific prior written permission.
  
  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
  A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OF
  THE UNIVERSITY OF CALIFORNIA, THE U.S. DEPARTMENT OF ENERGY OR
  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
  PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
   
   
  Additional BSD Notice 
   
  1. This notice is required to be provided under our contract with the
  U.S. Department of Energy (DOE).  This work was produced at the
  University of California, Lawrence Livermore National Laboratory under
  Contract No. W-7405-ENG-48 with the DOE.
   
  2. Neither the United States Government nor the University of
  California nor any of their employees, makes any warranty, express or
  implied, or assumes any liability or responsibility for the accuracy,
  completeness, or usefulness of any information, apparatus, product, or
  process disclosed, or represents that its use would not infringe
  privately-owned rights.
   
  3.  Also, reference herein to any specific commercial products,
  process, or services by trade name, trademark, manufacturer or
  otherwise does not necessarily constitute or imply its endorsement,
  recommendation, or favoring by the United States Government or the
  University of California.  The views and opinions of authors expressed
  herein do not necessarily state or reflect those of the United States
  Government or the University of California, and shall not be used for
  advertising or product endorsement purposes.
  
  </license>
  
*/

/* EOF */
