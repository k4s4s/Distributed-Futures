#!/usr/bin/python

import sys, getopt;
import os;

def main(argv):

	test = '';
	procs = '';
	outfile = '';
	try:
		opts, args = getopt.getopt(argv,"hr:n:o:",["pname=","procs=","outfile="])
	except getopt.GetoptError:
		print 'run.py -r <program name> -n <number of processes> -o <output file>'
		sys.exit(2)
	for opt, arg in opts:
		if opt == '-h':
		   print 'run.py -r <program name> -n <number of processes> -o <output file>'
		   sys.exit()
		elif opt in ("-r", "--pname"):
		   test = arg
		elif opt in ("-n", "--procs"):
		   procs = arg
		elif opt in ("-o", "--outfile"):
		   outfile = arg

	print "running "+test+" on "+procs+" processes" 
	os.system("mpirun -np "+procs+" tests/"+test+" > "+outfile);


if __name__ == "__main__":
   main(sys.argv[1:])

