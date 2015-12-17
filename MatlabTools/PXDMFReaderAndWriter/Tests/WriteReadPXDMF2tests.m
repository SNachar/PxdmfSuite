function res = WriteReadPXDMF2tests()
%
% This file is subject to the terms and conditions defined in
% file 'LICENSE.txt', which is part of this source code package.
%
% Principal developer : Felipe Bordeu (Felipe.Bordeu@ec-nantes.fr)
%

[~,~,~] = rmdir ('output/WriteReadPxdmf2_test_output','s');
[~,~,~] = rmdir ('output/WriteReadXdmf2_test_output','s');
res = true;
res = res & CheckXdmfWriter2;
res = res & CheckPxdmfWriter2;
res = res & CheckXdmfWritertemporalGrid;
end

function res = CheckPxdmfWriter2

nb_nodes = [5^2 10 10];
modes = 20;
%nb_nodes = [100^2 1000 1000];
%modes = 200;

nnb = sqrt(nb_nodes(1));

x = linspace(0,1,sqrt(nb_nodes(1)));
dx = (sin(x*nnb/2))*x(2);

Grid1 = XdmfGrid();
Grid2 = XdmfGrid();
Grid3 = XdmfGrid();

%% nodes generation 
cpt =1;
for i = 1: sqrt(nb_nodes(1))
    for j = 1:sqrt(nb_nodes(1))
        Grid1.geometry(cpt,:)= [x(i)-dx(j) x(j)+dx(i) 0];
        cpt = cpt +1;
    end
end


Grid2.geometry = [linspace(0,1,nb_nodes(2))' zeros(nb_nodes(2),2)];
Grid3.geometry = [linspace(0,1,nb_nodes(3))' zeros(nb_nodes(3),2)];

%% elements is a cell contaning the elements in each dimention
% 1 node element (nodal element),  2 node element (bar).
% 3 node element (triangle), 4 node element (square)
% 6 node element (wedge), 8 node element (hexa)
Grid1.topology = zeros((sqrt(nb_nodes(1))-1)^2*2,3 );

cpt =1;
for j = 1: sqrt(nb_nodes(1)) -1
    for i = 1: sqrt(nb_nodes(1)) -1
        Grid1.topology(cpt, :) = [i i+1 nnb+i]+(j-1)*nnb;
        cpt =cpt +1;
        Grid1.topology(cpt, :) = [i+1 nnb+i+1 nnb+i]+(j-1)*nnb;
        cpt =cpt +1;
    end
end

Grid1.topology = int32(Grid1.topology)-1;
Grid1.type = 'Triangle';

Grid2.topology = [(1:(size(Grid2.geometry,1)-1))' (2:size(Grid2.geometry,1))'  ];
Grid2.topology = int32(Grid2.topology)-1;
Grid2.type = 'Polyline';

Grid3.topology =  [(1:(size(Grid3.geometry,1)-1))' (2:size(Grid3.geometry,1))'  ];
Grid3.topology = int32(Grid3.topology)-1;
Grid3.type = 'Polyline';



%% names is a cell contaning the name of each dimention (the number of names determine the size of the dimention)
%firs columns names, second comlumns units
%names is a cell contaning the name of each dimention (the number of names determine the size of the dimention)
%firs columns names, second comlumns units

Grid1.coordNames = {'x' 'y'};
Grid1.coordUnits = {'m' 'm'};

Grid2.coordNames = {'z'};
Grid2.coordUnits= {'m'};

Grid3.coordNames= {'t'};
Grid3.coordUnits = {'K'} ;

%% three fields (dep_x, dep_y, dep_z)
% 2 because we have only 2 modes

%generation of modes
    Grid1.nodeFields{1} = zeros(nb_nodes(1),modes);
    Grid1.nodeFields{2} = zeros(nb_nodes(1),modes);
    Grid1.nodeFields{3} = zeros(nb_nodes(1),modes);
    for m= 1: modes
        Grid1.nodeFields{1}(:,m) = sin(Grid1.geometry(:,1)*m);
        Grid1.nodeFields{2}(:,m) = cos((Grid1.geometry(:,1)+Grid1.geometry(:,2))*m);
        Grid1.nodeFields{3}(:,m) = cos(Grid1.geometry(:,1)*m).*sin(Grid1.geometry(:,1)*(modes-m)) ;
    end


    Grid2.nodeFields{1} = zeros(nb_nodes(2),modes);
    Grid2.nodeFields{2} = zeros(nb_nodes(2),modes);
    Grid2.nodeFields{3} = zeros(nb_nodes(2),modes);
    for m= 1: modes
        Grid2.nodeFields{1}(:,m) = sin(Grid2.geometry(:,1)*m);
        Grid2.nodeFields{2}(:,m) = cos((Grid2.geometry(:,1)+Grid2.geometry(:,2))*m);
        Grid2.nodeFields{3}(:,m) = cos(Grid2.geometry(:,1)*m).*sin(Grid2.geometry(:,1)*(modes-m)) ;
    end



    Grid3.nodeFields{1} = zeros(nb_nodes(3),modes);
    Grid3.nodeFields{2} = zeros(nb_nodes(3),modes);
    Grid3.nodeFields{3} = zeros(nb_nodes(3),modes);
    for m= 1: modes
        Grid3.nodeFields{1}(:,m) = sin(Grid3.geometry(:,1)*m);
        Grid3.nodeFields{2}(:,m) = cos((Grid3.geometry(:,1)+Grid3.geometry(:,2))*m);
        Grid3.nodeFields{3}(:,m) = cos(Grid3.geometry(:,1)*m).*sin(Grid3.geometry(:,1)*(modes-m)) ;
    end

    
% so three fields names (dep_x, dep_y, dep_z)
Grid1.nodeFieldsNames= cell(1,3);
Grid1.nodeFieldsNames{1}= 'dep_x';
Grid1.nodeFieldsNames{2}= 'dep_y';
Grid1.nodeFieldsNames{3}= 'dep_z';
Grid2.nodeFieldsNames= cell(1,3);
Grid2.nodeFieldsNames{1}= 'dep_x';
Grid2.nodeFieldsNames{2}= 'dep_y';
Grid2.nodeFieldsNames{3}= 'dep_z';
Grid3.nodeFieldsNames= cell(1,3);
Grid3.nodeFieldsNames{1}= 'dep_x';
Grid3.nodeFieldsNames{2}= 'dep_y';
Grid3.nodeFieldsNames{3}= 'dep_z';

%% like the nodes_fields but for the cells here P is like the pressure
Grid1.elementFields{1} = rand(Grid1.GetNumberOfElements(),1);
Grid2.elementFields{1} = rand(Grid2.GetNumberOfElements(),1);
Grid3.elementFields{1} = rand(Grid3.GetNumberOfElements(),1);
Grid1.elementFieldsNames{1}= 'P';
Grid2.elementFieldsNames{1}= 'P';
Grid3.elementFieldsNames{1}= 'P';

Grids = [ Grid1 Grid2 Grid3];

%% Output. Store the return value to check if everythin is ok

Names = {'PX' };
alldatas = {Grids }; 

res = WriteAndCompare(Names,alldatas,'.pxdmf',false);

end


function res = CheckXdmfWriter2
 
Mdata1 = XdmfGrid();

%% Geometry And Topology 
Mdata1.coordNames = {'X' 'Y' 'Z'};
Mdata1.geometry =  [0.0    0.0    0.0
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
     
Mdata1.type  = 'Wedge';
Mdata1.topology = int32([0 1 4 2 3 6
                     4 5 9 6 7 11]);     
%% Element data
% One vectorial field with 2 terms (modes or timesteps)
Mdata1.nodeFields{1} = [[ 0.95 0.92 0.13 0.23 0.73 0.20 0.60 0.17 0.19 0.48 0.40 0.6038 0.89 0.93 0.27 0.76 0.91 0.19 0.45 0.41 0.01 0.01 0.89 0.74 0.8214    0.0579    0.4451 0.4447    0.3529    0.9318 0.6154    0.8132    0.4660 0.7919    0.0099    0.4186]
                            rand(1,Mdata1.GetNumberOfNodes() *3)  ]';

% One scalar field with 2 terms (modes or timesteps)                      
Mdata1.nodeFields{2} = [[0 1 2 3 4  5  6 4 5  6  7  8  ]
                           [0 2 4 6 8 10 12 8 10 12 14 16 ]]';
                       
Mdata1.nodeFieldsNames{1}= 'dep';
Mdata1.nodeFieldsNames{2}= 'temp';

%% cell fields  in the same way as for the nodes fields
Mdata1.elementFields{1} = [ [0 2]
                               [1 3] ];
Mdata1.elementFieldsNames{1}= 'P';

%% Rectilinear mesh    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
Rdata2 = XdmfGrid();
Rdata2.coordNames = {'X' 'Y'};

X = cos(pi:pi/10:2*pi);
dimy = floor(numel(X)/2);
Rdata2.geometry = zeros(numel(X),3);
Rdata2.geometry(:,1) = X';
Rdata2.geometry(1:dimy,2) = X(1:dimy);
Rdata2.geometry(:,3) = 0;

Rdata2.type  = 'Rectilinear';
Rdata2.topology = int32([numel(X)-1 floor(numel(X)/2) 0]);


Rdata2.nodeFields{1} = rand(Rdata2.GetNumberOfNodes(),2);
Rdata2.nodeFieldsNames{1} = 'datanode';


Rdata2.elementFields{1} = rand(Rdata2.GetNumberOfElements(),2);
Rdata2.elementFieldsNames{1} = 'datacell';

%% ConstRectilinear mesh %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
CRdata = XdmfGrid();
CRdata.coordNames = {'X' 'Y' 'Z'};

CRdata.geometry = [[0 0 1]; % start point of the data
                      [1 2 3]]; % dx dy dz
CRdata.type  = 'ConstRectilinear';
CRdata.topology = int32([2 3 6]); %size of the cube

CRdata.nodeFields{1} = rand(CRdata.GetNumberOfNodes(),3);
CRdata.nodeFieldsNames{1} = 'datanode';


CRdata.elementFields{1} = rand(CRdata.GetNumberOfElements(),4);
CRdata.elementFieldsNames{1} = 'datacell';

%% Output. Store the return value to check if everythin is ok

Names = {'M' 'R' 'CR' 'uint8' 'int8' 'int32' 'int64' };

uint8data = CRdata;
uint8data.nodeFields{1} = uint8(uint8data.nodeFields{1}*256);

int8data = CRdata;
int8data.nodeFields{1} = int8(int8data.nodeFields{1}*256-127);

int32data = CRdata;
int32data.nodeFields{1} = int32(int32data.nodeFields{1}*256*256);

int64data = CRdata;
int64data.nodeFields{1} = int64(int64data.nodeFields{1}*256*256*256*256*256*256);

alldatas = {Mdata1 Rdata2 CRdata uint8data int8data int32data int64data}; 
 
res = WriteAndCompare(Names,alldatas,'.xdmf',false);


end
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function res = CheckXdmfWritertemporalGrid()

Grids(4) = XdmfGrid();

%% The Grid 1
Grids(1).geometry = [[0 0 0];[1 0 0]; [1 1 0]; [0.5 1.5 0]; [0 1 0]];

Grids(1).topology = int32([5   0 1 2 4 ... % this is a quad
                     4   2 3 4]);      % this is a triangle
% Note: to know the number for each element please read the PXDMF format
% file document at rom.ec-nantes.fr.fr

Grids(1).coordNames = {'X' 'Y'};
Grids(1).coordUnits = {'m' 'm'};

Grids(1).nodeFields{1} = rand(Grids(1).GetNumberOfNodes(),1) ;% just to generate randoms modes
Grids(1).nodeFields{2} = rand(Grids(1).GetNumberOfNodes()*3,1) ;% just to generate randoms modes
Grids(1).nodeFieldsNames = {'Temperature' 'Dep'};

%% The Grid 2

Grids(2).geometry = [[0 0 0];   % <--- origin
                     [0.1 0.1 0.1]];   % <--- spacing

Grids(2).topology = int32([12 0 0]);    % Number of elements in each diretion

Grids(2).type = 'ConstRectilinear';

Grids(2).coordNames = {'z' };
Grids(2).coordUnits = {'m'};

% is important to put the geometry and the topology first so
% we can use the GetNumberOfNodes and GetNumberOfElements functions
Grids(2).nodeFields{1} = rand(Grids(2).GetNumberOfNodes(),1) ;% just to generate randoms modes
Grids(2).nodeFields{2} = rand(Grids(2).GetNumberOfNodes()*3,1) ;% just to generate randoms modes
Grids(2).nodeFieldsNames = {'Temperature' 'Dep'};

Grids(3) = Grids(1);
Grids(4) = Grids(2); 
alldatas = {Grids };
Names = {'TG'};
res = WriteAndCompare(Names,alldatas,'.xdmf',true);

end

function plotdata(data,filenames_clean, name)
fig = figure;
bar(data);
legend('Write Time (s)','Read Time (s)','File Size (Mb)');
set(gca,'FontSize',10)
set(gca,'XTick',1:numel(filenames_clean))
set(gca,'xticklabel',filenames_clean)
title(name);
name = regexprep(name,'[^\w'']','');
print(fig, '-depsc2',['output/' name ])
end

%% Function to test the Reader Writer
function res =comparedata(a,b,tol)
if ~exist('tol','var')
    tol = 1e-15;
end

if isa(a, 'cell')
    res = min(min( abs(cell2mat(a) - cell2mat(b))./cell2mat((a))< tol ))  ~=1 ;
else
try
    res = min(min( abs(a - b)./(a.*(a~=0)+(a==0))< tol ))  ~=1 ;
    catch
        keyboard 
    end
end
end
%% comparedataint
function res =comparedataint(ref,b)
if isa(ref, 'cell')
    res = max(max( abs(cell2mat(ref) - cell2mat(b))))  ~= 0 ;
else
    try
    res =  max(max( abs(ref - b)))  ~= 0 ;
    catch
        keyboard
    end
end
end
%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function res = comparesol(ref,a,tol)
if numel(ref) > 1 
    if numel(ref) == numel(a)
        res = 0;
        for i = 1:numel(ref)
            res = res + comparesol(ref(i),a(i),tol);
        end
    else
        res = 1;
    end
    return
end
    res = comparedata(ref.GetNodes(), a.GetNodes(),tol);
    if(res); disp([num2str(res) 'error in the diff nodes' ]); end
    res = res + comparedataint(ref.GetGrid(), a.GetGrid()) ;
    if(res); disp('error in the diff grids');end
    res = res + (ref.GetNumberOfNodeFields() - a.GetNumberOfNodeFields());
    if(res); disp('error in the diff number of nodes fields');end
    for i = 1:ref.GetNumberOfNodeFields
        res = res+ compare(ref.GetNodeField(i), a.GetNodeField(i), tol);
        if(res); disp(['error in the diff field : ' ref.GetNodeFieldName(i) ]);;end
    end 
    res = res + (ref.GetNumberOfElementFields() - a.GetNumberOfElementFields());
    if(res); disp('error in the diff number of Element fields');end
    for i = 1:ref.GetNumberOfElementFields
        res =res+comparedata(ref.GetElementField(i), a.GetElementField(i), tol);  
        if(res); disp(['error in the diff field : ' ref.GetElementFieldName(i) ]);end
    end
end
%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function res= compare(ref, a, tol)
switch class(ref)    
    case 'double'
        res = comparedata(ref, a, tol); 
    otherwise
        res = comparedataint(ref, a) ;
end
end
%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function res = WriteAndCompare(Names,alldatas,ext,temporalGrids)

thepath = 'output/WriteReadXdmf2_test_output';
[~,~,~] = mkdir(thepath);

if ext(2) == 'p'
    xdmf = false;
else
    xdmf = true;
end

%% Output. Store the return value to check if everythin is ok

%options
bin       = [ 0 1 0 0  0 1 0 0 ] ; bn = {'' '_bin'};
H5        = [ 0 0 1 1  0 0 1 1 ] ; hn = {'' '_H5'};
gz        = [ 0 0 0 1  0 0 0 1 ] ; gn = {'' 'gz'};
max_ASCII = [ 1 1 1 1  1 1 1 1 ] ;
precision = [ 0 0 0 0  1 1 1 1 ] ; precisionName = {'single' 'double'}; pn = {'_s' '_d'};

%output
numberoftest = numel(alldatas)*numel(bin);
alldatasindex = zeros(1,numberoftest);
filenames = cell(1,numberoftest);
output = zeros(1,numberoftest);
wtimes = zeros(1,numberoftest);
tol = ones(1,numberoftest)*1e-15;
metadatas = writepxdmf2();

%% Writing Files
cpt = 0;
for i= 1:numel(alldatas)
    for op = 1:numel(bin)
         cpt = cpt +1;
        alldatasindex(cpt) = i;
        filenames{cpt} = [thepath  '/' Names{i}  bn{bin(op)+1}  hn{H5(op)+1}  gn{gz(op)+1} pn{precision(op)+1} ext];
        disp(['writting : ' filenames{cpt}] )
        tic
        options = writepxdmf2();
		options.xdmf = xdmf;
        options.filename = filenames{cpt};
        options.binary = bin(op)==1;
        options.HDF5 = H5(op)==1;
        options.gzip = gz(op)==1;
        options.max_ASCII = max_ASCII(op);
        options.precision = precisionName{precision(op)+1};
        options.verbose = 0;
        options.temporalGrids = temporalGrids;
        metadatas(cpt) = options;
        output(cpt) = writepxdmf2( alldatas{i}, options);
        wtimes(cpt) = toc;
        tol(cpt) =1e-15*precision(op)+1e-6*(1- precision(op));
    end
end
 
disp('-------------------------')
%% Reading and comparing data

comp = zeros(1,length(filenames));
rtime = zeros(1,length(filenames));
for i= 1:length(filenames)
    disp(['reading ' int2str(i) ' : ' filenames{i}])
    tic
    [in_data, metadata] = readpxdmf2(filenames{i},'xdmf',xdmf,'temporalGrids',temporalGrids);
    assert( strcmp(metadata.filename, metadatas(i).filename),'metadata.filename not equal metadatas(i).filename  ');
    assert(metadata.binary == metadatas(i).binary,'metadata.binary not equal metadatas(i).binary  ');
    assert(strcmp(metadata.precision, metadatas(i).precision),'metadata.precision not equal metadatas(i).precision  ');
    assert(metadata.HDF5 == metadatas(i).HDF5,'metadata.HDF5 not equal metadatas(i).HDF5  ');
    assert(metadata.temporalGrids == metadatas(i).temporalGrids,'metadata.temporalGrids not equal metadatas(i).temporalGrids  ');
    assert(metadata.xdmf == metadatas(i).xdmf,'metadata.xdmf not equal metadatas(i).xdmf  ');
    
    
    
    rtime(i) = toc;
    comp(i) =comparesol(alldatas{alldatasindex(i)},in_data,tol(i));
end

%% Output 
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

if (sum(abs(comp)) ~= 0 )
	disp('Error in the I/O funtions')
    a = 1:length(comp);
    disp(filenames(comp~=0))
    disp(a(comp~=0))
    res = false;
else
	disp('Everything looks fine.')
    res = true;
end

plotdata([wtimes' rtime' filesize'],filenames_clean,['Check ' ext '  Writer 2'])



end
