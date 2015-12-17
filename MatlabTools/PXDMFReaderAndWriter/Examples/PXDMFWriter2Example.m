%% Matlab example of how to use the NEW writepxdmf2
%
% A directory named pxdmf2_examples is created with all the outpout files
%
% Example using 4 spaces 
%   1 nodal field (Temperatue) with 2 terms
%   1 nodal field (Dep) with 2 terms (vector 2)
%
% $$ \mbox{Temperature}(x,y,z) = \sum_{i=1}^{2} temp^i(x,y)\cdot temp^i(z)\cdot temp^i(r)\cdot temp^i(t1,t2)$$
% $$ \mbox{Dep}(x,y,z) = \sum_{i=1}^{2} dep^i(x,y)\cdot dep^i(z)\cdot dep^i(r)\cdot dep^i(t1,t2)$$
%
% space 1 (x,y) a 2d mesh unstructured  (mixed elemens, one quad, one tri)
%
% space 2  (z)  a 1D  const structured mesh  (linear element)
%
% space 3  (r)  a 1D  structured mesh  (linear element)
%
% space 4  (t)  a 2D  unstructured mesh  (only triangles)
%

% NOTE : the is a bug in the xdmf reader in ParaView. So 1D structured mesh and element
% data are incompatible. Please 1D unstructured mesh in this case (polyline elements)
%
%
% This file is subject to the terms and conditions defined in
% file 'LICENSE.txt', which is part of this source code package.
%
% Principal developer : Felipe Bordeu (Felipe.Bordeu@ec-nantes.fr)
%
%


%% Output Directory (creation if is needed)
dirname = 'pxdmf2_examples';
[~,~,~] = mkdir(dirname);

Grids(4) = XdmfGrid();

%% The Grid 1
Grids(1).geometry = [[0 0 0];[1 0 0]; [1 1 0]; [0.5 1.5 0]; [0 1 0]];

Grids(1).topology = [5   0 1 2 4 ... % this is a quad
                     4   2 3 4];      % this is a triangle
% Note: to know the number for each element please read the PXDMF format
% file document at rom.ec-nantes.fr.fr

Grids(1).coordNames = {'X' 'Y'};
Grids(1).coordUnits = {'m' 'm'};

Grids(1).nodeFields{1} = rand(Grids(1).GetNumberOfNodes(),2) ;% just to generate randoms modes
Grids(1).nodeFields{2} = rand(Grids(1).GetNumberOfNodes()*3,2) ;% just to generate randoms modes
Grids(1).nodeFieldsNames = {'Temperature' 'Dep'};

%% The Grid 2

Grids(2).geometry = [[0 0 0];   % <--- origin
                     [0.1 0.1 0.1]];   % <--- spacing

Grids(2).topology = [12 0 0];    % Number of elements in each diretion

Grids(2).type = 'ConstRectilinear';

Grids(2).coordNames = {'z' };
Grids(2).coordUnits = {'m'};

% is important to put the geometry and the topology first so
% we can use the GetNumberOfNodes and GetNumberOfElements functions
Grids(2).nodeFields{1} = rand(Grids(2).GetNumberOfNodes(),2) ;% just to generate randoms modes
Grids(2).nodeFields{2} = rand(Grids(2).GetNumberOfNodes()*3,2) ;% just to generate randoms modes
Grids(2).nodeFieldsNames = {'Temperature' 'Dep'};

%% The Grid 3

X = cos(pi:pi/30:2*pi);

Grids(3).geometry = zeros(numel(X),3);
Grids(3).geometry(:,1) = X';
Grids(3).geometry(:,2) = 0;
Grids(3).geometry(:,3) = 0;

Grids(3).topology = [numel(X)-1 0 0];% Number of elements in each diretion
   
Grids(3).type = 'Rectilinear';

Grids(3).coordNames = {'r' };
Grids(3).coordUnits = {'m'};

% is important to put the geometry and the topology first so
% we can use the GetNumberOfNodes and GetNumberOfElements functions
Grids(3).nodeFields{1} = rand(Grids(3).GetNumberOfNodes(),2) ;% just to generate randoms modes
Grids(3).nodeFields{2} = rand(Grids(3).GetNumberOfNodes()*3,2) ;% just to generate randoms modes
Grids(3).nodeFieldsNames = {'Temperature' 'Dep'};

%% The Grid 4
x = linspace(0,1,5);
dx = (sin(x*5/2))*x(2);
cpt =1;
for i = 1: 5
    for j = 1:5
        Grids(4).geometry(cpt,:)= [x(i)-dx(j) x(j)+dx(i) 0];
        cpt = cpt +1;
    end
end
Grids(4).type = 'Triangle';

Grids(4).topology = zeros((5-1)^2*2,3 );
cpt =1;
for j = 1: 4
    for i = 1: 4
        Grids(4).topology(cpt, :) = [i i+1 5+i]+(j-1)*5;
        cpt =cpt +1;
        Grids(4).topology(cpt, :) = [i+1 5+i+1 5+i]+(j-1)*5;
        cpt =cpt +1;
    end
end
Grids(4).topology = Grids(4).topology - 1;

Grids(4).coordNames = {'t1'  't2'};
Grids(4).coordUnits = {'m' 'm'};

% is important to put the geometry and the topology first so
% we can use the GetNumberOfNodes and GetNumberOfElements functions
Grids(4).nodeFields{1} = rand(Grids(4).GetNumberOfNodes(),2) ;% just to generate randoms modes
Grids(4).nodeFields{2} = rand(Grids(4).GetNumberOfNodes()*3,2) ;% just to generate randoms modes
Grids(4).nodeFieldsNames = {'Temperature' 'Dep'};

Grids.CheckIntegrity(true);

options = writepxdmf2();
options.binary = false;
options.filename = [dirname '/PXDMFWriter2Example'];
writepxdmf2(Grids, options);

%% Also we can print if this are 4 domains and 2 timesteps

options.xdmf = true;
options.filename = [dirname '/PXDMFWriter2ExampleTime'];
writepxdmf2(Grids, options);

%% if we have only one timestep per grid we can export every grid as a time step
Grid2s = Grids([1 4 1 4 1]);
for i = 1:numel(Grid2s)
    for j =1:2
        Grid2s(i).nodeFields{j} = Grid2s(i).nodeFields{j}(:,1);
    end
end

options.xdmf = true;
options.filename = [dirname '/PXDMFWriter2ExampleGridTime'];
options.temporalGrids= true;
writepxdmf2(Grid2s, options);

