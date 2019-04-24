#include "utils.h"

void logger(string description){
    timespec res;
    clock_gettime(CLOCK_REALTIME, &res);
    cout << res.tv_sec << " " << description.c_str() << " " << getpid() << endl;
}

bool isNaN(ldouble x){
	return x != x;
}

PixelReader::PixelReader() {
	sampleFormat = 0;
	byteSize = 0;
	buffer = NULL;
};

PixelReader::PixelReader(uint16 _sampleFormat, uint8 _byteSize, tdata_t _buffer){
	sampleFormat = _sampleFormat;
	byteSize = _byteSize;
	buffer = _buffer;
};

ldouble PixelReader::readPixel(uint32 colunm){
	ldouble ret = 0;
	switch(sampleFormat){
		case 1:
			{
				uint64 value = 0;
				memcpy(&value, buffer + (colunm * byteSize), byteSize);
				ret = value;
			}
			break;
		case 2:
			{
				int64 value = 0;
				memcpy(&value, buffer + (colunm * byteSize), byteSize);
				ret = value;
			}
			break;
		case 3:
			switch(byteSize){
				case 4:
					{
						float value = 0;
						memcpy(&value, buffer + (colunm * byteSize), byteSize);
						ret = value;
					}
					break;
				case 8:
					{
						double value = 0;
						memcpy(&value, buffer + (colunm * byteSize), byteSize);
						ret = value;
					}
					break;
				case 16:
					{
						long double value = 0;
						memcpy(&value, buffer + (colunm * byteSize), byteSize);
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