#include "buffer_manage.h"

BufferManager::BufferManager(){};

BufferManager::BufferManager(string path, uint64 megaBytes){
	this->input = TIFFOpen(path.c_str(), "rm");
  	TIFFGetField(input, TIFFTAG_IMAGEWIDTH, &this->width);
	TIFFGetField(input, TIFFTAG_IMAGELENGTH, &this->height);
	TIFFGetField(input, TIFFTAG_SAMPLEFORMAT, &this->sampleFormat);
	this->byteSize = TIFFScanlineSize(input) / this->width;
	bitseti = vector<bool>(this->height, false);
	positions = vector<int>(this->height, -1);
	megaBytes *= 1000 * 250; // change to bytes
	uint32 qtd_store = megaBytes / TIFFScanlineSize(input); // how much lines can store in ram?
	circular_queue = vector<int>(qtd_store, -1);
	buffer = vector<tdata_t>(qtd_store, NULL);
	queue_pointer = 0;
};

ldouble BufferManager::read(uint32 row, uint32 colunm){
	if(!bitseti[row]){
		uint32 curr = circular_queue[queue_pointer];
		if(curr != -1){
			bitseti[curr] = false;
		}else{
			buffer[queue_pointer] = _TIFFmalloc(TIFFScanlineSize(this->input));
		}
		bitseti[row] = true;
		positions[row] = queue_pointer;
		TIFFReadScanline(input, buffer[queue_pointer], row);
		circular_queue[queue_pointer] = row;
		queue_pointer++;
		if(queue_pointer >= circular_queue.size())
			queue_pointer = 0;	
	}
	return this->readPixel(positions[row], colunm);
}

ldouble BufferManager::readPixel(uint32 row, uint32 colunm){
	ldouble ret = 0;
	switch(sampleFormat){
		case 1:
			{
				uint64 value = 0;
				memcpy(&value, buffer[row] + (colunm * byteSize), byteSize);
				ret = value;
			}
			break;
		case 2:
			{
				int64 value = 0;
				memcpy(&value, buffer[row] + (colunm * byteSize), byteSize);
				ret = value;
			}
			break;
		case 3:
			switch(byteSize){
				case 4:
					{
						float value = 0;
						memcpy(&value, buffer[row] + (colunm * byteSize), byteSize);
						ret = value;
					}
					break;
				case 8:
					{
						double value = 0;
						memcpy(&value, buffer[row] + (colunm * byteSize), byteSize);
						ret = value;
					}
					break;
				case 16:
					{
						long double value = 0;
						memcpy(&value, buffer[row] + (colunm * byteSize), byteSize);
						ret = value;
					}
					break;
				default:
					cerr << "Unsupported operation!" << endl;
					exit(1);
			}
			break;
		default:
			cerr << "Unsupported operation!" << endl;
			exit(1);
	}
	return ret;
};

uint32 BufferManager::getHeight(){
	return this->height;
}

uint32 BufferManager::getWidth(){
	return this->width;
}

void BufferManager::close(){
	for(int i = this->height - 1; i >= 0; i--){
		if(circular_queue[i] != -1)
			_TIFFfree(buffer[i]);
	}
	TIFFClose(this->input);
}