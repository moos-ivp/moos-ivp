#ifndef __BYTE_ORDER_H__
#define __BYTE_ORDER_H__

#ifdef __cplusplus
extern "C"
{
#endif

#define __byteswap16(x) \
	( ((0xFF00 & ((unsigned short)x)) >> 8) | \
	  ((0x00FF & ((unsigned short)x)) << 8) )

#define __byteswap32(x) \
	( ((0xFF000000UL & ((unsigned long)x)) >> 24) | \
	  ((0x00FF0000UL & ((unsigned long)x)) >> 8) | \
	  ((0x0000FF00UL & ((unsigned long)x)) << 8) | \
	  ((0x000000FFUL & ((unsigned long)x)) << 24) )

#define __byteswap64(x) \
	( ((0xFF00000000000000ULL & ((unsigned long long)x)) >> 56) | \
	  ((0x00FF000000000000ULL & ((unsigned long long)x)) >> 40) | \
	  ((0x0000FF0000000000ULL & ((unsigned long long)x)) >> 24) | \
	  ((0x000000FF00000000ULL & ((unsigned long long)x)) >> 8)  | \
	  ((0x00000000FF000000ULL & ((unsigned long long)x)) << 8)  | \
	  ((0x0000000000FF0000ULL & ((unsigned long long)x)) << 24) | \
	  ((0x000000000000FF00ULL & ((unsigned long long)x)) << 40) | \
	  ((0x00000000000000FFULL & ((unsigned long long)x)) << 56) )

#define __ip_byteswap16(x) \
	do { \
		char c; \
		c = ((char *)x)[0]; \
		((char *)x)[0] = ((char *)x)[1]; \
		((char *)x)[1] = c; \
	} while(0)

#define __ip_byteswap32(x) \
	do { \
		char c[4]; \
		c[0] = ((char *)x)[3]; \
		c[1] = ((char *)x)[2]; \
		c[2] = ((char *)x)[1]; \
		c[3] = ((char *)x)[0]; \
		memcpy(((char *)x), c, 4); \
	} while(0)

#define __ip_byteswap64(x) \
	do { \
		char c[8]; \
		c[0] = ((char *)x)[7]; \
		c[1] = ((char *)x)[6]; \
		c[2] = ((char *)x)[5]; \
		c[3] = ((char *)x)[4]; \
		c[4] = ((char *)x)[3]; \
		c[5] = ((char *)x)[2]; \
		c[6] = ((char *)x)[1]; \
		c[7] = ((char *)x)[0]; \
		memcpy((char *)x, c, 8); \
	} while(0)

#ifdef _BIG_ENDIAN

#define le2me16(x) __byteswap16(x)
#define le2me32(x) __byteswap32(x)
#define le2me64(x) __byteswap64(x)
#define be2me16(x) (x)
#define be2me32(x) (x)
#define be2me64(x) (x)

#define ip_le2me16(x) __ip_byteswap16(x)
#define ip_le2me32(x) __ip_byteswap32(x)
#define ip_le2me64(x) __ip_byteswap64(x)
#define ip_be2me16(x) do {} while(0)
#define ip_be2me32(x) do {} while(0)
#define ip_be2me64(x) do {} while(0)

#define me2le16(x) __byteswap16(x)
#define me2le32(x) __byteswap32(x)
#define me2le64(x) __byteswap64(x)
#define me2be16(x) (x)
#define me2be32(x) (x)
#define me2be64(x) (x)

#define ip_me2le16(x) __ip_byteswap16(x)
#define ip_me2le32(x) __ip_byteswap32(x)
#define ip_me2le64(x) __ip_byteswap64(x)
#define ip_me2be16(x) do {} while(0)
#define ip_me2be32(x) do {} while(0)
#define ip_me2be32(x) do {} while(0)
	
#else

#define le2me16(x) (x)
#define le2me32(x) (x)
#define le2me64(x) (x)
#define be2me16(x) __byteswap16(x)
#define be2me32(x) __byteswap32(x)
#define be2me64(x) __byteswap64(x)

#define ip_be2me16(x) __ip_byteswap16(x)
#define ip_be2me32(x) __ip_byteswap32(x)
#define ip_be2me64(x) __ip_byteswap64(x)
#define ip_le2me16(x) do {} while(0)
#define ip_le2me32(x) do {} while(0)
#define ip_le2me64(x) do {} while(0)

#define me2be16(x) __byteswap16(x)
#define me2be32(x) __byteswap32(x)
#define me2be64(x) __byteswap64(x)
#define me2le16(x) (x)
#define me2le32(x) (x)
#define me2le64(x) (x)

#define ip_me2be16(x) __ip_byteswap16(x)
#define ip_me2be32(x) __ip_byteswap32(x)
#define ip_me2be64(x) __ip_byteswap64(x)
#define ip_me2le16(x) do {} while(0)
#define ip_me2le32(x) do {} while(0)
#define ip_me2le64(x) do {} while(0)

#endif

#ifdef __cplusplus
}

#endif
#endif /* __BYTE_ORDER_H__ */
