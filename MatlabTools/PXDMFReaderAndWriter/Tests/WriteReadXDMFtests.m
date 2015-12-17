function res = WriteReadXDMFtests
% Matlab example of how to use the writexdmf and readpxdmf
%
% A directory named examples is created with all the outpout files
%
% This file is subject to the terms and conditions defined in
% file 'LICENSE.txt', which is part of this source code package.
%
% Principal developer : Felipe Bordeu (Felipe.Bordeu@ec-nantes.fr)
%

%% the path to write the output files
disp('-WriteReadXDMFtests.m----------------------------------')

thepath = 'output/xdmf_test_output';
[~,~,~] = rmdir(thepath,'s');
[~,~,~] = mkdir(thepath);

%the nodes
nodes = [0.0    0.0    0.0
         1.0    0.0    0.0
         0.0    1.0    0.0
         1.0    1.0    0.0
         0.0    0.0    1.0
         1.0    0.0    1.0
         0.0    1.0    1.0
         1.0    1.0    1.0
         0.0    0.0    2.0
         1.0    0.0    2.0
         0.0    1.0    2.0
         1.0    1.0    2.0];
     
nbnodes = size(nodes ,1);

%% elements wedges 
elements = [0 1 4 2 3 6
           4 5 9 6 7 11];

elements = int32(elements);

%% node fileds
nodes_fields = cell(1,1);

% vector with 3 components
% dep = [[u1 v1 w1 u2 v2 w2 ...]   first time step
%         u1 v1 w1 u2 v2 w2 ...]]; second time step
nodes_fields{1,1} = [    0.9501    0.9218    0.1389 0.2311    0.7382    0.2028 0.6068    0.1763    0.1987 0.4860    0.4057    0.6038 0.8913    0.9355    0.2722     0.7621    0.9169    0.1988  0.4565    0.4103    0.0153 0.0185    0.8936    0.7468 0.8214    0.0579    0.4451 0.4447    0.3529    0.9318 0.6154    0.8132    0.4660 0.7919    0.0099    0.4186];
nodes_fields{1}(2,:) = rand(1,nbnodes *3);
% temp
% scalar 
nodes_fields{1,2} = [[0 1 2 3 4  5  6 4 5  6  7  8  ]
                     [0 2 4 6 8 10 12 8 10 12 14 16 ]];

%% names of the node fields
nodes_fields_names= cell(1,2);
nodes_fields_names{1}= 'dep';
nodes_fields_names{2}= 'temp';

%% cell fields 
cell_fields = cell(1,1) ;
cell_fields{1,1} = [ [0 1]
                    [2 3] ];
cell_fields_names= cell(1,1);

%%cell fiedls names
cell_fields_names{1}= 'P';

%% Output. Store the return value to check if everythin is ok
a = zeros(1,6);

%%ascii
filenames{1}=  [thepath  '/ASCII.xdmf'];
disp(['writting : ' filenames{1}] )
tic;
a(1)=writexdmf(filenames{1},  nodes, elements ,nodes_fields, cell_fields, nodes_fields_names, cell_fields_names,'debug',0,'verbose',0);
wtimes(1) = toc;
tol(1) = 1e-15;

% binary
filenames{2}= [thepath  '/binary.xdmf'];
disp(['writting : ' filenames{2}] )
tic
a(2)=writexdmf(filenames{2},  nodes, elements ,nodes_fields, cell_fields, nodes_fields_names, cell_fields_names,'bin',1,'max_ASCII',1);
wtimes(2) = toc;
tol(2) = 1e-15;

%H5 and compressed
filenames{3}= [thepath  '/H5.xdmf'];
disp(['writting : ' filenames{3}] )
tic
a(3)=writexdmf(filenames{3},  nodes, elements ,nodes_fields, cell_fields, nodes_fields_names, cell_fields_names,'HDF5',1,'max_ASCII',1);
wtimes(3) = toc;
tol(3) = 1e-15;

%H5
filenames{4}= [thepath  '/h5_comp.xdmf'];
disp(['writting : ' filenames{4}] )
tic
a(4)=writexdmf(filenames{4},  nodes, elements ,nodes_fields, cell_fields, nodes_fields_names, cell_fields_names,'HDF5',1,'gzip',1,'max_ASCII',1);
wtimes(4) = toc;
tol(4) = 1e-15;

%also work for a struct with the data
data = writexdmf();

data.filename = [thepath '/Ascii_struct.pxdmf'];
data.nodes = {nodes};
data.cells = {elements};
data.nodes_fields = nodes_fields;
data.cell_fields = cell_fields;
data.nodes_fields_names =nodes_fields_names;
data.cell_fields_names = cell_fields_names;
data.from1 = true;
filenames{5} = data.filename;
disp(['writting : ' filenames{5}] )
tic
writexdmf(data);
wtimes(5) = toc; %#ok<*NASGU>
tol(5) = 1e-15;

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% for constant structured data
Number_of_element = [2 3 6]; %size of the cube
origin = [0 0 1];  % start point of the data
spacing = [1 2 3]; % dx dy dz

nodes_fields = cell(1);
nodes_fields{1} = rand(2,prod(Number_of_element+1));
nodes_fields_names = cell(1);
nodes_fields_names{1} = 'datanode';

cell_fields = cell(1);
cell_fields{1} = rand(2,prod(Number_of_element));
cell_fields_names = cell(1);
cell_fields_names{1} = 'datacell';

filename = [thepath  '/ASCII_constrectilinear'];
disp(['writting : ' filename] )
writexdmf(filename, [origin;spacing] , Number_of_element, nodes_fields,cell_fields,nodes_fields_names,cell_fields_names,'rectilinear',1,'debug',0);
filename = [thepath  '/ASCII_constrectilinear_bin'];
disp(['writting : ' filename] )
writexdmf(filename, [origin;spacing] , Number_of_element, nodes_fields,cell_fields,nodes_fields_names,cell_fields_names,'rectilinear',1,'debug',0,'bin',true);

disp('-------------------------')
%% Reading and comparing data
comp = zeros(1,length(filenames));
rtime = zeros(1,length(filenames));
for i= 1:length(filenames)
    disp(['reading  : ' filenames{i}])
    tic
    in_data = readpxdmf(filenames{i}, 'From1',1,'xdmf',1);
    rtime(i) = toc;
    comp(i) =comparesol(data,in_data,tol(i));
end

%% Cleaning data 
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

%% figure

filesize = filesize/1000/1000;
fig = figure;

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

set(gca,'xticklabel',filenames_clean)
title('Check Xdmf Writer');
print(fig, '-depsc2','output/WriteReadXDMFTest')
runexamples();
return 

end
function runexamples()

run('../Examples/XDMFWriterExamples_Non_Structured.m'); 
run('../Examples/XDMFWriterExamples_Non_Structured_Tets.m'); 

op.format = 'pdf';
op.evalCode = false;

publish('../Examples/XDMFWriterExamples_Non_Structured.m',op);
publish('../Examples/XDMFWriterExamples_Non_Structured_Tets.m',op);

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
 %try
    res  = comparedata(ref.nodes, a.nodes,tol);
    nbdims =size(ref.cells,1);

    for i = 1 : nbdims
        res = min(res, comparedataint(ref.cells(i), a.cells(i)));
        res = min(res, comparedata(ref.nodes_fields(i), a.nodes_fields(i), tol));
        res = min(res, comparedata(ref.cell_fields(i), a.cell_fields(i), tol));  
    end
 %catch ME
 %   res = 1;
 %end
end



