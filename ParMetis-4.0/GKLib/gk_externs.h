/*!
\file gk_externs.h
\brief This file contains definitions of external variables created by GKlib

\date   Started 3/27/2007
\author George
\version\verbatim $Id: gk_externs.h,v 1.3 2015/07/20 20:36:49 lush Exp $ \endverbatim
*/

#ifndef _GK_EXTERNS_H_
#define _GK_EXTERNS_H_


/*************************************************************************
* Extern variable definition. Hopefully, the __thread makes them thread-safe.
**************************************************************************/
#ifndef _GK_ERROR_C_
/* declared in error.c */
extern __thread int gk_cur_jbufs;
extern __thread jmp_buf gk_jbufs[];
extern __thread jmp_buf gk_jbuf;

#endif

#endif
