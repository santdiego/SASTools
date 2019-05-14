function [Xm,Ym,Zm,Fxm,Fym,Fzm,Xs,Ys,Zs,Fxs,Fys,Fzs,h,t,n,deltaX,deltaY,deltaZ,deltaX1,deltaY1,deltaZ1,dXm,dYm,dZm,dXs,dYs,dZs,Fxmh,Fymh,Fzmh,Fxm_comp,Fym_comp, Fzm_comp]=datos_novint


%[Xm,Ym,Zm,Fxm,Fym,Fzm,Xs,Ys,Zs,Fxs,Fys,Fzs,h,t,n,deltaX,deltaY,deltaZ,deltaX1,deltaY1,deltaZ1,dXm,dYm,dZm,dXs,dYs,dZs,Fxmh,Fymh,Fzmh,Fxm_comp,Fym_comp, Fzm_comp]=datos_novint;

ts=1;%milisegundos
 
[filename, pathname] = uigetfile('*.bin', 'Data .bin archivo de datos');

archivo = strcat(pathname,filename);

fid=fopen(archivo,'r');

A=fread(fid,'float');
data_len=512;
data_size=31; %cantidad de floats en c/estructura de datos
n=length(A);

Total=n/data_size;

Xm=zeros(1,Total);
Ym=zeros(1,Total);
Zm=zeros(1,Total);

Fxm=zeros(1,Total);
Fym=zeros(1,Total);
Fzm=zeros(1,Total);

Fxm_comp=zeros(1,Total);
Fym_comp=zeros(1,Total);
Fzm_comp=zeros(1,Total);

Fxmh=zeros(1,Total);
Fymh=zeros(1,Total);
Fzmh=zeros(1,Total);

Xs=zeros(1,Total);
Ys=zeros(1,Total);
Zs=zeros(1,Total);

Fxs=zeros(1,Total);
Fys=zeros(1,Total);
Fzs=zeros(1,Total);

h=zeros(1,Total);

deltaX=zeros(1,Total);
deltaY=zeros(1,Total);
deltaZ=zeros(1,Total);

deltaX1=zeros(1,Total);
deltaY1=zeros(1,Total);
deltaZ1=zeros(1,Total);

dXs=zeros(1,Total);
dYs=zeros(1,Total);
dZs=zeros(1,Total);

dXm=zeros(1,Total);
dYm=zeros(1,Total);
dZm=zeros(1,Total);

Total=n/(data_len*data_size);

k=1;
for J=1:Total-1,
    for I = 1:data_len,
        Xm(k)=A(I+(J-1)*data_len*data_size);
        Ym(k)=A(data_len+I+(J-1)*data_len*data_size);
        Zm(k)=A(data_len*2+I+(J-1)*data_len*data_size);
        Xs(k)=A(data_len*3+I+(J-1)*data_len*data_size);
        Ys(k)=A(data_len*4+I+(J-1)*data_len*data_size);
        Zs(k)=A(data_len*5+I+(J-1)*data_len*data_size);
        h(k)=A(data_len*6+I+(J-1)*data_len*data_size);
        Fxm(k)=A(data_len*7+I+(J-1)*data_len*data_size);
        Fym(k)=A(data_len*8+I+(J-1)*data_len*data_size);
        Fzm(k)=A(data_len*9+I+(J-1)*data_len*data_size);
        Fxs(k)=A(data_len*10+I+(J-1)*data_len*data_size);
        Fys(k)=A(data_len*11+I+(J-1)*data_len*data_size);
        Fzs(k)=A(data_len*12+I+(J-1)*data_len*data_size);
        deltaX(k)=A(data_len*13+I+(J-1)*data_len*data_size);
        deltaY(k)=A(data_len*14+I+(J-1)*data_len*data_size);
        deltaZ(k)=A(data_len*15+I+(J-1)*data_len*data_size);
        deltaX1(k)=A(data_len*16+I+(J-1)*data_len*data_size);
        deltaY1(k)=A(data_len*17+I+(J-1)*data_len*data_size);
        deltaZ1(k)=A(data_len*18+I+(J-1)*data_len*data_size);
        dXs(k)=A(data_len*19+I+(J-1)*data_len*data_size);
        dYs(k)=A(data_len*20+I+(J-1)*data_len*data_size);
        dZs(k)=A(data_len*21+I+(J-1)*data_len*data_size);
        Fxmh(k)=A(data_len*22+I+(J-1)*data_len*data_size);
        Fymh(k)=A(data_len*23+I+(J-1)*data_len*data_size);
        Fzmh(k)=A(data_len*24+I+(J-1)*data_len*data_size);
        Fxm_comp(k)=A(data_len*25+I+(J-1)*data_len*data_size);
        Fym_comp(k)=A(data_len*26+I+(J-1)*data_len*data_size);
        Fzm_comp(k)=A(data_len*27+I+(J-1)*data_len*data_size);
        dXm(k)=A(data_len*28+I+(J-1)*data_len*data_size);
        dYm(k)=A(data_len*29+I+(J-1)*data_len*data_size);
        dZm(k)=A(data_len*30+I+(J-1)*data_len*data_size);

        k=k+1;
    end
end
n=0:(length(Xm)-1);
t=n*ts;
t=t/1000; %tiempo en segundos



