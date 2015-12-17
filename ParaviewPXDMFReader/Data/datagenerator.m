clear all

x = linspace(0,1,100);
y = linspace(0,1,100);
z = linspace(1,2,100);

M1X = x;
M1Y = y;
M1Z = z;

M2X = x.^2;
M2Y = sin(y*pi*2);
M2Z = sqrt(z);


D1X = ones(size(x));
D1Y = ones(size(y));
D1Z = ones(size(z));

D2X = 2*x;
D2Y = pi*2*cos(y*pi*2);
D2Z = 1./(2*sqrt(z));


nodes = cell(3);

nodes{1} = x';
nodes{2} = y';
nodes{3} = z';

elements = cell(3);

elements{1}  = [ [1:length(x)-1]' [2:length(x)]'];
elements{2}  = [ [1:length(y)-1]' [2:length(y)]'];
elements{3}  = [ [1:length(z)-1]' [2:length(z)]'];

names = cell(3,2);
name = cell (1);
name{1} = 'X';
names{1,1} = name;
name{1} = 'm';
names{1,2} = name;

name{1} = 'Y';
names{2,1} = name;
name{1} = 'Km';
names{2,2} = name;

name{1} = 'Z';
names{3,1} = name;
name{1} = 'K';
names{3,2} = name;

nodes_fields = cell(3,4);
nodes_fields{1,1} = [M1X; M2X];
nodes_fields{2,1} = [M1Y; M2Y];
nodes_fields{3,1} = [M1Z; M2Z];

nodes_fields{1,2} = [D1X; D2X];
nodes_fields{2,2} = [M1Y; M2Y];
nodes_fields{3,2} = [M1Z; M2Z];

nodes_fields{1,3} = [M1X; M2X];
nodes_fields{2,3} = [D1Y; D2Y];
nodes_fields{3,3} = [M1Z; M2Z];

nodes_fields{1,4} = [M1X; M2X];
nodes_fields{2,4} = [M1Y; M2Y];
nodes_fields{3,4} = [D1Z; D2Z];


nodes_fields_names = cell(4);

nodes_fields_names{1} = 'Scalar2modes';
nodes_fields_names{2} = 'Scalar2modesdX';
nodes_fields_names{3} = 'Scalar2modesdY';
nodes_fields_names{4} = 'Scalar2modesdZ';


writepxdmf('1D1D',nodes, elements, names, nodes_fields,{},nodes_fields_names, {},'from1','precision=double')