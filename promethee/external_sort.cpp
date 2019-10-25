#include "external_sort.h"

 ExternalSort::ExternalSort(){
     this->path = "";
     this->positions = "";
 }


 ExternalSort::ExternalSort(string path, long long megaBytes){
    this->path = path;
    this->megaBytes = megaBytes;
    this->positions = "";
 }

 void ExternalSort::main(vector <string> args){
    
    bool kWay_Only = hasFlag(args, "-kway");
    bool isReverse = hasFlag(args, "-reverse");

    this->path = args[0];

	string start = getCmdOption(args, "-start");
	if(!start.size()){
		this->start = -1;
	}else{
		this->start = atoi(start.c_str());
	}

	string end = getCmdOption(args, "-end");
	if(!end.size()){
		this->end = 1e9;
	}else{
		this->end = atoi(end.c_str());
	}

	this->input = BufferManager(this->path, TODO_CONSTANT);
    this->width = input.getWidth();
    this->height = input.getHeight();
    this->input.close();

    this->start = max(0, min(this->start, this->height));
    this->end = max(this->start, min(this->end, this->height));

    vector < pair <string, string> > param;
    
    for(int i = 3; i < args.size(); i+= 2){
        param.push_back({args[i - 1], args[i]});
    }
    if(kWay_Only){
        this->positions = args[1];
        this->k_wayMergesort(param);
    }else{
        if(isReverse){
            this->positions = args[1];
            this->reverse();
        } 
        else this->sort();
    }

 }

 void ExternalSort::fillBuffer(TIFF *dataset, ldouble buffer[], PixelReader &pr, int line, int &size){
    int total;
    TIFFGetField(dataset, TIFFTAG_IMAGELENGTH, &total);
    int idx = 0;
    size = min(size, total - line);
    for(register int a = 0; a < size; a++){
        TIFFReadScanline(dataset, pr.buffer, line + a);
        for(register int i = 0; i < this->width; i++)
		    buffer[idx++] = pr.readPixel(i);
    }

 }

 void ExternalSort::sort(){
    this->input = BufferManager(this->path, TODO_CONSTANT);
    this->width = input.getWidth();
    this->height = input.getHeight();
    this->parcialSort(this->start, this->end);
    this->input.close();
 }

 void ExternalSort::reverse(){
    this->input = BufferManager(this->positions, TODO_CONSTANT);
    this->width = this->input.getWidth();
    this->height = this->input.getHeight();
    this->output = BufferManager(this->path, TODO_CONSTANT);
    vector< pair < ldouble, ldouble > > segment;
    { // memory economy
        ldouble firstValue = this->start * this->width;
        int size = 1;
        int s2 = 1;

        for(int i = this->start; i < this->end; i++){
            for(int j = 0; j < this->width; j++){
                segment.push_back( {this->input.read(i, j), this->output.read(i, j)} );
            }
        }
    }
    this->input.close();
    this->output.close();

    std::sort(segment.begin(), segment.end());

    string nameParcialValue = "values." + to_string(start) + "-" + to_string(end) + "." + this->path;
    string nameParcialPositions = "positions." + to_string(start) + "-" + to_string(end) + "." + this->path;

    TIFF *values = TIFFOpen(nameParcialValue.c_str(), "w8m");
    TIFFSetField(values, TIFFTAG_IMAGEWIDTH     , this->width); 
    TIFFSetField(values, TIFFTAG_IMAGELENGTH    , end - start);
    TIFFSetField(values, TIFFTAG_BITSPERSAMPLE  , 64);
    TIFFSetField(values, TIFFTAG_SAMPLEFORMAT   , 3);
    TIFFSetField(values, TIFFTAG_COMPRESSION    , 1);
    TIFFSetField(values, TIFFTAG_PHOTOMETRIC    , 1);
    TIFFSetField(values, TIFFTAG_ORIENTATION    , 1);
    TIFFSetField(values, TIFFTAG_SAMPLESPERPIXEL, 1);
    TIFFSetField(values, TIFFTAG_ROWSPERSTRIP   , min(SIZESTRIP, end - start));
    TIFFSetField(values, TIFFTAG_RESOLUTIONUNIT , 1);
    TIFFSetField(values, TIFFTAG_XRESOLUTION    , 1);
    TIFFSetField(values, TIFFTAG_YRESOLUTION    , 1);
    TIFFSetField(values, TIFFTAG_PLANARCONFIG   , PLANARCONFIG_CONTIG   );

    TIFF *positions = TIFFOpen(nameParcialPositions.c_str(), "w8m");
    TIFFSetField(positions, TIFFTAG_IMAGEWIDTH     , this->width); 
    TIFFSetField(positions, TIFFTAG_IMAGELENGTH    , end - start);
    TIFFSetField(positions, TIFFTAG_BITSPERSAMPLE  , 64);
    TIFFSetField(positions, TIFFTAG_SAMPLEFORMAT   , 3);
    TIFFSetField(positions, TIFFTAG_COMPRESSION    , 1);
    TIFFSetField(positions, TIFFTAG_PHOTOMETRIC    , 1);
    TIFFSetField(positions, TIFFTAG_ORIENTATION    , 1);
    TIFFSetField(positions, TIFFTAG_SAMPLESPERPIXEL, 1);
    TIFFSetField(positions, TIFFTAG_ROWSPERSTRIP   , min(SIZESTRIP, end - start));
    TIFFSetField(positions, TIFFTAG_RESOLUTIONUNIT , 1);
    TIFFSetField(positions, TIFFTAG_XRESOLUTION    , 1);
    TIFFSetField(positions, TIFFTAG_YRESOLUTION    , 1);
    TIFFSetField(positions, TIFFTAG_PLANARCONFIG   , PLANARCONFIG_CONTIG   );

    long long pos = 0;

    ldouble *output_value = (ldouble*)malloc(sizeof(ldouble) * this->width);
    ldouble *output_position = (ldouble*)malloc(sizeof(ldouble) * this->width);

    for(int i = 0; i < this->end - this->start; i++){
        for(int j = 0; j < this->width; j++){
            pair <ldouble, ldouble> curr = segment[pos++];
            output_value[j] = curr.first;
            output_position[j] = curr.second;
        }
        TIFFWriteScanline(values, output_value, i);
        TIFFWriteScanline(positions, output_position, i);
    }
    TIFFClose(values);
    TIFFClose(positions);
 }


 void ExternalSort::k_wayMergesort(vector < pair < string, string> > paths){
    TIFF *valuesO = TIFFOpen(this->path.c_str(), "w8m");
    TIFFSetField(valuesO, TIFFTAG_IMAGEWIDTH     , width); 
    TIFFSetField(valuesO, TIFFTAG_IMAGELENGTH    , height);
    TIFFSetField(valuesO, TIFFTAG_BITSPERSAMPLE  , 64);
    TIFFSetField(valuesO, TIFFTAG_SAMPLEFORMAT   , 3);
    TIFFSetField(valuesO, TIFFTAG_COMPRESSION    , 1);
    TIFFSetField(valuesO, TIFFTAG_PHOTOMETRIC    , 1);
    TIFFSetField(valuesO, TIFFTAG_ORIENTATION    , 1);
    TIFFSetField(valuesO, TIFFTAG_SAMPLESPERPIXEL, 1);
    TIFFSetField(valuesO, TIFFTAG_ROWSPERSTRIP   , min(SIZESTRIP, height));
    TIFFSetField(valuesO, TIFFTAG_RESOLUTIONUNIT , 1);
    TIFFSetField(valuesO, TIFFTAG_XRESOLUTION    , 1);
    TIFFSetField(valuesO, TIFFTAG_YRESOLUTION    , 1);
    TIFFSetField(valuesO, TIFFTAG_PLANARCONFIG   , PLANARCONFIG_CONTIG   );

    TIFF *positionsO = TIFFOpen(this->positions.c_str(), "w8m");
    TIFFSetField(positionsO, TIFFTAG_IMAGEWIDTH     , width); 
    TIFFSetField(positionsO, TIFFTAG_IMAGELENGTH    , height);
    TIFFSetField(positionsO, TIFFTAG_BITSPERSAMPLE  , 64);
    TIFFSetField(positionsO, TIFFTAG_SAMPLEFORMAT   , 3);
    TIFFSetField(positionsO, TIFFTAG_COMPRESSION    , 1);
    TIFFSetField(positionsO, TIFFTAG_PHOTOMETRIC    , 1);
    TIFFSetField(positionsO, TIFFTAG_ORIENTATION    , 1);
    TIFFSetField(positionsO, TIFFTAG_SAMPLESPERPIXEL, 1);
    TIFFSetField(positionsO, TIFFTAG_ROWSPERSTRIP   , min(SIZESTRIP, height));
    TIFFSetField(positionsO, TIFFTAG_RESOLUTIONUNIT , 1);
    TIFFSetField(positionsO, TIFFTAG_XRESOLUTION    , 1);
    TIFFSetField(positionsO, TIFFTAG_YRESOLUTION    , 1);
    TIFFSetField(positionsO, TIFFTAG_PLANARCONFIG   , PLANARCONFIG_CONTIG   );

     BufferManager values[paths.size()];
     BufferManager positions[paths.size()];

     typedef pair < pair <ldouble, ldouble >, int > reduct;

     priority_queue <reduct, vector<reduct>, greater<reduct> > pq;

     int pointersX[paths.size()], pointersY[paths.size()];
     int size[paths.size()];

     ldouble orderData[this->width];
     ldouble orderPosition[this->width];
     int posX = 0;
     int posY = 0;

     for(int i = 0; i < paths.size(); i++){
         values[i] = BufferManager(paths[i].first.c_str(), TODO_CONSTANT / paths.size());
         positions[i] = BufferManager(paths[i].second.c_str(), TODO_CONSTANT / paths.size());
         pointersX[i] = 0;
         pointersY[i] = 0;
         
         size[i] = values[i].getHeight();
         pq.push( {{values[i].read(pointersX[i], pointersY[i]), positions[i].read(pointersX[i], pointersY[i])}, i} );
         pointersX[i]++;
     }
     while(!pq.empty()){

         reduct curr = pq.top();
         pq.pop();
         orderData[posX] = curr.first.first;
         orderPosition[posX++] = curr.first.second;
         if(posX == this->width){
            TIFFWriteScanline(valuesO, orderData, posY);
            TIFFWriteScanline(positionsO, orderPosition, posY++);
            posX = 0;
         }

         int i = curr.second;

         if(pointersX[i] == this->width && pointersY[i] == size[i])
            continue;

         pq.push({{values[i].read(pointersX[i], pointersY[i]), positions[i].read(pointersX[i], pointersY[i])}, i});
         pointersX[i]++;
         if(pointersX[i] == this->width){
             pointersX[i] = 0;
             if(pointersY[i] < size[i]){
                pointersY[i] ++;
             }else pointersX[i] = this->width;
         }
     }
     for(int i = paths.size() - 1; i >= 0; i--){
	     values[i].close();
         positions[i].close();
     }

     TIFFClose(valuesO);
     TIFFClose(positionsO);
 }

 pair < string, string> ExternalSort::parcialSort(int start, int end){
    
    vector< pair < ldouble, ldouble > > segment;

    { // main memory economy
        unsigned short byte_size = TIFFScanlineSize(this->input) / this->width;
         tdata_t line = _TIFFmalloc(TIFFScanlineSize(this->input));
         PixelReader pr = PixelReader(this->sampleFormat, byte_size, line);
        ldouble firstValue = (long long)this->start * (long long)this->width;
        ldouble bufferValue[this->width];
        int size = 1;
        for(int i = this->start; i < this->end; i++){
            for(int j = 0; j < this->width; j++){
                segment.push_back( {this->input.read(i, j), firstValue++} );
            }
        }
    }
    std::sort(segment.begin(), segment.end());

    string nameParcialValue = "values." + to_string(start) + "-" + to_string(end) + "." + this->path;
    string nameParcialPositions = "positions." + to_string(start) + "-" + to_string(end) + "." + this->path;

    TIFF *values = TIFFOpen(nameParcialValue.c_str(), "w8m");
    TIFFSetField(values, TIFFTAG_IMAGEWIDTH     , this->width); 
    TIFFSetField(values, TIFFTAG_IMAGELENGTH    , end - start);
    TIFFSetField(values, TIFFTAG_BITSPERSAMPLE  , 64);
    TIFFSetField(values, TIFFTAG_SAMPLEFORMAT   , 3);
    TIFFSetField(values, TIFFTAG_COMPRESSION    , 1);
    TIFFSetField(values, TIFFTAG_PHOTOMETRIC    , 1);
    TIFFSetField(values, TIFFTAG_ORIENTATION    , 1);
    TIFFSetField(values, TIFFTAG_SAMPLESPERPIXEL, 1);
    TIFFSetField(values, TIFFTAG_ROWSPERSTRIP   , min(SIZESTRIP, end - start));
    TIFFSetField(values, TIFFTAG_RESOLUTIONUNIT , 1);
    TIFFSetField(values, TIFFTAG_XRESOLUTION    , 1);
    TIFFSetField(values, TIFFTAG_YRESOLUTION    , 1);
    TIFFSetField(values, TIFFTAG_PLANARCONFIG   , PLANARCONFIG_CONTIG   );
    
    TIFF *positions = TIFFOpen(nameParcialPositions.c_str(), "w8m");
    TIFFSetField(positions, TIFFTAG_IMAGEWIDTH     , this->width); 
    TIFFSetField(positions, TIFFTAG_IMAGELENGTH    , end - start);
    TIFFSetField(positions, TIFFTAG_BITSPERSAMPLE  , 64);
    TIFFSetField(positions, TIFFTAG_SAMPLEFORMAT   , 3);
    TIFFSetField(positions, TIFFTAG_COMPRESSION    , 1);
    TIFFSetField(positions, TIFFTAG_PHOTOMETRIC    , 1);
    TIFFSetField(positions, TIFFTAG_ORIENTATION    , 1);
    TIFFSetField(positions, TIFFTAG_SAMPLESPERPIXEL, 1);
    TIFFSetField(positions, TIFFTAG_ROWSPERSTRIP   , min(SIZESTRIP, end - start));
    TIFFSetField(positions, TIFFTAG_RESOLUTIONUNIT , 1);
    TIFFSetField(positions, TIFFTAG_XRESOLUTION    , 1);
    TIFFSetField(positions, TIFFTAG_YRESOLUTION    , 1);
    TIFFSetField(positions, TIFFTAG_PLANARCONFIG   , PLANARCONFIG_CONTIG   );

    long long pos = 0;

    ldouble *output_value = (ldouble*)malloc(sizeof(ldouble) * this->width);
    ldouble *output_position = (ldouble*)malloc(sizeof(ldouble) * this->width);

    for(int i = 0; i < this->end - this->start; i++){
        for(int j = 0; j < this->width; j++){
            pair <ldouble, ldouble> curr = segment[pos++];
            output_value[j] = curr.first;
            output_position[j] = curr.second;
        }
        TIFFWriteScanline(values, output_value, i);
        TIFFWriteScanline(positions, output_position, i);
    }

    free(output_value);
    free(output_position);
    TIFFClose(values);
    TIFFClose(positions);
    return {nameParcialValue, nameParcialPositions};
 }
