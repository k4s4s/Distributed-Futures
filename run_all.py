#!/usr/bin/python

import os
import plot

#apps = {'power':{'small':'-a2 -n10', 'medium':'-a2 -n1000'}}

apps = {'fibonacci', 'quicksort', 'lu'}

workloads = {'large'}

args = {'power':{'large':'-a2 -n10000'}, 
				'factorial':{'large':'-n10000'},
				'fibonacci':{'large':'-a15'}, 
				'quicksort':{'large':'-n1000'}, 
				'lu':{'large':'-n2000 -b200'}}

seq_times = {	'fibonacci':2.3,
							'quicksort':2.9,
							'lu':1228}

ncores = ['1', '2', '3', '4', '5', '6', '7', '8', '9', '10', '11', '12']
#ncores = ['1']
iterations = 20;

#for app in apps:
#	#create lu threads dump directory
#	dumpdir = 'perfs/{0}'.format(app)
#	if not os.path.exists(dumpdir):
#		print "creating "+dumpdir+" directory" 
#		os.makedirs(dumpdir);
#	#run app
#	for workload in workloads:
#		for n in ncores:
#			for it in range(iterations):
#				dumpfile = dumpdir+"/{0}_load={1}_ncores={2}_{3}.dump".format(app, workload, n, it)
#				print "running ./tests/{0} {1} > {2}".format(app, args[app][workload], dumpfile)
#				os.system("mpirun -np {3} ./tests/{0} {1} > {2}".format(app, args[app][workload], dumpfile, n))



plot.plot_line_graph(	'scalability over number of cores', 'ncores', 'execution time (ms)', 'load',
											apps, ncores, workloads, iterations, 	
											'total time', 'perfs/apps_scalability',
											'perfs/#app#/#app#_load=#llabel#_ncores=#xlabel#_#iter#', 'raw')

plot.plot_line_graph(	'application speedup', 'ncores', 'speedup', 'load',
											apps, ncores, workloads, iterations, 
											'total time', 'perfs/apps_speedup',
											'perfs/#app#/#app#_load=#llabel#_ncores=#xlabel#_#iter#', 'speedup')

plot.plot_line_graph('application relative_slowdown', 'ncores', 'relative slowdown', 'load',
											apps, ncores, workloads, iterations, 
											'total time', 'perfs/apps_relative_slowdown',
											'perfs/#app#/#app#_load=#llabel#_ncores=#xlabel#_#iter#', 'relative_slowdown')

breakdowns = ['total job issue time', 
							'job execution time',
							'idle time',
							'rest']

stacks = ['master', 'worker#1', 'worker#2', 'worker#3', 'worker#4', 'worker#5']

plot.plot_bar_graph('application breakdowns for 6 cores', apps, stacks, breakdowns,
										'benchmarks', 'execution time (ms)', iterations,
										'perfs/#cluster#/#cluster#_load=large_ncores=6_#iter#.dump', 'stackcluster')

##create ping pong dump directory
#dumpdir = 'perfs/{0}'.format('ping_pong')
#if not os.path.exists(dumpdir):
#	print "creating "+dumpdir+" directory" 
#	os.makedirs(dumpdir);
##run app
#for it in range(iterations):
#	dumpfile = dumpdir+"/ping_pong_{0}.dump".format(it)
#	print "running ./tests/ping_pong > {0}".format(dumpfile)
#	os.system("mpirun -np 2 ./tests/ping_pong > {0}".format(dumpfile))

args_num = ['1', '2', '4', '8']
args_size = {	'1':'1000000', 
							'2':'500000', 
							'4':'250000', 
							'8':'125000'}

#create bench_issue_scalar dump directory
dumpdir = 'perfs/{0}'.format('bench_issue_scalar')
if not os.path.exists(dumpdir):
	print "creating "+dumpdir+" directory" 
	os.makedirs(dumpdir);
#run app
for argn in args_num:
	for it in range(iterations):
		dumpfile = dumpdir+"/bench_issue_scalar_argn={1}_size=1_{0}.dump".format(it, argn)
		print "running ./tests/bench_issue_scalar -a{1} > {0}".format(dumpfile, argn)
		os.system("mpirun -np 2 ./tests/bench_issue_scalar -a{1} > {0}".format(dumpfile, argn))

#create bench_issue_scalar dump directory
#dumpdir = 'perfs/{0}'.format('bench_issue_container')
#if not os.path.exists(dumpdir):
#	print "creating "+dumpdir+" directory" 
#	os.makedirs(dumpdir);
##run app
#for argn in args_num:
#	for it in range(iterations):
#		dumpfile = dumpdir+"/bench_issue_container_argn={1}_size=1_{0}.dump".format(it, argn)
#		print "running ./tests/bench_issue_container -a{1} -n1 > {0}".format(dumpfile, argn)
#		os.system("mpirun -np 2 ./tests/bench_issue_container -a{1} -n1 > {0}".format(dumpfile, argn))

#for argn in args_num:
#	for it in range(iterations):
#		dumpfile = dumpdir+"/bench_issue_container_argn={1}_size={2}_{0}.dump".format(it, argn, args_size[argn])
#		print "running ./tests/bench_issue_container -a{1} > {0}".format(dumpfile, argn)
#		os.system("mpirun -np 2 ./tests/bench_issue_container -a{1} -n{2} > {0}".format(dumpfile, argn, args_size[argn]))

plot.plot_bar_graph('application breakdowns for 6 cores', apps, stacks, breakdowns,
										'benchmarks', 'execution time (ms)', iterations,
										'perfs/#cluster#/#cluster#_load=large_ncores=6_#iter#.dump', 'stackcluster')

plot.plot_line_graph(	'task issue time over number of arguments', 'number of arguments', 'execution time (ms)', 'argument size',
											['bench_issue_container', 'bench_issue_scalar'], args_num, ['1'], iterations,
											'total job issue time', 'perfs/job_issue_time', 
											'perfs/#app#/#app#_argn=#xlabel#_size=#llable#_#iter#', 'raw')


