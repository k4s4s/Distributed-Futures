#!/usr/bin/python

import re

def median(mylist):
	sorts = sorted(mylist)
	length = len(sorts)
	if length == 1:
		return sorts[0]
	elif not length % 2:
		return (sorts[length / 2] + sorts[length / 2 - 1]) / 2.0
	return sorts[length / 2]

def parse_file(f, value_name, index):
	entry_found = False
	value_s = '0'
	for line in f:
		#find beginnig of index entry
		if not entry_found and index in line: 
#			print 'entry found'
			entry_found = True
		if entry_found and value_name in line:
#			print 'value found'
			value_s = line.partition(':')[2]
			value_s = value_s.partition('ms')[0]
			break
		#if we reached start of next entry, exit	
		if entry_found and value_s != '0' and "====" in line: break
#	print value_s
	return float(value_s)

def is_float(s):
    try:
        float(s)
        return True
    except ValueError:
        return False

def write_median_file(filename, iters):
	firstfile = filename.replace("#iter#", str(1))
	f = open(firstfile, 'r')
	outfile = open(filename.replace("#iter#", "median"), 'w')
	node = ""
	go = False
	for line in f:
		if "total time" in line: #start parsing after total time
			go = True;
		if not go: 
			continue
		name = line.partition(':')[0]
		value = line.partition(':')[2]
		value = value.partition('ms')[0]
		values = []
		if(is_float(value)):
			values.append(float(value))
			for i in range(2, iters):
				nextfile = filename.replace("#iter#", str(i))
				nxt_f = open(nextfile, 'r')
				values.append(parse_file(nxt_f, name, node))
			outfile.write("{0}:{1}\n".format(name, median(values)))
		elif "Worker" in name or "MASTER" in name:
			node = name.partition(' ')[2].partition(' ')[0]
			outfile.write(name);

