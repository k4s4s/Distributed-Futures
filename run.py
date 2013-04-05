#!/usr/bin/python

import sys, getopt;
import os;

def main(argv):

	#we just keep the argument configuration here
	applications = {'lu':{'test':{'-n8 -b2 -v'}, 
												'small':{'-n512 -b32'},
												'medium':{'-n1980 -b200'},
												'medium_b50':{'-n1980 -b50'},
												'medium_b100':{'-n1980 -b100'},
												'medium_b400':{'-n1980 -b400'}},
									'lu_plasma':{'test':{'-n8 -b2 -v'}, 
												'small':{'-n512 -b32'},
												'medium':{'-n1980'},
									'lu_threads':{'test':{'-n8 -b2 -v'}, 
												'small':{'-n512 -b32'},
												'medium':{'-n1980 -b200'},
												'medium_b50':{'-n1980 -b50'},
												'medium_b100':{'-n1980 -b100'},
												'medium_b400':{'-n1980 -b400'}}};
	application = '';
	psize = 'test';
	mpi_version = True;
	nproc_range = [3, 5, 7, 9, 11];
	iterations = 1;

	try:
		opts, args = getopt.getopt(argv,"hdp:s:r:",["mpi_off","pname=","problem_size=","repeats="])
	except getopt.GetoptError:
		print 'run.py -r <program name> -s <problem size>'
		sys.exit(2)
	for opt, arg in opts:
		if opt == '-h':
			print 'run.py -r <program name> -s <problem size>'
			sys.exit();
		elif opt in ("-d", "--mpi_off"):
			mpi_version = False
		elif opt in ("-p", "--pname"):
			application = arg;
		elif opt in ("-s", "--problem size"):
			psize = arg;
		elif opt in ("-r", "--repeats"):
			iterations = int(arg);			
	#create dump directory
	directory = 'perfs/'+application;
	if not os.path.exists(directory):
		os.makedirs(directory)
	#run application
	if(mpi_version):
		for nproc in nproc_range:
			for it in range(iterations):
				print "running "+application+" with "+psize+" problem size on {0}".format(nproc)+" processes"
				outfile = directory+'/'+application+'_'+psize+'_{0}'.format(nproc)+'_{0}.dump'.format(it)
				args = applications[application][psize];
				os.system("mpirun -np {0}".format(nproc)+" tests/{0} {1}".format(application,"".join(args))+" > "+outfile);
	else:
		print "Nothing to do"
		sys.exit();

if __name__ == "__main__":
   main(sys.argv[1:])

