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

	this->input = TIFFOpen(this->path.c_str(), "rm");
    TIFFGetField(input, TIFFTAG_SAMPLEFORMAT, &this->sampleFormat);
    TIFFGetField(input, TIFFTAG_IMAGEWIDTH, &this->width);
	TIFFGetField(input, TIFFTAG_IMAGELENGTH, &this->height);
    TIFFClose(this->input);

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
    this->input = TIFFOpen(this->path.c_str(), "rm");
    TIFFGetField(input, TIFFTAG_IMAGEWIDTH, &this->width);
    TIFFGetField(input, TIFFTAG_IMAGELENGTH, &this->height);
    TIFFGetField(input, TIFFTAG_SAMPLEFORMAT, &this->sampleFormat);
    this->parcialSort(this->start, this->end);
    TIFFClose(this->input);
 }

 void ExternalSort::reverse(){
    this->input = TIFFOpen(this->positions.c_str(), "rm");
    TIFFGetField(input, TIFFTAG_IMAGEWIDTH, &this->width);
    TIFFGetField(input, TIFFTAG_IMAGELENGTH, &this->height);
    TIFFGetField(input, TIFFTAG_SAMPLEFORMAT, &this->sampleFormat);
    this->output = TIFFOpen(this->path.c_str(), "rm");
    vector< pair < ldouble, ldouble > > segment;
    { // main memory economy
        unsigned short byte_size = TIFFScanlineSize(this->input) / this->width;
        tdata_t line = _TIFFmalloc(TIFFScanlineSize(this->input));
        PixelReader pr = PixelReader(this->sampleFormat, byte_size, line);
        ldouble bufferValue[this->width];
        ldouble bufferNumber[this->width];
        ldouble firstValue = this->start * this->width;
        int size = 1;
        int s2 = 1;

        for(int i = this->start; i < this->end; i++){
            fillBuffer(this->input, bufferValue, pr, i, size);
            fillBuffer(this->output, bufferNumber, pr, i, s2);
            for(int j = 0; j < this->width; j++){ 
                segment.push_back( {bufferValue[j], bufferNumber[j]} );
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
    TIFFSetField(values, TIFFTAG_ROWSPERSTRIP   , 1);
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
    TIFFSetField(positions, TIFFTAG_ROWSPERSTRIP   , 1);
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

    TIFFClose(this->input);
    TIFFClose(this->output);
 }


 void ExternalSort::k_wayMergesort(vector < pair < string, string> > paths){
    this->input = TIFFOpen(this->path.c_str(), "w8m");
    TIFFSetField(this->input, TIFFTAG_IMAGEWIDTH     , width); 
    TIFFSetField(this->input, TIFFTAG_IMAGELENGTH    , height);
    TIFFSetField(this->input, TIFFTAG_BITSPERSAMPLE  , 64);
    TIFFSetField(this->input, TIFFTAG_SAMPLEFORMAT   , 3);
    TIFFSetField(this->input, TIFFTAG_COMPRESSION    , 1);
    TIFFSetField(this->input, TIFFTAG_PHOTOMETRIC    , 1);
    TIFFSetField(this->input, TIFFTAG_ORIENTATION    , 1);
    TIFFSetField(this->input, TIFFTAG_SAMPLESPERPIXEL, 1);
    TIFFSetField(this->input, TIFFTAG_ROWSPERSTRIP   , 1);
    TIFFSetField(this->input, TIFFTAG_RESOLUTIONUNIT , 1);
    TIFFSetField(this->input, TIFFTAG_XRESOLUTION    , 1);
    TIFFSetField(this->input, TIFFTAG_YRESOLUTION    , 1);
    TIFFSetField(this->input, TIFFTAG_PLANARCONFIG   , PLANARCONFIG_CONTIG   );

    this->output = TIFFOpen(this->positions.c_str(), "w8m");
    TIFFSetField(this->output, TIFFTAG_IMAGEWIDTH     , width); 
    TIFFSetField(this->output, TIFFTAG_IMAGELENGTH    , height);
    TIFFSetField(this->output, TIFFTAG_BITSPERSAMPLE  , 64);
    TIFFSetField(this->output, TIFFTAG_SAMPLEFORMAT   , 3);
    TIFFSetField(this->output, TIFFTAG_COMPRESSION    , 1);
    TIFFSetField(this->output, TIFFTAG_PHOTOMETRIC    , 1);
    TIFFSetField(this->output, TIFFTAG_ORIENTATION    , 1);
    TIFFSetField(this->output, TIFFTAG_SAMPLESPERPIXEL, 1);
    TIFFSetField(this->output, TIFFTAG_ROWSPERSTRIP   , 1);
    TIFFSetField(this->output, TIFFTAG_RESOLUTIONUNIT , 1);
    TIFFSetField(this->output, TIFFTAG_XRESOLUTION    , 1);
    TIFFSetField(this->output, TIFFTAG_YRESOLUTION    , 1);
    TIFFSetField(this->output, TIFFTAG_PLANARCONFIG   , PLANARCONFIG_CONTIG   );

     TIFF *values[paths.size()];
     TIFF *positions[paths.size()];
     
     int sizeMultiplier = 6;

     tdata_t line = _TIFFmalloc((sizeof (ldouble)) * this->width);
     PixelReader pr = PixelReader(this->sampleFormat, sizeof (ldouble), line);

     ldouble *bufferV[paths.size()];
     ldouble *bufferP[paths.size()];


     for(int i = 0; i < paths.size(); i++){
         bufferV[i] = (ldouble*)malloc(sizeof (ldouble) * this->width * sizeMultiplier);
         bufferP[i] = (ldouble*)malloc(sizeof (ldouble) * this->width * sizeMultiplier);
     }

     typedef pair < pair <ldouble, ldouble >, int > reduct;

     priority_queue <reduct, vector<reduct>, greater<reduct> > pq;

     int pointersX[paths.size()], pointersY[paths.size()];
     int size[paths.size()];
     int endBuffer[paths.size()];

     ldouble orderData[this->width];
     ldouble orderPosition[this->width];
     int posX = 0;
     int posY = 0;

     for(int i = 0; i < paths.size(); i++){
         values[i] = TIFFOpen(paths[i].first.c_str(), "rm");
         positions[i] = TIFFOpen(paths[i].second.c_str(), "rm");
         pointersX[i] = 0;
         pointersY[i] = 0;
         
         TIFFGetField(values[i], TIFFTAG_IMAGELENGTH, &size[i]);

         endBuffer[i] = sizeMultiplier;
         
         fillBuffer(values[i], bufferV[i], pr, pointersY[i], endBuffer[i]);
         fillBuffer(positions[i], bufferP[i], pr, pointersY[i], endBuffer[i]);
         pointersY[i] += endBuffer[i];

         pq.push( {{bufferV[i][pointersX[i]], bufferP[i][pointersX[i]]}, i} );
         pointersX[i]++;
     }
     while(!pq.empty()){

         reduct curr = pq.top();
         pq.pop();
         orderData[posX] = curr.first.first;
         orderPosition[posX++] = curr.first.second;
         if(posX == this->width){

            TIFFWriteScanline(this->input, orderData, posY);
            TIFFWriteScanline(this->output, orderPosition, posY++);
            posX = 0;

         }

         int i = curr.second;

         if((pointersX[i] == (this->width * endBuffer[i])) && pointersY[i] == size[i])
            continue;

         pq.push({{bufferV[i][pointersX[i]], bufferP[i][pointersX[i]]}, i});
         pointersX[i]++;

         if(pointersX[i] == (this->width * endBuffer[i])){
             pointersX[i] = 0;
             if(pointersY[i] < size[i]){

                fillBuffer(values[i], bufferV[i], pr, pointersY[i], endBuffer[i]);
                fillBuffer(positions[i], bufferP[i], pr, pointersY[i], endBuffer[i]);
                pointersY[i] += endBuffer[i];

             }else pointersX[i] = this->width * endBuffer[i];
         }
        
     }
     for(int i = paths.size() - 1; i >= 0; i--){
         free(bufferV[i]);
         free(bufferP[i]);
	     TIFFClose(values[i]);
         TIFFClose(positions[i]);
     }

     TIFFClose(this->input);
     TIFFClose(this->output);
 }

 pair < string, string> ExternalSort::parcialSort(int start, int end){
    
    vector< pair < ldouble, ldouble > > segment;

    { // main memory economy
        unsigned short byte_size = TIFFScanlineSize(this->input) / this->width;
        tdata_t line = _TIFFmalloc(TIFFScanlineSize(this->input));
        PixelReader pr = PixelReader(this->sampleFormat, byte_size, line);
        ldouble bufferValue[this->width];
        ldouble firstValue = this->start * this->width;
        int size = 1;

        for(int i = this->start; i < this->end; i++){
            fillBuffer(this->input, bufferValue, pr, i, size);
            for(int j = 0; j < this->width; j++){ 
                segment.push_back( {bufferValue[j], firstValue++} );
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
    TIFFSetField(values, TIFFTAG_ROWSPERSTRIP   , 1);
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
    TIFFSetField(positions, TIFFTAG_ROWSPERSTRIP   , 1);
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
