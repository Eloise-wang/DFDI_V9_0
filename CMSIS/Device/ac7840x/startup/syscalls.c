/* Copyright Statement:
 *
 * This software/firmware and related documentation ("AutoChips Software") are
 * protected under relevant copyright laws. The information contained herein is
 * confidential and proprietary to AutoChips Inc. and/or its licensors. Without
 * the prior written permission of AutoChips inc. and/or its licensors, any
 * reproduction, modification, use or disclosure of AutoChips Software, and
 * information contained herein, in whole or in part, shall be strictly
 * prohibited.
 *
 * AutoChips Inc. (C) 2020. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("AUTOCHIPS SOFTWARE")
 * RECEIVED FROM AUTOCHIPS AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER
 * ON AN "AS-IS" BASIS ONLY. AUTOCHIPS EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR
 * NONINFRINGEMENT. NEITHER DOES AUTOCHIPS PROVIDE ANY WARRANTY WHATSOEVER WITH
 * RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
 * INCORPORATED IN, OR SUPPLIED WITH THE AUTOCHIPS SOFTWARE, AND RECEIVER AGREES
 * TO LOOK ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO.
 * RECEIVER EXPRESSLY ACKNOWLEDGES THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO
 * OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES CONTAINED IN AUTOCHIPS
 * SOFTWARE. AUTOCHIPS SHALL ALSO NOT BE RESPONSIBLE FOR ANY AUTOCHIPS SOFTWARE
 * RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND AUTOCHIPS'S
 * ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE AUTOCHIPS SOFTWARE
 * RELEASED HEREUNDER WILL BE, AT AUTOCHIPS'S OPTION, TO REVISE OR REPLACE THE
 * AUTOCHIPS SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE
 * CHARGE PAID BY RECEIVER TO AUTOCHIPS FOR SUCH AUTOCHIPS SOFTWARE AT ISSUE.
 */
/******************************************************************************
 * @version: V1.1.2
 ******************************************************************************/

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
extern int fgetc(FILE* f);
extern int fputc(int ch, FILE *f);

/***************************************************************************/

int _read_r(struct _reent *r, int file, char *ptr, int len) {
  char c;
  int i;
  for (i = 0; i < len; i++) {
    c = fgetc((FILE *)&file);
    if (c == 0x0D)
      break;
    *ptr++ = c;
    fputc(c, (FILE *)&file);
  }
  return len - i;
}

/***************************************************************************/

int _lseek_r(struct _reent *r, int file, int ptr, int dir) {
  r = r;
  file = file;
  ptr = ptr;
  dir = dir;

  return 0;
}

/***************************************************************************/

int _write_r(struct _reent *r, int file, char *ptr, int len) {
  r = r;
  file = file;
  ptr = ptr;

#if 1
  int index;
  /* For example, output string by UART or ITM*/
  for (index = 0; index < len; index++) {
    if (ptr[index] == '\n') {
    	fputc('\r', (FILE *)&file);
    }

    fputc(ptr[index], (FILE *)&file);
  }
#endif

  return len;
}

int _close_r(struct _reent *r, int file) { return 0; }

/***************************************************************************/

 /* Register name faking - works in collusion with the linker.  */
 register char *stack_ptr asm("sp");

 caddr_t _sbrk_r(struct _reent *r, int incr)
 {
     extern char end asm("end"); /* Defined by the linker.  */
     static char *heap_end;
     char *prev_heap_end;

     if (heap_end == NULL)
         heap_end = &end;

     prev_heap_end = heap_end;

     if (heap_end + incr > stack_ptr)
     {
         /* Some of the libstdc++-v3 tests rely upon detecting
         out of memory errors, so do not abort here.  */
 #if 0
       extern void abort (void);

       _write (1, "_sbrk: Heap and stack collision\n", 32);

       abort ();
 #else
         errno = ENOMEM;
         return (caddr_t)-1;
 #endif
    }

     heap_end += incr;

     return (caddr_t)prev_heap_end;
 }

 /***************************************************************************/

 int _fstat_r(struct _reent *r, int file, struct stat *st)
 {
     r = r;
     file = file;

     memset(st, 0, sizeof(*st));
    st->st_mode = S_IFCHR;
     return 0;
 }

 /***************************************************************************/

 int _isatty_r(struct _reent *r, int fd)
 {
     r = r;
    fd = fd;

    return 1;
 }

/*** EOF ***/
