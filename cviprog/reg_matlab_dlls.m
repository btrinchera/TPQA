% set fodler to this script
cd(fileparts(mfilename('fullpath')));

% create backup of PATH
bup_file = ['path backup ' strrep(datestr(now),':','-') '.txt'];
[s,old_path] = system('PATH');
fw = fopen(bup_file,'w');
fwrite(fw,old_path);
fclose(fw);

disp('Old %PATH% content:');
disp(old_path);

disp('Backup of your system variable PATH was store to file:');
disp(bup_file);
disp('In case something goes wrong, store the content of the file to your Windows PATH variable to restore original state ...');

% get Matlab DLL path
%dll_path = [matlabroot() filesep() 'bin' filesep() computer('arch') filesep()]

% try to add to the Windoze path (permanently)
% note: Matlab adds DLL path to its local PATH copy, so we just have to
% override original PATH...
[s,res] = system(['setx /M PATH "%PATH%']);

% and we are done
disp('Path added.');

[s,new_path] = system('PATH');
disp('New %PATH% content:');
disp(new_path);