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

	//�Ե����˳�mesh�ɼ��ԣ�����Ͷ�估��ײ����
	GetMesh()->bOnlyOwnerSee = false;
	GetMesh()->bOwnerNoSee = true;
	GetMesh()->bReceivesDecals = false;
	GetMesh()->SetCollisionObjectType(ECC_Pawn);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetCollisionResponseToChannel(COLLISION_PICKUP, ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(COLLISION_WEAPON, ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(COLLISION_PROJECTILE, ECR_Block);

	//���������ײ���ã�ֻ���ӵ�������Ӧ
	GetCapsuleComponent()->SetCollisionResponseToChannel(COLLISION_PROJECTILE,ECR_Block);
	GetCapsuleComponent()->SetCollisionResponseToChannel(COLLISION_WEAPON, ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(COLLISION_PICKUP, ECR_Ignore);


	//������������ص��������,�̶������λ�ã���pawn�����������ת
	/*FPCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("PawnCamera1p"));
	FPCamera->SetupAttachment(GetCapsuleComponent());
	FPCamera->SetRelativeLocation(FVector(0.0f, 0.0f, BaseEyeHeight));
	FPCamera->bUsePawnControlRotation = true;
	*/
	//�����ֱ����������������
	FPArm = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("PawnArm1p"));
	FPArm->SetupAttachment(GetCapsuleComponent());
	//FPArm->SetupAttachment(FPCamera);
	FPArm->bOnlyOwnerSee = true;
	FPArm->bOwnerNoSee = false;
	FPArm->bReceivesDecals = false;
	FPArm->bCastDynamicShadow = false;
	//������������̬����ֻ��������Ⱦʱ��Ч�ʿ���
	FPArm->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::OnlyTickPoseWhenRendered;
	//�ѵ�һ�˳�mesh�ĸ��������õ�������������µ���
	FPArm->PrimaryComponentTick.TickGroup = TG_PrePhysics;
	//�ֱ���ײ���ã����ڵ����˳�Mesh�о������ײ���ã�ѡ��رյ�һ�˳�Mesh����ײ
	FPArm->SetCollisionObjectType(ECC_Pawn);
	FPArm->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	FPArm->SetCollisionResponseToAllChannels(ECR_Ignore);
	//�������λ��
	//FPArm->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f - 86.0f - BaseEyeHeight));

	//����Ĭ����׼�ر�
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

	//������Ϣ���Ӧ�����������
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
	//��д��ǹֻ�ڷ�����������ͬ�����ͻ���;Role������actor����
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
	//�����һ�˳�Mesh����ڸ�����ľ���
	const FMatrix DefaultMeshLS = FRotationTranslationMatrix(DefaultMesh1P->RelativeRotation, DefaultMesh1P->RelativeLocation);
	//��ȡ�����������ռ�ı任����
	const FMatrix LocalToWorld = ActorToWorld().ToMatrixWithScale();

	const FRotator RootCameraYaw(0.0f, CameraRotation.Yaw, 0.0f);
	const FRotator RootCameraPitch(CameraRotation.Pitch, 0.0f, 0.0f);

	//�����������ڸ����ֻ��Yaw�ľ���
	const FMatrix LeveledCameraLS = FRotationTranslationMatrix(RootCameraYaw, CameraLocation) * LocalToWorld.Inverse();
	//�����������ڸ������pitch��yaw�ľ���
	const FMatrix PitchedCameraLS = FRotationMatrix(RootCameraPitch) * LeveledCameraLS;

	//�����һ�˳��ֱ����������ľ���
	const FMatrix MeshRelativeToCamera = DefaultMeshLS * LeveledCameraLS.Inverse();
	
	//�����һ�˳��ֱ�����ڸ�����ľ���
	const FMatrix PitchedMesh = MeshRelativeToCamera * PitchedCameraLS;

	//�����ֱ�����ڸ������λ���Լ���ת����
	FPArm->SetRelativeLocationAndRotation(PitchedMesh.GetOrigin(),PitchedMesh.Rotator());
}

void AShooterCharacter::MoveForward(float value)
{
	if (Controller != nullptr)
	{
		//���ýǶ����ƣ���ȡǰ������
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
	//��ȫ��⣬��ֹ�������������ط�����
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
			//���������������������
			SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			AShooterWeapon* NewWeapon= GetWorld()->SpawnActor<AShooterWeapon>(DefaultInventoryClass[i],SpawnInfo);
			if (NewWeapon)
			{
				AddWeaponObject(NewWeapon);
			}
			
		}
	}
	
	//���õ�һ������ΪĬ������
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
	//ȷ�����ǻ�����
	AShooterPlayerController* MyPC = Cast<AShooterPlayerController>(Controller);
	if (MyPC)
	{
		//ȷ����һ��װ��������û���
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
	//meshժ����״̬����
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
		//ж������
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
		//��ȡ��̫�涯������ʱ��
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

//��ȡ��������ͬ�����ԣ��ú�����ϵͳ����
void AShooterCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//�ռ���д�����ԣ����Ի������еĿͻ�����������ͬ��
	DOREPLIFETIME(AShooterCharacter, CurrentWeapon);
}