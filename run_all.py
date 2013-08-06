#!/usr/bin/python

import os
import plot
import mean

def range2string(start, end, step):
	str_range = []
	for i in xrange(start, end, step):
		str_range.append(str(i));
	return str_range;

def gen_stack(num_of_workers):
	stacks = ["MASTER"]
	for i in xrange(1, num_of_workers):
		stacks.append("Worker#"+str(i))
	return stacks

apps = {'fib', 'quicksort', 'lu'}

workloads = {'large'}

args = {'fibonacci':{'large':'-a45 -w30'}, 
				'quicksort':{'large':'-n10000000 -w100000'}, 
				'lu':{'large':'-n2000 -b200'}}

seq_times = {	'fibonacci':2.3,
							'quicksort':2.9,
							'lu':1228}

ncores = ['1']
ncores.extend(range2string(4, 65, 4));
iterations = 1;

#generate file with median values
for app in apps:
	for n in ncores:
		mean.write_median_file("stats/dumps_beowulf/{0}_p{1}_#iter#.txt".format(app, n), iterations)

#for app in apps:
	#create lu threads dump directory
#	dumpdir = 'perfs/{0}'.format(app)
#	if not os.path.exists(dumpdir):
#		print "creating "+dumpdir+" directory" 
#		os.makedirs(dumpdir);
	#run app
#	for workload in workloads:
#		for n in ncores:
#			for it in range(iterations):
#				dumpfile = dumpdir+"/{0}_load={1}_ncores={2}_{3}.dump".format(app, workload, n, it)
#				print "running ./tests/{0} {1} > {2}".format(app, args[app][workload], dumpfile)
#				os.system("mpirun -np {3} ./tests/{0} {1} > {2}".format(app, args[app][workload], dumpfile, n))



plot.plot_line_graph(	'scalability over number of cores', 'ncores', 'execution time (ms)', 'load',
											apps, ncores, workloads, 	
											'total time', 'stats/apps_scalability',
											'stats/dumps_beowulf/#app#_p#xlabel#_median', 'raw')

plot.plot_line_graph(	'speedup over number of cores', 'ncores', 'execution time (ms)', 'load',
											apps, ncores, workloads, 	
											'total time', 'stats/apps_speedup',
											'stats/dumps_beowulf/#app#_p#xlabel#_median', 'speedup')

breakdowns = {'init time':{'initialization_time'}, 
							'job issue time':{'job_issue_time'},
							'user code time':{'user_code_execution_time'},
							'idle time':{'idle_time'},
							'rest time':{	'total_execution_time', '-job_issue_time', 
														'-initialization_time', '-finalization_time',
														'-idle_time', '-user_code_execution_time'},
							'finalization time':{'finalization_time'}}

stacks = gen_stack(8)

plot.plot_bar_graph('application breakdowns for 8 cores', apps, stacks, breakdowns,
										'benchmarks', 'execution time (ms)',
										'stats/app_breakdowns',
										'stats/dumps_beowulf/#cluster#_p8_1.txt', 'stackcluster')

#plot.plot_line_graph(	'scalability over number of cores', 'ncores', 'execution time (ms)', 'load',
#											apps, ncores, workloads, iterations, 	
#											'total time', 'stats/apps_scalability',
#											'stats/dump/#app#_n=#xlabel#_#iter#', 'raw')

#plot.plot_line_graph(	'application speedup', 'ncores', 'speedup', 'load',
#											apps, ncores, workloads, iterations, 
#											'total time', 'perfs/apps_speedup',
#											'perfs/#app#/#app#_load=#llabel#_ncores=#xlabel#_#iter#', 'speedup')

#plot.plot_line_graph('application relative_slowdown', 'ncores', 'relative slowdown', 'load',
#											apps, ncores, workloads, iterations, 
#											'total time', 'perfs/apps_relative_slowdown',
#											'perfs/#app#/#app#_load=#llabel#_ncores=#xlabel#_#iter#', 'relative_slowdown')

#breakdowns = ['total job issue time', 
#							'job execution time',
#							'idle time',
#							'rest of time']

#stacks = ['master', 'worker#1', 'worker#2', 'worker#3', 'worker#4', 'worker#5']

#plot.plot_bar_graph('application breakdowns for 6 cores', apps, stacks, breakdowns,
#										'benchmarks', 'execution time (ms)', iterations,
#										'perfs/app_breakdowns',
#										'perfs/#cluster#/#cluster#_load=large_ncores=6_#iter#.dump', 'stackcluster')

#create ping pong dump directory
dumpdir = 'perfs/{0}'.format('ping_pong')
if not os.path.exists(dumpdir):
	print "creating "+dumpdir+" directory" 
	os.makedirs(dumpdir);
#run app
#for it in range(iterations):
#	dumpfile = dumpdir+"/ping_pong_{0}.dump".format(it)
#	print "running ./tests/ping_pong > {0}".format(dumpfile)
#	os.system("mpirun -np 2 ./tests/ping_pong > {0}".format(dumpfile))

args_num = ['1', '2', '4', '6', '8']
args_size = {	'1':'1200000', 
							'2':'600000', 
							'4':'300000',
							'6':'200000',
							'8':'150000'}
arg_sizes = ['1', '100', '1000', '10000', '100000', '1000000']

#create bench_issue_scalar dump directory
dumpdir = 'perfs/{0}'.format('bench_issue_scalar')
if not os.path.exists(dumpdir):
	print "creating "+dumpdir+" directory" 
	os.makedirs(dumpdir);
#run app
#for argn in args_num:
#	for it in range(iterations):
#		dumpfile = dumpdir+"/bench_issue_scalar_argn={1}_size=1_{0}.dump".format(it, argn)
#		print "running ./tests/bench_issue_scalar -a{1} > {0}".format(dumpfile, argn)
#		os.system("mpirun -np 2 ./tests/bench_issue_scalar -a{1} > {0}".format(dumpfile, argn))

#create bench_issue_scalar dump directory
dumpdir = 'perfs/{0}'.format('bench_issue_container')
if not os.path.exists(dumpdir):
	print "creating "+dumpdir+" directory" 
	os.makedirs(dumpdir);
#run app
#for argn in args_num:
#	for it in range(iterations):
#		dumpfile = dumpdir+"/bench_issue_container_argn={1}_size=1_{0}.dump".format(it, argn)
#		print "running ./tests/bench_issue_container -a{1} -n1 > {0}".format(dumpfile, argn)
#		os.system("mpirun -np 2 ./tests/bench_issue_container -a{1} -n1 > {0}".format(dumpfile, argn))

#for argn in args_num:
#	for it in range(iterations):
#		dumpfile = dumpdir+"/bench_issue_container_argn={1}_size=large_{0}.dump".format(it, argn, args_size[argn])
#		print "running ./tests/bench_issue_container -a{1} -n{2} > {0}".format(dumpfile, argn, args_size[argn])
#		os.system("mpirun -np 2 ./tests/bench_issue_container -a{1} -n{2} > {0}".format(dumpfile, argn, args_size[argn]))

#for sz in arg_sizes:
#	for it in range(iterations):
#		dumpfile = dumpdir+"/bench_issue_container_argn=1_size={1}_{0}.dump".format(it, sz)
#		print "running ./tests/bench_issue_container -a1 -n{1} > {0}".format(dumpfile, sz)
#		os.system("mpirun -np 2 ./tests/bench_issue_container -a1 -n{1} > {0}".format(dumpfile, sz))

num_of_jobs = [1, 100];

#create bench_issue_mul dump directory
dumpdir = 'perfs/{0}'.format('bench_issue_mul')
if not os.path.exists(dumpdir):
	print "creating "+dumpdir+" directory" 
	os.makedirs(dumpdir);
#run app
#for n in ncores:
#	for it in range(iterations):
#			for jobs in num_of_jobs:
#				dumpfile = dumpdir+"/bench_issue_mul_cores={1}_jobs={2}_{0}.dump".format(it, n, jobs)
#				print "running ./tests/bench_issue_container -a1 -n1200000 -w{1}  > {0}".format(dumpfile, jobs)
#				os.system("mpirun -np 2 ./tests/bench_issue_container -a1 -n1200000 -w{1}  > {0}".format(dumpfile, jobs))

#for n in ncores:
#	for it in range(iterations):
#		for jobs in num_of_jobs
#		dumpfile = dumpdir+"/bench_issue_mul_cores={1}_jobs={2}_{0}.dump".format(it, n, jobs)
#		print "running ./tests/bench_issue_container -a1 -n1200000 -w{1}  > {0}".format(dumpfile, jobs)
#		os.system("mpirun -np 2 ./tests/bench_issue_container -a1 -n1200000 -w{1}  > {0}".format(dumpfile, jobs))


#plot.plot_bar_graph('scalar and container task issue time comparison', 
#										args_num, ['bench_issue_container', 'bench_issue_scalar'], ['total job issue time'],
#										'number of arguments', 'job issue time (ms)', iterations,
#										'perfs/job_issue_time_scalar_vs_container_bars',
#										'perfs/#stack#/#stack#_argn=#cluster#_size=1_#iter#.dump', 'cluster')

#plot.plot_line_graph(	'task issue time for different number of arguments, distributing same size', 
#											'number of arguments', 'execution time (ms)', 'total argument size',
#											['bench_issue_container'], args_num, ['1200000'], iterations,
#											'total job issue time', 'perfs/job_issue_time_different_argnums_same_size', 
#											'perfs/#app#/#app#_argn=#xlabel#_size=large_#iter#', 'raw')

#plot.plot_bar_graph('task issue time for different number of arguments, distributing same size', 
#										args_num, ['bench_issue_container'], ['total job issue time'],
#										'number of arguments', 'job issue time (ms)', iterations,
#										'perfs/job_issue_time_different_argnums_same_size_bars',
#										'perfs/#stack#/#stack#_argn=#cluster#_size=large_#iter#.dump', 'simple')

#plot.plot_bar_graph('task issue time for different argument sizes', 
#										arg_sizes, ['bench_issue_container'], ['total job issue time'],
#										'argument sizes', 'job issue time (ms)', iterations,
#										'perfs/job_issue_time_different_argsizes',
#										'perfs/#stack#/#stack#_argn=1_size=#cluster#_#iter#.dump', 'simple')

#plot.plot_line_graph(	'task issue time depending on number of workers', 
#											'number of arguments', 'execution time (ms)', 'total argument size',
#											['bench_issue_container'], args_num, ['1200000'], iterations,
#											'total job issue time', 'perfs/job_issue_time_different_argnums_same_size', 
#											'perfs/#app#/#app#_argn=#xlabel#_size=large_#iter#', 'raw')

print 'DONE!'

