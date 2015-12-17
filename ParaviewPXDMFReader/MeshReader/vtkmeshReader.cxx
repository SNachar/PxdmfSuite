/*=========================================================================

  Program:   meshReader Plugin
  Module:    vtkmeshReader.cxx

  Copyright (c) GeM, Ecole Centrale Nantes.
  All rights reserved.
  Copyright: See COPYING file that comes with this distribution


     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

//vtk Includes
#include "vtkObjectFactory.h"
#include "vtkUnstructuredGrid.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkCellData.h"
#include "vtkPointData.h"
#include "vtkErrorCode.h"
#include "vtkType.h"
#include "vtkCellArray.h"
#include "vtkDoubleArray.h"
#include "vtkFloatArray.h"
#include "vtkIntArray.h"

// plugin Includes
#include "vtkmeshReader.h"


vtkStandardNewMacro(vtkmeshReader);

vtkmeshReader::vtkmeshReader() {
    this->FileName = NULL;
    this->NumberOfCells = 0;
    this->NumberOfNodes =0;

    this->FileStream = NULL;

    this->SetNumberOfInputPorts(0);
}

//----------------------------------------------------------------------------
vtkmeshReader::~vtkmeshReader() {
    if (this->FileName) {
        delete [] this->FileName;
    }
};

//----------------------------------------------------------------------------
void vtkmeshReader::PrintSelf(ostream& os, vtkIndent indent) {
    this->Superclass::PrintSelf(os, indent);

    os << indent << "File Name: "
       << (this->FileName ? this->FileName : "(none)") << "\n";
    os << indent << "Number Of Nodes: " << this->NumberOfNodes << endl;
    os << indent << "Number Of Cells: " << this->NumberOfCells << endl;

};

//----------------------------------------------------------------------------
int vtkmeshReader::RequestData(
    vtkInformation *vtkNotUsed(request),
    vtkInformationVector **vtkNotUsed(inputVector),
    vtkInformationVector *outputVector) {

    // get the info object
    vtkInformation *outInfo = outputVector->GetInformationObject(0);

    // get the ouptut
    vtkUnstructuredGrid *output = vtkUnstructuredGrid::SafeDownCast(
                                      outInfo->Get(vtkDataObject::DATA_OBJECT()));
    
    vtkDebugMacro( << "Reading mesh msh file");

    if ( this->FileStream )
    {
        this->ReadFile(output);
    }
    return 1;

};
void vtkmeshReader::ReadFile(vtkUnstructuredGrid *output)
{
    this->ReadGeometry(output);


    delete this->FileStream;
    this->FileStream = NULL;
}
//
//----------------------------------------------------------------------------
void vtkmeshReader::ReadGeometry(vtkUnstructuredGrid *output)
{

    vtkFloatArray *coords = vtkFloatArray::New();               // delete ok
    // nodal tags
    vtkIntArray *ntag = vtkIntArray::New();                     // delete ok
    ntag->SetName("nTag");

    // cell tags
    vtkIntArray *ctag = vtkIntArray::New();                     // delete ok
    ctag->SetName("cTag");


    std::string data;
    *(this->FileStream) >> data;
    if( data.compare("MeshVersionFormatted") ==  0) {
      *(this->FileStream) >> meshVersionFormatted;
    }
    
    *(this->FileStream) >> data;
    
    if( data.compare("Dimension") ==  0) {
      *(this->FileStream) >> Dimension;
    }
    
    // keep the position cout the element and read the elements
    std::streampos pos = this->FileStream->tellg();
    
    this->NumberOfCells = 0;
    
    int cells;
    while(!this->FileStream->eof()) {
       if( data.compare("Triangles") ==  0 ||  data.compare("Tetrahedra") ==  0) {
         *(this->FileStream) >> cells;
         this->NumberOfCells += cells;
       } 
       
       if( data.compare("End") ==  0) {
           break;
       }
       *(this->FileStream) >> data;
     }
     
    this->FileStream->seekg(pos);

    output->Allocate(this->NumberOfCells);
    ctag->SetNumberOfComponents(1);
    ctag->SetNumberOfTuples(this->NumberOfCells);
    //we start  the counting again
    this->NumberOfCells = 0;

     *(this->FileStream) >> data; 
    while(!this->FileStream->eof()) {
        //std::cout << data << std::endl;
        if( data.compare("Vertices") ==  0) {
            this->ReadXYZCoords(coords,ntag);
        }
            
       if( data.compare("Triangles") ==  0) {
           this->ReadASCIICellTopology(ctag, output,VTK_TRIANGLE,3);
       }
       if( data.compare("Tetrahedra") ==  0) {
           this->ReadASCIICellTopology(ctag, output,VTK_TETRA,4);
       }
       
       if( data.compare("End") ==  0) {
           break;
       }
       *(this->FileStream) >> data;
     }


    vtkPoints *points = vtkPoints::New();                               // delete ok
    points->SetData(coords);
    coords->Delete();

    output->SetPoints(points);
    points->Delete();

    // now add the nodal tags
    output->GetPointData()->AddArray(ntag);
    
    // now add the material array
    output->GetCellData()->AddArray(ctag);
    if (!output->GetCellData()->GetScalars())
    {
        output->GetCellData()->SetScalars(ctag);
    }
    ctag->Delete();
    ntag->Delete();    
}
//--------------------------------------------------------------------------
void vtkmeshReader::ReadXYZCoords(vtkFloatArray *coords, vtkIntArray *ntag) {

    *(this->FileStream) >> this->NumberOfNodes;

    coords->SetNumberOfComponents(3);
    coords->SetNumberOfTuples(this->NumberOfNodes);
    
    ntag->SetNumberOfComponents(1);
    ntag->SetNumberOfTuples(this->NumberOfNodes);

    int i;
    float *ptr = coords->GetPointer(0);
    int  *nptr = ntag->GetPointer(0);


    //int id;  // no check is done to see that they are monotonously increasing
    // read here the first node and check if its id number is 0
    for(i=0; i < this->NumberOfNodes; i++) {
        *(this->FileStream) >> ptr[3*i] >> ptr[3*i+1] >> ptr[3*i+2];
        *(this->FileStream) >> nptr[i];
    }
}
//----------------------------------------------------------------------------
void vtkmeshReader::ReadASCIICellTopology(vtkIntArray *ctag,
        vtkUnstructuredGrid *output,int vtk_type,int number_of_points_in_the_element) {

    int number_of_cells;
    *(this->FileStream) >> number_of_cells;

    int i, k;
    vtkIdType list[8];
    int *mat = ctag->GetPointer(this->NumberOfCells);
    this->NumberOfCells += number_of_cells;
    
    for(i=0; i < number_of_cells; i++){
        for(k=0; k < number_of_points_in_the_element; k++)
            {
                *(this->FileStream) >> list[k];
                list[k]--;
            }
            *(this->FileStream) >> mat[i];
            output->InsertNextCell(vtk_type, number_of_points_in_the_element, list);
        }
}

//----------------------------------------------------------------------------
int vtkmeshReader::RequestInformation(
    vtkInformation *vtkNotUsed(request),
    vtkInformationVector **vtkNotUsed(inputVector),
    vtkInformationVector *vtkNotUsed(outputVector))
{




    // first open file in binary mode to check the first byte.
    if ( !this->FileName )
    {
        vtkErrorMacro("No filename specified");
        return 0;
    }

#ifdef _WIN32
    this->FileStream = new ifstream(this->FileName, ios::in | ios::binary);
#else
    this->FileStream = new ifstream(this->FileName, ios::in);
#endif
    if (this->FileStream->fail())
    {
        this->SetErrorCode(vtkErrorCode::FileNotFoundError);
        delete this->FileStream;
        this->FileStream = NULL;
        vtkErrorMacro("Specified filename not found");
        return 0;
    }

    char magic_number='\0';
    this->FileStream->get(magic_number);
    this->FileStream->putback(magic_number);
    if(magic_number != 7)
    {   // most likely an ASCII file
//    this->BinaryFile = 0;
        delete this->FileStream; // close file to reopen it later
        this->FileStream = NULL;

        this->FileStream = new ifstream(this->FileName, ios::in);
        char c='\0';
        while (!FileStream->eof())
        {
            // skip leading whitespace
            while(isspace(FileStream->peek()))
            {
                FileStream->get(c);
            }
            // skip comment lines
            if (FileStream->peek() == '#')
            {
                while(this->FileStream->get(c))
                {
                    if (c == '\n')
                    {
                        break;
                    }
                }
            }
            else
            {
                break;
            }
        }
    }

    vtkDebugMacro( << "end of ExecuteInformation\n");

    return 1;
}
