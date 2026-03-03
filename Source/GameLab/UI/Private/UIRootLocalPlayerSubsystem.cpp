// Fill out your copyright notice in the Description page of Project Settings.

#include "GameLab/UI/Public/UIRootLocalPlayerSubsystem.h"
#include "GameLab/UI/Public/UIRootWidget.h"

#include "Engine/LocalPlayer.h"
#include "GameFramework/PlayerController.h"

void UUIRootLocalPlayerSubsystem::SetRootWidget(UUIRootWidget* InWidget)
{
	RootWidget = InWidget;
}

UUIRootWidget* UUIRootLocalPlayerSubsystem::GetRootWidget() const
{
	return RootWidget;
}

UUIRootWidget* UUIRootLocalPlayerSubsystem::CreateRootWidget(TSubclassOf<UUIRootWidget> WidgetClass, int32 ZOrder)
{
	if (RootWidget)
	{
		return RootWidget;
	}

	if (!WidgetClass)
	{
		return nullptr;
	}

	ULocalPlayer* LP = GetLocalPlayer();
	if (!LP)
	{
		return nullptr;
	}

	APlayerController* PC = LP->GetPlayerController(GetWorld());
	if (!PC)
	{
		return nullptr;
	}

	RootWidget = CreateWidget<UUIRootWidget>(PC, WidgetClass);
	if (RootWidget)
	{
		RootWidget->AddToViewport(ZOrder);
		RootWidget->ActivateWidget();
	}

	return RootWidget;
}

void UUIRootLocalPlayerSubsystem::HideRootWidget()
{
	if (RootWidget)
	{
		RootWidget->RemoveFromParent();
	}
}

void UUIRootLocalPlayerSubsystem::ClearRootWidget()
{
	if (RootWidget)
	{
		RootWidget->RemoveFromParent();
		RootWidget = nullptr;
	}
}

UCommonActivatableWidget* UUIRootLocalPlayerSubsystem::PushMain(TSubclassOf<UCommonActivatableWidget> WidgetClass)
{
	return RootWidget ? RootWidget->PushMain(WidgetClass) : nullptr;
}

UCommonActivatableWidget* UUIRootLocalPlayerSubsystem::PushModal(TSubclassOf<UCommonActivatableWidget> WidgetClass)
{
	return RootWidget ? RootWidget->PushModal(WidgetClass) : nullptr;
}

bool UUIRootLocalPlayerSubsystem::PopMain()
{
	return RootWidget ? RootWidget->PopMain() : false;
}

bool UUIRootLocalPlayerSubsystem::PopModal()
{
	return RootWidget ? RootWidget->PopModal() : false;
}
