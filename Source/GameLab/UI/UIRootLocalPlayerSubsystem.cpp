// Fill out your copyright notice in the Description page of Project Settings.

#include "UIRootLocalPlayerSubsystem.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerController.h"
#include "Engine/LocalPlayer.h"

void UUIRootLocalPlayerSubsystem::SetRootWidget(UUserWidget* InWidget)
{
	RootWidget = InWidget;
}

UUserWidget* UUIRootLocalPlayerSubsystem::GetRootWidget() const
{
	return RootWidget;
}

UUserWidget* UUIRootLocalPlayerSubsystem::CreateRootWidget(TSubclassOf<UUserWidget> WidgetClass, int32 ZOrder)
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

	RootWidget = CreateWidget<UUserWidget>(PC, WidgetClass);

	if (RootWidget)
	{
		RootWidget->AddToViewport(ZOrder);
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
