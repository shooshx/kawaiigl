#include "PresetModels.h"


const char* PresetModels::m_cubeModel = "a1={1,1,1}\n\
a2={1,-1,1}\n\
a3={-1,-1,1}\n\
a4={-1,1,1}\n\
add(a4,a3,a2,a1)\n\n\
b1={1,1,-1}\n\
b2={1,-1,-1}\n\
b3={-1,-1,-1}\n\
b4={-1,1,-1}\n\
add(b1,b2,b3,b4)\n\n\
add(a2,b2,b1,a1)\n\
add(a1,b1,b4,a4)\n\
add(a3,b3,b2,a2)\n\
add(a4,b4,b3,a3)\n";

const char* PresetModels::m_tetrahedronModel = "p1 = {1.0, 1.0, 1.0}\n\
p2 = {-1.0, -1.0, 1.0}\n\
p3 = {-1.0, 1.0, -1.0}\n\
p4 = {1.0, -1.0, -1.0}\n\n\
add(p3,p2,p1)\n\
add(p4,p2,p3)\n\
add(p1,p2,p4)\n\
add(p4,p3,p1)\n";


const char* PresetModels::m_holedTetrahedronModel = 
"d=3.0\n\
p1 = {1.0, 1.0, 1.0}\n\
p2 = {-1.0, -1.0, 1.0}\n\
p3 = {-1.0, 1.0, -1.0}\n\
p4 = {1.0, -1.0, -1.0}\n\n\
m1 = (p1 + p3 + p4) / 3.0\n\
m2 = (p2 + p3 + p4) / 3.0\n\
m3 = (p1 + p2 + p3) / 3.0\n\
m4 = (p1 + p2 + p4) / 3.0\n\n\
p11 = (2*m1 + p1) / 3.0\n\
p13 = (2*m1 + p3) / 3.0\n\
p14 = (2*m1 + p4) / 3.0\n\
p23 = (2*m2 + p3) / 3.0\n\
p22 = (2*m2 + p2) / 3.0\n\
p24 = (2*m2 + p4) / 3.0\n\
p31 = (2*m3 + p1) / 3.0\n\
p32 = (2*m3 + p2) / 3.0\n\
p33 = (2*m3 + p3) / 3.0\n\
p41 = (2*m4 + p1) / 3.0\n\
p42 = (2*m4 + p2) / 3.0\n\
p44 = (2*m4 + p4) / 3.0\n\n\
add(p11, p13, p3, p1)\n\
add(p14, p11, p1, p4)\n\
add(p13, p14, p4, p3)\n\n\
add(p23, p22, p2, p3)\n\
add(p24, p23, p3, p4)\n\
add(p22, p24, p4, p2)\n\n\
add(p33, p31, p1, p3)\n\
add(p32, p33, p3, p2)\n\
add(p31, p32, p2, p1)\n\n\
add(p42, p41, p1, p2)\n\
add(p44, p42, p2, p4)\n\
add(p41, p44, p4, p1)\n\n\
pi1 = (p11 + p31 + p41) / d\n\
pi2 = (p22 + p32 + p42) / d\n\
pi3 = (p13 + p23 + p33) / d\n\
pi4 = (p14 + p24 + p44) / d\n\n\
add(p13, pi3, pi4, p14)\n\
add(pi3, p23, p24, pi4)\n\n\
add(p11, p14, pi4, pi1)\n\
add(pi1, pi4, p44, p41)\n\n\
add(p24, p22, pi2, pi4)\n\
add(pi4, pi2, p42, p44)\n\n\
add(pi3, p13, p11, pi1)\n\
add(pi3, pi1, p31, p33)\n\n\
add(p22, p23, pi3, pi2)\n\
add(pi2, pi3, p33, p32)\n\n\
add(pi1, pi2, p32, p31)\n\
add(pi2, pi1, p41, p42)\n";




const char* PresetModels::m_holesCubeModel = 
"d=0.5\n\n\
a1={1,1,1}\n\
a2={1,-1,1}\n\
a3={-1,-1,1}\n\
a4={-1,1,1}\n\n\
pa1=a1-{d,d,0}\n\
pa2=a2-{d,-d,0}\n\
pa3=a3+{d,d,0}\n\
pa4=a4+{d,-d,0}\n\n\
add(pa1,pa2,a2,a1)\n\
add(pa2,pa3,a3,a2)\n\
add(pa3,pa4,a4,a3)\n\
add(pa4,pa1,a1,a4)\n\n\
b1={1,1,-1}\n\
b2={1,-1,-1}\n\
b3={-1,-1,-1}\n\
b4={-1,1,-1}\n\n\
pb1=b1-{d,d,0}\n\
pb2=b2-{d,-d,0}\n\
pb3=b3+{d,d,0}\n\
pb4=b4+{d,-d,0}\n\n\
add(b1,b2,pb2,pb1)\n\
add(b2,b3,pb3,pb2)\n\
add(b3,b4,pb4,pb3)\n\
add(b4,b1,pb1,pb4)\n\n\
p2b2=b2+{0,d,d}\n\
p2a2=a2+{0,d,-d}\n\
p2b1=b1-{0,d,-d}\n\
p2a1=a1-{0,d,d}\n\n\
add(a2,b2,p2b2,p2a2)\n\
add(b2,b1,p2b1,p2b2)\n\
add(b1,a1,p2a1,p2b1)\n\
add(a1,a2,p2a2,p2a1)\n\n\
p1a4=a4+{d,0,-d}\n\
p1b4=b4+{d,0,d}\n\
p1a1=a1-{d,0,d}\n\
p1b1=b1-{d,0,-d}\n\n\
add(b4,a4,p1a4,p1b4)\n\
add(a4,a1,p1a1,p1a4)\n\
add(a1,b1,p1b1,p1a1)\n\
add(b1,b4,p1b4,p1b1)\n\n\
p3a2=a2-{d,0,d}\n\
p3a3=a3-{-d,0,d}\n\
p3b3=b3+{d,0,d}\n\
p3b2=b2+{-d,0,d}\n\n\
add(a2,a3,p3a3,p3a2)\n\
add(a3,b3,p3b3,p3a3)\n\
add(b3,b2,p3b2,p3b3)\n\
add(b2,a2,p3a2,p3b2)\n\n\
p4a3=a3-{0,-d,d}\n\
p4b3=b3+{0,d,d}\n\
p4b4=b4+{0,-d,d}\n\
p4a4=a4-{0,d,d}\n\n\
add(b3,a3,p4a3,p4b3)\n\
add(a3,a4,p4a4,p4a3)\n\
add(a4,b4,p4b4,p4a4)\n\
add(b4,b3,p4b3,p4b4)\n\n\
pia1=a1-{d,d,d}\n\
pia2=a2-{d,-d,d}\n\
pia3=a3-{-d,-d,d}\n\
pia4=a4-{-d,d,d}\n\n\
add(pa4,pa3,pia3,pia4)\n\
add(pa3,pa2,pia2,pia3)\n\
add(pa2,pa1,pia1,pia2)\n\
add(pa1,pa4,pia4,pia1)\n\n\
pib1=b1-{d,d,-d}\n\
pib2=b2-{d,-d,-d}\n\
pib3=b3-{-d,-d,-d}\n\
pib4=b4-{-d,d,-d}\n\n\
add(pb3,pb4,pib4,pib3)\n\
add(pb2,pb3,pib3,pib2)\n\
add(pb1,pb2,pib2,pib1)\n\
add(pb4,pb1,pib1,pib4)\n\n\
add(p1a1,p1b1,pib1,pia1)\n\
add(p1b1,p1b4,pib4,pib1)\n\
add(p1b4,p1a4,pia4,pib4)\n\
add(p1a4,p1a1,pia1,pia4)\n\n\
add(p3a2,p3a3,pia3,pia2)\n\
add(p3a3,p3b3,pib3,pia3)\n\
add(p3b3,p3b2,pib2,pib3)\n\
add(p3b2,p3a2,pia2,pib2)\n\n\
add(p2b1,p2a1,pia1,pib1)\n\
add(p2a1,p2a2,pia2,pia1)\n\
add(p2a2,p2b2,pib2,pia2)\n\
add(p2b2,p2b1,pib1,pib2)\n\n\
add(p4b3,p4a3,pia3,pib3)\n\
add(p4a3,p4a4,pia4,pia3)\n\
add(p4a4,p4b4,pib4,pia4)\n\
add(p4b4,p4b3,pib3,pib4)\n";
