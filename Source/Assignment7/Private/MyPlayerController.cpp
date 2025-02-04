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

	//현재 PlayerController에 연결된 Local Player 객체 획득
	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		//Loca Player에서 EnhancedInputLocalPlayerSubsystem 획득
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			if (InputMappingContext)
			{
				//Subsystem으로 우리가 할당한 IMC 활성화
				//우선순위도 가장 높게
				Subsystem->AddMappingContext(InputMappingContext, 0);
			}
		}
	}
}


