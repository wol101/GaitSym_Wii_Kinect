Notes on bone models

These bones are from the scans performed by Chris Wood on the Liverpool
Anatomy Department skeleton. The forearm is pronated so that the
reconstruction can walk quadrupedally.

They are full size (units in metres)
The origin is at the centre of mass (using Zatsiorski's human CM data)
Positive x is distal (or caudal)
Positive y is right
Positive z is generally ventral (it is always a right hand coordinate system)

This equates to human in the anatomical position lying on its back with its 
head at the origin and feet along the x axis with a right handed coordinate system

The dimensions and mass properties of the bones are as follows:

            Endpoints               CM_%        Rap_%       Rml_%       Rlg_%       Mass_(kg)   Length_(m)  CM_(m)      MOIap_kgm2  MOIml_kgm2  MOIlg_kgm2
            Origin      Other                                                                                                                   
Torso+Head  MIDS        MIDH        31.575      47.571      45.832      18.925      50.769      0.600       0.189       4.1361      3.8391      0.6546
Arm         SJC         EJC         57.720      28.500      26.900      15.800      8.031       0.360       0.208       0.0845      0.0753      0.0260
Forearm     EJC         WJC         45.740      27.600      26.500      12.100      7.569       0.405       0.185       0.0946      0.0872      0.0182
Hand        WJC         DAC3        36.240      28.800      23.500      18.400      2.308       0.250       0.091       0.0120      0.0080      0.0049
Thigh       HJC         KJC         40.950      32.900      32.900      14.900      9.046       0.300       0.123       0.0881      0.0881      0.0181
Leg         KJC         AJC         43.950      25.100      24.600      10.200      4.800       0.315       0.138       0.0300      0.0288      0.0050
Foot        HEEL        TTIP        44.150      25.700      24.500      12.400      2.862       0.250       0.110       0.0118      0.0107      0.0027

Note that these vales are then shifted so the origin is at the centre of mass.
This puts the landmarks at the following coordinates:

            Endpoints                           
            Origin                  Other       
Torso+Head  MIDS        -0.189      MIDH        0.411
Arm         SJC         -0.208      EJC         0.152
Forearm     EJC         -0.185      WJC         0.220
Hand        WJC         -0.091      DAC3        0.159
Thigh       HJC         -0.123      KJC         0.177
Leg         KJC         -0.138      AJC         0.177
Foot        AJC         -0.110      TTIP        0.140

Joint z values are all 0.0

The shoulder joints have y values of +0.200 and -0.200
The hip joints have y values of +0.100 and -0.100
All the other joints have y values of 0.0


Abbreviations:
AJC, EJC, HJC, KJC, SJC, WJC-the joint centers of ankle, elbow, hip, knee, shoulder, and wrist, respectively 
(after de Leva 1996b; see Kinematics of Human Motion, section 5. 1); 
CERV-cervicale; DAC3-third dactylion; HEEL-pternion; LMAL-the most lateral point on the lateral malleolus;
MET3-third metacarpale; MIDG, MIDH, MIDS-midgonion, midhip, and midshoulder, respectively; OMPH-omphalion;
SPHY-sphyrion; STYL-stylion; SUPR-suprasternale; TTIP-the tip of the longest toe (acropodion); VERT-vertex; 
XIPH-xiphion
