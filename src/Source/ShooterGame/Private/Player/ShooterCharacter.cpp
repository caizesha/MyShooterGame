// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "ShooterCharacter.h"
#include "shooterWeapon.h"

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
	FPCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("PawnCamera1p"));
	FPCamera->SetupAttachment(GetCapsuleComponent());
	FPCamera->SetRelativeLocation(FVector(0.0f, 0.0f, BaseEyeHeight));
	FPCamera->bUsePawnControlRotation = true;

	//创建手臂组件，并进行设置
	FPArm = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("PawnArm1p"));
	FPArm->SetupAttachment(FPCamera);
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
	FPArm->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f - 86.0f - BaseEyeHeight));
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

USkeletalMeshComponent* AShooterCharacter::GetFirstPersonMesh()
{
	return FPArm;
}

FName AShooterCharacter::GetWeaponAttachPoint() const
{

	return WeaponAttachPoint;
}
