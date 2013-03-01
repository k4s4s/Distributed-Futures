#!/usr/bin/env python

import sys, getopt;
import os;

def main(argv):

	datafile = '';
	try:
		opts, args = getopt.getopt(argv,"hd:",["datafile="])
	except getopt.GetoptError:
		print 'plot.py -d <data file>'
		sys.exit(2)
	for opt, arg in opts:
		if opt == '-h':
		   print 'plot.py -d <data file>'
		   sys.exit()
		elif opt in ("-d", "--datafile"):
		   datafile = arg

	program_name=os.path.splitext(datafile.partition('/')[2])[0]
	out_script=os.path.splitext(datafile)[0]+'.perf' 
	f=open(out_script, 'w')
	f.write('set xlabel "number of workers"\n') 
	f.write('set ylabel "execution time (ms)"\n')
	f.write('set yrange[0:]\n')
	f.write('set term png\n')

	#for datafile in glob.iglob("*.dat"):
	# Here, you can tweak the output png file name.
	f.write('set output "{output}.png"\n'.format( output=os.path.splitext(datafile)[0] ))
	f.write('plot ')
	f.write('"{file_name}" using 1:2 w l'.format( file_name = datafile )+' title"'+program_name+'"')

	f.close()

if __name__ == "__main__":
   main(sys.argv[1:])

