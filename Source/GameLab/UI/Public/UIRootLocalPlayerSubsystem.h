// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "UIRootLocalPlayerSubsystem.generated.h"

class UUIRootWidget;
class UCommonActivatableWidget;

/**
 * LocalPlayer UI root manager.
 * Creates and stores a single UUIRootWidget reference (Blueprint: WBP_UIRoot).
 */
UCLASS()
class GAMELAB_API UUIRootLocalPlayerSubsystem : public ULocalPlayerSubsystem
{
	GENERATED_BODY()

public:
	/** Assign an existing root widget instance. */
	UFUNCTION(BlueprintCallable, Category="UI|Root",
		meta=(ToolTip="Assign an existing root widget instance."))
	void SetRootWidget(UUIRootWidget* InWidget);

	/** Get the current root widget instance. */
	UFUNCTION(BlueprintPure, Category="UI|Root",
		meta=(ToolTip="Get the current root widget instance."))
	UUIRootWidget* GetRootWidget() const;

	/** Create root widget if missing and add it to the viewport. */
	UFUNCTION(BlueprintCallable, Category="UI|Root",
		meta=(ToolTip="Create the root widget if missing and add it to the viewport."))
	UUIRootWidget* CreateRootWidget(TSubclassOf<UUIRootWidget> WidgetClass, int32 ZOrder = 0);

	/** Remove root from viewport but keep the reference. */
	UFUNCTION(BlueprintCallable, Category="UI|Root",
		meta=(ToolTip="Remove root from viewport but keep the reference."))
	void HideRootWidget();

	/** Destroy root completely and clear reference. */
	UFUNCTION(BlueprintCallable, Category="UI|Root",
		meta=(ToolTip="Destroy root completely and clear reference."))
	void ClearRootWidget();

	/** Convenience: push into MainStack. */
	UFUNCTION(BlueprintCallable, Category="UI|Root",
		meta=(ToolTip="Push a widget into the root MainStack."))
	UCommonActivatableWidget* PushMain(TSubclassOf<UCommonActivatableWidget> WidgetClass);

	/** Convenience: push into ModalStack. */
	UFUNCTION(BlueprintCallable, Category="UI|Root",
		meta=(ToolTip="Push a widget into the root ModalStack."))
	UCommonActivatableWidget* PushModal(TSubclassOf<UCommonActivatableWidget> WidgetClass);

	/** Convenience: pop MainStack. */
	UFUNCTION(BlueprintCallable, Category="UI|Root",
		meta=(ToolTip="Pop the top widget from the root MainStack."))
	bool PopMain();

	/** Convenience: pop ModalStack. */
	UFUNCTION(BlueprintCallable, Category="UI|Root",
		meta=(ToolTip="Pop the top widget from the root ModalStack."))
	bool PopModal();

private:
	UPROPERTY(Transient, meta=(ToolTip="Stored root widget instance (per LocalPlayer)."))
	TObjectPtr<UUIRootWidget> RootWidget;
};
