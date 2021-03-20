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
}

void AShooterCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	FActorSpawnParameters SpawnInfo;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	CurrentWeapon = GetWorld()->SpawnActor<AShooterWeapon>(ShooterWeaponClass, SpawnInfo);

	if (CurrentWeapon)
	{
		CurrentWeapon->SetPawnOwner(this);
		CurrentWeapon->AttachMeshToPawn();
	}
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

USkeletalMeshComponent* AShooterCharacter::GetFirstPersonMesh()
{
	return FPArm;
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
	//todo
}

float AShooterCharacter::TakeDamage(float Damage, FDamageEvent const & DamageEvent, AController * EventInstigator, AActor * DamageCauser)
{
	const float ActualDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
	if (ActualDamage > 0.0) {
		Health -= ActualDamage;
	}
	return ActualDamage;
}
float AShooterCharacter::GetCurrentHealth()
{
	return Health;
}

AShooterWeapon* AShooterCharacter::GetCurrentWeapon()
{
	return CurrentWeapon;
}
