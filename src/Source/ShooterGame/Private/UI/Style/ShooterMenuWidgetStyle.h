// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "SlateWidgetStyleContainerBase.h"
#include "ShooterMenuWidgetStyle.generated.h"

/**
 * 
 */

//FShooterMenuStyle:����SlateBrush����ͼƬ��Դ���Ա��ڻ�ȡ����¶���༭��
USTRUCT()
struct FShooterMenuStyle :public FSlateWidgetStyle
{
	GENERATED_USTRUCT_BODY()

	FShooterMenuStyle();
	virtual ~FShooterMenuStyle();

	virtual void GetResources(TArray<const FSlateBrush *> & OutBrushes) const override;

	static const FName TypeName;
	
	virtual const FName GetTypeName() const override { return TypeName;}

	static const FShooterMenuStyle& GetDefault();

	//ͼƬ��ʽ��Դ��
	UPROPERTY(EditAnywhere, Category = "Appearance")
	FSlateBrush HeaderBackgroundBrush;
	FShooterMenuStyle& SetHeaderBackgroundBrush(FSlateBrush& InHeaderBackgroundBrush) { HeaderBackgroundBrush = InHeaderBackgroundBrush; }

	UPROPERTY(EditAnywhere, Category = "Appearance")
	FSlateBrush LeftBackgroundBrush;
	FShooterMenuStyle& SetLeftackgroundBrush(FSlateBrush& InLeftBackgroundBrush) { LeftBackgroundBrush = InLeftBackgroundBrush; }

	UPROPERTY(EditAnywhere, Category = "Appearance")
	FSlateBrush RightBackgroundBrush;
	FShooterMenuStyle& SetRightBackgroundBrush(FSlateBrush& InRightBackgroundBrush) { RightBackgroundBrush = InRightBackgroundBrush; }
};


//��ʵ�����ݵķ�װ�����࣬������ͼ��չ���ಢ�༭���á�
//��һ����װ��������ṹ�屩¶�ڱ༭�����Ӷ����Ա༭���á�
//hidecategories����ֻ��¶�ṹ��MenuStyle,����Object���Ա������������ MinimalAPI�����ٷ�����Ϣ���ռ�
UCLASS(hidecategories = Object, MinimalAPI)
class UShooterMenuWidgetStyle : public USlateWidgetStyleContainerBase
{
	GENERATED_UCLASS_BODY()

public:
	~UShooterMenuWidgetStyle();

	UPROPERTY(EditAnywhere, Category = "Appearance", meta = (ShowOnlyInnerProperties))
	FShooterMenuStyle MenuStyle;

	virtual const struct FSlateWidgetStyle* const GetStyle() const override
	{
		return static_cast<const struct FSlateWidgetStyle*>(&MenuStyle);
	}
};
