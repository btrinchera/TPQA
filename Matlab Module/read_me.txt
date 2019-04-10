Simple CVI interface for TWM Matlab functions.

It uses Matlab "engine", which are DLL libs distributed with Matlab.
This demo was build with 64bit Matlab so it must be a 64bit application.
It uses "libeng.lib" and "libmx.lib" libraries from the Matlab folder:
.\extern\lib\win64\microsoft\...

Other Matlab versions may have the libs at different places.
Note to make it run windows has to know where the engine DLL files are!
In my case it was Matlab folder:
.\bin\win64

To tell windoze where the DLL is it may be necessary to add that folder
to the windows PATH variable.
The PATH can be added by starting Matlab and running script 
"reg_matlab_dlls.m" in project root.



Usage:
First thing to do is to download TWM tool or at least the "octprog"
folder with the Matlab scripts.
Then open "config.ini" file in this project folder and enter the path
to the "octprog" folder to the key "twm_octave_folder" and save it.

The application should open the Matlab at the start and detect the TWM.
If it's ok, one may select result file (see attached folder "testdata")
and use REFRESH to refresh the results view.