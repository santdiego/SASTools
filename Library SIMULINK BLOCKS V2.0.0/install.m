%Ejecute este Script para Instalar la libreria Simulink del INAUT 
%Luego de la instalacion actualice Library Browser de simulink 
%(Abra el Library Browser y presione F5)
libPath=fileparts( mfilename('fullpath') );
addpath(libPath);
savepath;
slblocks;
prompt = 'Before installing re-start matlab with administrator privilege. CLOSE MATLAB? Y/N [N]';
str = input(prompt,'s');
if isempty(str)
    str = 'N';
end
if str=='Y'
    exit
end

%Instala drivers para el bloque de tiempo real de simulink
disp('This is Optional')
sldrtkernel -install


prompt = 'This Installs Vrep Models and Plugins into VREP (Optional). Continue? Y/N [N]';
str = input(prompt,'s');
if isempty(str)
    str = 'N';
    disp('SAS will not be installed on VREP');
end
if str=='Y'
   if ispc

        if (exist('C:\Program Files\V-REP3'))
             disp('Installing on C:\Program Files\V-REP3\V-REP_PRO_EDU')
            st=copyfile('../Library V-REP Models & ScenesV1.0.0/*.ttm','C:\Program Files\V-REP3\V-REP_PRO_EDU\models\SAS Models');
            if st ==0 
                disp('Error installing Models on V-REP3 (x64)')
            end
            st=copyfile('../Plugin V-REP Shared Memory V3.0.0\Build Windows\v_repExtShareMemory.dll','C:\Program Files\V-REP3\V-REP_PRO_EDU');
            if st ==0 
                disp('Error installing Plugin on V-REP3 (x64)')
            end
        else
            disp('V-REP not found in C:\Program Files\V-REP3')
        end 
    
   end
elseif isunix
disp ('Automatic installation not available on Linux. Please manually install the VREP plugins and models.')
end
