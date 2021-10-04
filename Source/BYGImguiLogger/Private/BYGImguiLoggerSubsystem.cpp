// Copyright Brace Yourself Games. All Rights Reserved.

#include "BYGImguiLoggerSubsystem.h"

void UBYGImguiLoggerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	check(GLog);
	GLog->AddOutputDevice(this);
	Console.InitImgui();
}

void UBYGImguiLoggerSubsystem::Deinitialize()
{
	GLog->RemoveOutputDevice(this);
}

void UBYGImguiLoggerSubsystem::Draw(bool* bIsOpen)
{
	Console.Draw("Console", bIsOpen);
}

void UBYGImguiLoggerSubsystem::Serialize(const TCHAR* Message, ELogVerbosity::Type Verbosity, const class FName& Category)
{
	Console.AddLog(Verbosity, TCHAR_TO_ANSI(*Category.ToString()), TCHAR_TO_ANSI(Message));
}
