// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "TakeHitInfo.h"
#include "ShooterCharacter.h"

FTakeHitInfo::FTakeHitInfo()
	:ActualDamage(0),
	DamageTypeClass(NULL),
	PawnInstigator(NULL),
	DamageCauser(NULL),
	DamageEventClassID(0),
	bKilled(false)
{

}

FDamageEvent FTakeHitInfo::GetDamageEvent()
{
	switch (DamageEventClassID)
	{
	case FPointDamageEvent::ClassID:
		//防止信息丢失
		if (PointDamageEvent.DamageTypeClass == NULL)
		{
			PointDamageEvent.DamageTypeClass = DamageTypeClass ? DamageTypeClass : UDamageType::StaticClass();
		}
		return PointDamageEvent;
	case FRadialDamageEvent::ClassID:
		if (RadialDamageEvent.DamageTypeClass == NULL)
		{
			RadialDamageEvent.DamageTypeClass = DamageTypeClass ? DamageTypeClass : UDamageType::StaticClass();
		}
		return RadialDamageEvent;
	default:
		if (GeneralDamageEvent.DamageTypeClass == NULL)
		{
			GeneralDamageEvent.DamageTypeClass = DamageTypeClass ? DamageTypeClass : UDamageType::StaticClass();
		}
		return GeneralDamageEvent;
	}
}

void FTakeHitInfo::SetDamageEvent(const FDamageEvent& DamageEvent)
{
	//根据ID判断具体伤害类型，并保存
	DamageEventClassID = DamageEvent.GetTypeID();
	switch (DamageEventClassID)
	{
	case FPointDamageEvent::ClassID:
		PointDamageEvent = *(FPointDamageEvent*)(&DamageEvent);//强制类型转换，防止父类赋值到子类时信息丢失，只剩父类信息
		break;
	case  FRadialDamageEvent::ClassID:
		RadialDamageEvent = *(FRadialDamageEvent*)(&DamageEvent);
		break;
	default:
		GeneralDamageEvent = DamageEvent;
		break;
	}

	DamageTypeClass = DamageEvent.DamageTypeClass;
}
