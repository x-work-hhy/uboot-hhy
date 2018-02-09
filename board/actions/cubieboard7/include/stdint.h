#ifndef _MACHINE__TYPES_H_
#define _MACHINE__TYPES_H_

#define __WORDSIZE  64

/* Types for signed  */
typedef signed char		int8_t;
typedef short			int16_t;
typedef int			int32_t;
#if __WORDSIZE == 64
typedef long			int64_t;
#else
typedef long long 		int64_t;
#endif

/* Types for unsigned  */
typedef unsigned char		uint8_t;
typedef unsigned short		uint16_t;
typedef unsigned int		uint32_t;
#if __WORDSIZE == 64
typedef unsigned long		uint64_t;
#else
typedef unsigned long long	uint64_t;
#endif

/* Types for `void *' pointers.  */
#if __WORDSIZE == 64
typedef long			intptr_t;
typedef unsigned long		uintptr_t;
#else
typedef int			intptr_t;
typedef unsigned int		uintptr_t;
#endif

/*
 * Unusual type definitions.
 */
#ifdef __GNUCLIKE_BUILTIN_VARARGS
typedef __builtin_va_list	__va_list;	/* internally known to gcc */
#else
typedef	char *			__va_list;
#endif /* __GNUCLIKE_BUILTIN_VARARGS */
#if defined(__GNUCLIKE_BUILTIN_VAALIST) && !defined(__GNUC_VA_LIST) \
    && !defined(__NO_GNUC_VA_LIST)
#define __GNUC_VA_LIST
typedef __va_list		__gnuc_va_list;	/* compatibility w/GNU headers*/
#endif



#endif	/* __STDLIB_STDINT_H__ */
