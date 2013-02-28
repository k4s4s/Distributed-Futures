#!/usr/bin/python

import os;

procs_range = [2, 5, 7, 9, 12]
#tests = ["hello_world", "power\ -a2\ -n10"];
tests = ["fibonacci\ -a10"]


for test in tests:
	for procs in procs_range:
		os.system("python run.py -r "+test+" -n {0}".format(procs));
