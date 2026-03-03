// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "UIRootWidget.generated.h"

class UCommonActivatableWidgetStack;
class UCommonActivatableWidget;
class UAutoFocusLocalPlayerSubsystem;

/**
 * CommonUI Root widget (Blueprint: WBP_UIRoot).
 * Expected hierarchy (Designer):
 * - Root (Overlay)
 *   - MainStack  (CommonActivatableWidgetStack) [Is Variable]
 *   - ModalStack (CommonActivatableWidgetStack) [Is Variable]
 */
UCLASS(Abstract, BlueprintType)
class GAMELAB_API UUIRootWidget : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	/** Main navigation stack (e.g. menus). BindWidget name must be 'MainStack'. */
	UPROPERTY(meta=(BindWidget), BlueprintReadOnly, Category="UI|Root",
		meta=(ToolTip="Main CommonActivatableWidgetStack. Bind to 'MainStack' in WBP_UIRoot."))
	TObjectPtr<UCommonActivatableWidgetStack> MainStack;

	/** Modal stack (e.g. confirm dialogs). BindWidget name must be 'ModalStack'. */
	UPROPERTY(meta=(BindWidget), BlueprintReadOnly, Category="UI|Root",
		meta=(ToolTip="Modal CommonActivatableWidgetStack. Bind to 'ModalStack' in WBP_UIRoot."))
	TObjectPtr<UCommonActivatableWidgetStack> ModalStack;

	/** Push a widget into the MainStack. Returns the created widget instance (pooled). */
	UFUNCTION(BlueprintCallable, Category="UI|Root",
		meta=(ToolTip="Push a widget into MainStack. Returns the created (pooled) instance."))
	UCommonActivatableWidget* PushMain(TSubclassOf<UCommonActivatableWidget> WidgetClass);

	/** Push a widget into the ModalStack. Returns the created widget instance (pooled). */
	UFUNCTION(BlueprintCallable, Category="UI|Root",
		meta=(ToolTip="Push a widget into ModalStack. Returns the created (pooled) instance."))
	UCommonActivatableWidget* PushModal(TSubclassOf<UCommonActivatableWidget> WidgetClass);

	/**
	 * Pop the top widget from MainStack in a CommonUI-safe way.
	 * Preferred: Deactivate the active widget (stack will auto-remove on deactivate).
	 * Fallback: RemoveWidget(active).
	 */
	UFUNCTION(BlueprintCallable, Category="UI|Root",
		meta=(ToolTip="Pop the active widget from MainStack (CommonUI-safe)."))
	bool PopMain(bool bPreferDeactivate = true);

	/**
	 * Pop the top widget from ModalStack in a CommonUI-safe way.
	 * Preferred: Deactivate the active widget (stack will auto-remove on deactivate).
	 * Fallback: RemoveWidget(active).
	 */
	UFUNCTION(BlueprintCallable, Category="UI|Root",
		meta=(ToolTip="Pop the active widget from ModalStack (CommonUI-safe)."))
	bool PopModal(bool bPreferDeactivate = true);

	/** Return the currently active widget, giving priority to ModalStack. */
	UFUNCTION(BlueprintPure, Category="UI|Root",
		meta=(ToolTip="Return active widget (ModalStack has priority)."))
	UCommonActivatableWidget* GetTopmostActivatable() const;

protected:
	/** Register this root in AutoFocus subsystem when activated. */
	virtual void NativeOnActivated() override;

	/** Fallback focus target: topmost activatable's desired focus target. */
	virtual UWidget* NativeGetDesiredFocusTarget() const override;

private:
	/** Resolve AutoFocus subsystem for the owning local player. */
	UAutoFocusLocalPlayerSubsystem* GetAutoFocusSubsystem() const;

	/** Register this root as AutoFocus fallback root. */
	void RegisterAsAutoFocusRoot();

	/** If input is focus-driven, request a focus restore after push/pop. */
	void RequestAutoFocusRestoreIfNeeded();
};
