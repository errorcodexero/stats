#!/usr/bin/env python

from glob import glob
from homes import assignments,seconds,firsts,print_lines

#str->str
def parse_file_name(s):
	sp=s.split('-')[1].lstrip().split('(')[0].rstrip()
	return ' '.join(sp.split()[:-1])

#Team=int
#str->[Team]
def parse_file(filename):
	f=filter(lambda x: '/team/' in x and 'media' not in x,open(filename).read().splitlines())
	def parse_line(s):
		return int(s.split('>')[1].split('<')[0])
	return map(parse_line,f)

def teams_by_event():
	event_pages=glob('reg_data/*.html')
	#print event_pages
	#for a in event_pages:
	#	print parse_file_name(a)
	#	print parse_file(a)
	return dict([(parse_file_name(a),parse_file(a)) for a in event_pages])

def group(f,collection):
	r={}
	for elem in collection:
		x=f(elem)
		if x not in r:
			r[x]=[]
		r[x].append(elem)
	return r

def main1():
#	print teams_by_event()
#	print assignments()

	#For each event
		#how many of the teams are signed up
		#how many teams are not signed up
		#how many teams are signed up that weren't assigned that even originally
	assigned=assignments()
	a_event=set(seconds(assigned))
	b_event=set(teams_by_event())
	#print a_event
	#print b_event
	assert a_event==b_event

	#print assigned
	g=group(lambda x: x[1],assigned)

	m=[]

	for (event_name,signed_up) in teams_by_event().iteritems():
		#assigned=filter(lambda x: x[1]==event_name,assigned)
		assigned=firsts(g[event_name])
		not_signed_up=list(set(assigned)-set(signed_up))
		mystery=list(set(signed_up)-set(assigned))
		print event_name
		print '\tassigned:',len(assigned)
		print '\tsigned up:',len(signed_up)
		print '\tnot signed up:',len(not_signed_up)
		print '\tSigned up but not assigned:',mystery
		m.append((event_name,len(assigned),len(signed_up)))
	#print_lines(m)
	for x in m: print '\t'.join(map(str,x))

if __name__=='__main__':
	main1()
