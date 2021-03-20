// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AIController.h"
#include "ShooterAIController.generated.h"

/**
 * 
 */
//class AShooterCharacter;
class UBehaviorTreeComponent;
class UBlackboardComponent;

UCLASS()
class SHOOTERGAME_API AShooterAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	AShooterAIController();

	virtual void Possess(APawn* InPawn) override;
	virtual void UnPossess() override;


	//Line Of Sight�����ߣ����ų�ĳЩ����
	UFUNCTION(BlueprintCallable, Category = "Behavior")
	void FindClosestEnemyWithLOS(AShooterCharacter* ExcludeEnemy);
	
	//�������Ƿ�ɼ�
	bool HasWeaponLOSToEnemy(AActor* InEnemyActor, const bool bAnyEnemy) const;

	UFUNCTION(BlueprintCallable, Category = "Behavior")
	void ShootEnemy();

	class AShooterCharacter* GetEnemy();

	void SetEnemy(class APawn* InPawn);

private:
	UBehaviorTreeComponent* BehaviorComp;
	UBlackboardComponent* BlackboardComp;

	int32 EnemyKeyID;

};
