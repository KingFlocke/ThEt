clear all; close all; clc

infilename = uigetfile('*');


if infilename ~= 0
    infile = fopen(infilename,'r');
    tmp = fopen('temp.dat','w');
    
    tline = fgetl(infile);
    disp('remove header...');
    
    
end