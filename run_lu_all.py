#!/usr/bin/python

import os
import re

CSI="\x1B["
reset=CSI+"m"
bold='\033[1m'

#finds the median value given a list of values
def median(mylist):
	sorts = sorted(mylist)
	length = len(sorts)
	if length == 1:
		return sorts[0]
	elif not length % 2:
		return (sorts[length / 2] + sorts[length / 2 - 1]) / 2.0
	return sorts[length / 2]

#parses a file fi
def parse_file(f, value_name):
	content = f.read()
	items=re.findall("{0}.*$".format(value_name), content, re.MULTILINE)
	#print content
	value_s = '0'
	for item in items:
		value_s = item.partition(':')[2]
		value_s = value_s.partition('ms')[0]
	return float(value_s)

def valid_line(f, col):
	content = f.read()
	elems = content.split(' ')
	if(elems[col].split('\n')[0] == '0'):
		return False;
	return True;	

#function to plot scalability graphs
def plot_scalability(apps, title,  xlabel, xlabel_range, llabel, llabel_range, iterations, dump_file_format):
	print "plotting {0}".format(title)
	datafile_n = ""
	perf_n = "" 
	if(len(apps) == 1):
		datafile_n = "perfs/{0}/{0}_{1}_scale_over_{2}.dat".format(apps[0], llabel, xlabel)
		perf_n = "perfs/{0}/{0}_{1}_scale_over_{2}.perf".format(apps[0], llabel, xlabel)
	else:
		datafile_n = "perfs/{0}_{1}_scale_over_{2}.dat".format(apps[-1], llabel, xlabel)
		perf_n = "perfs/{0}_{1}_scale_over_{2}.perf".format(apps[-1], llabel, xlabel)
	datafile = open(datafile_n, 'w')
	for x in xlabel_range:
		datafile.write("{0}".format(x))
		for app in apps:
			for l in llabel_range:
				value_v = [];
				total_time = 0;
				try:
					for it in range(iterations):
						dumpfile_n = dump_file_format.replace("#0", x)
						dumpfile_n = dumpfile_n.replace("#1", l)
						dumpfile_n = dumpfile_n.replace("#2", str(it))
						dumpfile_n = "perfs/{0}/{0}_{1}.dump".format(app, dumpfile_n)
						#get value
						dumpfile = open(dumpfile_n)
						#print dumpfile_n
						value_v.append(parse_file(dumpfile, "total time"))
					#write value to cvs file
					total_time = median(value_v)
				except IOError:
					total_time = 0
				datafile.write(" {0}".format(total_time))

		datafile.write("\n")
	datafile.close();
	#create gnuplot script
	perf=open(perf_n, 'w');
	perf.write('set title "{0}"\n'.format(title))
	perf.write('set xlabel "{0}"\n'.format(xlabel))
	perf.write('set ylabel "execution time (ms)"\n')
	perf.write('set yrange[0:]\n')
	perf.write('set term postscript eps color font 12\n')
	perf.write('set output "{0}.eps"\n'.format(os.path.splitext(datafile_n)[0]))
	perf.write('plot ')
	col=2;
	is_first_entry = True
	for app in apps:
		for l in llabel_range:
			if(valid_line(open(datafile_n, 'r'), col-1) == True):
				if(col != 2 and not is_first_entry):
					perf.write(", \\\n")
				perf.write('"{0}" using 1:{1} w linespoints'.format(datafile_n, col)+' title "{0} with {1}={2}"'.format(app, llabel, l))
				is_first_entry = False;
			col+=1
	perf.write("\n")

	print CSI+"31;40m" + bold + "gnuplot " + perf_n + CSI + "0m"
	os.system("gnuplot "+perf_n)

ncores = ['2', '4', '6', '8', '12']
#ncores = ['12']
psize = ['400', '1200', '2000']
bsize = ['50', '100', '200', '400']
#bsize = ['200']
iterations = 100


#create lu threads dump directory
dumpdir = 'perfs/lu_threads'
if not os.path.exists(dumpdir):
	print "creating "+dumpdir+" directory" 
	os.makedirs(dumpdir);
#run lu thread version
#for s in ['400', '1200']:
#	for b in ['50']:
#		for it in range(iterations):
#			dumpfile = dumpdir+"/lu_threads_problem_size={0}_block_size={1}_ncores=6_{2}.dump".format(s, b, it)
#			print "running ./tests/lu_threads -n{0} -b{1} > {2}".format(s, b, dumpfile)
#			os.system("./tests/lu_threads -n{0} -b{1} > {2}".format(s, b, dumpfile))

#create lu boost dump directory
dumpdir = 'perfs/lu_boost'
if not os.path.exists(dumpdir):
	print "creating "+dumpdir+" directory" 
	os.makedirs(dumpdir);
#run lu boost version
#for s in psize:
#	for b in bsize:
#		for it in range(iterations):
#			dumpfile = dumpdir+"/lu_boost_problem_size={0}_block_size={1}_ncores=6_{2}.dump".format(s, b, it)
#			print "running ./tests/lu_boost -n{0} -b{1} > {2}".format(s, b, dumpfile)
#			os.system("./tests/lu_boost -n{0} -b{1} > {2}".format(s, b, dumpfile))

#create lu plasma dump directory
dumpdir = 'perfs/lu_plasma';
if not os.path.exists(dumpdir):
	print "creating "+dumpdir+" directory"
	os.makedirs(dumpdir)
#plasma version, has fixed block size
#for n in ncores:
#	for s in psize:
#		for it in range(iterations):
#			dumpfile = dumpdir+"/lu_plasma_problem_size={0}_block_size=200_ncores={1}_{2}.dump".format(s, n, it);
#			print "running ./tests/lu_plasma -n{0} -b200 > {1}".format(s, dumpfile)
#			os.system("./tests/lu_plasma -n{0} -a{1} > {2}".format(s, n, dumpfile));

#create lu hpx dump directory
dumpdir = 'perfs/lu_hpx'
if not os.path.exists(dumpdir):
	print "creating "+dumpdir+" directory"
	os.makedirs(dumpdir)
#hpx version
#for n in ncores:
#	for s in psize:
#		for b in bsize:
#			for it in range(iterations):
#				dumpfile = dumpdir+"/lu_hpx_problem_size={0}_block_size={1}_ncores={2}_{3}.dump".format(s, b, n, it)
#				print "running ./tests/lu_hpx --s {0} --b {1} on {2} cores > {3}".format(s, b, n, dumpfile)
#				os.system("./tests/lu_hpx --s {0} --b {1} -t {2} > {3}".format(s, b, n, dumpfile))

#create lu mpi-f dump directory
#dumpdir = 'perfs/lu';
#if not os.path.exists(dumpdir):
#	print "creating "+dumpdir+" directory"; 
#	os.makedirs(dumpdir);
#run lu thread version
#for s in psize:
#	for b in bsize:
#		for it in range(iterations):
#			dumpfile = dumpdir+"/lu_s{0}_b{1}_{2}".format(s, b, it);
#			os.system("mpirun -np 6 ./tests/lu -s{0} -b{1} > {2}".format(s, b, dumpfile));

plot_scalability(["lu_threads"], "Scalability of lu threads version, for different block sizes over problem size",
 								"problem_size", psize, "block_size", bsize, iterations,
								"problem_size=#0_block_size=#1_ncores=6_#2")

plot_scalability(["lu_boost"], "Scalability of lu boost version, for different block sizes over problem size",
 								"problem_size", psize, "block_size", bsize, iterations,
								"problem_size=#0_block_size=#1_ncores=6_#2")

plot_scalability(["lu_hpx"], "Scalability of lu hpx version, for different block sizes over problem size",
								"problem_size", psize, "block_size", bsize, iterations,
								"problem_size=#0_block_size=#1_ncores=6_#2")

plot_scalability(["lu_plasma"], "Scalability of lu plasma version, for different block sizes over problem size",
								"problem_size", psize, "block_size", bsize, iterations,
								"problem_size=#0_block_size=#1_ncores=6_#2")

plot_scalability(["lu_threads", "lu_boost", "lu_hpx", "lu_plasma"], 
								"Scalability of all lu versions, for different block sizes over problem size",
								"problem_size", psize, "block_size", bsize, iterations,
								"problem_size=#0_block_size=#1_ncores=6_#2")

plot_scalability(["lu_hpx"], "Scalability of lu hpx version",
								"ncores", ncores, "problem_size", psize, iterations,
								"problem_size=#1_block_size=200_ncores=#0_#2")

plot_scalability(["lu_plasma"], "Scalability of lu plasma version",
								"ncores", ncores, "problem_size", psize, iterations,
								"problem_size=#1_block_size=200_ncores=#0_#2")

plot_scalability(["lu_hpx", "lu_plasma"], "Scalability of all lu versions",
								"ncores", ncores, "problem_size", psize, iterations,
								"problem_size=#1_block_size=200_ncores=#0_#2")

