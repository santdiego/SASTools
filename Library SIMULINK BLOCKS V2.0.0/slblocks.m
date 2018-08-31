function blkStruct = slblocks
		% This function specifies that the library should appear
		% in the Library Browser
		% and be cached in the browser repository
        %La version que se instala es la 14b , Versiones de matlab
        %anteriores podrian tener problemas. 
        Browser.Library = 'LibrarySimulinkSAS';
%         if verLessThan('matlab', '9')
%             if verLessThan('matlab', '8.4')
%                 disp('SAS runing under Matlab R2012b Compatibility mode');
%                 Browser.Library = 'LibrarySimulinkSAS2012b';
%             else 
%                 disp('SAS runing Matlab R2014b version');
%                 Browser.Library = 'LibrarySimulinkSASv2014b';
%             end
%         else
%                 disp('SAS runing Matlab R2016a version');
%                 Browser.Library = 'LibrarySimulinkSASv2016a';
%         end
        Browser.Name = 'Library SAS';
		blkStruct.Browser = Browser;
end