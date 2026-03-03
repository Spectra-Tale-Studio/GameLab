// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "CommonInputTypeEnum.h"
#include "AutoFocusLocalPlayerSubsystem.generated.h"

class UWidget;
class UCommonActivatableWidget;

/**
 * Per-LocalPlayer autofocus manager.
 * Tracks the last interactable widget and restores focus on input method changes.
 *
 * Key safety rules:
 * - Never restore focus synchronously from focus-change callbacks (prevents recursion/stack overflow).
 * - Focus restoration is always deferred to next tick and guarded against re-entrancy.
 */
UCLASS()
class GAMELAB_API UAutoFocusLocalPlayerSubsystem : public ULocalPlayerSubsystem
{
	GENERATED_BODY()

public:
	/** Initializes bindings to CommonInputSubsystem (input method changes). */
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	/** Removes bindings and timers. */
	virtual void Deinitialize() override;

	/**
	 * Notify the subsystem that this widget is the latest interactable (hovered or focused).
	 * This function must be "write-only": it should NEVER attempt to set focus directly.
	 */
	UFUNCTION(BlueprintCallable, Category="UI|AutoFocus",
		meta=(ToolTip="Notify that this widget is the latest interactable (hovered or focused). Does not set focus immediately."))
	void NotifyInteractable(UWidget* Widget);

	/**
	 * Notify mouse activity (to distinguish Mouse vs Keyboard within MouseAndKeyboard).
	 * Call from wrappers on mouse move/enter.
	 */
	UFUNCTION(BlueprintCallable, Category="UI|AutoFocus",
		meta=(ToolTip="Notify mouse activity to distinguish Mouse vs Keyboard within MouseAndKeyboard input type."))
	void NotifyMouseActivity();

	/**
	 * Sets the current UI root activatable used as a fallback for GetDesiredFocusTarget().
	 */
	UFUNCTION(BlueprintCallable, Category="UI|AutoFocus",
		meta=(ToolTip="Set the UI root activatable used as fallback for GetDesiredFocusTarget()."))
	void SetRootActivatable(UCommonActivatableWidget* InRoot);

	/**
	 * Request a focus restore (deferred).
	 * This call is safe from anywhere (coalesced + next-tick + re-entrancy guarded).
	 */
	UFUNCTION(BlueprintCallable, Category="UI|AutoFocus",
		meta=(ToolTip="Request focus restoration (deferred to next tick, re-entrancy guarded)."))
	void RestoreBestFocus();

	/** Returns current CommonInput type (Gamepad or MouseAndKeyboard). */
	UFUNCTION(BlueprintPure, Category="UI|AutoFocus",
		meta=(ToolTip="Return current CommonInput type (Gamepad or MouseAndKeyboard)."))
	ECommonInputType GetCurrentInputType() const { return CurrentInputType; }

	/** True if the subsystem considers the mouse recently active. */
	UFUNCTION(BlueprintPure, Category="UI|AutoFocus",
		meta=(ToolTip="True if the subsystem considers the mouse recently active."))
	bool IsMouseRecentlyActive() const { return bMouseRecentlyActive; }

private:
	/** Called by CommonInputSubsystem when the input method changes. */
	void HandleInputMethodChanged(ECommonInputType NewType);

	/** Queue a focus restore for next tick (coalesces multiple requests). */
	void QueueRestoreBestFocus();

	/** Performs the actual focus restoration (called next tick). */
	void RestoreBestFocus_Internal();

	/** Attempts to focus a widget, resolving AutoFocusTargetInterface first if available. */
	bool TryFocusWidget(UWidget* Widget);

	/** Resolves interface focus target if implemented, otherwise returns the widget itself. */
	UWidget* ResolveFocusTarget(UWidget* Widget) const;

	/** Clears the mouse activity flag after a short delay. */
	void ClearMouseActivity();

private:
	/** Last known CommonInput type (Gamepad or MouseAndKeyboard). */
	UPROPERTY(Transient, meta=(ToolTip="Last known CommonInput type (Gamepad or MouseAndKeyboard)."))
	ECommonInputType CurrentInputType = ECommonInputType::MouseAndKeyboard;

	/** True when mouse activity was detected recently. */
	UPROPERTY(Transient, meta=(ToolTip="True when mouse activity was detected recently."))
	bool bMouseRecentlyActive = false;

	/** Time window used to consider the mouse 'recently active'. */
	UPROPERTY(EditDefaultsOnly, Category="UI|AutoFocus", meta=(ToolTip="Seconds after mouse activity during which we treat input as Mouse."))
	float MouseActiveSeconds = 0.35f;

	/** Last interactable widget (hovered or focused). */
	TWeakObjectPtr<UWidget> LastInteractable;

	/** Root activatable widget used for fallback focus. */
	TWeakObjectPtr<UCommonActivatableWidget> RootActivatable;

	/** Timer to clear mouse activity flag. */
	FTimerHandle MouseActivityTimerHandle;

	/** Delegate handle for input method changes. */
	FDelegateHandle InputChangedHandle;

	/** Re-entrancy guard for focus restoration. */
	bool bIsRestoringFocus = false;

	/** Coalescing flag: ensures we restore at most once per tick. */
	bool bRestoreQueued = false;
};
