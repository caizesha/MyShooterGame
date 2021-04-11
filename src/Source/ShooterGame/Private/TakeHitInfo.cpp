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
		//��ֹ��Ϣ��ʧ
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
	//����ID�жϾ����˺����ͣ�������
	DamageEventClassID = DamageEvent.GetTypeID();
	switch (DamageEventClassID)
	{
	case FPointDamageEvent::ClassID:
		PointDamageEvent = *(FPointDamageEvent*)(&DamageEvent);//ǿ������ת������ֹ���ำֵ������ʱ��Ϣ��ʧ��ֻʣ������Ϣ
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
