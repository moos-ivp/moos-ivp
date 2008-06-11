#ifndef __BYTE_ORDER_H__
#define __BYTE_ORDER_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define __byteswap16(x) \
	( ((0xFF00 & ((uint16_t)x)) >> 8) | \
	  ((0x00FF & ((uint16_t)x)) << 8) )

#define __byteswap32(x) \
	( ((0xFF000000UL & ((uint32_t)x)) >> 24) | \
	  ((0x00FF0000UL & ((uint32_t)x)) >> 8) | \
	  ((0x0000FF00UL & ((uint32_t)x)) << 8) | \
	  ((0x000000FFUL & ((uint32_t)x)) << 24) )

#define __byteswap64(x) \
	( ((0xFF00000000000000ULL & ((uint64_t)x)) >> 56) | \
	  ((0x00FF000000000000ULL & ((uint64_t)x)) >> 40) | \
	  ((0x0000FF0000000000ULL & ((uint64_t)x)) >> 24) | \
	  ((0x000000FF00000000ULL & ((uint64_t)x)) >> 8)  | \
	  ((0x00000000FF000000ULL & ((uint64_t)x)) << 8)  | \
	  ((0x0000000000FF0000ULL & ((uint64_t)x)) << 24) | \
	  ((0x000000000000FF00ULL & ((uint64_t)x)) << 40) | \
	  ((0x00000000000000FFULL & ((uint64_t)x)) << 56) )

#define __ip_byteswap16(i, o) \
	do { \
		uint8_t c; \
		c = ((uint8_t *)(i))[0]; \
		((uint8_t *)(o))[0] = ((uint8_t *)(i))[1]; \
		((uint8_t *)(o))[1] = c; \
	} while(0)

#define __ip_byteswap32(i, o) \
	do { \
		uint8_t c[4]; \
		c[0] = ((uint8_t *)(i))[3]; \
		c[1] = ((uint8_t *)(i))[2]; \
		c[2] = ((uint8_t *)(i))[1]; \
		c[3] = ((uint8_t *)(i))[0]; \
		memcpy(((uint8_t *)(o)), c, 4); \
	} while(0)

#define __ip_byteswap64(i, o) \
	do { \
		uint8_t c[8]; \
		c[0] = ((uint8_t *)(i))[7]; \
		c[1] = ((uint8_t *)(i))[6]; \
		c[2] = ((uint8_t *)(i))[5]; \
		c[3] = ((uint8_t *)(i))[4]; \
		c[4] = ((uint8_t *)(i))[3]; \
		c[5] = ((uint8_t *)(i))[2]; \
		c[6] = ((uint8_t *)(i))[1]; \
		c[7] = ((uint8_t *)(i))[0]; \
		memcpy((uint8_t *)(o), c, 8); \
	} while(0)

#ifdef _BIG_ENDIAN

#define le2me16(x) __byteswap16(x)
#define le2me32(x) __byteswap32(x)
#define le2me64(x) __byteswap64(x)
#define be2me16(x) (x)
#define be2me32(x) (x)
#define be2me64(x) (x)

#define ip_le2me16(x) __ip_byteswap16(x, x)
#define ip_le2me32(x) __ip_byteswap32(x, x)
#define ip_le2me64(x) __ip_byteswap64(x, x)
#define ip_be2me16(x) do {} while(0)
#define ip_be2me32(x) do {} while(0)
#define ip_be2me64(x) do {} while(0)

#define mc_le2me16(o, i) __ip_byteswap16(i, o)
#define mc_le2me32(o, i) __ip_byteswap32(i, o)
#define mc_le2me64(o, i) __ip_byteswap64(i, o)
#define mc_be2me16(o, i) memcpy(o, i, 2)
#define mc_be2me32(o, i) memcpy(o, i, 4)
#define mc_be2me64(o, i) memcpy(o, i, 8)

#define me2le16(x) __byteswap16(x)
#define me2le32(x) __byteswap32(x)
#define me2le64(x) __byteswap64(x)
#define me2be16(x) (x)
#define me2be32(x) (x)
#define me2be64(x) (x)

#define ip_me2le16(x) __ip_byteswap16(x,x)
#define ip_me2le32(x) __ip_byteswap32(x,x)
#define ip_me2le64(x) __ip_byteswap64(x,x)
#define ip_me2be16(x) do {} while(0)
#define ip_me2be32(x) do {} while(0)
#define ip_me2be32(x) do {} while(0)

#define mc_me2le16(o, i) __ip_byteswap16(i, o)
#define mc_me2le32(o, i) __ip_byteswap32(i, o)
#define mc_me2le64(o, i) __ip_byteswap64(i, o)
#define mc_me2be16(o, i) memcpy(o, i, 2)
#define mc_me2be32(o, i) memcpy(o, i, 4)
#define mc_me2be64(o, i) memcpy(o, i, 8)
	
#else

#define le2me16(x) (x)
#define le2me32(x) (x)
#define le2me64(x) (x)
#define be2me16(x) __byteswap16(x)
#define be2me32(x) __byteswap32(x)
#define be2me64(x) __byteswap64(x)

#define ip_be2me16(x) __ip_byteswap16(x,x)
#define ip_be2me32(x) __ip_byteswap32(x,x)
#define ip_be2me64(x) __ip_byteswap64(x,x)
#define ip_le2me16(x) do {} while(0)
#define ip_le2me32(x) do {} while(0)
#define ip_le2me64(x) do {} while(0)

#define mc_be2me16(o, i) __ip_byteswap16(i, o)
#define mc_be2me32(o, i) __ip_byteswap32(i, o)
#define mc_be2me64(o, i) __ip_byteswap64(i, o)
#define mc_le2me16(o, i) memcpy(o, i, 2)
#define mc_le2me32(o, i) memcpy(o, i, 4)
#define mc_le2me64(o, i) memcpy(o, i, 8)

#define me2be16(x) __byteswap16(x)
#define me2be32(x) __byteswap32(x)
#define me2be64(x) __byteswap64(x)
#define me2le16(x) (x)
#define me2le32(x) (x)
#define me2le64(x) (x)

#define ip_me2be16(x) __ip_byteswap16(x,x)
#define ip_me2be32(x) __ip_byteswap32(x,x)
#define ip_me2be64(x) __ip_byteswap64(x,x)
#define ip_me2le16(x) do {} while(0)
#define ip_me2le32(x) do {} while(0)
#define ip_me2le64(x) do {} while(0)

#define mc_me2be16(o, i) __ip_byteswap16(i, o)
#define mc_me2be32(o, i) __ip_byteswap32(i, o)
#define mc_me2be64(o, i) __ip_byteswap64(i, o)
#define mc_me2le16(o, i) memcpy(o, i, 2)
#define mc_me2le32(o, i) memcpy(o, i, 4)
#define mc_me2le64(o, i) memcpy(o, i, 8)

#endif

#ifdef __cplusplus
}
#endif

#endif /* __BYTE_ORDER_H__ */
