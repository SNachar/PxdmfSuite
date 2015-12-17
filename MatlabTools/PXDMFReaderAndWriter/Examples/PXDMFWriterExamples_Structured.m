%% Matlab example of how to use the writepxdmf
%
% A directory named pxdmf_examples is created with all the outpout files
%
% Example using 2 spaces 1 nodal field temperatue with 2 terms
%
% $$ \mbox{temperature}(x,y,z) = \sum_{i=1}^{2} t^i(x,y)\cdot t^i(z)$$
%
% space 1 (x,y) a 2d mesh structured  (quad elemens)
%
% space 2  (z)  a 1D structured mesh  (linear element)
%
% 1 field :
%   1 scalar nodal field (temperature) 2 modes
%
% NOTE : the is a bug in the xdmf reader in ParaView. So 1D structured mesh and element
% data are incompatible. Please 1D unstructured mesh in this case
%
%
% This file is subject to the terms and conditions defined in
% file 'LICENSE.txt', which is part of this source code package.
%
% Principal developer : Felipe Bordeu (Felipe.Bordeu@ec-nantes.fr)
%
%

%% 
clear all



%% Output Directory (creation if is needed)
dirname = 'pxdmf_examples';
[SUCCESS,MESSAGE,MESSAGEID] = mkdir(dirname);


%% nodes : is a cell contaning the nodes in each space

nodes = cell(2,1);

% space (x,y)
% first point origin
% second point spacing
% the spacing must allways be positive (no negative, no zero)
nodes{1} = [ [0 0   0 ]   %<---- origin
             [1 0.5 1]];  %<---- spacing

% space (z) 
nodes{2} = [ [0   0 0]    %<---- origin
             [0.1 1 1]];  %<---- spacing

%% cells : is a cell contaning the elements in each dimension
% in this case we use structured mesh so only line, quad, and hexa are
% generated

cells = cell(2,1);

% space (x,y)
cells{1} = [10 20 0 ]; % <--- number of elements in each coordinate

% space (z) 
cells{2} = [ 100 0 0 ]; % <--- number of elements in each coordinate


%% names : is a cell contaning the name of each coordinate for every space 
% (the number of names determine the size of the space, 1D, 2D, 3D)
% firs columns names, second comlumns units
names = cell(2,2);   %size(number_ofdims,2)

% space (x,y)
names{1,1} = {'X' 'Y'};
names{1,2} = { 'm' 'm' };

% space (z)
names{2,1} = {'Z'};
names{2,2} = {'m'};

%% Definitions of the nodal field
nodes_fields = cell(2,1);

% temperature
nodes_fields{1,1} =   rand(1,prod(cells{1}+1)); % just to generate randoms modes
% the number of nodes in each direction is the number of element plus one.

nodes_fields{2,1} =    1:101 ;

%% Names for the fields

nodes_fields_names= cell(1,1);
nodes_fields_names{1}= 'temperature';

%%no cell data

cell_fields = {};
cell_fields_names = {};

%% Ouput of the file with different options 
%Note : we use the 'rectilinear'  and  'mixed' option the tell the type of
%topology for each dimension

filename= [dirname '/Ascii_Structured.pxdmf'];
writepxdmf(filename, nodes, cells, names, nodes_fields, cell_fields, nodes_fields_names, cell_fields_names, 'rectilinear',[1 1]);

 
filename= [dirname '/Ascii_Structured_single.pxdmf'];
writepxdmf(filename, nodes, cells, names, nodes_fields, cell_fields, nodes_fields_names, cell_fields_names, 'from1',1,'precision','single','rectilinear',[1 1]);
 
filename= [dirname '/Binary_Structured.pxdmf'];
writepxdmf(filename, nodes, cells, names, nodes_fields, cell_fields, nodes_fields_names, cell_fields_names, 'bin',1,'from1',1,'rectilinear',[1 1]);

filename= [dirname '/Binary_Structured_single.pxdmf'];
writepxdmf(filename, nodes, cells, names, nodes_fields, cell_fields, nodes_fields_names, cell_fields_names, 'bin',1,'from1',1,'precision','single','rectilinear',[1 1]);


%%  Output using a struct to hold the data
% also work for a struct with the data

% To get the structure form the file.
data  = writepxdmf();

data.filename = [dirname '/Ascii_Structured_struct.pxdmf'];
data.nodes = nodes;
data.cells = cells;
data.names = names;
data.nodes_fields = nodes_fields;
data.cell_fields = cell_fields;
data.nodes_fields_names =nodes_fields_names;
data.cell_fields_names = cell_fields_names;
data.rectilinear = [1 1];
writepxdmf(data);

%% You can use the reader to read data from a pxdmf file 

ReadData = readpxdmf([dirname '/Ascii_Structured.pxdmf']);

