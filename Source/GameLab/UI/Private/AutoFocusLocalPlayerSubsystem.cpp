// Fill out your copyright notice in the Description page of Project Settings.

#include "GameLab/UI/Public/AutoFocusLocalPlayerSubsystem.h"

#include "GameLab/UI/Public/AutoFocusTargetInterface.h"
#include "CommonInputSubsystem.h"
#include "CommonActivatableWidget.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/PlayerController.h"
#include "TimerManager.h"

void UAutoFocusLocalPlayerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	if (ULocalPlayer* LP = GetLocalPlayer())
	{
		if (UCommonInputSubsystem* CIS = LP->GetSubsystem<UCommonInputSubsystem>())
		{
			CurrentInputType = CIS->GetCurrentInputType();
			InputChangedHandle = CIS->OnInputMethodChangedNative.AddUObject(
				this, &UAutoFocusLocalPlayerSubsystem::HandleInputMethodChanged
			);
		}
	}
}

void UAutoFocusLocalPlayerSubsystem::Deinitialize()
{
	if (ULocalPlayer* LP = GetLocalPlayer())
	{
		if (UCommonInputSubsystem* CIS = LP->GetSubsystem<UCommonInputSubsystem>())
		{
			CIS->OnInputMethodChangedNative.Remove(InputChangedHandle);
		}
	}

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(MouseActivityTimerHandle);
	}

	Super::Deinitialize();
}

void UAutoFocusLocalPlayerSubsystem::NotifyInteractable(UWidget* Widget)
{
	if (IsValid(Widget))
	{
		// Write-only: never restore focus from here to avoid recursion (focus callbacks may call this).
		LastInteractable = Widget;
	}
}

void UAutoFocusLocalPlayerSubsystem::NotifyMouseActivity()
{
	bMouseRecentlyActive = true;

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(MouseActivityTimerHandle);
		World->GetTimerManager().SetTimer(
			MouseActivityTimerHandle,
			this,
			&UAutoFocusLocalPlayerSubsystem::ClearMouseActivity,
			MouseActiveSeconds,
			false
		);
	}
	
	if (APlayerController* PC = GetLocalPlayer() ? GetLocalPlayer()->GetPlayerController(GetWorld()) : nullptr)
	{
		PC->SetShowMouseCursor(true);
	}
}

void UAutoFocusLocalPlayerSubsystem::ClearMouseActivity()
{
	bMouseRecentlyActive = false;
}

void UAutoFocusLocalPlayerSubsystem::SetRootActivatable(UCommonActivatableWidget* InRoot)
{
	RootActivatable = InRoot;
}

void UAutoFocusLocalPlayerSubsystem::HandleInputMethodChanged(ECommonInputType NewType)
{
	CurrentInputType = NewType;

	// Restore focus when entering focus-driven input modes.
	// - Gamepad always requires a valid focus target
	// - MouseAndKeyboard restores only when behaving like keyboard (mouse not recently active),
	//   otherwise avoid fighting the cursor.
	if (NewType == ECommonInputType::Gamepad)
	{
		QueueRestoreBestFocus();
	}
	else if (NewType == ECommonInputType::MouseAndKeyboard)
	{
		if (!bMouseRecentlyActive)
		{
			QueueRestoreBestFocus();
		}
	}
	
	if (APlayerController* PC = GetLocalPlayer() ? GetLocalPlayer()->GetPlayerController(GetWorld()) : nullptr)
	{
		if (NewType == ECommonInputType::Gamepad)
		{
			PC->SetShowMouseCursor(false);
		}
		else // MouseAndKeyboard
		{
			PC->SetShowMouseCursor(true);
		}
	}
}

void UAutoFocusLocalPlayerSubsystem::RestoreBestFocus()
{
	QueueRestoreBestFocus();
}

void UAutoFocusLocalPlayerSubsystem::QueueRestoreBestFocus()
{
	if (bRestoreQueued)
	{
		return;
	}

	bRestoreQueued = true;

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimerForNextTick(this, &UAutoFocusLocalPlayerSubsystem::RestoreBestFocus_Internal);
	}
}

void UAutoFocusLocalPlayerSubsystem::RestoreBestFocus_Internal()
{
	bRestoreQueued = false;

	if (bIsRestoringFocus)
	{
		return;
	}

	TGuardValue<bool> Guard(bIsRestoringFocus, true);

	// 1) Last interactable
	if (TryFocusWidget(LastInteractable.Get()))
	{
		return;
	}

	// 2) Root desired focus target fallback
	if (UCommonActivatableWidget* Root = RootActivatable.Get())
	{
		if (UWidget* Desired = Root->GetDesiredFocusTarget())
		{
			TryFocusWidget(Desired);
		}
	}
}

UWidget* UAutoFocusLocalPlayerSubsystem::ResolveFocusTarget(UWidget* Widget) const
{
	if (!IsValid(Widget))
	{
		return nullptr;
	}

	if (Widget->GetClass()->ImplementsInterface(UAutoFocusTargetInterface::StaticClass()))
	{
		if (UWidget* Target = IAutoFocusTargetInterface::Execute_GetAutoFocusTarget(Widget))
		{
			return Target;
		}
	}

	return Widget;
}

bool UAutoFocusLocalPlayerSubsystem::TryFocusWidget(UWidget* Widget)
{
	UWidget* FocusTarget = ResolveFocusTarget(Widget);
	if (!IsValid(FocusTarget))
	{
		return false;
	}

	APlayerController* PC = GetLocalPlayer() ? GetLocalPlayer()->GetPlayerController(GetWorld()) : nullptr;
	if (!PC)
	{
		return false;
	}

	// Avoid focus bounce: if already focused, do nothing.
	if (FocusTarget->HasUserFocus(PC))
	{
		return true;
	}

	// Optional sanity checks (prevents choosing dead/unfocusable targets).
	if (!FocusTarget->GetIsEnabled())
	{
		return false;
	}

	if (FocusTarget->GetVisibility() == ESlateVisibility::Collapsed)
	{
		return false;
	}

	FocusTarget->SetUserFocus(PC);
	
	if (FocusTarget->HasUserFocus(PC))
	{
		return true;
	}
	
	FocusTarget->SetKeyboardFocus();
	
	return true;
}
