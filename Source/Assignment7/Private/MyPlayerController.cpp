// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayerController.h"
#include "EnhancedInputSubsystems.h"

AMyPlayerController::AMyPlayerController() :
	InputMappingContext(nullptr),
	MoveForwardAction(nullptr),
	MoveRightAction(nullptr),
	LookAction(nullptr)
{}

void AMyPlayerController::BeginPlay()
{
	Super::BeginPlay();

	//���� PlayerController�� ����� Local Player ��ü ȹ��
	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		//Loca Player���� EnhancedInputLocalPlayerSubsystem ȹ��
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			if (InputMappingContext)
			{
				//Subsystem���� �츮�� �Ҵ��� IMC Ȱ��ȭ
				//�켱������ ���� ����
				Subsystem->AddMappingContext(InputMappingContext, 0);
			}
		}
	}
}


