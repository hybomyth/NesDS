#ifndef NESTYPES_H__
#define NESTYPES_H__
/*
#if defined(_MSC_VER)
#define NEVER_REACH __assume(0);
#elif defined(__BORLANDC__)
#define __fastcall __msfastcall
#elif defined(__GNUC__)
#define __inline		__inline__

	#if !defined(__fastcall)
	#define __fastcall
	#endif

#else
#define __inline
#define __fastcall
#endif
*/
#ifndef NEVER_REACH
#define NEVER_REACH
#endif

typedef int				Int;
typedef unsigned int	Uint;
typedef signed int		Int32;
typedef unsigned int	Uint32;
typedef signed short	Int16;
typedef unsigned short	Uint16;
typedef signed char		Int8;
typedef unsigned char	Uint8;
typedef char			Char;

#define XMEMSET(d,c,n)	memset(d,c,n)

#endif /* NESTYPES_H__ */
