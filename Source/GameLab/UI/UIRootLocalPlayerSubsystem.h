// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "UIRootLocalPlayerSubsystem.generated.h"

class UUserWidget;
class APlayerController;

/**
 * Local Player UI Root manager.
 * Stores and manages a single UUserWidget reference (Blueprint UI Root).
 */
UCLASS()
class GAMELAB_API UUIRootLocalPlayerSubsystem : public ULocalPlayerSubsystem
{
	GENERATED_BODY()

public:

	/* Assign an existing widget as the root */
	UFUNCTION(BlueprintCallable, Category="UI|Root")
	void SetRootWidget(UUserWidget* InWidget);

	/* Get current root widget */
	UFUNCTION(BlueprintPure, Category="UI|Root")
	UUserWidget* GetRootWidget() const;

	/* Create root widget if missing and add to viewport */
	UFUNCTION(BlueprintCallable, Category="UI|Root")
	UUserWidget* CreateRootWidget(TSubclassOf<UUserWidget> WidgetClass, int32 ZOrder = 0);

	/* Remove from viewport but keep reference */
	UFUNCTION(BlueprintCallable, Category="UI|Root")
	void HideRootWidget();

	/* Destroy completely */
	UFUNCTION(BlueprintCallable, Category="UI|Root")
	void ClearRootWidget();

private:

	UPROPERTY(Transient)
	TObjectPtr<UUserWidget> RootWidget;
};
