import warnings
warnings.filterwarnings('ignore')
import sys
import math as m
import numpy as np

def hms2deg(r):

	return 15*(r[0]+(r[1]+(r[2]/60.0))/60.0)

def dms2deg(d):

	sign = 1

	if (( d[0] == 0 and d[1] == 0 and d[2] < 0 ) \
	or  ( d[0] == 0 and d[1] < 0 )               \
	or  ( d[0] < 0 )):
		sign = -1

	return sign*(abs(d[0])+(abs(d[1])+(abs(d[2])/60.0))/60.0)

def c2e(r, d):

	d2r = m.pi/180.  # degrees to radians conversion factor
	eps = 23.4  # degrees - obliquity of the ecliptic

	# Convert angles from degrees to radians
	er = eps*d2r
	rr = r*d2r
	dr = d*d2r


	br = m.asin(m.sin(dr)*m.cos(er)-m.cos(dr)*m.sin(er)*m.sin(rr))

	lr = m.acos(m.cos(dr)*m.cos(rr)/m.cos(br))

	if (rr > m.pi):
		lr = 2.0*m.pi - lr

	return br/d2r, lr/d2r  # latitude, longitude

def printResult(r, d):

	print("")
	print("Celstial Coordinates.")
	print("Declination, Right Ascension: (", d, ", ", r, ")")
	print("")
	print("Ecliptic Coordinates.")
	print("Ecliptic latitude, longitude: ",c2e(r,d))
	print("")

	return

def main():

#    print([sys.argv[i] for i in range(1,7)])
#    print(sys.argv[1])
	coordstr = sys.argv[1].split()

	RA = [float(coordstr[i]) for i in range(0,3)]
	dec = [float(coordstr[i]) for i in range(3,6)]

	lng, lat = c2e(hms2deg(RA),dms2deg(dec))

	flag = 0
	if abs(lng) > 15:
		flag = 1

	sys.exit([flag])

if __name__ == '__main__':
    main()
