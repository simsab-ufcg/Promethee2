#include "distribution_sort.h"

 DistributionSort::DistributionSort(){
     this->path = "";
     this->positions = "";
 }


 DistributionSort::DistributionSort(string path, long long megaBytes){
    this->path = path;
    this->megaBytes = megaBytes;
    this->positions = "";
 }

 void DistributionSort::main(vector <string> args){
    
    bool sweepOnly = hasFlag(args, "-sweep");

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
    if(sweepOnly){
        this->positions = args[1];
        this->sweepMerge(param);
    }else{
        this->positions = args[1];
        this->sort();
    }

 }

 void DistributionSort::sort(){
    this->input = BufferManager(this->positions, TODO_CONSTANT);
    this->width = input.getWidth();
    this->height = input.getHeight();
    this->output = BufferManager(this->path, TODO_CONSTANT);
    int buckets = 1;
    int sizeofBucket = this->height / buckets;
    ldouble output_line[this->width][buckets];
    ldouble position_line[this->width][buckets];
    int output_pointer[buckets];
    TIFF *bucket_tiff[buckets];
    TIFF *bucket_position[buckets];
    for(int i = 0; i < buckets; i++){
        bucket_tiff[i] = TIFFOpen((to_string(i) + "_" + this->path).c_str() , "w8m");
        bucket_position[i] = TIFFOpen((to_string(i) + "_" + this->positions).c_str() , "w8m");
        int bucketLength = min((i + 1) * sizeofBucket, this->height) - (i * sizeofBucket);
        output_pointer[i] = 0;
        TIFFSetField(bucket_tiff[i], TIFFTAG_IMAGEWIDTH     , width); 
        TIFFSetField(bucket_tiff[i], TIFFTAG_IMAGELENGTH    , bucketLength);
        TIFFSetField(bucket_tiff[i], TIFFTAG_BITSPERSAMPLE  , 64);
        TIFFSetField(bucket_tiff[i], TIFFTAG_SAMPLEFORMAT   , 3);
        TIFFSetField(bucket_tiff[i], TIFFTAG_COMPRESSION    , 1);
        TIFFSetField(bucket_tiff[i], TIFFTAG_PHOTOMETRIC    , 1);
        TIFFSetField(bucket_tiff[i], TIFFTAG_ORIENTATION    , 1);
        TIFFSetField(bucket_tiff[i], TIFFTAG_SAMPLESPERPIXEL, 1);
        TIFFSetField(bucket_tiff[i], TIFFTAG_ROWSPERSTRIP   , min(SIZESTRIP, bucketLength));
        TIFFSetField(bucket_tiff[i], TIFFTAG_RESOLUTIONUNIT , 1);
        TIFFSetField(bucket_tiff[i], TIFFTAG_XRESOLUTION    , 1);
        TIFFSetField(bucket_tiff[i], TIFFTAG_YRESOLUTION    , 1);
        TIFFSetField(bucket_tiff[i], TIFFTAG_PLANARCONFIG   , PLANARCONFIG_CONTIG);

        TIFFSetField(bucket_position[i], TIFFTAG_IMAGEWIDTH     , width); 
        TIFFSetField(bucket_position[i], TIFFTAG_IMAGELENGTH    , bucketLength);
        TIFFSetField(bucket_position[i], TIFFTAG_BITSPERSAMPLE  , 64);
        TIFFSetField(bucket_position[i], TIFFTAG_SAMPLEFORMAT   , 3);
        TIFFSetField(bucket_position[i], TIFFTAG_COMPRESSION    , 1);
        TIFFSetField(bucket_position[i], TIFFTAG_PHOTOMETRIC    , 1);
        TIFFSetField(bucket_position[i], TIFFTAG_ORIENTATION    , 1);
        TIFFSetField(bucket_position[i], TIFFTAG_SAMPLESPERPIXEL, 1);
        TIFFSetField(bucket_position[i], TIFFTAG_ROWSPERSTRIP   , min(SIZESTRIP, bucketLength));
        TIFFSetField(bucket_position[i], TIFFTAG_RESOLUTIONUNIT , 1);
        TIFFSetField(bucket_position[i], TIFFTAG_XRESOLUTION    , 1);
        TIFFSetField(bucket_position[i], TIFFTAG_YRESOLUTION    , 1);
        TIFFSetField(bucket_position[i], TIFFTAG_PLANARCONFIG   , PLANARCONFIG_CONTIG);
    }

    for(int i = 0; i < this->height; i++){
        for(int j = 0; j < this->width; j++){
            ldouble position = input.read(i, j);
            ldouble value = output.read(i, j);
            int which_bucket = ((long long)position / this->height) / buckets;
        }
    }
    this->output.close();
    this->input.close();
 }