#!/usr/bin/env python

#open('2017.tsv.csv').read().splitlines()

import csv

def firsts(a):
	return map(lambda x: x[0],a)

def seconds(a):
	return map(lambda x: x[1],a)

def flatten(a):
	r=[]
	for elem in a: r.extend(elem)
	return r

def print_lines(x):
	for a in x: print a

def correct_event_name(s):
	if s=='Central WA University':
		return 'Central Washington University'
	if s=='Clackamus Academy':
		return 'Clackamas Academy of Industrial Science'
	return s

def assignments():
	reader=csv.reader(open('2017.csv'))
	return map(lambda x: (int(x[0]),correct_event_name(x[-3])),reader)

#print_lines(assignments())

def points():
	def parse_line(s):
		sp=s.split('\t')
		#print sp
		team=int(sp[2].split()[0])
		def parse_event(s):
			sp=s.split('-')
			if len(sp)!=2: return None
			return sp[1].split(']')[0].split(' Event')[0][1:]
		events=[]
		def add_col(i):
			event=parse_event(sp[i])
			if event is not None:
				events.append(event)
		add_col(3)
		add_col(4)
		return team,events
	return map(parse_line,open('last_year.txt').read().splitlines())

#print_lines(points())

#For each team determine if home event is one that they went to last year
by_team={}
p=dict(points())
a=dict(assignments())
for team in set(firsts(points())+firsts(assignments())):
	by_team[team]=p[team],a[team]

#print_lines(sorted(by_team.iteritems()))

def same(team):
	prev,new=by_team[team]
	return new in prev

print 'Total teams:',len(by_team)
print 'Home is one that earned points at last year:',len(filter(same,by_team))
print

#for each event:
	#how many of the teams from last year have it this year
	#how many teams w/ it as a home event went there last year?

events=set(seconds(assignments())+flatten(seconds(points())))

#print_lines(sorted(events))

by_event={}
for event in events:
	#print event
	points_last_year=set(firsts(filter(lambda x: event in x[1],points())))
	home_event=set(firsts(filter(lambda x: event==x[1],assignments())))
	by_event[event]=points_last_year,home_event

print '2016\t2017\tEvent name'
print 'Teams\tHome'
for event,(last,home) in sorted(by_event.iteritems()):
	print '%s\t%s\t%s'%(len(last),len(home),event)
print

print 'Teams that did not earn points at their 2017 home event in 2016:'
for event,(points_last_year,home_event) in sorted(by_event.iteritems()):
	print event
	print '\t',sorted(home_event-points_last_year)
print

#print 'At event previously but not designated as home:'
#for event,(points_last_year,home_event) in by_event:
#	print '\tNot home but prev',len(sorted(points_last_year-home_event))

def count(a):
	r={}
	for elem in a:
		if elem not in r:
			r[elem]=0
		r[elem]+=1
	return r

def print_ind(a):
	for elem in a:
		print '\t',elem

def swap_pairs(a):
	return map(lambda (p1,p2): (p2,p1),a)

print 'Of the teams that earned points at an event last year what is their 2017 home event?'
for event,(last,home) in sorted(by_event.iteritems()):
	print event
	nexts=count(map(lambda team: by_team[team][1],last))
	#for event1 in by_event:
	#	def 
	#	print filter(lambda x: x[1],by_team.iteritems())
	print_ind(reversed(sorted(swap_pairs(nexts.iteritems()))))
print

print 'Of teams w/ a designated 2017 home, which events did they earn points at last year?'
for event,(last,home) in sorted(by_event.iteritems()):
	print event
	c=count(flatten(map(lambda team: by_team[team][0],home)))
	print_ind(reversed(sorted(swap_pairs(c.iteritems()))))
