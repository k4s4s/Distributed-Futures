#!/usr/bin/python

import sys, getopt;
import os;
#import mmap;
import re;

def main(argv):

	#we just keep the argument configuration here
	applications = {'lu':{'test':{'-n8 -b2 -v'}, 
												'small':{'-n512 -b32'}}};
	application = '';
	psize = 'test';
	nproc_range = [3, 5, 7, 9, 11];
	iterations = 1;

	try:
		opts, args = getopt.getopt(argv,"hr:s:",["pname=","problem_size="])
	except getopt.GetoptError:
		print 'run.py -r <program name> -s <problem size>'
		sys.exit(2)
	for opt, arg in opts:
		if opt == '-h':
			print 'run.py -r <program name> -s <problem size>'
			sys.exit()
		elif opt in ("-r", "--pname"):
			application = arg
		elif opt in ("-s", "--problem size"):
			psize = arg		

	directory = 'perfs/'+application
	if not os.path.exists(directory):
		os.makedirs(directory)
	#plot scalability graph for application
	print "plotting scalability graph for "+application+" with "+psize+"problem size";
	datafile = directory+'/'+application+'_'+psize+'_scalability.dat'
	dat_file = open(datafile, 'w');
	for nproc in nproc_range:
		total_time = 0;
		for it in range(iterations):
			outfile = directory+'/'+application+'_'+psize+'_{0}'.format(nproc)+'_{0}.dump'.format(it)
			#get value
			f = open(outfile);
			s = f.read();
			items=re.findall("total time.*$", s, re.MULTILINE);
			for item in items:
				value_s = item.partition(':')[2];
				value_s = value_s.partition('ms')[0];
				total_time += float(value_s);
			#write value to cvs file
			total_time = total_time/iterations;
			dat_file.write("{0} {1}".format(nproc, total_time));
		dat_file.write("\n");
	#create gnuplot script
	os.system("python perfs/plot.py -d "+datafile);
	perf_file = "{output}.perf".format(output=os.path.splitext(datafile)[0]);
	print "gnuplot "+perf_file;
	#os.system(gnuplot "+perf_file);
	
if __name__ == "__main__":
   main(sys.argv[1:])

