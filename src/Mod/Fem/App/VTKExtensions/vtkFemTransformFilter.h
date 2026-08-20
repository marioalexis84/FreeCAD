#pragma once

#include <vtkTransformFilter.h>
#include <vtkAbstractTransform.h>

namespace Fem
{

class vtkFemTransformFilter: public vtkTransformFilter
{
public:
    static vtkFemTransformFilter* New();
    vtkTypeMacro(vtkFemTransformFilter, vtkTransformFilter);

    vtkSetObjectMacro(Scale, vtkAbstractTransform);
    vtkGetObjectMacro(Scale, vtkAbstractTransform);

protected:
    vtkFemTransformFilter();
    ~vtkFemTransformFilter() override;

    int RequestData(
        vtkInformation* request,
        vtkInformationVector** inputVector,
        vtkInformationVector* outputVector
    ) override;
    vtkAbstractTransform* Scale;
};

}  // namespace Fem
