%% Matlab example of how to use the writexdmf for a xdmf output
%
% A directory named xdmf_examples is created with all the outpout files
%
% Example using a mesh of quads and 3 fields and 2 timesteps
%
%  2D non structured mesh   (quad elemens)
%
% 3 field : 2 time steps
%   1 scalar nodal field (temperature)
%   1 vector nodal field (despacement) 
%   1 scalar element field (density)   
%
%
% This file is subject to the terms and conditions defined in
% file 'LICENSE.txt', which is part of this source code package.
%
% Principal developer : Felipe Bordeu (Felipe.Bordeu@ec-nantes.fr)
%


%% 
clear all


%% Output Directory (creation if is needed)
dirname = 'xdmf_examples';
[SUCCESS,MESSAGE,MESSAGEID] = mkdir(dirname);

%% nodes : nodes of the mesh size(number_of_nodes, [1-2-3])
nodes = [ [0 0] 
          [1 0]
          [2 0]
          [0 1] 
          [1 1]
          [2 1] ];

%% cells : the elements of the mesh (connectivity)
% One element in each line. 
% 1 node element (nodal element),  2 node element (bar).
% 3 node element (triangle), 4 node element (quads)
% 6 node element (wedge), 8 node element (hexa)
% NOTE : for tetrehedron please use the 'cells_name' option 
% NOTE : for mixed meshs please use the 'mixed' option

cells = [[1 2 5 4 ]
         [2 3 6 5 ]];
     
%% Definitions of the nodal fields 
% two fields (temperature, despacement)

nodes_fields = cell(1,2);

% temperature
nodes_fields{1} =   rand(2,size(nodes,1)); % just to generate randoms values (2 time steps)

% displacement
% in this case the value of the field is stored in the form x1 y1 z1 x2 y2 z2... 
 
nodes_fields{2} =   [ 0 0 1 1 0 1 2 0 1 0 1 1 1 1 1 2 1 1    %  x1 y1 z1 x2 y2 z2...  for the first time step
                        1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 ];  %  x1 y1 z1 x2 y2 z2...  for the second time step

%% Names for the fields
nodes_fields_names={ 'temperature' 'displacement' };

%% Definition of the cells fields and cells fields names
% In the same way as for the nodes

cell_fields = cell(1) ;
cell_fields{1} = [ 0.9 1 ; 1 1.2];
cell_fields_names= { 'Density' };

%% Ouput of the file with different options 
% NOTE : We need to put the 'from1' option because in the XDMF format the connectivity start from ZERO and not from one.

filename= [dirname '/Ascii.xdmf'];
writexdmf(filename, nodes, cells,  nodes_fields, cell_fields, nodes_fields_names, cell_fields_names, 'from1',1,'verbose',1);

filename= [dirname '/Ascii_single.xdmf'];
writexdmf(filename, nodes, cells,  nodes_fields, cell_fields, nodes_fields_names, cell_fields_names, 'from1',1,'precision','single');

filename= [dirname '/Binary.xdmf'];
writexdmf(filename, nodes, cells,  nodes_fields, cell_fields, nodes_fields_names, cell_fields_names, 'bin',1,'from1',1);

filename= [dirname '/Binary_single.xdmf'];
writexdmf(filename, nodes, cells,  nodes_fields, cell_fields, nodes_fields_names, cell_fields_names, 'bin',1, 'from1',1,'precision','single');

%%  Output using a struct to hold the data
% also work for a struct with the data

% To get the structure form the file.
data  = writexdmf();

data.filename = [dirname '/Ascii_struct.xdmf'];
data.nodes = nodes;
data.cells = cells;
data.nodes_fields = nodes_fields;
data.cell_fields = cell_fields;
data.nodes_fields_names = nodes_fields_names;
data.cell_fields_names = cell_fields_names;
data.verbose = 1;
data.from1 = 1;
writexdmf(data);

%% You can use the pxdmf reader to read data from a xdmf file 

ReadData = readpxdmf([dirname '/Ascii.xdmf']);

