%% Matlab example of how to use the writepxdmf
%
% A directory named pxdmf_examples is created with all the outpout files
%
% Example using 3 spaces 2 nodal field and 1 element field
%
% $$ \mbox{temperature}(x,y,z) = \sum_{i=1}^{2} t^i(x,y)\cdot t^i(z)$$
%
% space 1 (x,y) a 2d mesh non structured  (quad elemens)
%
% space 2  (z)  a 1D non structured mesh  (linear element)
%
% space 3  (t)  a 1D non structured mesh  (nodal elements)
%
% 3 field :
%   1 scalar nodal field (temperature) 2 modes
%   1 vector nodal field (despacement) 2 modes 
%   1 scalar element field (density)   1 mode
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

nodes = cell(3,1);

% space (x,y)
nodes{1} = [ [0 0] 
             [1 0]
             [2 0]
             [0 1] 
             [1 1]
             [2 1] ];

% space (z) 
nodes{2} = [ 0    
             0.5 
             1   
             1.5  
             1.75
             1.8 
             1.9 
             2    ];

% space (t) 
nodes{3} = [ 0 
             1 
             2 
             3  
             4 
             5 ];

%% cells : is a cell contaning the elements (connectivity)  in each dimension
% One element in each line. 
% 1 node element (nodal element),  2 node element (bar).
% 3 node element (triangle), 4 node element (quads)
% 6 node element (wedge), 8 node element (hexa)
% NOTE : for tetrehedron please use the 'cells_name' option 
% NOTE : for mixed meshs please use the 'mixed' option

cells = cell(3,1);   %<------- three dimension

% space (x,y) quads
cells{1} = [[1 2 5 4 ]
            [2 3 6 5 ]];

% space (z)  2 nodes lines
cells{2} = [(1:(size(nodes{2},1)-1))' (2:size(nodes{2},1))'  ];

% this is equal to :
%
%cells{2} = [ 1 2
%             2 3
%             3 4
%             4 5
%             5 6 ];
%

% space (t) 1 node element 
cells{3} =  (1:(size(nodes{3},1)))' ;
 
% this is iqual to 
%cells{3} = [ 1 
%             2 
%             3 
%             4 
%             5
%             6 ];

%% names : is a cell contaning the name of each coordinate for every space 
% (the number of names determine the size of the space, 1D, 2D, 3D)
% firs columns names, second comlumns units
names = cell(3,2);  

names{1,1} = {'X' 'Y'};
names{1,2} = { 'm' 'm' };

names{2,1} = {'Z'};
names{2,2} = {'m'};

names{3,1} = {'T'};
names{3,2} = {'s'};

%% Definitions of the nodal fields 
nodes_fields = cell(3,2); % <--- three space, two fields


% temperature
nodes_fields{1,1} =   rand(2,size(nodes{1},1)); % just to genera randoms modes

nodes_fields{2,1} =   [ 1 1 1 1 1 1 1 1
                        1 2 3 2 1 2 3 1 ];

nodes_fields{3,1} =   [ 1    2 3 4 5 6 
                        0.1 -1 2 4 5 9]; 

% displacement
% in this case the value of the field is stored in the form x1 y1 z1 x2 y2 z2... 
 
nodes_fields{1,2} =   [ 0 0 1 1 0 1 2 0 1 0 1 1 1 1 1 2 1 1    %  x1 y1 z1 x2 y2 z2...  for the first mode
                        1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 ];  %  x1 y1 z1 x2 y2 z2...  for the second mode mode

nodes_fields{2,2} =   [ 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 
                        1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 ];

nodes_fields{3,2} =   [ -1 1 1 1 1 -1 1 1 -1 1 1 1 -1 1 1 -1 1 -1 
                        -1 1 1 1 1 1 1 1 1 1 -1 1 1 -1 1 1 1 1 ];
                    
%% Names for the fields   
nodes_fields_names= { 'Temperature' 'Displacement'};

%% Definition of the cells fields and cells fields names
%  In the same way as for the nodes 

cell_fields = cell(3,1) ;
cell_fields{1,1} = [ 0.9 1 ];
cell_fields{2,1} = [ 1 1 1 1 1 1 1 ];
cell_fields{3,1} = [ 1 0.9 0.8 0.7 0.6 0.5];
cell_fields_names = { 'Density' };

%% Ouput of the file with different options 
% NOTE : We need to put the 'from1' option because in the XDMF format the connectivity start from ZERO and not from one.

filename= [dirname '/Ascii.pxdmf'];
writepxdmf(filename, nodes, cells, names, nodes_fields, cell_fields, nodes_fields_names, cell_fields_names, 'from1',1,'verbose',1);

filename= [dirname '/Ascii_single.pxdmf'];
writepxdmf(filename, nodes, cells, names, nodes_fields, cell_fields, nodes_fields_names, cell_fields_names, 'from1',1,'precision','single');

filename= [dirname '/Binary.pxdmf'];
writepxdmf(filename, nodes, cells, names, nodes_fields, cell_fields, nodes_fields_names, cell_fields_names, 'bin',1,'from1',1);


filename= [dirname '/Binary_single.pxdmf'];
writepxdmf(filename, nodes, cells, names, nodes_fields, cell_fields, nodes_fields_names, cell_fields_names, 'bin',1, 'from1',1,'precision','single');

%%  Output using a struct to hold the data
% also work for a struct with the data

% To get the structure form the file.
data  = writepxdmf();

data.filename = [dirname '/Ascii_struct.pxdmf'];
data.nodes = nodes;
data.cells = cells;
data.names = names;
data.nodes_fields = nodes_fields;
data.cell_fields = cell_fields;
data.nodes_fields_names = nodes_fields_names;
data.cell_fields_names = cell_fields_names;
data.verbose = 1;
data.from1 = 1;
writepxdmf(data);

%% You can use the reader to read data from a pxdmf file 

ReadData = readpxdmf([dirname '/Ascii.pxdmf']);

