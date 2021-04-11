// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "TakeHitInfo.generated.h"

/**
 *
 */
 //�ռ�ͬ�����˺���Ϣ
USTRUCT()
struct FTakeHitInfo
{
	GENERATED_USTRUCT_BODY()

		UPROPERTY()
		float ActualDamage;

	//�����ж��˺�����
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
	//������չ
	UPROPERTY()
		FDamageEvent GeneralDamageEvent;

	//�Ը���洢���ܻᶪʧ��Ϣ�����������ౣ�������˺�����
	UPROPERTY()
		FPointDamageEvent PointDamageEvent;

	UPROPERTY()
		FRadialDamageEvent RadialDamageEvent;

public:
	FTakeHitInfo();

	FDamageEvent GetDamageEvent();

	//��damageevnt���浽�ṹ����
	void SetDamageEvent(const FDamageEvent& DamageEvent);
};
