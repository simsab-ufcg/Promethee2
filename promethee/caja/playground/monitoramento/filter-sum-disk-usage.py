#!/usr/bin/env python
# -*- coding: utf-8 -*-

# Script to filter and sum the instances of process made by RScript in the same second

import sys

def main():
	args = sys.argv

	disk_usage_file = open(args[1])
	filtered_disk_usage = open(args[2], "w+")

	wanted_head = ["TIMESTAMP", "KB_READ/S", "KB_WRITE/S\n"]
	a_head = disk_usage_file.readline().rsplit(", ")
	head = ""
	for h in a_head:
		if(h in wanted_head):
			head += h + ", "

	head = head[0 : len(head) - 2]

	filtered_disk_usage.write(head)

	filtered_disk_usage_dict = {}

	for line in disk_usage_file:
		a_line = line.rsplit(", ")

		ts = a_line[0]

		cmd = a_line[2]

		kb_rw = [float(a_line[3]), float(a_line[4])]

		if(cmd == "R"):
			if(ts in filtered_disk_usage_dict.keys()):
				filtered_disk_usage_dict[ts][0] += kb_rw[0]
				filtered_disk_usage_dict[ts][1] += kb_rw[1]
			else:
				filtered_disk_usage_dict[ts] = kb_rw

	filtered_disk_usage_list = list(filtered_disk_usage_dict.keys())
	filtered_disk_usage_list.sort()

	for x in filtered_disk_usage_list:
		filtered_disk_usage.write(x + ", " + str(filtered_disk_usage_dict[x][0]) + ", " + str(filtered_disk_usage_dict[x][1]) + "\n")

	disk_usage_file.close()
	filtered_disk_usage.close()

main()