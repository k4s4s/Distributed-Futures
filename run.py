#!/usr/bin/python

import sys, getopt;
import os;

def main(argv):

	test = '';
	procs = '';
	extra_args = '';
	try:
		opts, args = getopt.getopt(argv,"hr:n:a:",["pname=","procs=","extra_args="])
	except getopt.GetoptError:
		print 'run.py -r <program name> -n <number of processes> -a <extra args>'
		sys.exit(2)
	for opt, arg in opts:
		if opt == '-h':
		   print 'run.py -r <program name> -n <number of processes> -a <extra args>'
		   sys.exit()
		elif opt in ("-r", "--pname"):
		   test = arg
		elif opt in ("-n", "--procs"):
		   procs = arg
		elif opt in ("-a", "--extra_args"):
		   extra_args = arg

	#f=open()
	output = 'perf/'+test+'/'+procs+'.dat'

	print "running "+test+" on "+procs+" processes" 
	os.system("mpirun -np "+procs+" tests/"+test+" "+extra_args+" > "+output);


if __name__ == "__main__":
   main(sys.argv[1:])

