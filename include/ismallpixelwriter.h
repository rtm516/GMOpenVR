//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//
//===========================================================================//


#ifndef PIXELWRITER_H
#define PIXELWRITER_H
#include <stdint.h>
#define int64 int64_t

#ifdef _WIN32
#pragma once
#endif

#define FORCEINLINE //inline

#ifdef _WIN32
#define FORCEINLINE_PIXEL FORCEINLINE
#elif POSIX
#define FORCEINLINE_PIXEL inline
#else
#error "implement me"
#endif

/*#include "bitmap/imageformat.h"
#include "tier0/dbg.h"
#include "mathlib/compressed_vector.h"
#include "mathlib/ssemath.h"*/

#define Assert
#define Msg
#define RESTRICT
#define FLTX4 void*

typedef float vec_t;

inline unsigned long& FloatBits(vec_t& f)
{
	return *reinterpret_cast<unsigned long*>(&f);
}

inline unsigned long const& FloatBits(vec_t const& f)
{
	return *reinterpret_cast<unsigned long const*>(&f);
}

inline vec_t BitsToFloat(unsigned long i)
{
	return *reinterpret_cast<vec_t*>(&i);
}

inline bool IsFinite(vec_t f)
{
	return ((FloatBits(f) & 0x7F800000) != 0x7F800000);
}

inline unsigned long FloatAbsBits(vec_t f)
{
	return FloatBits(f) & 0x7FFFFFFF;
}

enum ImageFormat
{
	IMAGE_FORMAT_UNKNOWN = -1,
	IMAGE_FORMAT_RGBA8888 = 0,
	IMAGE_FORMAT_ABGR8888,
	IMAGE_FORMAT_RGB888,
	IMAGE_FORMAT_BGR888,
	IMAGE_FORMAT_RGB565,
	IMAGE_FORMAT_I8,
	IMAGE_FORMAT_IA88,
	IMAGE_FORMAT_P8,
	IMAGE_FORMAT_A8,
	IMAGE_FORMAT_RGB888_BLUESCREEN,
	IMAGE_FORMAT_BGR888_BLUESCREEN,
	IMAGE_FORMAT_ARGB8888,
	IMAGE_FORMAT_BGRA8888,
	IMAGE_FORMAT_DXT1,
	IMAGE_FORMAT_DXT3,
	IMAGE_FORMAT_DXT5,
	IMAGE_FORMAT_BGRX8888,
	IMAGE_FORMAT_BGR565,
	IMAGE_FORMAT_BGRX5551,
	IMAGE_FORMAT_BGRA4444,
	IMAGE_FORMAT_DXT1_ONEBITALPHA,
	IMAGE_FORMAT_BGRA5551,
	IMAGE_FORMAT_UV88,
	IMAGE_FORMAT_UVWQ8888,
	IMAGE_FORMAT_RGBA16161616F,
	IMAGE_FORMAT_RGBA16161616,
	IMAGE_FORMAT_UVLX8888,
	IMAGE_FORMAT_R32F,			// Single-channel 32-bit floating point
	IMAGE_FORMAT_RGB323232F,
	IMAGE_FORMAT_RGBA32323232F,

	// Depth-stencil texture formats for shadow depth mapping
	IMAGE_FORMAT_NV_DST16,		// 
	IMAGE_FORMAT_NV_DST24,		//
	IMAGE_FORMAT_NV_INTZ,		// Vendor-specific depth-stencil texture
	IMAGE_FORMAT_NV_RAWZ,		// formats for shadow depth mapping 
	IMAGE_FORMAT_ATI_DST16,		// 
	IMAGE_FORMAT_ATI_DST24,		//
	IMAGE_FORMAT_NV_NULL,		// Dummy format which takes no video memory

	// Compressed normal map formats
	IMAGE_FORMAT_ATI2N,			// One-surface ATI2N / DXN format
	IMAGE_FORMAT_ATI1N,			// Two-surface ATI1N format

#if defined( _X360 )
	// Depth-stencil texture formats
	IMAGE_FORMAT_X360_DST16,
	IMAGE_FORMAT_X360_DST24,
	IMAGE_FORMAT_X360_DST24F,
	// supporting these specific formats as non-tiled for procedural cpu access
	IMAGE_FORMAT_LINEAR_BGRX8888,
	IMAGE_FORMAT_LINEAR_RGBA8888,
	IMAGE_FORMAT_LINEAR_ABGR8888,
	IMAGE_FORMAT_LINEAR_ARGB8888,
	IMAGE_FORMAT_LINEAR_BGRA8888,
	IMAGE_FORMAT_LINEAR_RGB888,
	IMAGE_FORMAT_LINEAR_BGR888,
	IMAGE_FORMAT_LINEAR_BGRX5551,
	IMAGE_FORMAT_LINEAR_I8,
	IMAGE_FORMAT_LINEAR_RGBA16161616,

	IMAGE_FORMAT_LE_BGRX8888,
	IMAGE_FORMAT_LE_BGRA8888,
#endif

	NUM_IMAGE_FORMATS
};

//-----------------------------------------------------------------------------
// Color writing class 
//-----------------------------------------------------------------------------

class CPixelWriter
{
public:
	FORCEINLINE void SetPixelMemory( ImageFormat format, void* pMemory, int stride );
	FORCEINLINE void *GetPixelMemory() { return m_pBase; }

	// this is no longer used:
#if 0 // defined( _X360 )
	// set after SetPixelMemory() 
	FORCEINLINE void ActivateByteSwapping( bool bSwap );
#endif

	FORCEINLINE void Seek( int x, int y );
	FORCEINLINE void* SkipBytes( int n );
	FORCEINLINE void SkipPixels( int n );	
	FORCEINLINE void WritePixel( int r, int g, int b, int a = 255 );
	FORCEINLINE void WritePixelNoAdvance( int r, int g, int b, int a = 255 );
	FORCEINLINE void WritePixelSigned( int r, int g, int b, int a = 255 );
	FORCEINLINE void WritePixelNoAdvanceSigned( int r, int g, int b, int a = 255 );
	FORCEINLINE void ReadPixelNoAdvance( int &r, int &g, int &b, int &a );

	// Floating point formats
	FORCEINLINE void WritePixelNoAdvanceF( float r, float g, float b, float a = 1.0f );
	FORCEINLINE void WritePixelF( float r, float g, float b, float a = 1.0f );

	// SIMD formats
	FORCEINLINE void WritePixel( FLTX4 rgba );
	FORCEINLINE void WritePixelNoAdvance( FLTX4 rgba );
#ifdef _X360
	// here are some explicit formats so we can avoid the switch:
	FORCEINLINE void WritePixelNoAdvance_RGBA8888( FLTX4 rgba );
	FORCEINLINE void WritePixelNoAdvance_BGRA8888( FLTX4 rgba );
	// as above, but with m_pBits passed in to avoid a LHS
	FORCEINLINE void WritePixelNoAdvance_BGRA8888( FLTX4 rgba, void *pBits );
	// for writing entire SIMD registers at once when they have
	// already been packed, and when m_pBits is vector-aligned
	// (which is a requirement for write-combined memory)
	// offset is added to m_pBits (saving you from the obligatory
	// LHS of a SkipBytes)
	FORCEINLINE void WriteFourPixelsExplicitLocation_BGRA8888( FLTX4 rgba, int offset );
#endif


	FORCEINLINE unsigned char GetPixelSize() { return m_Size; }	

	FORCEINLINE bool IsUsingFloatFormat() const;
	FORCEINLINE unsigned char *GetCurrentPixel() { return m_pBits; }

private:
	enum
	{
		PIXELWRITER_USING_FLOAT_FORMAT       = 0x01,
		PIXELWRITER_USING_16BIT_FLOAT_FORMAT = 0x02,
		PIXELWRITER_SWAPBYTES                = 0x04,
	};

	unsigned char*	m_pBase;
	unsigned char*	m_pBits;
	unsigned short	m_BytesPerRow;
	unsigned char	m_Size;
	unsigned char	m_nFlags;
	signed short	m_RShift;
	signed short	m_GShift;
	signed short	m_BShift;
	signed short	m_AShift;
	unsigned int	m_RMask;
	unsigned int	m_GMask;
	unsigned int	m_BMask;
	unsigned int	m_AMask;

#ifdef _X360
	ImageFormat		m_Format;
public:
	inline const ImageFormat &GetFormat() { return m_Format; }
private:
#endif
};

FORCEINLINE_PIXEL bool CPixelWriter::IsUsingFloatFormat() const
{
	return (m_nFlags & PIXELWRITER_USING_FLOAT_FORMAT) != 0;
}

FORCEINLINE_PIXEL void CPixelWriter::SetPixelMemory( ImageFormat format, void* pMemory, int stride )
{
	m_pBits = (unsigned char*)pMemory;
	m_pBase = m_pBits;
	m_BytesPerRow = (unsigned short)stride;
	m_nFlags = 0;
#ifdef _X360
	m_Format = format;
#endif

	switch ( format )
	{
	case IMAGE_FORMAT_R32F: // NOTE! : the low order bits are first in this naming convention.
		m_Size = 4;
		m_RShift = 0;
		m_GShift = 0;
		m_BShift = 0;
		m_AShift = 0;
		m_RMask = 0xFFFFFFFF;
		m_GMask = 0x0;
		m_BMask = 0x0;
		m_AMask = 0x0;
		m_nFlags |= PIXELWRITER_USING_FLOAT_FORMAT;
		break;

	case IMAGE_FORMAT_RGBA32323232F:
		m_Size = 16;
		m_RShift = 0;
		m_GShift = 32;
		m_BShift = 64;
		m_AShift = 96;
		m_RMask = 0xFFFFFFFF;
		m_GMask = 0xFFFFFFFF;
		m_BMask = 0xFFFFFFFF;
		m_AMask = 0xFFFFFFFF;
		m_nFlags |= PIXELWRITER_USING_FLOAT_FORMAT;
		break;

	case IMAGE_FORMAT_RGBA16161616F:
		m_Size = 8;
		m_RShift = 0;
		m_GShift = 16;
		m_BShift = 32;
		m_AShift = 48;
		m_RMask = 0xFFFF;
		m_GMask = 0xFFFF;
		m_BMask = 0xFFFF;
		m_AMask = 0xFFFF;
		m_nFlags |= PIXELWRITER_USING_FLOAT_FORMAT | PIXELWRITER_USING_16BIT_FLOAT_FORMAT;
		break;

	case IMAGE_FORMAT_RGBA8888:
#if defined( _X360 )
	case IMAGE_FORMAT_LINEAR_RGBA8888:
#endif
		m_Size = 4;
		m_RShift = 0;
		m_GShift = 8;
		m_BShift = 16;
		m_AShift = 24;
		m_RMask = 0xFF;
		m_GMask = 0xFF;
		m_BMask = 0xFF;
		m_AMask = 0xFF;
		break;

	case IMAGE_FORMAT_BGRA8888: // NOTE! : the low order bits are first in this naming convention.
#if defined( _X360 )
	case IMAGE_FORMAT_LINEAR_BGRA8888:
#endif
		m_Size = 4;
		m_RShift = 16;
		m_GShift = 8;
		m_BShift = 0;
		m_AShift = 24;
		m_RMask = 0xFF;
		m_GMask = 0xFF;
		m_BMask = 0xFF;
		m_AMask = 0xFF;
		break;

	case IMAGE_FORMAT_BGRX8888:
#if defined( _X360 )
	case IMAGE_FORMAT_LINEAR_BGRX8888:
#endif
		m_Size = 4;
		m_RShift = 16;
		m_GShift = 8;
		m_BShift = 0;
		m_AShift = 24;
		m_RMask = 0xFF;
		m_GMask = 0xFF;
		m_BMask = 0xFF;
		m_AMask = 0x00;
		break;

	case IMAGE_FORMAT_BGRA4444:
		m_Size = 2;
		m_RShift = 4;
		m_GShift = 0;
		m_BShift = -4;
		m_AShift = 8;
		m_RMask = 0xF0;
		m_GMask = 0xF0;
		m_BMask = 0xF0;
		m_AMask = 0xF0;
		break;

	case IMAGE_FORMAT_BGR888:
		m_Size = 3;
		m_RShift = 16;
		m_GShift = 8;
		m_BShift = 0;
		m_AShift = 0;
		m_RMask = 0xFF;
		m_GMask = 0xFF;
		m_BMask = 0xFF;
		m_AMask = 0x00;
		break;

	case IMAGE_FORMAT_BGR565:
		m_Size = 2;
		m_RShift = 8;
		m_GShift = 3;
		m_BShift = -3;
		m_AShift = 0;
		m_RMask = 0xF8;
		m_GMask = 0xFC;
		m_BMask = 0xF8;
		m_AMask = 0x00;
		break;

	case IMAGE_FORMAT_BGRA5551:
	case IMAGE_FORMAT_BGRX5551:
		m_Size = 2;
		m_RShift = 7;
		m_GShift = 2;
		m_BShift = -3;
		m_AShift = 8;
		m_RMask = 0xF8;
		m_GMask = 0xF8;
		m_BMask = 0xF8;
		m_AMask = 0x80;
		break;

	// GR - alpha format for HDR support
	case IMAGE_FORMAT_A8:
		m_Size = 1;
		m_RShift = 0;
		m_GShift = 0;
		m_BShift = 0;
		m_AShift = 0;
		m_RMask = 0x00;
		m_GMask = 0x00;
		m_BMask = 0x00;
		m_AMask = 0xFF;
		break;

	case IMAGE_FORMAT_UVWQ8888:
		m_Size = 4;
		m_RShift = 0;
		m_GShift = 8;
		m_BShift = 16;
		m_AShift = 24;
		m_RMask = 0xFF;
		m_GMask = 0xFF;
		m_BMask = 0xFF;
		m_AMask = 0xFF;
		break;

	case IMAGE_FORMAT_RGBA16161616:
#if defined( _X360 )
	case IMAGE_FORMAT_LINEAR_RGBA16161616:
#endif		
		m_Size = 8;
		m_RShift = 0;
		m_GShift = 16;
		m_BShift = 32;
		m_AShift = 48;
		m_RMask = 0xFFFF;
		m_GMask = 0xFFFF;
		m_BMask = 0xFFFF;
		m_AMask = 0xFFFF;
		break;

	case IMAGE_FORMAT_I8:
		// whatever goes into R is considered the intensity.
		m_Size = 1;
		m_RShift = 0;
		m_GShift = 0;
		m_BShift = 0;
		m_AShift = 0;
		m_RMask = 0xFF;
		m_GMask = 0x00;
		m_BMask = 0x00;
		m_AMask = 0x00;
		break;
	// FIXME: Add more color formats as need arises
	default:
		{
			static bool format_error_printed[NUM_IMAGE_FORMATS];
			if ( !format_error_printed[format] )
			{
				Assert( 0 );
				Msg( "CPixelWriter::SetPixelMemory:  Unsupported image format %i\n", format );
				format_error_printed[format] = true;
			}
			m_Size = 0; // set to zero so that we don't stomp memory for formats that we don't understand.
			m_RShift = 0;
			m_GShift = 0;
			m_BShift = 0;
			m_AShift = 0;
			m_RMask = 0x00;
			m_GMask = 0x00;
			m_BMask = 0x00;
			m_AMask = 0x00;
		}
		break;
	}
}

#if 0 // defined( _X360 )
FORCEINLINE void CPixelWriter::ActivateByteSwapping( bool bSwap )
{
	// X360TBD: Who is trying to use this?
	// Purposely not hooked up because PixelWriter has been ported to read/write native pixels only
	Assert( 0 );

	if ( bSwap && !(m_nFlags & PIXELWRITER_SWAPBYTES ) )
	{
		m_nFlags |= PIXELWRITER_SWAPBYTES;

		// only tested with 4 byte formats
		Assert( m_Size == 4 );
	}
	else if ( !bSwap && (m_nFlags & PIXELWRITER_SWAPBYTES ) )
	{
		m_nFlags &= ~PIXELWRITER_SWAPBYTES;
	}
	else
	{
		// same state
		return;
	}

	// swap the shifts
	m_RShift = 24-m_RShift;
	m_GShift = 24-m_GShift;
	m_BShift = 24-m_BShift;
	m_AShift = 24-m_AShift;
}
#endif

//-----------------------------------------------------------------------------
// Sets where we're writing to
//-----------------------------------------------------------------------------
FORCEINLINE_PIXEL void CPixelWriter::Seek( int x, int y )
{
	m_pBits = m_pBase + y * m_BytesPerRow + x * m_Size;
}


//-----------------------------------------------------------------------------
// Skips n bytes:
//-----------------------------------------------------------------------------
FORCEINLINE_PIXEL void* CPixelWriter::SkipBytes( int n ) RESTRICT
{
	m_pBits += n;
	return m_pBits;
}


//-----------------------------------------------------------------------------
// Skips n pixels:
//-----------------------------------------------------------------------------
FORCEINLINE_PIXEL void CPixelWriter::SkipPixels( int n )
{
	SkipBytes( n * m_Size );
}


//-----------------------------------------------------------------------------
// Writes a pixel without advancing the index		PC ONLY
//-----------------------------------------------------------------------------
FORCEINLINE_PIXEL void CPixelWriter::WritePixelNoAdvanceF( float r, float g, float b, float a )
{
	Assert( IsUsingFloatFormat() );

	// X360TBD: Not ported

	if (PIXELWRITER_USING_16BIT_FLOAT_FORMAT & m_nFlags)
	{
		float16 fp16[4];
		fp16[0].SetFloat( r );
		fp16[1].SetFloat( g );
		fp16[2].SetFloat( b );
		fp16[3].SetFloat( a );
		// fp16
		unsigned short pBuf[4] = { 0, 0, 0, 0 };
		pBuf[ m_RShift >> 4 ] |= (fp16[0].GetBits() & m_RMask) << ( m_RShift & 0xF );
		pBuf[ m_GShift >> 4 ] |= (fp16[1].GetBits() & m_GMask) << ( m_GShift & 0xF );
		pBuf[ m_BShift >> 4 ] |= (fp16[2].GetBits() & m_BMask) << ( m_BShift & 0xF );
		pBuf[ m_AShift >> 4 ] |= (fp16[3].GetBits() & m_AMask) << ( m_AShift & 0xF );
		memcpy( m_pBits, pBuf, m_Size );
	}
	else
	{
		// fp32
		int pBuf[4] = { 0, 0, 0, 0 };
		pBuf[ m_RShift >> 5 ] |= (FloatBits(r) & m_RMask) << ( m_RShift & 0x1F );
		pBuf[ m_GShift >> 5 ] |= (FloatBits(g) & m_GMask) << ( m_GShift & 0x1F );
		pBuf[ m_BShift >> 5 ] |= (FloatBits(b) & m_BMask) << ( m_BShift & 0x1F );
		pBuf[ m_AShift >> 5 ] |= (FloatBits(a) & m_AMask) << ( m_AShift & 0x1F );
		memcpy( m_pBits, pBuf, m_Size );
	}
}*/

//-----------------------------------------------------------------------------
// Writes a pixel, advances the write index 
//-----------------------------------------------------------------------------

FORCEINLINE_PIXEL void CPixelWriter::WritePixelF( float r, float g, float b, float a )
{
	WritePixelNoAdvanceF(r, g, b, a);
	m_pBits += m_Size;
}

	
//-----------------------------------------------------------------------------
// Writes a pixel, advances the write index 
//-----------------------------------------------------------------------------
FORCEINLINE_PIXEL void CPixelWriter::WritePixel( int r, int g, int b, int a )
{
	WritePixelNoAdvance(r,g,b,a);
	m_pBits += m_Size;
}

//-----------------------------------------------------------------------------
// Writes a pixel, advances the write index 
//-----------------------------------------------------------------------------

FORCEINLINE_PIXEL void CPixelWriter::WritePixelSigned( int r, int g, int b, int a )
{
	WritePixelNoAdvanceSigned(r,g,b,a);
	m_pBits += m_Size;
}


//-----------------------------------------------------------------------------
// Writes a pixel without advancing the index
//-----------------------------------------------------------------------------
FORCEINLINE_PIXEL void CPixelWriter::WritePixelNoAdvance( int r, int g, int b, int a )
{
	Assert( !IsUsingFloatFormat() );

	if ( m_Size <= 0 )
	{
		return;
	}
	if ( m_Size < 5 )
	{
		unsigned int val = (r & m_RMask) << m_RShift;
		val |=  (g & m_GMask) << m_GShift;
		val |= (m_BShift > 0) ? ((b & m_BMask) << m_BShift) : ((b & m_BMask) >> -m_BShift);
		val |=	(a & m_AMask) << m_AShift;

		switch( m_Size )
		{
		default:
			Assert( 0 );
			return;
		case 1:
			{
				m_pBits[0] = (unsigned char)((val & 0xff));
				return;
			}
		case 2:
			{
				((unsigned short *)m_pBits)[0] = (unsigned short)((val & 0xffff));
				return;
			}
		case 3:
			{
				
					((unsigned short *)m_pBits)[0] = (unsigned short)((val & 0xffff));
					m_pBits[2] = (unsigned char)((val >> 16) & 0xff);
				
				return;
			}
		case 4:
			{
				((unsigned int *)m_pBits)[0] = val;
				return;
			}
		}
	}
	else	// RGBA32323232 or RGBA16161616 -- PC only.
	{
		int64 val = ( ( int64 )(r & m_RMask) ) << m_RShift;
		val |=  ( ( int64 )(g & m_GMask) ) << m_GShift;
		val |= (m_BShift > 0) ? ((( int64 )( b & m_BMask)) << m_BShift) : (((int64)( b & m_BMask)) >> -m_BShift);
		val |=	( ( int64 )(a & m_AMask) ) << m_AShift;

		switch( m_Size )
		{
		case 6:
			{
				((unsigned int *)m_pBits)[0] = val & 0xffffffff;
					((unsigned short *)m_pBits)[2] = (unsigned short)( ( val >> 32 ) & 0xffff );
				
				return;
			}
		case 8:
			{
				((unsigned int *)m_pBits)[0] = val & 0xffffffff;
					((unsigned int *)m_pBits)[1] = ( val >> 32 ) & 0xffffffff;
				
				return;
			}
		default:
			Assert( 0 );
			return;
		}
	}
}

//-----------------------------------------------------------------------------
// Writes a signed pixel without advancing the index
//-----------------------------------------------------------------------------

FORCEINLINE_PIXEL void CPixelWriter::WritePixelNoAdvanceSigned( int r, int g, int b, int a )
{
	Assert( !IsUsingFloatFormat() );

	if ( m_Size <= 0 )
	{
		return;
	}

	if ( m_Size < 5 )
	{
		int val = (r & m_RMask) << m_RShift;
		val |=  (g & m_GMask) << m_GShift;
		val |= (m_BShift > 0) ? ((b & m_BMask) << m_BShift) : ((b & m_BMask) >> -m_BShift);
		val |=	(a & m_AMask) << m_AShift;
		signed char *pSignedBits = (signed char *)m_pBits;

		
			switch ( m_Size )
			{
			case 4:
				pSignedBits[3] = (signed char)((val >> 24) & 0xff);
				// fall through intentionally.
			case 3:
				pSignedBits[2] = (signed char)((val >> 16) & 0xff);
				// fall through intentionally.
			case 2:
				pSignedBits[1] = (signed char)((val >> 8) & 0xff);
				// fall through intentionally.
			case 1:
				pSignedBits[0] = (signed char)((val & 0xff));
				// fall through intentionally.
				return;
			}
		
	}
	else
	{
		int64 val = ( ( int64 )(r & m_RMask) ) << m_RShift;
		val |=  ( ( int64 )(g & m_GMask) ) << m_GShift;
		val |= (m_BShift > 0) ? ((( int64 )( b & m_BMask)) << m_BShift) : (((int64)( b & m_BMask)) >> -m_BShift);
		val |=	( ( int64 )(a & m_AMask) ) << m_AShift;
		signed char *pSignedBits = ( signed char * )m_pBits;

	
			switch( m_Size )
			{
			case 8:
				pSignedBits[7] = (signed char)((val >> 56) & 0xff);
				pSignedBits[6] = (signed char)((val >> 48) & 0xff);
				// fall through intentionally.
			case 6:
				pSignedBits[5] = (signed char)((val >> 40) & 0xff);
				pSignedBits[4] = (signed char)((val >> 32) & 0xff);
				// fall through intentionally.
			case 4:
				pSignedBits[3] = (signed char)((val >> 24) & 0xff);
				// fall through intentionally.
			case 3:
				pSignedBits[2] = (signed char)((val >> 16) & 0xff);
				// fall through intentionally.
			case 2:
				pSignedBits[1] = (signed char)((val >> 8) & 0xff);
				// fall through intentionally.
			case 1:
				pSignedBits[0] = (signed char)((val & 0xff));
				break;
			default:
				Assert( 0 );
				return;
			}
		
	}
}

FORCEINLINE_PIXEL void CPixelWriter::ReadPixelNoAdvance( int &r, int &g, int &b, int &a )
{
	Assert( !IsUsingFloatFormat() );

	int val = m_pBits[0];
	if ( m_Size > 1 )
	{
			val |= (int)m_pBits[1] << 8;
			if ( m_Size > 2 )
			{
				val |= (int)m_pBits[2] << 16;
				if ( m_Size > 3 )
				{
					val |= (int)m_pBits[3] << 24;
				}
			}
	}

	r = (val>>m_RShift) & m_RMask;
	g = (val>>m_GShift) & m_GMask;
	b = (val>>m_BShift) & m_BMask;
	a = (val>>m_AShift) & m_AMask;
}

#endif // PIXELWRITER_H;
