/*=========================================================================

  Program:   gmshReader Plugin
  Module:    vtkgmshReader.cxx

  Copyright (c) GeM, Ecole Centrale Nantes.
  All rights reserved.
  Copyright: See COPYING file that comes with this distribution


     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
//std Includes
#include <string>
#include <map>
#include <vector>
//#include <fstream>
//#include <iostream>

//boost Includes
//#include<boost/tokenizer.hpp>

//vtk Includes
#include "vtkObjectFactory.h"
#include "vtkInformationVector.h"
#include "vtkUnstructuredGrid.h"
#include "vtkMultiBlockDataSet.h"
#include "vtkInformation.h"
#include "vtkDoubleArray.h"
#include "vtkIntArray.h"
#include "vtkFloatArray.h"
#include "vtkPointData.h"
#include "vtkCellData.h"
#include "vtkErrorCode.h"
#include "vtkDataObjectTypes.h"
#include "vtkDemandDrivenPipeline.h"
#include "vtkMultiProcessController.h"
#include "vtkCommunicator.h"
#include <vtksys/SystemTools.hxx>


// boost include (for the gz compression)
//#include <boost/iostreams/filtering_streambuf.hpp>
//#include <boost/iostreams/copy.hpp>
//#include <boost/iostreams/filter/gzip.hpp>
#include <gzstream.h>


// plugin Includes
#include "vtkgmshReader.h"
//
vtkStandardNewMacro(vtkgmshReader);
//
//#define GEM_DEBUG(x) x
#define GEM_DEBUG(x) 
//
std::map<std::string, vtkgmshReader::CellType > vtkgmshReader::posFileDic;
std::map<int, vtkgmshReader::CellType > vtkgmshReader::mshFileDic;
//
vtkgmshReader::vtkgmshReader() {
  this->SetNumberOfOutputPorts(1);
  this->SetNumberOfInputPorts(0);
  this->FileName = NULL;
  this->NumberOfCells = 0;
  this->NumberOfNodes =0;

  this->legacy = 0;
  //this->posfile = 0;
  this->fileType = vtkgmshReader::POS;
  this->FileStream = NULL;
    
  posFileDic["SP"] = CellType(VTK_VERTEX,1,1);//Scalar point                  SP    3            1  * nb-time-steps
  posFileDic["VP"] = CellType(VTK_VERTEX,1,3);// Vector point                  VP    3            3  * nb-time-steps
  posFileDic["TP"] = CellType(VTK_VERTEX,1,9);// Tensor point                  TP    3            9  * nb-time-steps
  posFileDic["SL"] = CellType(VTK_LINE,2,1);// Scalar line                   SL    6            2  * nb-time-steps
  posFileDic["VL"] = CellType(VTK_LINE,2,3);// Vector line                   VL    6            6  * nb-time-steps
  posFileDic["TL"] = CellType(VTK_LINE,2,9);// Tensor line                   TL    6            18 * nb-time-steps
  posFileDic["ST"] = CellType(VTK_TRIANGLE,3,1);// Scalar triangle               ST    9            3  * nb-time-steps
  posFileDic["VT"] = CellType(VTK_TRIANGLE,3,3);// Vector triangle               VT    9            9  * nb-time-steps
  posFileDic["TT"] = CellType(VTK_TRIANGLE,3,9);// Tensor triangle               TT    9            27 * nb-time-steps
  posFileDic["SQ"] = CellType(VTK_QUAD,4,1);//Scalar quadrangle             SQ    12           4  * nb-time-steps
  posFileDic["VQ"] = CellType(VTK_QUAD,4,3);// Vector quadrangle             VQ    12           12 * nb-time-steps
  posFileDic["TQ"] = CellType(VTK_QUAD,4,9);// Tensor quadrangle             TQ    12           36 * nb-time-steps
  posFileDic["SS"] = CellType(VTK_TETRA,4,1);// Scalar tetrahedron            SS    12           4  * nb-time-steps
  posFileDic["VS"] = CellType(VTK_TETRA,4,3);//Vector tetrahedron            VS    12           12 * nb-time-steps
  posFileDic["TS"] = CellType(VTK_TETRA,4,9);// Tensor tetrahedron            TS    12           36 * nb-time-steps
  posFileDic["SH"] = CellType(VTK_HEXAHEDRON,8,1);// Scalar hexahedron             SH    24           8  * nb-time-steps
  posFileDic["VH"] = CellType(VTK_HEXAHEDRON,8,3);// Vector hexahedron             VH    24           24 * nb-time-steps
  posFileDic["TH"] = CellType(VTK_HEXAHEDRON,8,9);// Tensor hexahedron             TH    24           72 * nb-time-steps
  posFileDic["SI"] = CellType(VTK_WEDGE,6,1);// Scalar prism                  SI    18           6  * nb-time-steps
  posFileDic["VI"] = CellType(VTK_WEDGE,6,3);// Vector prism                  VI    18           18 * nb-time-steps
  posFileDic["TI"] = CellType(VTK_WEDGE,6,9);// Tensor prism                  TI    18           54 * nb-time-steps
  posFileDic["SY"] = CellType(VTK_PYRAMID,5,1);// Scalar pyramid                SY    15           5  * nb-time-steps
  posFileDic["VY"] = CellType(VTK_PYRAMID,5,3);// Vector pyramid                VY    15           15 * nb-time-steps
  posFileDic["TY"] = CellType(VTK_PYRAMID,5,9);// Tensor pyramid                TY    15           45 * nb-time-steps

  mshFileDic[15] = CellType(VTK_VERTEX,1,1);//Scalar point                  
  mshFileDic[1] = CellType(VTK_LINE,2,1);// Scalar line                   
  mshFileDic[2] = CellType(VTK_TRIANGLE,3,1);// Scalar triangle               
  mshFileDic[3] = CellType(VTK_QUAD,4,1);//Scalar quadrangle             
  mshFileDic[4] = CellType(VTK_TETRA,4,1);// Scalar tetrahedron            
  mshFileDic[5] = CellType(VTK_HEXAHEDRON,8,1);// Scalar hexahedron             
  mshFileDic[6] = CellType(VTK_WEDGE,6,1);// Scalar prism                  
  mshFileDic[7] = CellType(VTK_PYRAMID,5,1);// Scalar pyramid                
  mshFileDic[10] = CellType(VTK_QUADRATIC_QUAD,9,1);// Scalar pyramid                
  
  this->Controller     = vtkMultiProcessController::GetGlobalController();
}
//
vtkgmshReader::~vtkgmshReader() {
  if (this->FileName) {
    delete [] this->FileName;
  }
};
//
void vtkgmshReader::PrintSelf(ostream& os, vtkIndent indent) {
  this->Superclass::PrintSelf(os, indent);

  os << indent << "File Name: " << (this->FileName ? this->FileName : "(none)") << "\n";
  switch(this->fileType){
    case POS : os << indent << "Type of file : pos" << endl;  break;
    case MSH : os << indent << "Type of file : " << (this->legacy?"legacy":"standart") << endl;   break;
    case POSGZ : os << indent << "Type of file : pos.gz" << endl;  break;
    case MPOS  : os << indent << "Type of file : mpos" << endl;  break;
  }
      
  os << indent << "Number Of Nodes: " << this->NumberOfNodes << endl;
  os << indent << "Number Of Cells: " << this->NumberOfCells << endl;

};
//
int vtkgmshReader::RequestData(vtkInformation *vtkNotUsed(request), vtkInformationVector **vtkNotUsed(inputVector), vtkInformationVector *outputVector) {



  // get the info object
  vtkInformation *outInfo = outputVector->GetInformationObject(0);

  vtkDataObject* output = outInfo->Get(vtkDataObject::DATA_OBJECT());

  vtkDebugMacro( << "Reading gmsh file");
  

  // If ExecuteInformation() failed FileStream will be NULL and
  // ExecuteInformation() will have spit out an error.
  vtkDataObject* space_data = this->GetData();
  
  output->ShallowCopy(space_data);
  space_data->Delete();
  return 1;
};
//
vtkDataObject* vtkgmshReader::GetData(){
  
  this->GetOutputType();
  
  vtkDataObject* Data = NULL;
  
    // If ExecuteInformation() failed FileStream will be NULL and
  // ExecuteInformation() will have spit out an error.
  if(this->FileStream ){
    switch(this->fileType){
      case(POS) : return this->ReadPosFile();
      case(POSGZ) : return this->ReadPosGZFile();
      case(MSH) : return this->ReadMeshFile();
      case(MPOS) : return this->ReadMainPosFile();
    }
  } else {
    return 0;
  }
  return 0;
  
} 
//
vtkMultiBlockDataSet* vtkgmshReader::ReadMainPosFile(){
  
  GEM_DEBUG(std::cout << "Reading Mapn Pos" << std::endl;);
  
  vtkMultiBlockDataSet *output = vtkMultiBlockDataSet::New();
  std::vector<std::string> vfilenames;
  
  std::string filename;
  std::string mainFilePath =  vtksys::SystemTools::GetFilenamePath(this->FileName);
  for(*(this->FileStream) >> filename;!this->FileStream->eof();*(this->FileStream) >> filename) {
    vfilenames.push_back(mainFilePath+'/'+filename);
    
  }
  
  int NumberOfBlocks = vfilenames.size();
  std::vector<char> fexist;
  fexist.resize(NumberOfBlocks);
  std::cout << "List : ";
  for(int i =0; i < NumberOfBlocks; ++i){
    fexist[i] = vtksys::SystemTools::FileExists(vfilenames[i].c_str(),true);
    std::cout << "file " << vfilenames[i] << (fexist[i]?" :  found ":" :not found")  << std::endl;
  };
  std::cout << std::endl;
  
 
  int lprocc =0;
  if( this->IsParallel() ){
    // we make a restriction of the fexist to load only once every data
    int nproc = this->Controller->GetNumberOfProcesses();
    lprocc = this->Controller->GetLocalProcessId();
    //std::cout << "nproc : " << nproc << std::endl;
    //std::cout << "lprocc : " << lprocc<< std::endl;
    
    // global matrix with the exist files
    std::vector<char> fexistall;
    fexistall.resize(nproc*NumberOfBlocks);
    //for(int i =0; i < nproc*NumberOfBlocks; ++i){  
    //  fexistall[i] = 0;
    //}
    // local to global
    //for(int i =0; i < NumberOfBlocks; ++i){  
    //  fexistall[lprocc*NumberOfBlocks+i] = fexist[i];
    //}
    //
    this->Controller->AllGather(&fexist[0],&fexistall[0],NumberOfBlocks);

    //make the partition
    for(int i =0; i < NumberOfBlocks; ++i){  
      bool ok = false;
      for(int p =i%nproc; p < nproc; ++p){    
        if(ok == true){
          fexistall[p*NumberOfBlocks+i] = false;
          continue;
        }
        if(fexistall[p*NumberOfBlocks+i]){
          ok = true;
          continue;
        };
        
      }
      for(int p =0; p < i%nproc; ++p){    
        if(ok == true){
          fexistall[p*NumberOfBlocks+i] = false;
          continue;
        }
        if(fexistall[p*NumberOfBlocks+i]){
          ok = true;
          continue;
        };  
      }
    }
    // we put back the data to the local 
    for(int i =0; i < NumberOfBlocks; ++i){  
      fexist[i] = fexistall[lprocc*NumberOfBlocks+i];
    }
    
    std::cout << lprocc <<") Global partition "<<std::endl  ;
    for(int i =0; i < NumberOfBlocks; ++i){
      for(int p =0; p < nproc; ++p){ 
        std::cout << (fexistall[p*NumberOfBlocks+i]?'1':'0') << " " ;
      }
      std::cout << std::endl;
    }
    
  } 
  //std::cout <<  " Local matrix" << std::endl;
  //for(int i =0; i < NumberOfBlocks; ++i){
  //  std::cout <<  (fexist[i]?'1':'0') << " " << std::endl;
  //}
  
  output->SetNumberOfBlocks(NumberOfBlocks);
  vtkgmshReader* serialreader = vtkgmshReader::New();
  for(int i = 0; i < NumberOfBlocks ; i++){
    if(fexist[i]){
      std::cout << lprocc << ")Reading file : " << vfilenames[i] << std::endl;
      serialreader->SetFileName(vfilenames[i].c_str());
      if(serialreader->RequestInformation((NULL),(NULL), (NULL))){
        vtkDataObject* block = serialreader->GetData();
        output->SetBlock(i,block);
        output->GetMetaData(i)->Set(vtkCompositeDataSet::NAME(), vfilenames[i].c_str());
        block->Delete();
      }
    }
  };
  std::cout << lprocc << ")Done Reading files" << std::endl;
  serialreader->Delete();

  // If this instance of the reader is not parallel, block until all processes
  // read their blocks.
  if( this->IsParallel() ){
    this->Controller->Barrier();
  } 
  return output;
}; 
//
bool vtkgmshReader::IsParallel( )
{
  if( this->Controller == NULL )
    {
    return false;
    }

  if( this->Controller->GetNumberOfProcesses() > 1 )
    {
    return true;
    }

  return false;
}
//
int vtkgmshReader::GetOutputType(){

  if((std::string (this->FileName)).find(".pos.gz") != std::string::npos){
    this->fileType = POSGZ;
  } else if ((std::string (this->FileName)).find(".pos") != std::string::npos) {
    this->fileType = POS;
  } else if ((std::string (this->FileName)).find(".msh") != std::string::npos) {
    this->fileType = MSH;
  } else if ((std::string (this->FileName)).find(".mpos") != std::string::npos) {
    this->fileType = MPOS;
  }
  
   if(this->fileType == MPOS){
    return  VTK_MULTIBLOCK_DATA_SET;
   }
   return VTK_UNSTRUCTURED_GRID;
}
//
int vtkgmshReader::RequestDataObject(vtkInformationVector *outputVector) {
    GEM_DEBUG(std::cout << "In RequestDataObject " << std::endl);

    vtkDataObject* output = vtkDataSet::GetData(outputVector, 0);
    
   int outputType = this->GetOutputType();
    

    if (output && (output->GetDataObjectType() == outputType)) {
        return 1;
    }
    
    output = vtkDataObjectTypes::NewDataObject(outputType );                                    // delete ok 
    outputVector->GetInformationObject(0)->Set(vtkDataObject::DATA_OBJECT(), output );
    output->Delete();

    return 1;
}


//
vtkUnstructuredGrid*  vtkgmshReader::ReadPosGZFile(){
    // get the ouptut
  vtkUnstructuredGrid *output = vtkUnstructuredGrid::New();
  
  GEM_DEBUG(std::::cout << "Reading POS.GZ file " << std::endl;)
  //we close the filestream and open a new with the correct gz filter.
  
  //delete this->FileStream;
  //std::ifstream file(this->FileName, std::ios_base::in | std::ios_base::binary);
  //boost::iostreams::filtering_streambuf<boost::iostreams::input> inbuf;
  //inbuf.push(boost::iostreams::gzip_decompressor());
  //inbuf.push(file);
  //this->FileStream = new std::istream(&inbuf);
  
  delete this->FileStream;
  this->FileStream =   new igzstream(this->FileName);


  return ReadPosFile();
  
}
//
vtkUnstructuredGrid* vtkgmshReader::ReadPosFile(){
   vtkUnstructuredGrid *output = vtkUnstructuredGrid::New();
   
  output->Allocate();
  
  // we don't know the number of element or nodes in the pos files
  vtkDoubleArray *coords = vtkDoubleArray::New();                       // delete ok
  coords->SetNumberOfComponents(3);
  
  vtkDoubleArray *fdata = vtkDoubleArray::New();                        // delete ok
    
  
  std::string data1,fieldname;
  *(this->FileStream) >> data1 >> fieldname;

  // we remove the "" around the name 
  fieldname = fieldname.substr(1,fieldname.size()-2);
  fieldname = fieldname.substr(0,fieldname.find_last_of("_"));
  fdata->SetName(fieldname.c_str());
  
  *(this->FileStream) >> data1;
    
  vtkIdType pntid[30];
    
  std::string elemType;
  int cpt =0;
  bool firsttime = true;
  while(!FileStream->eof()){
    
    *(this->FileStream) >> elemType;
      
    CellType data =  posFileDic[elemType];
    
    if(firsttime){
      fdata->SetNumberOfComponents(data.ncomp);
      firsttime = false;
    }
      
    getline(*this->FileStream,data1);
    
    if(data.vtkType == 0){
      break;
    }
    
    ///VS (-1,-1,-1,-0.389022,-1,-0.597395,-0.677711,-0.621421,-1,-1,-0.649243,-0.634423){0,0,0,-0.673916,0.188514,1.22543,-0.156311,0.0185769,0.222893,-0.327872,-0.0111133,0.162226};
    /// ^  ^-------      point coordinates    ------------------------------------^       ^-----------------                 field values                        ----------------^
    /// --element type
    
    std::size_t op = data1.find('(');
    std::size_t cp = data1.find(')');
    std::size_t oa = data1.find('{');
    std::size_t ca = data1.find('}');
    std::string first = data1.substr(op+1,cp-(op+1));
    std::string second = data1.substr(oa+1,ca-(oa+1));
      
    //boost::tokenizer<boost::escaped_list_separator<char> > tok(first);
    ///// reading the point coordinates
    //for(boost::tokenizer<boost::escaped_list_separator<char> >::iterator beg=tok.begin(); beg!=tok.end();++beg){
    //  coords->InsertNextValue(atof((*beg).c_str()));
    //}
    
    std::vector<char> vcfirst(first.begin(), first.end()); vcfirst.push_back('\0');
    char * cfirst = &(vcfirst.front());
    char *p = strtok(cfirst, " ,");
    while (p) {
      //printf ("Token: %s\n", p);
      coords->InsertNextValue(atof(p));
      p = strtok(NULL, " ,");
    }
    
    /// reading   field values
    //boost::tokenizer<boost::escaped_list_separator<char> > tos(second);
    //for(boost::tokenizer<boost::escaped_list_separator<char> >::iterator beg=tos.begin(); beg!=tos.end();++beg){
    //  fdata->InsertNextValue(atof((*beg).c_str()));
    //}
    
    
    std::vector<char> vcsecond(second.begin(), second.end()); vcsecond.push_back('\0');
    char * csecond = &(vcsecond.front());
    p = strtok(csecond, " ,");
    while (p) {
      //printf ("Token: %s\n", p);
      fdata->InsertNextValue(atof(p));
      p = strtok(NULL, " ,");
    }
    
    /// add cell to the mesh
    for(int i = 0; i< data.npoints; ++i){
      pntid[i] =  cpt++;
    }
    output->InsertNextCell(data.vtkType, data.npoints, pntid);
    ++this->NumberOfCells;
  }
  this->NumberOfNodes = cpt;
  
  /// closing the file 
  if(this->FileStream){
    delete this->FileStream;
    this->FileStream = NULL;
  }
  
  /// setting up the output
  vtkPoints *points = vtkPoints::New();                                         // delete ok
  points->SetData(coords);
  coords->Delete();  
  
  output->SetPoints(points);
  points->Delete();
    
  output->GetPointData()->SetScalars(fdata);
  fdata->Delete();
 

  return output;
}
//
vtkUnstructuredGrid* vtkgmshReader::ReadMeshFile( ){
   vtkUnstructuredGrid *output = vtkUnstructuredGrid::New();
  this->ReadGeometry(output);

  if(this->FileStream){
    delete this->FileStream;
    this->FileStream = NULL;
  }
return output;
  
}
//
void vtkgmshReader::ReadGeometry(vtkUnstructuredGrid *output){
  
  /// material array
  vtkIntArray *materials = vtkIntArray::New();                                // delete ok
  materials->SetName("PhyTag");

  /// geo array
  vtkIntArray *GeoTag = NULL;

  /// coordinate array and point id array
  vtkFloatArray *coords = vtkFloatArray::New();                               // delete ok
  vtkIntArray   *nodesId = vtkIntArray::New();                               // delete ok
  nodesId->SetName("Original Ids");
  
  std::string data;
  *(this->FileStream) >> data;
  if( data.compare("$MeshFormat") ==  0) {
    double version;
    *(this->FileStream) >> version;// std::cout << "version : " << version << std::endl;
    int filetype;
    *(this->FileStream) >> filetype; //std::cout << "filetype : " << filetype<< std::endl;
    int datasize;
    *(this->FileStream) >> datasize; //std::cout << "datasize : " << datasize<< std::endl;
    *(this->FileStream) >> data; // to trash the $EndMeshFormat

    for( *(this->FileStream) >> data;!this->FileStream->eof(); *(this->FileStream) >> data) {
      
      GEM_DEBUG(std::cout << data << std::endl);
      
      if( data.compare("$Nodes") ==  0) {
        GEM_DEBUG(std::cout << "Reading Nodes ..."<< std::endl);
        this->ReadXYZCoords(coords,nodesId);
        continue;
      }
            
      if( data.compare("$Elements") ==  0 ) {
        GEM_DEBUG(std::cout << "Reading Elements ..."<< std::endl);
        GeoTag = vtkIntArray::New();                                    // delete ok
        GeoTag->SetName("GeoTag");
        this->ReadASCIICellTopology(materials, output, GeoTag,nodesId);
        continue;
      }
            
      if( data.compare("$NodeData") ==  0) {
        GEM_DEBUG(std::cout << "Reading NodeData ..."<< std::endl);
        vtkDoubleArray *data = this->ReadASCIIData();
        if(data) {
          output->GetPointData()->AddArray(data);
          data->Delete();
        }
        continue;
      }
            
      if( data.compare("$ElementData") ==  0) {
        GEM_DEBUG(std::cout << "Reading ElementData ..."<< std::endl);
        vtkDoubleArray *data = this->ReadASCIIData();
        if(data) {
          output->GetCellData()->AddArray(data);
          data->Delete();
        }
        continue;
      }
      GEM_DEBUG(std::cout << "this data is not treated "  << data << std::endl);
    }
  } else {
    //legacy format
    this->legacy = true;
    // reset of the file pointer.
    this->FileStream->seekg(0);
    for(*(this->FileStream) >> data;!this->FileStream->eof();*(this->FileStream) >> data) {
      if( data.compare("$NOD") ==  0) {
        this->ReadXYZCoords(coords,nodesId);
        continue;
      }
      if( data.compare("$ELM") ==  0) {
        this->ReadASCIICellTopology(materials, output, NULL, nodesId);
        continue;
      }
    }
  }

  vtkPoints *points = vtkPoints::New();                                       // delete ok
  points->SetData(coords);
  coords->Delete();

  output->SetPoints(points);
  points->Delete();

  output->GetPointData()->AddArray(nodesId);
  nodesId->Delete();
  
  // now add the material array  
  output->GetCellData()->AddArray(materials);
  if(GeoTag) {
    output->GetCellData()->AddArray(GeoTag);
    GeoTag->Delete();
  }
  
  if (!output->GetCellData()->GetScalars()) {
    output->GetCellData()->SetScalars(materials);
  }
  materials->Delete();
  
  //GEM_DEBUG(output->Print(std::cout));
}
//--------------------------------------------------------------------------
vtkDoubleArray * vtkgmshReader::ReadASCIIData() {
  int number_of_string_tags;
  *(this->FileStream) >> number_of_string_tags;
    
  std::string str1;
    
  std::vector<std::string> string_tags;
  string_tags.resize(number_of_string_tags);
    
  getline(*this->FileStream,str1); // to read the rest of the line of number_of_string_tags
  for(int i =0; i < number_of_string_tags; ++i) {
    getline(*this->FileStream,str1);
    string_tags[i] = str1.substr(1,str1.size()-2); // to remove the "" around the name
    GEM_DEBUG(std::cout << "Reading tag : " << string_tags[i] << std::endl);
  }
    
  int number_of_real_tags;
  *(this->FileStream) >> number_of_real_tags; // this is used for the time normaly

  double doubledata;
  for(int i =0; i < number_of_real_tags; ++i) {
    *(this->FileStream) >> doubledata;
  }

  int number_of_integer_tags;
  *(this->FileStream) >> number_of_integer_tags;
  double timestep = 0;
  if(number_of_integer_tags>0) *(this->FileStream) >> timestep;

  int componnents = 0;
  if(number_of_integer_tags>1) *(this->FileStream) >> componnents;

  int number_of_values = 0;
  if(number_of_integer_tags>2) *(this->FileStream) >> number_of_values;

  int intdata;
  for(int i = 3 ; i < number_of_integer_tags; ++i) {
    *(this->FileStream) >> intdata; // we trash the rest of the tags
  }

  if(number_of_values) {
    vtkDoubleArray *data = vtkDoubleArray::New();                           // delete ok
    data->SetName(string_tags[0].c_str());
    data->SetNumberOfComponents(componnents);
    data->SetNumberOfTuples(number_of_values);
    double *ptr = data->GetPointer(0);
    int id; // we ignore this values
    for(int i =0; i < number_of_values ; ++i) {
      *(this->FileStream) >> id;
      for(int j =0; j < componnents ; ++j) {
        *(this->FileStream) >> ptr[componnents*i+j] ;
      }
    }
    *(this->FileStream) >> str1; // to trash the $*Data keyword
      return data;
  }
  return NULL;
};
//--------------------------------------------------------------------------
void vtkgmshReader::ReadXYZCoords(vtkFloatArray *coords, vtkIntArray *nodesId) {
    GEM_DEBUG(std::cout << "In vtkgmshReader::ReadXYZCoords" << std::endl);
  
    *(this->FileStream) >> this->NumberOfNodes;

    coords->SetNumberOfComponents(3);
    coords->SetNumberOfTuples(this->NumberOfNodes);

    nodesId->SetNumberOfComponents(1);
    nodesId->SetNumberOfTuples(this->NumberOfNodes);
    
    int i;
    float *ptr = coords->GetPointer(0);
    int   *idptr = nodesId->GetPointer(0);

    for(i=0; i < this->NumberOfNodes; i++) {
        *(this->FileStream) >> idptr[i];
        *(this->FileStream) >> ptr[3*i] >> ptr[3*i+1] >> ptr[3*i+2];
    }
    
    std::string tmpdata;
    *(this->FileStream) >> tmpdata; // to trash the $ENDNOD keyword
}
//----------------------------------------------------------------------------
void vtkgmshReader::ReadASCIICellTopology(vtkIntArray *materials,
        vtkUnstructuredGrid *output,vtkIntArray *GeoTag,vtkIntArray* nodesId) {
  GEM_DEBUG(std::cout << "In vtkgmshReader::ReadASCIICellTopology" << std::endl);
  

  int maxpointid = 0;
  int* ptr = nodesId->GetPointer(0);
  for(int i =0; i < nodesId->GetNumberOfTuples(); ++i){
    maxpointid = std::max(ptr[i],maxpointid);
  }
  std::vector<int> gmhToVtkids;
  gmhToVtkids.resize(maxpointid);
  for(int i =0; i < nodesId->GetNumberOfTuples(); ++i){
    gmhToVtkids[ptr[i]] = i;
  }
  
  *(this->FileStream) >> this->NumberOfCells;

  GEM_DEBUG(std::cout << "Number of elements " << this->NumberOfCells << std::endl;);
  
  materials->SetNumberOfComponents(1);
  materials->SetNumberOfTuples(this->NumberOfCells);

  int i, k;
  vtkIdType list[30];
  int *mat = materials->GetPointer(0);
  int *geo ;                                
  int ctype;
  int elem;
  int nbnodes;
  int nbtags;
  int other_tag;

  if(!this->legacy) {
      GeoTag->SetNumberOfComponents(1);
      GeoTag->SetNumberOfTuples(this->NumberOfCells);
      geo = GeoTag->GetPointer(0);
  }

  output->Allocate();
  for(i=0; i < this->NumberOfCells; i++){
    
    int id;  // no check is done to see that they are monotonously increasing
    *(this->FileStream) >> id;
    *(this->FileStream) >> ctype;
    if(this->legacy) {
      *(this->FileStream) >> mat[i];
      *(this->FileStream) >> elem;
      *(this->FileStream) >> nbnodes;
    } else {
      *(this->FileStream) >> nbtags;
      if(nbtags){
        *(this->FileStream) >> mat[i];
        --nbtags;
      } else {
        mat[i] = -1 ;
      }
      
      if(nbtags){
        *(this->FileStream) >> geo[i];
        --nbtags;
      } else {
        geo[i] = -1;
      }
      
      while(nbtags--) {
        *(this->FileStream) >> other_tag; // whe keep only the first two tag
      }
    }

    CellType data = mshFileDic[ctype];
    
    if(data.vtkType == 0){ 
      std::cout << "This type of element is not supported (" << ctype << ")" << std::endl;
      break;
    }
    //GEM_DEBUG(std::cout << "inserting cell " << "data.vtkType " << data.vtkType << " data.npoints " << data.npoints << std::endl);
    for(k=0; k < data.npoints; k++){
      *(this->FileStream) >> list[k];
      list[k] = gmhToVtkids[list[k]];
    }
    output->InsertNextCell(data.vtkType, data.npoints, list);
    
  }  
  std::string tmpdata;
  *(this->FileStream) >> tmpdata; // to trash the $ENDELM keyword
}

//----------------------------------------------------------------------------
int vtkgmshReader::RequestInformation(
    
    vtkInformation *vtkNotUsed(request),
    vtkInformationVector **vtkNotUsed(inputVector),
    vtkInformationVector *vtkNotUsed(outputVector)){
  GEM_DEBUG(std::cout << "RequestInformation" << std::endl;)
    // first open file in binary mode to check the first byte.
  if ( !this->FileName ) {
    vtkErrorMacro("No filename specified");
    return 0;
  }
  
#ifdef _WIN32
  this->FileStream = new ifstream(this->FileName, ios::in | ios::binary);
#else
  this->FileStream = new ifstream(this->FileName, ios::in);
#endif
  if (this->FileStream->fail()){
    this->SetErrorCode(vtkErrorCode::FileNotFoundError);
    delete this->FileStream;
    this->FileStream = NULL;
    std::cout << "-" <<this->FileName << std::endl;
    vtkErrorMacro("Specified filename not found");
    std::cout << "-" << std::endl;
    return 0;
  }
   
  char magic_number='\0';
  this->FileStream->get(magic_number);
  this->FileStream->putback(magic_number);
  if(magic_number != 7){   // most likely an ASCII file
    delete this->FileStream; // close file to reopen it later
    this->FileStream = NULL;
    this->FileStream = new ifstream(this->FileName, ios::in);
    char c='\0';
    while (!FileStream->eof()){
      // skip leading whitespace
      while(isspace(FileStream->peek())){
        FileStream->get(c);
      }
      // skip comment lines
      if (FileStream->peek() == '#'){
        while(this->FileStream->get(c)){
          if (c == '\n'){
            break;
          }
        }
      } else {
        break;
      }
    }
    //*(this->FileStream) >> this->NumberOfNodes;
    //*(this->FileStream) >> this->NumberOfCells;
  }
  vtkDebugMacro( << "end of ExecuteInformation\n");
  return 1;
}
//-----------------------------------------
int vtkgmshReader::FillOutputPortInformation(int, vtkInformation *info) {
    info->Set(vtkDataObject::DATA_TYPE_NAME(), "vtkDataObject");
    return 1;
};
//
int vtkgmshReader::ProcessRequest(vtkInformation *request, vtkInformationVector **inputVector, vtkInformationVector *outputVector) {
        
        
    if (request->Has(vtkDemandDrivenPipeline::REQUEST_DATA())){
        return this->RequestData(request, inputVector, outputVector);  
    }
    
    if (request->Has(vtkDemandDrivenPipeline::REQUEST_DATA_OBJECT())) {
        return this->RequestDataObject(outputVector);
    }

    // execute information
    if(request->Has(vtkDemandDrivenPipeline::REQUEST_INFORMATION())){   
        return this->RequestInformation(request, inputVector, outputVector);
    }
    

    
    return this->Superclass::ProcessRequest(request, inputVector, outputVector);
};
//