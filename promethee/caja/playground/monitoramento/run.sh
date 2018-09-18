#!/bin/bash

CPU_FILE_PATH=$1
MEM_FILE_PATH=$2
DISK_FILE_PATH=$3
PROCTIMES_FILE_PATH=$4

RESULT_DISK_FILE_PATH=$(pwd)/global_disk_usage.csv

echo "Filtering and summing disk usage data..."

python filter-sum-disk-usage.py $DISK_FILE_PATH $RESULT_DISK_FILE_PATH

echo "Generating graphics..."

Rscript plot-usages.r $CPU_FILE_PATH $MEM_FILE_PATH $RESULT_DISK_FILE_PATH $PROCTIMES_FILE_PATH 