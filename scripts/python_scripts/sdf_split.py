#!/bin/python 
"""Jean-Remy Marchand 2014
This script aims at spliting a SDF file
"""
import sys, os

if len(sys.argv)!=3:
	print "Usage = python script.py file wheretocp"

out=""
c=0
name=""
names=[]

a=open(sys.argv[1], "r")
bahbah=a.readlines()
a.close()
for line in bahbah:
    c+=1
    if c==1:
    	d=2
        if len(line) > 0 and line.split()[0] in names:
            while line.split()[0]+"_"+str(d) in names:
            	d+=1
            name=line.split()[0]+"_"+str(d)
        if len(line) > 0 and line.split()[0] not in names:
            name=line.split()[0]
        names.append(name)
    out+=line
    if "$$$" in line:
        a=open(sys.argv[2]+"/"+name+".sdf", "w")
        a.write(out)
        a.close()
        out=""
        c=0
