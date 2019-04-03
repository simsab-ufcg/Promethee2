#include "external_sort.h"

 ExternalSort::ExternalSort(){
     this->path = "";
     this->positions = "";
 }


 ExternalSort::ExternalSort(string path, int megaBytes){
    this->path = path;
    this->megaBytes = megaBytes;
    this->positions = "";
 }


 void ExternalSort::fillBuffer(TIFF *dataset, ldouble buffer[], PixelReader &pr, int line){
	TIFFReadScanline(dataset, pr.buffer, line);
	for(register int i = 0; i < this->width; i++)
		buffer[i] = pr.readPixel(i);
 }

 string ExternalSort::sort(){

    this->input = TIFFOpen(this->path.c_str(), "rm");
    TIFFGetField(input, TIFFTAG_IMAGEWIDTH, &this->width);
    TIFFGetField(input, TIFFTAG_IMAGELENGTH, &this->height);
    TIFFGetField(input, TIFFTAG_SAMPLEFORMAT, &this->sampleFormat);

    this->positions = "positions_" + this->path;

    this->output  = TIFFOpen(this->positions.c_str(), "w8m");
    TIFFSetField(this->output, TIFFTAG_IMAGEWIDTH     , width); 
    TIFFSetField(this->output, TIFFTAG_IMAGELENGTH    , height);
    TIFFSetField(this->output, TIFFTAG_BITSPERSAMPLE  , 64);
    TIFFSetField(this->output, TIFFTAG_SAMPLEFORMAT   , 3);
    TIFFSetField(this->output, TIFFTAG_COMPRESSION    , 1);
    TIFFSetField(this->output, TIFFTAG_PHOTOMETRIC    , 1);
    TIFFSetField(this->output, TIFFTAG_ORIENTATION    , 1);
    TIFFSetField(this->output, TIFFTAG_SAMPLESPERPIXEL, 1);
    TIFFSetField(this->output, TIFFTAG_ROWSPERSTRIP   , 8);
    TIFFSetField(this->output, TIFFTAG_RESOLUTIONUNIT , 1);
    TIFFSetField(this->output, TIFFTAG_XRESOLUTION    , 1);
    TIFFSetField(this->output, TIFFTAG_YRESOLUTION    , 1);
    TIFFSetField(this->output, TIFFTAG_PLANARCONFIG   , PLANARCONFIG_CONTIG   );

    long long cont = 0;

    ldouble output_line[this->width];

    for(int i = 0; i < this->height; i++){
        for(int j = 0; j < this->width; j++){
            output_line[j] = (ldouble)cont++;
        }
        TIFFWriteScanline(this->output, output_line, i);
    }

    TIFFClose(this->output);
    this->output = TIFFOpen(this->positions.c_str(), "rm");

    long long sizeInput = TIFFScanlineSize(this->input) * ((long long)this->height);

    this->bucketAmount = sizeInput / this->megaBytes;
    this->bucketSize = this->height / this->bucketAmount;
    vector < pair < string, string> > buckets;

    for(int i = 0; i < this->height; i+= this->bucketSize){
        buckets.push_back( this->parcialSort(i, min(i + (int)this->bucketSize, this->height)) );
    }

    TIFFClose(this->input);
    TIFFClose(this->output);

    this->input = openFile(this->path, this->width, this->height);
    this->output = openFile(this->positions, this->width, this->height);

    this->k_wayMergesort(buckets);

    TIFFClose(this->input);
    TIFFClose(this->output);
 }

 string ExternalSort::reverse(){
     this->input = TIFFOpen(this->positions.c_str(), "rm");
     this->output = TIFFOpen(this->path.c_str(), "rm");
     vector < pair < string, string> > buckets;

     for(int i = 0; i < this->height; i+= this->bucketSize){
         buckets.push_back( this->parcialSort(i, min(i + (int)this->bucketSize, this->height)) );
     }

     TIFFClose(this->input);
     TIFFClose(this->output);

     this->input = openFile(this->path, this->width, this->height);
     this->output = openFile(this->positions, this->width, this->height);

     this->k_wayMergesort(buckets);

     TIFFClose(this->input);
     TIFFClose(this->output);
 }


 void ExternalSort::k_wayMergesort(vector < pair < string, string> > paths){
     
     TIFF *values[paths.size()];
     TIFF *positions[paths.size()];
     
     tdata_t line = _TIFFmalloc((sizeof (ldouble)) * this->width);
     PixelReader pr = PixelReader(this->sampleFormat, sizeof (ldouble), line);

     ldouble bufferV[paths.size()][this->width];
     ldouble bufferP[paths.size()][this->width];

     typedef pair < pair <ldouble, ldouble >, int > reduct;

     priority_queue <reduct, vector<reduct>, greater<reduct> > pq;

     int pointersX[paths.size()], pointersY[paths.size()];
     int size[paths.size()];

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
         
         fillBuffer(values[i], bufferV[i], pr, pointersY[i]);
         fillBuffer(positions[i], bufferP[i], pr, pointersY[i]++);
         
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

         if(pointersX[i] == this->width && pointersY[i] == size[i])
            continue;

         pq.push({{bufferV[i][pointersX[i]], bufferP[i][pointersX[i]]}, i});
         pointersX[i]++;

         if(pointersX[i] == this->width){
             pointersX[i] = 0;
             if(pointersY[i] < size[i]){
                fillBuffer(values[i], bufferV[i], pr, pointersY[i]);
                fillBuffer(positions[i], bufferP[i], pr, pointersY[i]++);
             }else pointersX[i] = this->width;
         }
        
     }
 }

 pair < string, string> ExternalSort::parcialSort(int start, int end){
    
    vector< pair < ldouble, ldouble > > segment;


    { // main memory economy
        unsigned short byte_size = TIFFScanlineSize(this->input) / this->width;
        tdata_t line = _TIFFmalloc(TIFFScanlineSize(this->input));
        PixelReader pr = PixelReader(this->sampleFormat, byte_size, line);

        ldouble bufferValue[this->width];
        ldouble bufferPosition[this->width];
        
        for(int i = start; i < end; i++){
            fillBuffer(this->input, bufferValue, pr, i);
            fillBuffer(this->output, bufferPosition, pr, i);
            for(int j = 0; j < this->width; j++){ 
                segment.push_back( {bufferValue[j], bufferPosition[j]} );
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
    TIFFSetField(values, TIFFTAG_ROWSPERSTRIP   , 8);
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
    TIFFSetField(positions, TIFFTAG_ROWSPERSTRIP   , 8);
    TIFFSetField(positions, TIFFTAG_RESOLUTIONUNIT , 1);
    TIFFSetField(positions, TIFFTAG_XRESOLUTION    , 1);
    TIFFSetField(positions, TIFFTAG_YRESOLUTION    , 1);
    TIFFSetField(positions, TIFFTAG_PLANARCONFIG   , PLANARCONFIG_CONTIG   );;

    long long pos = 0;

    ldouble *output_value = (ldouble*)malloc(sizeof(ldouble) * this->width);
    ldouble *output_position = (ldouble*)malloc(sizeof(ldouble) * this->width);

    for(int i = 0; i < end - start; i++){
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