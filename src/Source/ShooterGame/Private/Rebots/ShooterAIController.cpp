// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "ShooterAIController.h"
#include "ShooterCharacter.h"
#include "ShooterBot.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"

AShooterAIController::AShooterAIController()
{
	BehaviorComp = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorComp"));
	BlackboardComp= CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComp"));

	EnemyKeyID = 0;
}

void AShooterAIController::Possess(APawn* InPawn)
{
	//启动行为树，智能化机器人
	Super::Possess(InPawn);

	AShooterBot* Bot = Cast<AShooterBot>(InPawn);
	if (Bot && Bot->BotBehavior)
	{
		if (Bot->BotBehavior->BlackboardAsset)
		{
			BlackboardComp->InitializeBlackboard(*(Bot->BotBehavior->BlackboardAsset));
		}
		EnemyKeyID = BlackboardComp->GetKeyID("Enemy");
		BehaviorComp->StartTree(*Bot->BotBehavior);
	}
	

}
void AShooterAIController::UnPossess()
{
	Super::UnPossess();
	BehaviorComp->StopTree();

}

void AShooterAIController::FindClosestEnemyWithLOS(AShooterCharacter* ExcludeEnemy)
{
	APawn* MyBot = GetPawn();
	if (MyBot)
	{
		const FVector MyLoc = MyBot->GetActorLocation();
		float BestDistSq = MAX_FLT;
		AShooterCharacter* BestPawn = nullptr;
		//遍历所有character,找到距离AI最近且可见的敌人
		for (FConstPawnIterator It = GetWorld()->GetPawnIterator(); It; It++)
		{
			AShooterCharacter* TestPawn = Cast<AShooterCharacter>(*It);
			if (TestPawn && TestPawn != ExcludeEnemy)
			{
				if (HasWeaponLOSToEnemy(TestPawn, true))
				{
					const float DistSq = (TestPawn->GetActorLocation() - MyLoc).SizeSquared();
					if (DistSq < BestDistSq)
					{
						BestDistSq = DistSq;
						BestPawn = TestPawn;
					}
				}
			}
		}
		//将敌人记录到黑板中
		if (BestPawn)
		{
			SetEnemy(BestPawn);
		}
	}
}

//判断敌人是否可见
bool AShooterAIController::HasWeaponLOSToEnemy(AActor* InEnemyActor, const bool bAnyEnemy) const
{
	bool bHasLOS = false;
	static FName LosTag = FName(TEXT("AIWeaponLosTrace"));
	FCollisionQueryParams TraceParams(LosTag, true, GetPawn());
	TraceParams.bTraceAsyncScene = true;
	TraceParams.bReturnPhysicalMaterial = true;

	AShooterBot* MyBot = Cast<AShooterBot>(GetPawn());
	FVector StartLocation = MyBot->GetActorLocation();
	StartLocation.Z += MyBot->BaseEyeHeight;

	const FVector EndLocation = InEnemyActor->GetActorLocation();
	FHitResult Hit(ForceInit);
	//利用射线求交，跟只响应Weapon碰撞通道的对象求交
	GetWorld()->LineTraceSingleByChannel(Hit, StartLocation, EndLocation, COLLISION_WEAPON, TraceParams);
	if (Hit.bBlockingHit)
	{
		AActor* HitActor = Hit.GetActor();
		if (HitActor)
		{
			if (HitActor == InEnemyActor)
			{
				bHasLOS = true;
			}
		}
	}
	return bHasLOS;
}


void AShooterAIController::ShootEnemy()
{
	AShooterBot* MyBot = Cast<AShooterBot>(GetPawn());
	if (MyBot == nullptr)
	{
		return;
	}
	bool bCanShoot = false;
	AShooterCharacter* Enemy = GetEnemy();
	if (Enemy /*&& Enemy->GetCurrentHealth() > 0*/)
	{
		//防止敌人已走开
		if (LineOfSightTo(Enemy, MyBot->GetActorLocation()))
		{
			bCanShoot = true;
		}
	}
	
	if (bCanShoot)
	{
		MyBot -> OnStartFire();
	}
	else
	{
		MyBot -> OnStopFire();
	}
}

class AShooterCharacter* AShooterAIController::GetEnemy()
{
	if (BlackboardComp)
	{
		return Cast<AShooterCharacter>(BlackboardComp->GetValue<UBlackboardKeyType_Object>(EnemyKeyID));

	}
	return nullptr;
}

void AShooterAIController::SetEnemy(class APawn* InPawn)
{
	if (BlackboardComp)
	{
		BlackboardComp->SetValue<UBlackboardKeyType_Object>(EnemyKeyID, InPawn);
		//控制焦点在敌人身上
		SetFocus(InPawn);
	}
}

