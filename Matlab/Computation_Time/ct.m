%% Computation Time Graph

% clear all variables
clear all; close all; clc

minEntries = 50;

%% file handle and cleaning

% get input file interactively
infilename = uigetfile('*');
error = false;

% check if user choosed a file
if infilename ~= 0
        
    % read inputfile
    fprintf('load file...');
    lines = fileread(infilename);
    fprintf(' ok\n');
    
    % remove unwanted lines and characters
    fprintf('extract lines...');
    lines = regexp(lines,'[^\n]*Simstep = [^\n]*','match');
    fprintf(' ok\n');
    fprintf('remove unnecessary whitespace...');
    lines = regexprep(lines,'\s+',' ');
    fprintf(' ok\n');
    
    % check file
    fprintf('check input...');
    numEntries = numel(lines);
    if numEntries >= minEntries
         fprintf(' ok, %d entries imported\n', numEntries);
    else
        if numEntries ~= 0
            fprintf(' fail, file has too few entries. %d entries needed, file has %d\n', minEntries, numEntries);
            error = true;
        else
            fprintf(' fail, invalid file. String: "Stimstep = " not found\n');
            error = true;
        end
    end
    
    %% read entries into array
    if error == false
        fprintf('read entries into array...');
        Entries = zeros(2, numEntries);
        
        for i = 1:numEntries
           Entries(:, i) = sscanf(lines{i}, '%*s %f %*s Simstep = %u');
        end
        
        % free memory
        clear lines;
        
        fprintf(' ok\n');
    
        %% create graph
        fprintf('draw graph...');
        figure;
        plot(Entries(2, :), Entries(1, :));
        xlabel('Simulationsschritte');
        ylabel('Simulationszeit');
        grid on;
        fprintf(' ok\n');
    end
else
    fprintf('error: please select an input file.\n');
end