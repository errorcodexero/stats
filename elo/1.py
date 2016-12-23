#!/usr/bin/env python

def firsts(x): return [y[0] for y in x]

def mean(x): return sum(x)/len(x)

def print_lines(x):
	for a in x: print a

lines=file('FRC_Elo_2008-2016.csv').read().splitlines()

def possibly_int(x):
	try:
		return int(x)
	except:
		return x

labels=map(possibly_int,lines[0].split(','))

def process_item(item):
	if item=='': return None
	x=int(item)
	if x==0: return None
	return x

def parse_line(line):
	return dict(zip(labels,map(process_item,line.split(','))))

data=map(parse_line,lines[1:])

#print 'labels:',labels
#print_lines(data)

def get_year(year):
	#int->[(elo,team#)]
	r=[]
	for elem in data:
		team=elem['Team']
		if year in elem and elem[year] is not None:
			r.append((elem[year],team))
	return r

#print_lines(sorted(get_year(2016)))

years=range(2008,2017)

def median(x):
	y=sorted(x)
	return y[len(y)/2]

def first_q(x):
	y=sorted(x)
	return y[len(y)/4]

def third_q(x):
	y=sorted(x)
	return y[len(y)*3/4]

def tenth(x):
	return sorted(x)[len(x)*5/100]

def nintyth(x):
	return sorted(x)[len(x)*95/100]

def get_percentile(options,value):
	return int(100*len(filter(lambda x: x<value,options))/len(options))

for year in years:
	ours=filter(lambda x: x[1]==1425,get_year(year))[0][0]
	elos=firsts(get_year(year))
	placement=get_percentile(elos,ours)
	print year,' '.join(map(
		lambda x: str(x(elos)),
		[min,tenth,first_q,median,third_q,nintyth,max]
		)),ours,placement
