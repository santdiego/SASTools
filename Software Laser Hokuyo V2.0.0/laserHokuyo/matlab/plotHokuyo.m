function Laser = plotHokuyo(medidas,x,y,z,roll,pitch,yaw,range_min,range_max,color)

%   HELICOP_LASER define la posicion del barrido laser
%       mientras el helicoptero esta navegando
%
%       medidas = valores obtenidos de las mediciones utilizando el sensor
%       laser, Matriz de 61x31 (mxn) 
%       [x,y,z,roll,pitch,yaw] = postura del robot en el medio, ambiente
%
%       Laser = es el vector que contiene las graficas

if nargin<10 || isempty(color), color=[0.7 0.7 1]; end
if nargin<9 || isempty(range_max), range_max=4000; end
if nargin<8 || isempty(range_min), range_min=20; end
if nargin<7 || isempty(yaw), yaw=0; end
if nargin<6 || isempty(pitch), pitch=0; end
if nargin<5 || isempty(roll), roll=0; end
if nargin<4 || isempty(z), z=0; end
if nargin<3 || isempty(y), y=0; end
if nargin<2 || isempty(x), x=0; end

medidas(medidas > range_max) = range_max;
medidas(medidas < range_min) = range_min;

medidas=medidas/1000; %paso a metros

res_ang=360/1024;  %resolución de 360/1024=0.3515625 grados en XY
reg_ang=240;%región de 240 grados en XY

theta = deg2rad(res_ang);

%%% Matriz de Rotación
RotX = [1 0 0; 0 cos(roll) -sin(roll); 0 sin(roll) cos(roll)];
RotY = [cos(pitch) 0 sin(pitch); 0 1 0; -sin(pitch) 0 cos(pitch)];
RotZ = [cos(yaw) -sin(yaw) 0; sin(yaw) cos(yaw) 0; 0 0 1];

Rot = RotZ*RotY*RotX;

%Inicializo las distancias xyz de los puntos finales de los rayos
num=ceil(reg_ang/res_ang);
dist=zeros(3,num);

%%% Plotar el barrido laser al la altura real
for i=1:num
    medidasxyz=[cos(-deg2rad(reg_ang/2)+(i-1)*theta)* medidas(i);...
    sin(-deg2rad(reg_ang/2)+(i-1)*theta)* medidas(i);...
    0];
    medidasxyz = Rot * medidasxyz;
    dist(:,i)=medidasxyz;
end
% Laser1 = patch([x x+dist(1,:)],[y y+dist(2,:)],[z z+dist(3,:)],[0.7 0.7 1],'FaceAlpha',0.05); %240º Laser
% Laser2 = scatter3(x+dist(1,:),y+dist(2,:),z+dist(3,:),'Marker','.','MarkerFaceColor',color,'MarkerEdgeColor',color,'MarkerFaceAlpha',0.5);
% Laser = [Laser1;Laser2];
Laser=[x x+dist(1,:);y y+dist(2,:);z z+dist(3,:)];