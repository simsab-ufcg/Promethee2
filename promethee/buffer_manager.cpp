#include "buffer_manager.h"

BufferManager::BufferManager(){};

BufferManager::BufferManager(string path, uint64 megaBytes){
	this->input = TIFFOpen(path.c_str(), "rm");
  	TIFFGetField(input, TIFFTAG_IMAGEWIDTH, &this->width);
	TIFFGetField(input, TIFFTAG_IMAGELENGTH, &this->height);
	TIFFGetField(input, TIFFTAG_SAMPLEFORMAT, &this->sampleFormat);
	this->byteSize = TIFFStripSize64(this->input) / (this->width * 1LL * SIZESTRIP);
	bitseti = vector<bool>(TIFFNumberOfStrips(this->input), false);
	positions = vector<int>(TIFFNumberOfStrips(this->input), -1);
	megaBytes *= 1000 * 250; // change to bytes
	qtd_store = megaBytes / TIFFStripSize64(this->input); // how much lines can store in ram?
	if(qtd_store == 0){
		cerr << "Low memory" << endl;
		exit(1);
	}
	circular_queue = vector<int>(qtd_store, -1);
	buffer = vector<tdata_t>(qtd_store, NULL);
	queue_pointer = 0;
};

ldouble BufferManager::read(uint32 row, uint32 colunm){
	tstrip_t stripNeeded = row / SIZESTRIP;
	if(!bitseti[stripNeeded]){
		uint32 curr = circular_queue[queue_pointer];
		if(curr != -1){
			bitseti[curr] = false;
		}else{
			buffer[queue_pointer] = _TIFFmalloc(TIFFStripSize64(this->input));
		}
		bitseti[stripNeeded] = true;
		positions[stripNeeded] = queue_pointer;
		TIFFReadEncodedStrip(input, stripNeeded, buffer[queue_pointer], (tsize_t) -1);
		circular_queue[queue_pointer] = stripNeeded;
		queue_pointer++;
		if(queue_pointer >= circular_queue.size())
			queue_pointer = 0;	
	}
	row %= SIZESTRIP;
	colunm = (row * this->width) + colunm;
	return this->readPixel(positions[stripNeeded], colunm);
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
	for(int i = qtd_store - 1; i >= 0; i--){
		if(circular_queue[i] != -1)
			_TIFFfree(buffer[i]);
	}
	TIFFClose(this->input);
}