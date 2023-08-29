﻿// Copyright Epic Games, Inc. All Rights Reserved.

#include "HeroBrushCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/GameplayStatics.h"
#include "BaseWeaponBullets.h"
#include "Components/TextRenderComponent.h"
#include "Item.h"
#include "Weapon.h"
#include "GameplayController.h"
#include "Interactable.h"

//////////////////////////////////////////////////////////////////////////
// AHeroBrushCharacter

AHeroBrushCharacter::AHeroBrushCharacter()
{
	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller


	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	MyArms = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Character MyArms"));
	MyArms->SetOnlyOwnerSee(true);
	MyArms->SetupAttachment(CameraThird);
	MyArms->bCastDynamicShadow = false;
	MyArms->CastShadow = false;
	// MyArms->SetRelativeRotation(FRotator(0.9f, -19.19f, 5.2f));
	MyArms->SetRelativeLocation(FVector(-30.f, 0.f, -150.f));

	// Create a follow camera
	CameraThird = CreateDefaultSubobject<UCameraComponent>(TEXT("HeroThirdCamera"));
	CameraThird->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	CameraThird->SetRelativeLocation(FVector(20.0f, 100.0f, 100.0f));
	CameraThird->bUsePawnControlRotation = true; // Camera does not rotate relative to arm

	CameraShoot = CreateDefaultSubobject<UCameraComponent>(TEXT("HeroShootCamera"));
	CameraShoot->SetupAttachment(CameraBoom);
	CameraShoot->SetRelativeLocation(FVector(30.0f, 80.0f, 80.0f));

}

void AHeroBrushCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
	PostInitializeComponents();
}




//void AHeroBrushCharacter::EndPlay()
//{
//	Super::EndPlay();
//	
//	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_Flash_Attack);
//	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_Flash_waitTime);
//	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_PrimaryAttack);
//
//}

//////////////////////////////////////////////////////////////////////////
// Input

void AHeroBrushCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {

		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AHeroBrushCharacter::Move);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AHeroBrushCharacter::Look);

		//changeView
		PlayerInputComponent->BindAction("ChangeView", IE_Pressed, this, &AHeroBrushCharacter::ChangeView);
		PlayerInputComponent->BindAction("Primary_Attack", IE_Pressed, this, &AHeroBrushCharacter::Primary_Attack);
		PlayerInputComponent->BindAction("Burden_Attack", IE_Pressed, this, &AHeroBrushCharacter::Burden_Attack);
		PlayerInputComponent->BindAction("Flash_Attack", IE_Pressed, this, &AHeroBrushCharacter::Flash_Attack);
		PlayerInputComponent->BindAction("TurnOnSpeed", IE_Pressed, this, &AHeroBrushCharacter::TurnOnSpeed);
		PlayerInputComponent->BindAction("TurnOffSpeed", IE_Released, this, &AHeroBrushCharacter::TurnOffSpeed);
		
		//Interact
		PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &AHeroBrushCharacter::OnInteract);
	}

}

void AHeroBrushCharacter::OnInteract() 
{
	AWeapon* Weapon = Cast<AWeapon>(ActiveOverlapItem);
	if (Weapon) {
		Weapon->Equip(this);
		ActiveOverlapItem = nullptr;
	}

}

void AHeroBrushCharacter::SetWeapon(AWeapon* Weapon)
{
	if (EquiqedWeapon) {
		EquiqedWeapon->Destroy();
	}
	EquiqedWeapon = Weapon;
	Weapon->TextCue->SetVisibility(false);

}


void AHeroBrushCharacter::ChangeView()
{
	APlayerController* OurPlayerController = UGameplayStatics::GetPlayerController(this, 0);
	if (CameraThird->IsActive()) {
		CameraBoom->TargetArmLength = 200.0f;
		CameraShoot->SetActive(true);

		CameraThird->SetActive(false);
		
	}
	else if (CameraShoot->IsActive()) {
		CameraBoom->TargetArmLength = 400.0f;
		CameraThird->SetActive(true);

		CameraShoot->SetActive(false);
	}
	else {
		CameraBoom->TargetArmLength = 400.0f;
		CameraThird->SetActive(true);	
		CameraShoot->SetActive(false);
	}
}






void AHeroBrushCharacter::ChangeEnergy(bool IsLong, int TimePeriod, float EnergyRange) {
	if (IsLong) {
		
	}
	else {
		ChangeOnceEnergy(EnergyRange);
	}
}
void AHeroBrushCharacter::ChangeOnceEnergy(float EnergyRange) {
	if (CurEnergy + EnergyRange < TotalEnergy && CurEnergy + EnergyRange > 0)
		CurEnergy += EnergyRange;
	else if (CurEnergy + EnergyRange >= TotalEnergy)
		CurEnergy = TotalEnergy;
	else if (CurEnergy + EnergyRange <= 0)
		CurEnergy = 0.0f;
}

//Basic Attack
void AHeroBrushCharacter::Primary_Attack() {
	if (AttackAnimSeq == 0) {
		PlayAnimMontage(AttackAnim1);
		HandLocation = GetMesh()->GetSocketLocation("Muzzle_01");
		AttackAnimSeq++;
		AttackAnimSeq = AttackAnimSeq % 2;
	}
	else if (AttackAnimSeq == 1) {
		PlayAnimMontage(AttackAnim2);
		HandLocation = GetMesh()->GetSocketLocation("Muzzle_02");
		AttackAnimSeq++;
		AttackAnimSeq = AttackAnimSeq % 2;
	}
	else if (AttackAnimSeq == 2) {
		PlayAnimMontage(AttackAnim3);
		HandLocation = GetMesh()->GetSocketLocation("Muzzle_03");
		AttackAnimSeq++;
		AttackAnimSeq = AttackAnimSeq % 3;
	}

	GetWorldTimerManager().SetTimer(TimerHandle_PrimaryAttack, this, &AHeroBrushCharacter::PrimaryAttack_TimeElapsed, 0.2f);
	
}

// you can set shoot location and shoot speed
void AHeroBrushCharacter::PrimaryAttack_TimeElapsed() {

	FromLocation = HandLocation; // 设置开始的rotation
	GetAimRotation(); // 得到rotation

	const FTransform SpawnTM = FTransform(AimRotation, FromLocation);
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.Instigator = this;

	GetWorld()->SpawnActor<AActor>(ProjectileClass, SpawnTM, SpawnParams); // 使用已有的蓝图加载，在蓝图中设置
}

void AHeroBrushCharacter::Burden_Attack()
{
	PlayAnimMontage(BurdenAnim);
	HandLocation = GetMesh()->GetSocketLocation("Muzzle_01");

	GetWorldTimerManager().SetTimer(TimerHandle_Burden_Attack, this, &AHeroBrushCharacter::Burden_Attack_TimeElapsed, 1.0f);
}
void AHeroBrushCharacter::Burden_Attack_TimeElapsed() {

	FromLocation = HandLocation; // 设置开始的rotation

	const FTransform SpawnTM = FTransform(this->GetActorRotation(), FromLocation);
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.Instigator = this;

	GetWorld()->SpawnActor<AActor>(ProjectileClass_Burden, SpawnTM, SpawnParams); // 使用已有的蓝图加载，在蓝图中设置
}
void AHeroBrushCharacter::Flash_Attack()
{
	if (isFlashActive)
	{
		isFlashActive = false;
		FVector PresentLocation = this->GetActorLocation();
		FVector ForwardLocation = GetActorForwardVector();
		FVector NextLocation = PresentLocation + ForwardLocation * FlashDistance;

		FromLocation = PresentLocation;
		AimLocation = NextLocation;
		GetWorldTimerManager().SetTimer(TimerHandle_PrimaryAttack, this, &AHeroBrushCharacter::Flash_TimeElapsed, 0.2f);


		this->SetActorLocation(NextLocation, false);
	}
}

void AHeroBrushCharacter::Flash_TimeElapsed() {

	
	GetAimRotation(); // 得到rotation

	const FTransform SpawnTM = FTransform(AimRotation, FromLocation);
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.Instigator = this;

	GetWorld()->SpawnActor<AActor>(ProjectileClass_Flash, SpawnTM, SpawnParams); // 使用已有的蓝图加载，在蓝图中设置
}

void AHeroBrushCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// 如果已经被使用进入冷却，记录时间，直到time=0，则技能继续可以被使用，不会进入冷却状态。
	if (!isFlashActive) {
		Flash_Attack_Time--;
	}
	if (Flash_Attack_Time <= 0) {
		isFlashActive = true;
		Flash_Attack_Time = 180;
	}
	
	CheckForInteractables();

}

void AHeroBrushCharacter::TurnOnSpeed()
{
	//SpeedUpDelegate.BindUObject(this, &AHealthCharacter::ChangeOnceEnergy, -0.5f);
	GetCharacterMovement()->MaxWalkSpeed = SetSpeedMax;

	//GetWorld()->GetTimerManager().SetTimer(SpeedUpTimer, SpeedUpDelegate, 0.05f, true, -1);

}
void AHeroBrushCharacter::TurnOffSpeed()
{
	//SpeedUpDelegate.Unbind();
	GetCharacterMovement()->MaxWalkSpeed = 500.0f;
	//GetWorld()->GetTimerManager().ClearTimer(SpeedUpTimer);
}

void AHeroBrushCharacter::NotifyActorBeginOverlap(AActor* OtherActor)
{
	CheckTouchActor(OtherActor);

}
void AHeroBrushCharacter::CheckTouchActor(AActor* OtherActor)
{
	auto actor = Cast<ABaseWeaponBullets>(OtherActor);
	// 如果是来自敌人的bullets
	if (actor != nullptr && !actor->DamageFrom) {
		if (actor->BulletsInfo == 0) {
			ChangeHealth(false, -1, -5.0f); // 第一类的掉血
		} 
	}
}

void AHeroBrushCharacter::CheckForInteractables() {
	// 射线拾取
	FHitResult HitResult; // 存储碰撞的结果的变量

	FVector StartTrace = CameraBoom->GetComponentLocation(); // 射线起始点
	FVector EndTrace = (CameraBoom->GetForwardVector() * 300) + StartTrace; // 射线终止点。

	FCollisionQueryParams QueryParams; // 储存了碰撞相关的信息
	QueryParams.AddIgnoredActor(this); // 将我们角色自身忽略掉，减少性能开销

	AGameplayController* controller = Cast<AGameplayController>(GetController()); // 玩家控制器

	if (GetWorld()->LineTraceSingleByChannel(HitResult, StartTrace, EndTrace, ECC_Visibility, QueryParams) && controller) {
		//检查我们点击的项目是否是一个可交互的项目
		if (AInteractable* Interactable = Cast<AInteractable>(HitResult.GetActor())) {
			controller->CurrentInteractable = Interactable;
			return;
		}
	}

	//如果我们没有击中任何东西，或者我们击中的东西不是一个可交互的，设置currentinteractable为nullptr
	controller->CurrentInteractable = nullptr;

}