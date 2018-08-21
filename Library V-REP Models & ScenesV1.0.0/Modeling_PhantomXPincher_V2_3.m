% This script generates a mathematical model for the robot manipulator 
% Phantom X pincher used in V-REP. 
%To use with Peter Corke Robotic toobox 9.10 under Matlab 2014b 
%http://www.petercorke.com/RTB/

clear all;
close all;
clc

m1=0.05;
m2=0.1;
m3=0.1;
m4=0.1;

l0=0.0859;%base large(z distance of the first join)
l1=0.043; %length of link 1
l2=0.1066;%length of link 2
l3=0.1066;%length of link 3
l4=0.1066;%length of link 4

%Major Moments of Inertia
I1=[.003 .0025 .0016]*m1;
I2=[.0055 .005 .0016]*m2;
I3=[.0055 .005 .0016]*m3;
I4=[.0055 .005 .0016]*m4;

Lm(1)=Link('d',l0+l1,   'a',0,    'alpha',-pi/2,  'revolute', 'm',m1,'I',I1,                'r',[0      0 l0+l1/2   ]);
Lm(2)=Link('d',0,       'a',-l2,  'alpha',0,      'revolute', 'm',m2,'I',I2,  'offset',pi/2,'r',[-l2/2	0 0         ]);
Lm(3)=Link('d',0,       'a',-l3,  'alpha',0,      'revolute', 'm',m3,'I',I3,                'r',[-l3/2	0 0         ]);
Lm(4)=Link('d',0,       'a',-l4,  'alpha',0,      'revolute', 'm',m4,'I',I4,                'r',[-l4/2	0 0         ]);
Arm = SerialLink  (Lm, 'name','PhantomXPincher_VREP');
Arm.plot(zeros(1,4),'workspace',[-.6,.6,-.6,.6,0,.6]);
%figure
Arm.plot([pi/2 pi/4 pi/4 pi/4]);

% cg = CodeGenerator(Arm);
% cg.geneverything();
% clear all
% close all
% clc
addpath ArmRobot

libPath=fileparts( mfilename('fullpath') );
addpath(libPath);



