// // Fill out your copyright notice in the Description page of Project Settings.

// #pragma once

// #include "Blueprint/UserWidget.h"

// #include "Player_MainUI.generated.h"

// #define PLAYERUI_API ARRANGER_API

// /**Forward declaration */
// class UWidgetAnimation;

// /**
//  * 
//  */
// UCLASS()
// class UPlayer_MainUI : public UUserWidget
// {
// 	GENERATED_BODY()

// public:

//   UFUNCTION(BlueprintImplementableEvent, Category = "ARRanger|MainUI", meta = (DisplayName = "On Punch Button Pressed"))
//   PLAYERUI_API void K2_OnPunchButtonEvent();

//   UFUNCTION(BlueprintImplementableEvent, Category = "ARRanger|MainUI", meta = (DisplayName = "On Kick Button Pressed"))
//   PLAYERUI_API void K2_OnKickButtonEvent();

//   UFUNCTION(BlueprintImplementableEvent, Category = "ARRanger|MainUI", meta = (DisplayName = "On LockOn Button Pressed"))
//   PLAYERUI_API void K2_OnLockOnButtonEvent();

//   UFUNCTION(BlueprintImplementableEvent, Category = "ARRanger|MainUI", meta = (DisplayName = "On Jump Button Pressed"))
//   PLAYERUI_API void K2_OnJumpButtonEvent();
	
// private:
  
//   UPROPERTY(meta = (BindWidgetAnimOptional))
//   TObjectPtr<UWidgetAnimation> PunchAnim_Icon;
  
//   UPROPERTY(meta = (BindWidgetAnimOptional))
//   TObjectPtr<UWidgetAnimation> KickAnim_Icon;
  
//   UPROPERTY(meta = (BindWidgetAnimOptional))
//   TObjectPtr<UWidgetAnimation> LockOnAnim_Icon;
  
//   UPROPERTY(meta = (BindWidgetAnimOptional))
//   TObjectPtr<UWidgetAnimation> JumpAnim_Icon;


// };

// #undef PLAYERUI_API