/*=========================================================================

  Program:   PXDMFReader Plugin
  Module:    FileDataAbstract.h

  Copyright (c) GeM, Ecole Centrale Nantes.
  All rights reserved.
  Copyright: See COPYING file that comes with this distribution


     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME FileDataAbstract
// .SECTION Abstract class to wrap the file reading class from the document.

#ifndef FILEDATAABSTRACT_H
#define FILEDATAABSTRACT_H

// STD Includes
#include <string>
#include <vector>
#include <map>

class vtkDataSet;

// Value Types
#define XDMF_ATTRIBUTE_TYPE_NONE  0
#define XDMF_ATTRIBUTE_TYPE_SCALAR  1
#define XDMF_ATTRIBUTE_TYPE_VECTOR  2
#define XDMF_ATTRIBUTE_TYPE_TENSOR  3
#define XDMF_ATTRIBUTE_TYPE_MATRIX  4
#define XDMF_ATTRIBUTE_TYPE_TENSOR6 5
#define XDMF_ATTRIBUTE_TYPE_GLOBALID 6

// Where Values are Assigned
#define XDMF_ATTRIBUTE_CENTER_GRID  0
#define XDMF_ATTRIBUTE_CENTER_CELL  1
#define XDMF_ATTRIBUTE_CENTER_FACE  2
#define XDMF_ATTRIBUTE_CENTER_EDGE  3
#define XDMF_ATTRIBUTE_CENTER_NODE  4

struct GridInfo {
    GridInfo() {
        dimensionality =0;
        }
    std::string name;
    int type;
    int dimensionality;
    std::vector<std::string> coordNames;
    std::vector<std::string> coordUnits;
    std::vector<double> mins;
    std::vector<double> maxs;
    double origin[3];
    double spacing[3];
    int extents[3];
    };
//
// XdmfAttribute.h
struct AttributeInfo {
    std::string name;
    int center;
    };

class XdmfArraySelection : public std::map<std::string, bool> {
    public:
        void AddArray ( const char* name, bool status=true ) {
            ( *this ) [name] = status;
            }
        //
        int GetNumberOfArrays() {
            return static_cast<int> ( this->size() );
            }
        //
        void SetArrayStatus ( const char* name, bool status ) {
            this->AddArray ( name, status );
            }
        //
        int GetArraySetting ( const char* name ) {
            return this->ArrayIsEnabled ( name ) ? 1 : 0;
            }
        //
        bool ArrayIsEnabled ( const char* name ) {
            XdmfArraySelection::iterator iter = this->find ( name );
            if ( iter != this->end() ) {
                return iter->second;
                }
            // don't know anything about this array, enable it by default.
            return true;
            }
        const char* GetArrayName ( int index ) {
            int cc=0;
            for ( XdmfArraySelection::iterator iter = this->begin();
                    iter != this->end(); ++iter ) {

                if ( cc==index ) {
                    return iter->first.c_str();
                    }
                cc++;
                }
            return NULL;
            }

    };



class FileDataAbstract {
    public:
        FileDataAbstract() {
            this->changeInSelectedPointCellArrays = true;
            this->fileParsed = 0;
            NumberOfPXDMFDims = 0;
            mtime = 0;
            }
        virtual ~FileDataAbstract(){};

        /// if file has been parsed
        virtual bool Init ( char* filename ) =0;
        virtual bool SetActiveDommain ( char* domainName ) =0;
        virtual bool NeedUpdate() =0;
        virtual int GetNumberOfGrids() =0;

        virtual vtkDataSet* GetVTKDataSet ( int i,int* stride, int* extents ) =0;
        bool changeInSelectedPointCellArrays;                          ///
        long mtime;
        virtual void ForceUpdate() = 0;
        virtual int GetDomainVTKDataType() =0;

        virtual GridInfo GetGridData ( int j ) =0;
        //
        virtual int GetNumberOfAttributes() =0;
        virtual AttributeInfo GetAttributeData ( int j ) =0;

        virtual int GetNumberOfPointArrays() =0;
        virtual void SetPointArraySelection ( const char* ,bool ) =0;
        virtual bool GetPointArraySelection ( const char * name ) =0;
        virtual const char* GetPointArrayName ( int& index ) =0;

        virtual int GetNumberOfCellArrays() =0;
        virtual void SetCellArraySelection ( const char*,bool ) =0;
        virtual bool GetCellArraySelection ( const char * name ) =0;
        virtual const char* GetCellArrayName ( int& index ) =0;

    protected:
        bool fileParsed;
        int NumberOfPXDMFDims;

    };

#endif // FILEDATAABSTRACTCLASS_H
