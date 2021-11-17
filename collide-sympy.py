from sympy import *

ma, mb= symbols('ma mb')
x, y, va, vb, da, db= symbols('x y va vb da db')
phi= atan2(y, x)

newda= da-phi
newvax= va*cos(da-phi)
newvay= va*sin(da-phi)

newdb= db-phi
newvbx= vb*cos(db-phi)
newvby= vb*sin(db-phi)

vax= ((ma-mb)*newvax + (ma+mb)*newvbx)/(ma+mb)
vbx= ((mb-ma)*newvbx + (ma+mb)*newvax)/(ma+mb)

M=Matrix([ [cos(phi), cos(phi+pi/2)], [sin(phi), sin(phi+pi/2)] ] )
Va= Matrix([vax, newvay])
Vb= Matrix([vbx, newvby])

print(nsimplify(M*Va))

z= (ma-mb)/(ma+mb) * va * x * ( x*cos(da) + y*sin(da)) \
                   + va * y * (-x*sin(da) + y*cos(da)) \
                   + vb * x * ( x*cos(db) + y*sin(db))

print(z)
#  
#  -va*(y)*sin(da - atan2(y, x))/sqrt((x)**2 + (y)**2)  + (x)*(va*(ma - mb)*cos(da - atan2(y, x)) + vb*(ma + mb)*cos(db - atan2(y, x)))/((ma + mb)*sqrt((x)**2 + (y)**2))
#   va*(x)*sin(da - atan2(y, x))/sqrt((x)**2 + (y)**2)  + (y)*(va*(ma - mb)*cos(da - atan2(y, x)) + vb*(ma + mb)*cos(db - atan2(y, x)))/((ma + mb)*sqrt((x)**2 + (y)**2))
#  
#  
#  -va*(y)*sin(da - atan2(y, x))  + (x)*(va*(ma - mb)*cos(da - atan2(y, x)) + vb*(ma + mb)*cos(db - atan2(y, x)))/(ma + mb)
#   va*(x)*sin(da - atan2(y, x))  + (y)*(va*(ma - mb)*cos(da - atan2(y, x)) + vb*(ma + mb)*cos(db - atan2(y, x)))/(ma + mb)
#  
#  {
#  (ma-mb)/(ma+mb) va x ( x cos(da) + y sin(da))
#                  va y (-x sin(da) + y cos(da))
#                  vb x ( x cos(db) + y sin(db))
#  }/sqrt(x^2+y^2)
