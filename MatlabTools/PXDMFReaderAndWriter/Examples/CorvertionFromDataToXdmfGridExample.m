%% Generation of the old data struct
%
% This file is subject to the terms and conditions defined in
% file 'LICENSE.txt', which is part of this source code package.
%
% Principal developer : Felipe Bordeu (Felipe.Bordeu@ec-nantes.fr)
%
data = writepxdmf();

data.nodes = cell(2,1);


data.nodes{1} = [ [0 0 0]  
             [1 0 0]
             [2 0 0]
             [3 0 0]
             [4 0 0]
             [5 0 0]
             [6 0 0]
             [4 1 0]
             [5 1 0]             
             [6 1 0]
             [0 2 0]
             [1 1 0]
             [2 2 0]];

data.nodes{2} = [ [0   0 0]     % <----- origin
             [0.5 1 1]];   % <----- spacing

data.cells = cell(2,1);

data.cells{1} = [ 1 1 0 ...        % this is a polynode element with one node 
             2 2 1 2  ...     % this is a polyline with two nodes
             4   3 7 4  ...   % this is a triangle
             5   5 8 9 6  ... % this is a quad
             34  10 12 11];   % this is a XDMF_EDGE_3
         
data.cells{2} = [12 0 0 ];  % <----- number of element per coordinate


data.names = cell(2,2);       % space (x,y)

data.names{1,1} = {'X' 'Y'};
data.names{1,2} = { 'm' 'm' };
data.names{2,1} = {'Z'};
data.names{2,2} = {'m'};

data.nodes_fields = cell(2,1);   % <--- two spaces, one field
data.nodes_fields{1,1} =   1:13; % just to generate randoms modes (one mode)
data.nodes_fields{2,1} =  rand(1,prod(data.cells{2}+1)) ;% just to generate randoms modes
data.nodes_fields_names =  { 'Temperature' };

%no cell data
%data.cell_fields = {};
%data.cell_fields_names = {};

data.filename = 'exampleConvertion.pxdmf';
data.verbose = 1;
data.rectilinear = [0 1];
data.mixed = [1 0];

%% This is the part to convert the data from the old structure to the new structure.
Grids = XdmfGrid(data);
options = writepxdmfset();
options.filename = data.filename;
disp(options)
Grids.Print()
writepxdmf2(Grids,options)
%% export only the first dim in xdmf
options.xdmf = true;
options.filename ='exampleConvertion1';
writepxdmf2(Grids(1),options)
%% export only the second dim in xdmf
options.xdmf = true;
options.filename ='exampleConvertion2';
writepxdmf2(Grids(2),options)

