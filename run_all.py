#!/usr/bin/python

import os;

procs_range = [3, 5, 7, 9, 11]
#tests = ["hello_world", "power\ -a2\ -n10"];
tests = ["fibonacci\ -a15"]

for test in tests:
	test_name = test.partition('\\')[0]
	args = test.partition(' ')[2]
	directory = 'perfs/'+test_name
	#create dir if it does not exist
	if not os.path.exists(directory):
		os.makedirs(directory)
			
	#delete any files in it	
#	filelist = [ f for f in os.listdir(directory) if f.endswith(".dump") ]
#	for f in filelist:
#		os.remove(directory+"/"+f)
		
	for procs in procs_range:
		output = directory+'/'+test_name+args+'_{0}'.format(procs)+'.dump'
		os.system("python run.py -r "+test+" -n {0}".format(procs)+" -o "+output);
