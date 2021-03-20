// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "BTTask_FindPointNearEnemy.h"
#include "ShooterAIController.h"
#include "ShooterCharacter.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"

EBTNodeResult::Type UBTTask_FindPointNearEnemy::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AShooterAIController* MyController = Cast<AShooterAIController>(OwnerComp.GetOwner());
	if (MyController == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	APawn* MyBot = MyController->GetPawn();
	AShooterCharacter* Enemy = MyController->GetEnemy();
	if (MyBot&&Enemy)
	{
		const FVector SearchOrigin = Enemy->GetActorLocation() + 600.0f * (MyBot->GetActorLocation() - Enemy->GetActorLocation()).SafeNormal();
		const float SearchRadius = 200.0f;
		//在敌人周围寻找一个随机攻击位置，但不能太靠近敌人
		const FVector SearchResult = UNavigationSystem::GetRandomReachablePointInRadius(MyController, SearchOrigin, SearchRadius);
		if (SearchResult != FVector::ZeroVector)
		{
			OwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Vector>(BlackboardKey.GetSelectedKeyID(), SearchResult);
			return EBTNodeResult::Succeeded;
		}
	}
	return EBTNodeResult::Failed;
}




