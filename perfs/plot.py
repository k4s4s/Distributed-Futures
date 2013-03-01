#!/usr/bin/env python

import glob

f=open("test.perf", 'w')
f.write('set xlabel "Label1"\n') 
f.write('set ylabel "Label2"\n')
f.write('set yrange[0:]\n')
f.write('set term png\n')

for datafile in glob.iglob("*.dat"):
	# Here, you can tweak the output png file name.
	f.write('set output "{output}.png"\n'.format( output=os.path.splitext(datafile)[0] ))
	f.write('plot ')
	f.write('"{file_name}" using 1:2 w l title "Graph title"\n'.format( file_name = datafile ))
f.close()

