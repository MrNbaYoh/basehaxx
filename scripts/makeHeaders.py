from datetime import datetime
import sys
import ast

def outputConstantsH(d):
	out=""
	out+=("#ifndef CONSTANTS_H")+"\n"
	out+=("#define CONSTANTS_H")+"\n"
	for k in d:
		out+=("	#define "+k+" "+str(d[k]))+"\n"
	out+=("#endif")+"\n"
	return out

def outputConstantsS(d):
	out=""
	for k in d:
		out+=(k+" equ ("+str(d[k])+")")+"\n"
	return out

def outputConstantsPY(d):
	out=""
	for k in d:
		out+=(k+" = ("+str(d[k])+")")+"\n"
	return out

if len(sys.argv)<1:
	print("use : "+sys.argv[0]+" <extensionless_output_name> <input_file1> <const=value> <input_file2> ...")
	exit()

l = {"BUILDTIME" : "\""+datetime.now().strftime("%Y-%m-%d %H:%M:%S")+"\""}

for a in sys.argv[2:]:
	if "=" in a:
		a = a.split("=")
		l[a[0]] = a[1]
	else:
		s=open(a, "r").read()
		if len(s) > 0:
			l.update(ast.literal_eval(s))

l["ORAS_VERSION"] = "\""+l["ORAS_VERSION"]+"\""		
	
open(sys.argv[1]+".h","w").write(outputConstantsH(l))
open(sys.argv[1]+".s","w").write(outputConstantsS(l))
open(sys.argv[1]+".py","w").write(outputConstantsPY(l))
