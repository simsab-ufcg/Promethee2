#pragma once

#include "utils.h"

struct BufferManager{
	uint16 sampleFormat;
	uint32 byteSize;
	uint32 width, height;
	uint32 rowperstrip;
	vector<tdata_t> buffer;
	vector<bool> bitseti;
	vector<int> positions;
	vector<int> circular_queue;
	int queue_pointer;
	uint32 qtd_store;
	TIFF *input;

	BufferManager();
	BufferManager(string path, uint64 megaBytes);

	ldouble read(uint32 row, uint32 colunm);
	ldouble readPixel(uint32 row, uint32 colunm);
	uint32 getWidth();
	uint32 getHeight();
	void close();
};