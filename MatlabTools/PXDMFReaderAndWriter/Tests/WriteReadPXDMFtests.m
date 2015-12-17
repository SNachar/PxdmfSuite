function res = WriteReadPXDMFtests
% Matlab test file for the writepxdmf and readpxdmf function
% 
%   A figure es created with the times for writing an reading the data
%   and sizes of the files. 
%
% A directory is crated with all the output files
%
%  Problem with 4 dimension (x,y),z,t
%
% 4 fields :
%    3 nodes fields : 'dep_x' 'dep_y' 'dep_z'
%    1 element fields : 'P'
%
%
% This file is subject to the terms and conditions defined in
% file 'LICENSE.txt', which is part of this source code package.
%
% Principal developer : Felipe Bordeu (Felipe.Bordeu@ec-nantes.fr)
%
disp('-WriteReadPXDMFtests.m----------------------------------')

dirname = 'output/pxdmf_test_output' ;
[~,~,~] = mkdir(dirname);

%nodes is a cell contaning the nodes in each dimention
%the nodes

nb_nodes = [5^2 10 10];
modes = 20;
%nb_nodes = [100^2 1000 1000];
%modes = 200;

nnb = sqrt(nb_nodes(1));

nodes = cell(3,1);

x = linspace(0,1,sqrt(nb_nodes(1)));
dx = (sin(x*nnb/2))*x(2);


% nodes generation 
nodes{1} = zeros(nb_nodes(1),3);
cpt =1;
for i = 1: sqrt(nb_nodes(1))
    for j = 1:sqrt(nb_nodes(1))
        nodes{1}(cpt,:)= [x(i)-dx(j) x(j)+dx(i) 0];
        cpt = cpt +1;
    end
end
nodes{2} = [linspace(0,1,nb_nodes(2))' zeros(nb_nodes(2),2)];
nodes{3} = [linspace(0,1,nb_nodes(3))' zeros(nb_nodes(3),2)];


%elements is a cell contaning the elements in each dimention
% 1 node element (nodal element),  2 node element (bar).
% 3 node element (triangle), 4 node element (square)
% 6 node element (wedge), 8 node element (hexa)

cells = cell(3,1);

% nodes generation 
cells{1} = zeros((sqrt(nb_nodes(1))-1)^2*2,3 );
cpt =1;
for j = 1: sqrt(nb_nodes(1)) -1
    for i = 1: sqrt(nb_nodes(1)) -1
        cells{1}(cpt, :) = [i i+1 nnb+i]+(j-1)*nnb;
        cpt =cpt +1;
        cells{1}(cpt, :) = [i+1 nnb+i+1 nnb+i]+(j-1)*nnb;
        cpt =cpt +1;
    end
end

cells{2} = [(1:(size(nodes{2},1)-1))' (2:size(nodes{2},1))'  ];
cells{3} =  [(1:(size(nodes{3},1)-1))' (2:size(nodes{3},1))'  ];
 
% change to int32 
for i = 1:3
    cells{i} = int32(cells{i});
end


nodes = cell(3,1);

x = linspace(0,1,sqrt(nb_nodes(1)));
dx = (sin(x*nnb/2))*x(2);


% nodes generation 
nodes{1} = zeros(nb_nodes(1),3);
cpt =1;
for i = 1: sqrt(nb_nodes(1))
    for j = 1:sqrt(nb_nodes(1))
        nodes{1}(cpt,:)= [x(i)-dx(j) x(j)+dx(i) 0];
        cpt = cpt +1;
    end
end
nodes{2} = [linspace(0,1,nb_nodes(2))' zeros(nb_nodes(2),2)];
nodes{3} = [linspace(0,1,nb_nodes(3))' zeros(nb_nodes(3),2)];


%elements is a cell contaning the elements in each dimention
% 1 node element (nodal element),  2 node element (bar).
% 3 node element (triangle), 4 node element (square)
% 6 node element (wedge), 8 node element (hexa)

cells = cell(3,1);

% nodes generation 
cells{1} = zeros((sqrt(nb_nodes(1))-1)^2*2,3 );
cpt =1;
for j = 1: sqrt(nb_nodes(1)) -1
    for i = 1: sqrt(nb_nodes(1)) -1
        cells{1}(cpt, :) = [i i+1 nnb+i]+(j-1)*nnb;
        cpt =cpt +1;
        cells{1}(cpt, :) = [i+1 nnb+i+1 nnb+i]+(j-1)*nnb;
        cpt =cpt +1;
    end
end

cells{2} = [(1:(size(nodes{2},1)-1))' (2:size(nodes{2},1))'  ];
cells{3} =  [(1:(size(nodes{3},1)-1))' (2:size(nodes{3},1))'  ];
 
% change to int32 
for i = 1:3
    cells{i} = int32(cells{i});
end

%names is a cell contaning the name of each dimention (the number of names determine the size of the dimention)
%firs columns names, second comlumns units
%names is a cell contaning the name of each dimention (the number of names determine the size of the dimention)
%firs columns names, second comlumns units
names = cell(3,2);

names{1,1} = {'x' 'y'};
names{1,2} = {'m' 'm'};

names{2,1} = {'z'};
names{2,2} = {'m'};

names{3,1} = {'t'};
names{3,2} = {'K'} ;

% three fields (dep_x, dep_y, dep_z)
nodes_fields = cell(3,3);
% 2 because we have only 2 modes

%generation of modes

for dim= 1:3
    nodes_fields{dim,1} = zeros(modes,nb_nodes(dim));
    nodes_fields{dim,2} = zeros(modes,nb_nodes(dim));
    nodes_fields{dim,3} = zeros(modes,nb_nodes(dim));
    for m= 1: modes
        nodes_fields{dim,1}(m,:) = sin(nodes{dim}(:,1)*m);
        nodes_fields{dim,2}(m,:) = cos((nodes{dim}(:,1)+nodes{dim}(:,2))*m);
        nodes_fields{dim,3}(m,:) = cos(nodes{dim}(:,1)*m).*sin(nodes{dim}(:,1)*(modes-m)) ;
    end
end

%nodes_fields{1,1} =   rand(2,size(nodes{1},1)); % just to genera randoms modes
%nodes_fields{2,1} =   [ 1 1 1 1 1 1  1 1 1 1 1 1 
%                        2 1 1 4 1 7  2 1 1 4 1 7 ];
%nodes_fields{3,1} =   [ 3 1 1 1 1 1  3 1 1 1 1 1 
%                        4 1 1 1 1 1  4 1 1 1 1 1];	

%nodes_fields{1,2} =   	rand(1,size(nodes{1},1));
%nodes_fields{2,2} =   [ 6 1 1 1 1 1 1 1 1 1 1 1 ];
%nodes_fields{3,2} =   [ 7 1 1 1 1 1 1 1 1 1 1 1 ];
	
%for i=1:3
% nodes_fields{i,3} = rand(1,size(nodes{i},1));
%end

% so three fields names (dep_x, dep_y, dep_z)
nodes_fields_names= cell(1,2);
nodes_fields_names{1}= 'dep_x';
nodes_fields_names{2}= 'dep_y';
nodes_fields_names{3}= 'dep_z';

% like the nodes_fields but for the cells here P is like the pressure
cell_fields = cell(3,1) ;
cell_fields{1,1} = rand(1,size(cells{1},1));
cell_fields{2,1} = rand(1,size(cells{2},1));
cell_fields{3,1} = rand(1,size(cells{3},1));
cell_fields_names= cell(1,1);
cell_fields_names{1}= 'P';


wtimes = zeros(1,8);
filenames = cell(1,8);

filename= [ dirname '/HDF5_gzip.pxdmf'];
filenames{1} = filename;
disp(['writting : ' filename] )
tic
writepxdmf(filename, nodes, cells, names, nodes_fields, cell_fields, nodes_fields_names, cell_fields_names, 'HDF5',1, 'gzip',1, 'from1',1);
wtimes(1) = toc;
tol(1) = 1e-15;


filename= [dirname '/HDF5.pxdmf'];
filenames{2} = filename;
disp(['writting : ' filename] )
tic
writepxdmf(filename, nodes, cells, names, nodes_fields, cell_fields, nodes_fields_names, cell_fields_names, 'HDF5',1, 'from1',1);
wtimes(2) = toc;
tol(2) = 1e-15;


filename= [dirname '/Ascii.pxdmf'];
filenames{3} = filename;
disp(['writting : ' filename] )
tic
writepxdmf(filename, nodes, cells, names, nodes_fields, cell_fields, nodes_fields_names, cell_fields_names, 'from1',1);
wtimes(3) = toc;
tol(3) = 1e-15;


filename= [dirname '/HDF5_gzip_single.pxdmf'];
filenames{4} = filename;
disp(['writting : ' filename] )
tic
writepxdmf(filename, nodes, cells, names, nodes_fields, cell_fields, nodes_fields_names, cell_fields_names, 'HDF5',1, 'gzip',1, 'from1',1,'precision','single');
wtimes(4) = toc;
tol(4) = 1e-7;


filename= [dirname '/HDF5_single.pxdmf'];
filenames{5} = filename;
disp(['writting : ' filename] )
tic
writepxdmf(filename, nodes, cells, names, nodes_fields, cell_fields, nodes_fields_names, cell_fields_names, 'HDF5',1, 'from1',1,'precision','single');
wtimes(5) = toc;
tol(5) = 1e-7;


filename= [dirname '/Ascii_single.pxdmf'];
filenames{6} = filename;
disp(['writting : ' filename] )
tic
writepxdmf(filename, nodes, cells, names, nodes_fields, cell_fields, nodes_fields_names, cell_fields_names, 'from1',1,'precision','single');
wtimes(6) = toc;
tol(6) = 1e-7;


filename= [dirname '/Binary.pxdmf'];
filenames{7} = filename;
disp(['writting : ' filename] )
tic
writepxdmf(filename, nodes, cells, names, nodes_fields, cell_fields, nodes_fields_names, cell_fields_names, 'bin',1,'from1',1);
wtimes(7) = toc;
tol(7) = 1e-15;


filename= [dirname '/Binary_single.pxdmf'];
filenames{8} = filename;
disp(['writting : ' filename] )
tic
writepxdmf(filename, nodes, cells, names, nodes_fields, cell_fields, nodes_fields_names, cell_fields_names, 'bin',1, 'from1',1,'precision','single');
wtimes(8) = toc;
tol(8) = 1e-15;

%also work for a struct with the data
data.filename = [dirname '/Ascii_struct.pxdmf'];
data.nodes = nodes;
data.cells = cells;
data.names = names;
data.nodes_fields = nodes_fields;
data.cell_fields = cell_fields;
data.nodes_fields_names =nodes_fields_names;
data.cell_fields_names = cell_fields_names;
data.from1 = 1;
filenames{9} = data.filename;
disp(['writting : ' data.filename] )
tic
writepxdmf(data);
wtimes(9) = toc;
tol(9) = 1e-15;
disp('-------------------------')

comp = zeros(1,length(filenames));
rtime = zeros(1,length(filenames));
for i= 1:length(filenames)
    disp(['reading  : ' filenames{i}])
    tic
    in_data = readpxdmf(filenames{i}, 'From1',1);
    rtime(i) = toc;
    comp(i) =comparesol(data,in_data,tol(i));
end

%figure(1);


cpt = 1;
filenames_clean  = cell(size(filenames));
filesize = zeros(1,length(filenames));

for i = filenames
    %disp(i)
    name = i{1};
    filenames_clean{cpt} = name(1:find(name=='.')-1);
    s = dir([filenames_clean{cpt} '.*']);
    filesize(cpt) = 0;
    for f= 1:size(s,1)
        filesize(cpt) = filesize(cpt) + s(f).bytes;    
    end
    index = find(name=='/');
    filenames_clean{cpt} = name(index(end)+1:find(name=='.')-1);
    cpt = cpt +1;
end

filesize = filesize/1000/1000;
fig = figure ;
bar([wtimes' rtime' filesize']);
legend('Write Time (s)','Read Time (s)','File Size (Mb)');
set(gca,'FontSize',8)

if (sum(abs(comp)) ~= 0 )
	disp('Error in the I/O funtions')
    a = 1:length(comp);
    disp(a(comp~=0))
    %disp(result(comp~=0)')
    res = false;
else
	disp('Everything looks fine.')
    res = true;
end

%set(get(1,'CurrentAxes'),'XTickLabel',filenames)
set(gca,'xticklabel',filenames_clean);
title('Check Pxdmf Writer');
print(fig, '-depsc2','output/WriteReadPXDMFTest')

runexamples();


end

function runexamples()
disp('runing examples ' );

run('../Examples/PXDMFWriterExamples_Mixed.m');
run('../Examples/PXDMFWriterExamples_Non_Structured.m'); 
run('../Examples/PXDMFWriterExamples_Structured.m');

op.format = 'pdf';
op.evalCode = false;

publish('../Examples/PXDMFWriterExamples_Mixed.m',op);
publish('../Examples/PXDMFWriterExamples_Non_Structured.m',op);
publish('../Examples/PXDMFWriterExamples_Structured.m',op);

end


function res =comparedata(a,b,tol)
    if ~exist('tol','var')
        tol = 1e-15;
    end
    res = min(min( abs(cell2mat(a) - cell2mat(b))./cell2mat((a))< tol ))  ~=1 ;
end

function res =comparedataint(a,b)
    res = max(max( abs(cell2mat(a) - cell2mat(b))))  ~= 0 ;
end

function res = comparesol(ref,a,tol)

    res  = comparedata(ref.nodes, a.nodes,tol);
    nbdims =size(ref.cells,1);

    for i = 1 : nbdims
        res = min(res, comparedataint(ref.cells(i), a.cells(i)));
        res = min(res, comparedata(ref.nodes_fields(i), a.nodes_fields(i), tol));
        res = min(res, comparedata(ref.cell_fields(i), a.cell_fields(i), tol));  
    end
end