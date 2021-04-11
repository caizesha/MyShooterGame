// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "TakeHitInfo.generated.h"

/**
 *
 */
 //收集同步的伤害信息
USTRUCT()
struct FTakeHitInfo
{
	GENERATED_USTRUCT_BODY()

		UPROPERTY()
		float ActualDamage;

	//用于判断伤害类型
	UPROPERTY()
		UClass* DamageTypeClass;

	UPROPERTY()
		TWeakObjectPtr<class APawn> PawnInstigator;

	UPROPERTY()
		TWeakObjectPtr<class AActor> DamageCauser;

	UPROPERTY()
		int32 DamageEventClassID;

	UPROPERTY()
		bool bKilled;

private:
	//用于拓展
	UPROPERTY()
		FDamageEvent GeneralDamageEvent;

	//以父类存储可能会丢失信息，所以用子类保存具体的伤害类型
	UPROPERTY()
		FPointDamageEvent PointDamageEvent;

	UPROPERTY()
		FRadialDamageEvent RadialDamageEvent;

public:
	FTakeHitInfo();

	FDamageEvent GetDamageEvent();

	//将damageevnt保存到结构体中
	void SetDamageEvent(const FDamageEvent& DamageEvent);
};
