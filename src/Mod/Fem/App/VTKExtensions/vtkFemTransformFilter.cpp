#include "PreCompiled.h"

#ifndef _PreComp_
# include <algorithm>
#endif


#include "vtkFemTransformFilter.h"

using namespace Fem;

vtkStandardNewMacro(vtkFemTransformFilter);

vtkFemTransformFilter::vtkFemTransformFilter()
{
    this->Scale = nullptr;
    //    printf("arranca\n");
}

vtkFemTransformFilter::~vtkFemTransformFilter()
{
    //    printf("scale: %p\n", this->Scale);
    this->SetScale(nullptr);
}

int vtkFemTransformFilter::RequestData(
    vtkInformation* request,
    vtkInformationVector** inputVector,
    vtkInformationVector* outputVector
)
{
    //    printf("aca va\n");
    return this->Superclass::RequestData(request, inputVector, outputVector);
}
