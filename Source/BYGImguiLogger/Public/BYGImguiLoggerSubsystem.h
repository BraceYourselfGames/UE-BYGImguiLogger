// Copyright Brace Yourself Games. All Rights Reserved.

// Putting the logging hookup stuff inside GameInstance subsystem rather than in the module
// because the module initialization means it tries to serialize when it shouldn't.

#pragma once

#include "ImguiConsole.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "BYGImguiLoggerSubsystem.generated.h"

UCLASS()
class BYGIMGUILOGGER_API UBYGImguiLoggerSubsystem : public UGameInstanceSubsystem, public FOutputDevice
{
	GENERATED_BODY()
public:
	// Begin USubsystem
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	// End USubsystem
	
	void Draw(bool* bIsOpen);

protected:
	// Begin FOutputDevice
	virtual void Serialize(const TCHAR* Message, ELogVerbosity::Type Verbosity, const class FName& Category) override;
	// End FOutputDevice
	
	FBYGAppConsole Console;
};
