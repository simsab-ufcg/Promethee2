#pragma once

#include "types.h"
#include <iostream>
#include <string.h>
#include <math.h>

struct PixelReader{
	uint16 sampleFormat;
	uint8 byteSize;
	tdata_t buffer;

	PixelReader();
	PixelReader(uint16 _sampleFormat, uint8 _byteSize,tdata_t _buffer);

	ldouble readPixel(uint32 colunm);

};