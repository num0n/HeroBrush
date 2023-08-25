// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HeroCharacter.h"
#include "Enemy.generated.h"

/**
 * 
 */
class UWidgetComponent;
UCLASS()
class HEROBRUSH_API AEnemy : public AHeroCharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	 AEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void NotifyActorBeginOverlap(AActor* OtherActor);

	void CheckTouchActor(AActor* OtherActor);
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Info, meta = (AllowPrivateAccess = "true"))
		class UWidgetComponent* StatusWidgetComponent;
};
