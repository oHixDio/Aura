// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/AuraHUD.h"

#include "UI/Widget/AuraUserWidget.h"
#include "UI/WidgetController/OverlayWidgetController.h"

UOverlayWidgetController* AAuraHUD::GetOverlayWidgetControllerInstance(const FWidgetControllerParams& WidgetControllerParams)
{
	// Instanceが生成されていないため、生成する.
	if (OverlayWidgetControllerInstance == nullptr)
	{
		checkf(OverlayWidgetControllerClass, TEXT("OverlayWidgetControllerのクラスが設定されていません。BPから設定してください。"));
		
		// WidgetControllerの生成し、Paramsを設定する.
		OverlayWidgetControllerInstance = NewObject<UOverlayWidgetController>(this, OverlayWidgetControllerClass);
		OverlayWidgetControllerInstance->SetWidgetControllerParams(WidgetControllerParams);
		
		return OverlayWidgetControllerInstance;
	}
	
	// Instanceが生成済みの為、そのまま返却.
	return OverlayWidgetControllerInstance;
}

void AAuraHUD::InitOverlay(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS)
{
	checkf(OverlayWidgetClass, TEXT("OverlayWidgetのクラスが設定されていません。BPから設定してください。"));

	// OverlayWidgetの生成し、WidgetControllerを与える.
	OverlayWidget = CreateWidget<UAuraUserWidget>(GetWorld(), OverlayWidgetClass);
	const FWidgetControllerParams WidgetControllerParams(PC, PS, ASC, AS);
	UOverlayWidgetController* WidgetController = GetOverlayWidgetControllerInstance(WidgetControllerParams);
	OverlayWidget->SetWidgetController(WidgetController);
	// 表示！
	OverlayWidget->AddToViewport();
}