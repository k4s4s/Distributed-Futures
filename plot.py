
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
def parse_file(f, value_name, index):
	content = f.read()
	items=re.findall("{0}.*$".format(value_name), content, re.MULTILINE)
	#print content
	value_s = '0'
	if(len(items) == 0): return 0
	value_s = items[index].partition(':')[2]
	value_s = value_s.partition('ms')[0]
	return float(value_s)

def valid_line(f, col):
	content = f.read()
	elems = content.split(' ')
	if(elems[col].split('\n')[0] == '0'):
		return False;
	return True;	

def calc_speedup(T_s, T_p):
	return T_s/T_p

def calc_relative_slowdown(T_s, T_p, p):
	return calc_speedup(T_s, T_p)*p

#function to plot scalability graphs
def plot_line_graph(title, xlabel, ylabel, llabel,
										apps, xlabel_range, llabel_range, iterations,
										value_n, outfile_n, dump_file_format,
										graph_type):
	print "plotting {0}".format(title)
	datafile_n = "{0}.dat".format(outfile_n)
	perf_n = "{0}.perf".format(outfile_n)
	
	datafile = open(datafile_n, 'w')
	seq_time = 0;
	for x in xlabel_range:
		datafile.write("{0}".format(x))
		for app in apps:
			for l in llabel_range:
				value_v = [];
				total_time = 0;
			try:
				for it in range(iterations):
					dumpfile_n = dump_file_format.replace("#app#", app)
					dumpfile_n = dumpfile_n.replace("#xlabel#", x)
					dumpfile_n = dumpfile_n.replace("#llabel#", l)
					dumpfile_n = dumpfile_n.replace("#iter#", str(it))
					dumpfile_n = "{0}.dump".format(dumpfile_n)
					#get value
					#print dumpfile_n
					dumpfile = open(dumpfile_n)
					#print dumpfile_n
					value_v.append(parse_file(dumpfile, value_n, 0))
				#write value to cvs file
				total_time = median(value_v)
				if(x == '1'):
					seq_time = total_time
			except IOError:
				total_time = 0
			if(graph_type == 'speedup'):
				total_time = calc_speedup(seq_time, total_time)
			elif(graph_type == 'relative_slowdown'):
				total_time = calc_relative_slowdown(seq_time, total_time, float(x))
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
				perf.write('"{0}" using 1:{1} w linespoints'.format(datafile_n, col)+' title "{0}, {1}={2}"'.format(app, llabel, l))
				is_first_entry = False;
			col+=1
	perf.write("\n")

	print CSI+"31;40m" + bold + "gnuplot " + perf_n + CSI + "0m"
	os.system("gnuplot "+perf_n)


def plot_bar_graph(	title, clusters, stacks, breakdowns,
										xlabel, ylabel, iterations, 
										dump_file_format, graph_type):
	print "plotting {0}".format(title)
	datafile_n = ""
	perf_n = ""
	if(len(clusters) == 1):
		perf_n = "perfs/{0}/{0}_breakdowns.perf".format(stacks[0])
	else:
		perf_n = "perfs/{0}_breakdowns.perf".format('all')
	outfile_n = '{0}.eps'.format(os.path.splitext(perf_n)[0])
	perf=open(perf_n, 'w');
	perf.write('={0};'.format(graph_type))
	for b in breakdowns:
		perf.write('{0};'.format(b))
	perf.write('\n')
	perf.write('=noupperright\n')
	perf.write('legendx=right\n')
	perf.write('legendy=center\n')
	perf.write('title={0}\n'.format(title))
	perf.write('yformat=%g\n')
	perf.write('xlabel={0}\n'.format(xlabel))
	perf.write('ylabel={0}\n'.format(ylabel)) 
	perf.write('rotateby=45.0\n')
	perf.write('xticshift=-4,-3\n')
	perf.write('=table\n')
	if(graph_type == 'stackcluster'):
		for cluster in clusters:
			perf.write('multimulti={0}\n'.format(cluster))
			stack_i = 0;
			for stack in stacks:
				perf.write('{0}'.format(stack))
				for value_n in breakdowns:
					value_v = [];
					total_time = 0;
					try:
						for it in range(iterations):
							dumpfile_n = dump_file_format.replace("#cluster#", cluster)
							dumpfile_n = dumpfile_n.replace("#stack#", stack)
							dumpfile_n = dumpfile_n.replace("#iter#", str(it))
							#get value
							dumpfile = open(dumpfile_n)
							#print dumpfile_n
							value_v.append(parse_file(dumpfile, value_n, stack_i))
						#write value to cvs file
						total_time = median(value_v)
					except IOError:
						total_time = 0
					perf.write('\t{0}'.format(total_time))
				perf.write('\n')
				stack_i += 1;

	print CSI+"31;40m" + bold + "bargraph.pl {0} > {1}".format(perf_n, outfile_n) + CSI + "0m"
	#os.system("bargraph.pl {0} > {1}".format(perf_n, outfile_n))

