%% Matlab example of how to use the writepxdmf
%
% A directory named pxdmf_examples is created with all the outpout files
%
% Example using 2 spaces 1 nodal field temperatue with 2 terms
%
% $$ \mbox{temperature}(x,y,z) = \sum_{i=1}^{2} t^i(x,y)\cdot t^i(z)$$
%
% space 1 (x,y) a 2d mesh unstructured  (mixed elemens)
%
% space 2  (z)  a 1D  structured mesh  (linear element)
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

%% 
clear all; %#ok<CLSCR>



%% Output Directory (creation if is needed)
dirname = 'pxdmf_examples';
[SUCCESS,MESSAGE,MESSAGEID] = mkdir(dirname);


%% nodes : is a cell contaning the nodes in each space

nodes = cell(2,1);

% space (x,y)
nodes{1} = [ [0 0 0]  
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

% space (z) (structured)
% first point origin
% second point spacing
% NOTE : the spacing must allways be positive (no negative, no zero)
nodes{2} = [ [0   0 0]     % <----- origin
             [0.5 1 1]];   % <----- spacing

%% cells : is a cell contaning the elements (connectivity) in each dimension
% One element in each line. 
% 1 node element (nodal element),  2 node element (bar).
% 3 node element (triangle), 4 node element (quads)
% 6 node element (wedge), 8 node element (hexa)
% NOTE : for tetrehedron please use the 'cells_name' option 
% NOTE : for mixed meshs please use the 'mixed' option

cells = cell(2,1);

% space (x,y) mixed element 
% in the case of mixed element no matrix is posible so a long vector
% is needed
cells{1} = [ 1 1 0 ...        % this is a polynode element with one node 
             2 2 1 2  ...     % this is a polyline with two nodes
             4   3 7 4  ...   % this is a triangle
             5   5 8 9 6  ... % this is a quad
             34  10 12 11];   % this is a XDMF_EDGE_3
% Note: to know the number for each element please read the PXDMF format
% file document at rom.ec-nantes.fr.fr

% space(z) 
cells{2} = [12 0 0 ];  % <----- number of element per coordinate


%% names : is a cell contaning the name of each coordinate for every space 
% (the number of names determine the size of the space, 1D, 2D, 3D)
% firs columns names, second comlumns units
names = cell(2,2);       % space (x,y)

% space (x,y)
names{1,1} = {'X' 'Y'};
names{1,2} = { 'm' 'm' };

% space (z)
names{2,1} = {'Z'};
names{2,2} = {'m'};



%% Definitions of the nodal fields 
nodes_fields = cell(2,1);   % <--- two spaces, one field

% temperature
nodes_fields{1,1} =   1:13; % just to generate randoms modes (one mode)

nodes_fields{2,1} =  rand(1,prod(cells{2}+1)) ;% just to generate randoms modes
%(the number of nodes in each direction is the number of element plus one)

%% Names for the fields
nodes_fields_names =  { 'Temperature' };

%no cell data

cell_fields = {};
cell_fields_names = {};

%% Ouput of the file with different options 
%Note : we use the 'rectilinear'  and  'mixed' option the tell the type of
%topology for each dimension

filename= [dirname '/Ascii_Mixed.pxdmf'];
writepxdmf(filename, nodes, cells, names, nodes_fields, cell_fields, nodes_fields_names, cell_fields_names, 'rectilinear',[0 1],'mixed',[1 0]);
 
filename= [dirname '/Ascii_Mixed_single.pxdmf'];
writepxdmf(filename, nodes, cells, names, nodes_fields, cell_fields, nodes_fields_names, cell_fields_names, 'precision','single','rectilinear',[0 1],'mixed',[1 0]);
 
filename= [dirname '/Binary_Mixed.pxdmf'];
writepxdmf(filename, nodes, cells, names, nodes_fields, cell_fields, nodes_fields_names, cell_fields_names, 'bin',1,'rectilinear',[0 1],'mixed',[1 0]);

filename= [dirname '/Binary_Mixed_single.pxdmf'];
writepxdmf(filename, nodes, cells, names, nodes_fields, cell_fields, nodes_fields_names, cell_fields_names, 'bin',1, 'precision','single','rectilinear',[0 1],'mixed',[1 0]);
 
%%  Output using a struct to hold the data
% also work for a struct with the data

% To get the structure form the file.
data  = writexdmf();

% To get the structure form the file.
data.filename = [dirname '/Ascii_Mixed_struct.pxdmf'];
data.nodes = nodes;
data.cells = cells;
data.names = names;
data.nodes_fields = nodes_fields;
data.cell_fields = cell_fields;
data.nodes_fields_names =nodes_fields_names;
data.cell_fields_names = cell_fields_names;
data.verbose = 1;
data.rectilinear = [0 1];
data.mixed = [1 0];
writepxdmf(data);

%% You can use the pxdmf reader to read data from a pxdmf file 

ReadData = readpxdmf([dirname '/Ascii_Mixed.pxdmf'],'verbose',1);

