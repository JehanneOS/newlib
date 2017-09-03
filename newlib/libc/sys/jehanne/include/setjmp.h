/*
 * This file is part of Jehanne.
 *
 * Copyright (C) 2017 Giacomo Tesio <giacomo@tesio.it>
 *
 * This is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, version 3 of the License.
 *
 * Jehanne is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with Jehanne.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _SETJMP_H_
#ifdef __cplusplus
extern "C" {
#endif
#define _SETJMP_H_

typedef	uintptr_t jmp_buf[8];	/* we use newlib's x86_64 functions */

#include <signal.h> /* for sigset_t and sigprocmask */

typedef struct __sigjmpbuf
{
  jmp_buf __buf;
  int __is_mask_saved;
  sigset_t __saved_mask;
} sigjmp_buf;

extern void longjmp(jmp_buf env, int val) __attribute__ ((__noreturn__));
extern int setjmp(jmp_buf env);
extern void siglongjmp(sigjmp_buf env, int val) __attribute__ ((__noreturn__));
extern int sigsetjmp(sigjmp_buf env, int savemask);

extern void _longjmp (jmp_buf, int) __attribute__ ((__noreturn__));
extern int _setjmp (jmp_buf);

/* sigsetjmp is implemented as macro using setjmp */

#define sigsetjmp(__jmpb, __savemask) \
                 ( __jmpb.__is_mask_saved = __savemask && \
                   (sigprocmask (SIG_BLOCK, NULL, &__jmpb.__saved_mask) == 0), \
                    setjmp (__jmpb.__buf) )

#define _longjmp(e, v) longjmp(e, v)
#define _setjmp(e) setjmp(e)

#ifdef __cplusplus
}
#endif
#endif /* _SETJMP_H_ */

