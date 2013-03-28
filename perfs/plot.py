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

	benchmark=os.path.splitext(datafile.split('/')[-1])[0];
	program_name = benchmark.split('_');
	out_script=os.path.splitext(datafile)[0]+'.perf'; 
	f=open(out_script, 'w');
	f.write('set title "Scalability of {0} for {1} problem sizes"\n'.format(program_name[0], program_name[1]));
	f.write('set xlabel "number of workers"\n');
	f.write('set ylabel "execution time (ms)"\n');
	f.write('set yrange[0:]\n');
	f.write('set term png\n');

	#for datafile in glob.iglob("*.dat"):
	# Here, you can tweak the output png file name.
	f.write('set output "{output}.png"\n'.format( output=os.path.splitext(datafile)[0] ))
	f.write('plot ')
	f.write('"{file_name}" using 1:2 w l'.format( file_name = datafile )
					+' title "{0}"'.format(program_name[0]));

	f.close()

if __name__ == "__main__":
   main(sys.argv[1:])

