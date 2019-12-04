function Laser = plotRPLIDAR(ang,dist,num,x,y,z,roll,pitch,yaw,range_min,range_max,color)

%   HELICOP_LASER define la posicion del barrido laser
%       mientras el helicoptero esta navegando
%
%       medidas = valores obtenidos de las mediciones utilizando el sensor
%       laser, Matriz de 61x31 (mxn) 
%       [x,y,z,roll,pitch,yaw] = postura del robot en el medio, ambiente
%
%       Laser = es el vector que contiene las graficas

if nargin<12 || isempty(color), color=[0.7 0.7 1]; end
if nargin<11 || isempty(range_max), range_max=40; end
if nargin<10 || isempty(range_min), range_min=0; end
if nargin<9 || isempty(yaw), yaw=0; end
if nargin<8 || isempty(pitch), pitch=0; end
if nargin<7 || isempty(roll), roll=0; end
if nargin<6 || isempty(z), z=0; end
if nargin<5 || isempty(y), y=0; end
if nargin<4 || isempty(x), x=0; end

dist(dist > range_max) = range_max;
dist(dist < range_min) = range_min;

%%% Matriz de Rotación
RotX = [1 0 0; 0 cos(roll) -sin(roll); 0 sin(roll) cos(roll)];
RotY = [cos(pitch) 0 sin(pitch); 0 1 0; -sin(pitch) 0 cos(pitch)];
RotZ = [cos(yaw) -sin(yaw) 0; sin(yaw) cos(yaw) 0; 0 0 1];

Rot = RotZ*RotY*RotX;

%Inicializo las distancias xyz de los puntos finales de los rayos
distancia=zeros(3,num);

%%% Plotar el barrido laser al la altura real
for i=1:num
    medidasxyz=[cos(deg2rad(ang(i)))* dist(i);...
    -sin(deg2rad(ang(i)))* dist(i);...
    0];
    medidasxyz = Rot * medidasxyz;
    distancia(:,i)=medidasxyz;
end
Laser=[x x+distancia(1,:);y y+distancia(2,:);z z+distancia(3,:)];