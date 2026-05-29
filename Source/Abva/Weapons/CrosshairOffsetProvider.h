

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "CrosshairOffsetProvider.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UCrosshairOffsetProvider : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ABVA_API ICrosshairOffsetProvider
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void UpdateCrosshairSpread(float spread) = 0;
	
};
