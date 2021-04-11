// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "ShooterCharacter.h"
#include "ShooterWeapon.h"
#include "ShooterPlayerController.h"

// Sets default values
AShooterCharacter::AShooterCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//对第三人称mesh可见性，贴花投射及碰撞设置
	GetMesh()->bOnlyOwnerSee = false;
	GetMesh()->bOwnerNoSee = true;
	GetMesh()->bReceivesDecals = false;
	GetMesh()->SetCollisionObjectType(ECC_Pawn);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetCollisionResponseToChannel(COLLISION_PICKUP, ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(COLLISION_WEAPON, ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(COLLISION_PROJECTILE, ECR_Block);

	//胶囊体的碰撞设置，只对子弹进行响应
	GetCapsuleComponent()->SetCollisionResponseToChannel(COLLISION_PROJECTILE,ECR_Block);
	GetCapsuleComponent()->SetCollisionResponseToChannel(COLLISION_WEAPON, ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(COLLISION_PICKUP, ECR_Ignore);


	//创建相机并挂载到根组件上,固定其相对位置，由pawn控制相机的旋转
	/*FPCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("PawnCamera1p"));
	FPCamera->SetupAttachment(GetCapsuleComponent());
	FPCamera->SetRelativeLocation(FVector(0.0f, 0.0f, BaseEyeHeight));
	FPCamera->bUsePawnControlRotation = true;
	*/
	//创建手臂组件，并进行设置
	FPArm = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("PawnArm1p"));
	FPArm->SetupAttachment(GetCapsuleComponent());
	//FPArm->SetupAttachment(FPCamera);
	FPArm->bOnlyOwnerSee = true;
	FPArm->bOwnerNoSee = false;
	FPArm->bReceivesDecals = false;
	FPArm->bCastDynamicShadow = false;
	//骨骼动画的姿态更新只发生在渲染时，效率考量
	FPArm->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::OnlyTickPoseWhenRendered;
	//把第一人称mesh的更新组设置到优先于物理更新的组
	FPArm->PrimaryComponentTick.TickGroup = TG_PrePhysics;
	//手臂碰撞设置：由于第三人称Mesh有具体的碰撞设置，选择关闭第一人称Mesh的碰撞
	FPArm->SetCollisionObjectType(ECC_Pawn);
	FPArm->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	FPArm->SetCollisionResponseToAllChannels(ECR_Ignore);
	//设置相对位置
	//FPArm->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f - 86.0f - BaseEyeHeight));

	//设置默认瞄准关闭
	bIsTargeting = false;
}

// Called when the game starts or when spawned
void AShooterCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AShooterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//输入信息与对应函数、对象绑定
	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &AShooterCharacter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &AShooterCharacter::MoveRight);
	
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &AShooterCharacter::AddControllerYawInput);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &AShooterCharacter::AddControllerPitchInput);

	PlayerInputComponent->BindAction(TEXT("AimAt"), IE_Pressed, this, &AShooterCharacter::OnStartTargeting);
	PlayerInputComponent->BindAction(TEXT("AimAt"), IE_Released, this, &AShooterCharacter::OnStopTargeting);

	PlayerInputComponent->BindAction(TEXT("Fire"), IE_Pressed, this, &AShooterCharacter::OnStartFire);
	PlayerInputComponent->BindAction(TEXT("Fire"), IE_Released, this, &AShooterCharacter::OnStopFire);

	PlayerInputComponent->BindAction(TEXT("Reload"), IE_Pressed, this, &AShooterCharacter::OnReload);

	PlayerInputComponent->BindAction(TEXT("SwitchWeapon"), IE_Pressed, this, &AShooterCharacter::OnNextWeapon);

}

void AShooterCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	//改写，枪只在服务器产生，同步到客户端;Role定义在actor里面
	if (Role == ROLE_Authority)
	{
		SpawnDefaultInventory();
	}
}

bool AShooterCharacter::IsFirstPerson() const
{
	return IsAlive() && Controller != NULL && Controller->IsLocalPlayerController();
}

void AShooterCharacter::OnCameraUpdate(const FVector& CameraLocation, const FRotator& CameraRotation)
{
	USkeletalMeshComponent* DefaultMesh1P = Cast<USkeletalMeshComponent>(GetClass()->GetDefaultSubobjectByName(TEXT("PawnArm1p")));
	//构造第一人称Mesh相对于根组件的矩阵
	const FMatrix DefaultMeshLS = FRotationTranslationMatrix(DefaultMesh1P->RelativeRotation, DefaultMesh1P->RelativeLocation);
	//获取根组件到世界空间的变换矩阵
	const FMatrix LocalToWorld = ActorToWorld().ToMatrixWithScale();

	const FRotator RootCameraYaw(0.0f, CameraRotation.Yaw, 0.0f);
	const FRotator RootCameraPitch(CameraRotation.Pitch, 0.0f, 0.0f);

	//计算相机相对于根组件只带Yaw的矩阵
	const FMatrix LeveledCameraLS = FRotationTranslationMatrix(RootCameraYaw, CameraLocation) * LocalToWorld.Inverse();
	//计算相机相对于根组件带pitch和yaw的矩阵
	const FMatrix PitchedCameraLS = FRotationMatrix(RootCameraPitch) * LeveledCameraLS;

	//计算第一人称手臂相对于相机的矩阵
	const FMatrix MeshRelativeToCamera = DefaultMeshLS * LeveledCameraLS.Inverse();
	
	//计算第一人称手臂相对于根组件的矩阵
	const FMatrix PitchedMesh = MeshRelativeToCamera * PitchedCameraLS;

	//设置手臂相对于根组件的位置以及旋转设置
	FPArm->SetRelativeLocationAndRotation(PitchedMesh.GetOrigin(),PitchedMesh.Rotator());
}

void AShooterCharacter::MoveForward(float value)
{
	if (Controller != nullptr)
	{
		//设置角度限制，获取前进方向
		bool bLimitRotation = (GetCharacterMovement()->IsMovingOnGround()) || (GetCharacterMovement()->IsFalling());
		FRotator Rotation = bLimitRotation ? GetActorRotation() : Controller->GetControlRotation();
		FVector Direction = FRotationMatrix(Rotation).GetScaledAxis(EAxis::X);
		AddMovementInput(Direction, value);
	}
}

void AShooterCharacter::MoveRight(float value)
{
	if (Controller != nullptr)
	{
		bool bLimitRotation = (GetCharacterMovement()->IsMovingOnGround()) || (GetCharacterMovement()->IsFalling());
		FRotator Rotation = bLimitRotation ? GetActorRotation() : Controller->GetControlRotation();
		FVector Direction = FRotationMatrix(Rotation).GetScaledAxis(EAxis::Y);
		AddMovementInput(Direction, value);
	}
}

USkeletalMeshComponent* AShooterCharacter::GetSpecificPawnMesh(bool bWant1P) const
{
	return bWant1P ? FPArm : GetMesh();
}

FName AShooterCharacter::GetWeaponAttachPoint() const
{

	return WeaponAttachPoint;
}

FRotator AShooterCharacter::GetAimOffset() const
{
	FVector AimDirectionWS = GetBaseAimRotation().Vector();
	FVector AimDirectionLS = ActorToWorld().InverseTransformVectorNoScale(AimDirectionWS);
	return AimDirectionLS.Rotation();
}

void AShooterCharacter::OnStartTargeting()
{
	AShooterPlayerController* Player = Cast<AShooterPlayerController>(Controller);
	if (Player)
	{
		SetIsTargeting(true);
	}

}

void AShooterCharacter::OnStopTargeting()
{
	SetIsTargeting(false);
}

bool AShooterCharacter::GetIsTargeting() const
{
	return bIsTargeting;
}

void AShooterCharacter::SetIsTargeting(bool bNewIsTargeting)
{
	bIsTargeting = bNewIsTargeting;
}

void AShooterCharacter::OnStartFire()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->StartFire();
	}
}

void AShooterCharacter::OnStopFire()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->StopFire();
	}
}

float AShooterCharacter::TakeDamage(float Damage, FDamageEvent const & DamageEvent, AController * EventInstigator, AActor * DamageCauser)
{
	const float ActualDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
	if (ActualDamage > 0.0) {
		Health -= ActualDamage;
	}
	return ActualDamage;
}
int32 AShooterCharacter::GetCurrentHealth() const
{
	return Health;
}

int32 AShooterCharacter::GetMaxHealth() const
{
	return GetClass()->GetDefaultObject<AShooterCharacter>()->Health;
}

AShooterWeapon* AShooterCharacter::GetCurrentWeapon()
{
	return CurrentWeapon;
}

bool AShooterCharacter::CanFire() const
{ 
	bool CanFire = IsAlive();
	return CanFire;
}

bool AShooterCharacter::IsAlive() const
{
	return Health > 0;
}

void AShooterCharacter::OnReload()
{
	AShooterPlayerController* MyPC = Cast<AShooterPlayerController>(Controller);
	if (MyPC)
	{
		if (CurrentWeapon)
		{
			CurrentWeapon->StartReload();
		}
	}
}

void AShooterCharacter::SpawnDefaultInventory()
{
	//安全检测，防止本函数被其他地方调用
	if (Role < ROLE_Authority)
	{
		return;
	}

	int32 NumWeaponClasses = DefaultInventoryClass.Num();
	for (int i = 0; i < NumWeaponClasses; i++)
	{
		if (DefaultInventoryClass[i])
		{
			FActorSpawnParameters SpawnInfo;
			//不管情况都生成武器对象
			SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			AShooterWeapon* NewWeapon= GetWorld()->SpawnActor<AShooterWeapon>(DefaultInventoryClass[i],SpawnInfo);
			if (NewWeapon)
			{
				AddWeaponObject(NewWeapon);
			}
			
		}
	}
	
	//设置第一把武器为默认武器
	if (Inventory.Num() > 0)
	{
		CurrentWeapon = Inventory[0];
		CurrentWeapon->OnEquip(nullptr);
	}
}

void AShooterCharacter::AddWeaponObject(AShooterWeapon* weapon)
{
	if (weapon)
	{
		weapon->SetPawnOwner(this);
		Inventory.AddUnique(weapon);
	}
}

void AShooterCharacter::OnNextWeapon()
{
	//确保不是机器人
	AShooterPlayerController* MyPC = Cast<AShooterPlayerController>(Controller);
	if (MyPC)
	{
		//确保上一个装备动作还没完成
		if (Inventory.Num() > 0 && (CurrentWeapon->GetCurrentWeaponState() != EWeaponState::Equiping))
		{
			const int32 CurrentWeaponIndex = Inventory.IndexOfByKey(CurrentWeapon);
			const int32 NextWeaponIndex = (CurrentWeaponIndex+1) % Inventory.Num();
			AShooterWeapon* NewWeapon = Inventory[NextWeaponIndex];
			EquipWeapon(NewWeapon);
		}
	}
}

void AShooterCharacter::EquipWeapon(AShooterWeapon* weapon)
{
	if (weapon)
	{
		SetCurrentWeapon(weapon, CurrentWeapon);
	}
}

void AShooterCharacter::SetCurrentWeapon(AShooterWeapon* NewWeapon, AShooterWeapon* LastWeapon)
{
	//mesh摘除、状态归零
	AShooterWeapon* LocalLastWeapon = nullptr;
	if (LastWeapon != nullptr)
	{
		LocalLastWeapon = LastWeapon;
	}
	else if(NewWeapon!= CurrentWeapon)
	{
		LocalLastWeapon = CurrentWeapon;
	}

	if (LocalLastWeapon)
	{
		//卸载武器
		LocalLastWeapon->OnUnEquip();
	}
	CurrentWeapon = NewWeapon;
	if (NewWeapon)
	{
		NewWeapon->SetPawnOwner(this); 
		NewWeapon->OnEquip(LastWeapon);
	}
}

float AShooterCharacter::PlayAnimMontage(class UAnimMontage* AnimMontage, float InPlayRate, FName StartSectionName)
{
	USkeletalMeshComponent* UseMesh  = GetSpecificPawnMesh(true);
	if (UseMesh && AnimMontage && UseMesh->AnimScriptInstance)
	{
		//获取蒙太奇动画播放时间
		return UseMesh->AnimScriptInstance->Montage_Play(AnimMontage, InPlayRate);
	}
	else
	{
		return 0.0f;
	}
}

void AShooterCharacter::StopAnimMontage(class UAnimMontage* AnimMontage)
{
	USkeletalMeshComponent* UseMesh = GetSpecificPawnMesh(true);
	if (UseMesh && AnimMontage && UseMesh->AnimScriptInstance)
	{
		return UseMesh->AnimScriptInstance->Montage_Stop(AnimMontage->BlendOut.GetBlendTime(), AnimMontage);
	}

}

void AShooterCharacter::OnRep_CurrentWeapon(AShooterWeapon* LastWeapon)
{
	SetCurrentWeapon(CurrentWeapon, LastWeapon);
}

//获取生命周期同步属性，该函数由系统调用
void AShooterCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//收集改写的属性，属性会在所有的客户端连接里面同步
	DOREPLIFETIME(AShooterCharacter, CurrentWeapon);
}