%% Matlab example of how to use the writexdmf for a xdmf output
%
% A directory named xdmf_examples is created with all the outpout files
%
% Example using a mesh of tets and 3 fields and 2 timesteps
%
%  2D non structured mesh   (tets elemens)
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
nodes = [ [0    0   0] 
          [1    0   0]
          [2    0   0]
          [0    1   0] 
          [1    1   0]
          [2    1   0] 
          [0.5 0.5  1]
          [1.5 0.5  1]];
      
%% cells : the elements of the mesh (connectivity)
% One element in each line. 
% 1 node element (nodal element),  2 node element (bar).
% 3 node element (triangle), 4 node element (quads)
% 6 node element (wedge), 8 node element (hexa)
% NOTE : for tetrehedron please use the 'cells_name' option 
% NOTE : for mixed meshs please use the 'mixed' option
      
% NOTE : tet have 4 nodes like quads, so an extra option must by added
cells = [[1 2 4 7]
         [2 5 4 7]
         [5 2 3 8]
         [3 6 5 8]];
     
%% Definitions of the nodal fields 
% two fields (temperature, despacement)  
nodes_fields = cell(1,2);

% temperature
nodes_fields{1} =   rand(2,size(nodes,1)); % just to genera randoms values (2 time steps)

% displacement
% in this case the value of the field is stored in the form x1 y1 z1 x2 y2 z2... 
 
nodes_fields{2} =   [ 1 1 1 2 2 2 3 3 3 4 4 4 5 5 5 6 6 6 7 7 7 8 8 8    %  x1 y1 z1 x2 y2 z2...  for the first timestep
                      1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 ];  %  x1 y1 z1 x2 y2 z2...  for the second timestep

%% Names for the fields
nodes_fields_names={ 'temperature' 'displacement' };

%%  Definition of the cells fields and cells fields names
cell_fields = cell(1) ;
cell_fields{1} = [ 1 2 3 4 ; -1 -2 -3 -4];
cell_fields_names= { 'density' };

%% Ouput of the file with different options 
% NOTE : We need to put the 'from1' option  because in the XDMF format the connectivity start from ZERO and not from one.
% NOTE : We need to put the 'cell_names' to tell the element type used.

filename= [dirname '/Tet_Ascii.xdmf'];
writexdmf(filename, nodes, cells,  nodes_fields, cell_fields, nodes_fields_names, cell_fields_names, 'from1',1,'verbose',1,'cell_names','tetrahedron');

filename= [dirname '/Tet_Ascii_single.xdmf'];
writexdmf(filename, nodes, cells,  nodes_fields, cell_fields, nodes_fields_names, cell_fields_names, 'from1',1,'precision','single','cell_names','tetrahedron');

filename= [dirname '/Tet_Binary.xdmf'];
writexdmf(filename, nodes, cells,  nodes_fields, cell_fields, nodes_fields_names, cell_fields_names, 'bin',1,'from1',1,'cell_names','tetrahedron');

filename= [dirname '/Tet_Binary_single.xdmf'];
writexdmf(filename, nodes, cells,  nodes_fields, cell_fields, nodes_fields_names, cell_fields_names, 'bin',1, 'from1',1,'precision','single','cell_names','tetrahedron');

%%  Output using a struct to hold the data
% also work for a struct with the data

% To get the structure form the file.
data  = writexdmf();

data.filename = [dirname '/Tet_Ascii_struct.xdmf'];
data.nodes = nodes;
data.cells = cells;
data.nodes_fields = nodes_fields;
data.cell_fields = cell_fields;
data.nodes_fields_names =nodes_fields_names;
data.cell_fields_names = cell_fields_names;
data.verbose = 1;
data.cell_names = 'tetrahedron';
data.from1 = 1;
writexdmf(data);

%% You can use the pxdmf reader to read data from a xdmf file 

ReadData = readpxdmf([dirname '/Tet_Ascii.xdmf']);

