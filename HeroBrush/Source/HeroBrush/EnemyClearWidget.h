// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "EnemyClearWidget.generated.h"

/**
 * 
 */
UCLASS()
class HEROBRUSH_API UEnemyClearWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
		UButton* MenuButt;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
		UButton* NextMapButt;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
		UButton* AddBuffButt;
};
