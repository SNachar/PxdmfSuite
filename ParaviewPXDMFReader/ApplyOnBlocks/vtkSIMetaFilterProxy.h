#ifndef VTKSIMETAFILTERPROXY_H
#define VTKSIMETAFILTERPROXY_H

//#include "vtkPVServerImplementationCoreModule.h" //needed for exports
#include "vtkSISourceProxy.h"

class vtkSIMetaFilterProxy :  public vtkSISourceProxy{
  public:
  static vtkSIMetaFilterProxy* New();
  vtkTypeMacro(vtkSIMetaFilterProxy, vtkSISourceProxy);
  void PrintSelf(ostream& os, vtkIndent indent);

 //BTX

protected:
  vtkSIMetaFilterProxy();
  ~vtkSIMetaFilterProxy();

  // Description:
  // Creates the VTKObjects. Overridden to add post-filters to the pipeline.
  virtual bool CreateVTKObjects(vtkSMMessage* message);

  // Description:
  // Read xml-attributes.
  virtual bool ReadXMLAttributes(vtkPVXMLElement* element);

private:
  vtkSIMetaFilterProxy(const vtkSIMetaFilterProxy&); // Not implemented
  void operator=(const vtkSIMetaFilterProxy&); // Not implemented

//ETX 
  
  
};

#endif // VTKSIMETAFILTERPROXY_H
