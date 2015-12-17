#include "vtkSIMetaFilterProxy.h"



/*=========================================================================

  Program:   ParaView
  Module:    vtkSIMetaFilterProxy.cxx

  Copyright (c) Kitware, Inc.
  All rights reserved.
  See Copyright.txt or http://www.paraview.org/HTML/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkSIMetaFilterProxy.h"

#include "vtkAlgorithm.h"
#include "vtkClientServerInterpreter.h"
#include "vtkClientServerStream.h"
#include "vtkInformation.h"
#include "vtkMultiProcessController.h"
#include "vtkObjectFactory.h"
#include "vtkPVXMLElement.h"
#include "vtkStreamingDemandDrivenPipeline.h"

#include <vector>

#include <assert.h>

vtkStandardNewMacro(vtkSIMetaFilterProxy);
//
vtkSIMetaFilterProxy::vtkSIMetaFilterProxy(){}
//
vtkSIMetaFilterProxy::~vtkSIMetaFilterProxy(){}
//
bool vtkSIMetaFilterProxy::CreateVTKObjects(vtkSMMessage* message)
{
  if(!this->Superclass::CreateVTKObjects(message))
    {
    return false;
    }

//   // Connect reader and set filename method
   vtkObjectBase *filter = this->GetSubSIProxy("Filter")->GetVTKObject();
   if (!filter){
     vtkErrorMacro("Missing subproxy: Filter");
     return false;
   }
// 
   vtkClientServerStream stream;
   stream << vtkClientServerStream::Invoke
          << this->GetVTKObject() << "SetFilter" << filter
          << vtkClientServerStream::End;
//   if (this->GetFileNameMethod())
//     {
//     stream << vtkClientServerStream::Invoke
//            << this->GetVTKObject()
//            << "SetFileNameMethod"
//            << this->GetFileNameMethod()
//            << vtkClientServerStream::End;
//     }
   if (!this->Interpreter->ProcessStream(stream)){
     return false;
   }
  return true;
}

//----------------------------------------------------------------------------
bool vtkSIMetaFilterProxy::ReadXMLAttributes(vtkPVXMLElement* element)
{
  bool ret = this->Superclass::ReadXMLAttributes(element);
//   const char* fileNameMethod = element->GetAttribute("file_name_method");
//   if(fileNameMethod && ret)
//     {
//     this->SetFileNameMethod(fileNameMethod);
//     }
 return ret;
}

//----------------------------------------------------------------------------
void vtkSIMetaFilterProxy::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}
