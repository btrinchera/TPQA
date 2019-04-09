clc;
clear all;

warning('off');

mfld = fileparts(mfilename('fullpath'));
cd(mfld); 

addpath([mfld filesep() 'info']);
addpath([mfld filesep() 'qwtb']);

alg = 'TWM-MODTDPS';
qwtb(alg,'addpath');


for k = 1:1

    %qwtb('TWM-THDWFFT','test')
    %qwtb('TWM-PWRTEST','test')
    %qwtb('TWM-TEST','test')
    %qwtb('TWM-PSFE','test')
    qwtb(alg,'test')
    %qwtb('TWM-FPNLSF','test')    
    %qwtb('TWM-PWRTDI','test')
    %qwtb('TWM-HCRMS','test')
    %qwtb('TWM-InDiSwell','test')
    %qwtb('TWM-Flicker','test')
end

%twm_selftest

