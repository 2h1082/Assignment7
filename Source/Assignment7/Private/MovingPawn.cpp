// Fill out your copyright notice in the Description page of Project Settings.


#include "MovingPawn.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "MyPlayerController.h"

// Sets default values
AMovingPawn::AMovingPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	//캡슐 컴포넌트 생성
	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	CapsuleComponent->InitCapsuleSize(42.0f,96.0f);
	SetRootComponent(CapsuleComponent);

	//스켈레탈 메시 생성
	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	SkeletalMesh->SetupAttachment(RootComponent);

	//스프링 암 생성
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SprintArm"));
	//루트 컴포넌트에 부착
	SpringArmComp->SetupAttachment(RootComponent);
	//폰과 카메라 사이 거리 설정
	SpringArmComp->TargetArmLength = 300.0f;
	//컨트롤러 회전에 따라 스프링 암 같이 회전
	SpringArmComp->bUsePawnControlRotation = false;
	//Pawn의 Ptich 회전은 따라가지 않음
	SpringArmComp->bInheritPitch = false;
	//카메라 생성
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	//스프링 암의 소켓 위치에 부착
	CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);
	//카메라는 스프링 암과 같이 회전하니 PawnControlRotation Off
	CameraComp->bUsePawnControlRotation = false;

	MoveSpeed = 600.0f;
	RotationSpeed = 100.0f;
	
	//Pawn이 컨트롤러의 회전을 따르게 설정
	//bUseControllerRotationYaw = true;
}

// Called when the game starts or when spawned
void AMovingPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

void AMovingPawn::MoveForward(const FInputActionValue& Value)
{
	//입력 값 변환
	float AxisValue = Value.Get<float>(); // 1(정면), -1(후면)
	if (AxisValue != 0.0f)
	{
		//폰 정면 방향 벡터 획득
		FVector ForwardVector = GetActorForwardVector();
		//true-> 충돌 감안하여 정면방향 이동
		//MoveSpeed*DeltaSeconds를 하여 프레임 독립성 보장
		AddActorWorldOffset(ForwardVector * AxisValue * MoveSpeed * GetWorld()->GetDeltaSeconds(), true); 
	}
}

void AMovingPawn::MoveRight(const FInputActionValue& Value)
{
	//입력 값 변환
	float AxisValue = Value.Get<float>(); // 1(우측), -1(좌측)
	if (AxisValue != 0.0f)
	{
		//폰 우측 방향 벡터 획득
		FVector RightVector = GetActorRightVector();
		//true -> 충돌 감안하여 우측방향 이동
		//MoveSpeed*DeltaSeconds를 하여 프레임 독립성 보장
		AddActorWorldOffset(RightVector * AxisValue * MoveSpeed * GetWorld()->GetDeltaSeconds(), true);
	}
}

void AMovingPawn::Look(const FInputActionValue& Value)
{
	//입력 값 변환 (X: 좌우, Y: 상하)
	FVector2D LookAxis = Value.Get<FVector2D>();

	//Yaw 회전 (폰 회전)
	FRotator YawRotation(0.0f, LookAxis.X * RotationSpeed * GetWorld()->GetDeltaSeconds(), 0.0f);
	AddActorLocalRotation(YawRotation);
	//스프링 암 회전(Yaw,Pitch)
	FRotator SpringArmRotation = SpringArmComp->GetRelativeRotation();
	//RotationSpeed*DeltaSeconds를 하여 프레임 독립성 보장
	//Clamp로 회전 각도 -80~80 제한 (Pitch: 상하값만)
	SpringArmRotation.Pitch = FMath::Clamp(SpringArmRotation.Pitch + LookAxis.Y * RotationSpeed * GetWorld()->GetDeltaSeconds(), -80.0f, 80.0f);
	SpringArmRotation.Yaw = 0.0f;
	//스프링 암 회전 적용
	SpringArmComp->SetRelativeRotation(SpringArmRotation);
	//FRotator NewRotation = Controller->GetControlRotation();
	//RotationSpeed*DeltaSeconds를 하여 프레임 독립성 보장
	//NewRotation.Yaw += LookAxis.X * RotationSpeed * GetWorld()->GetDeltaSeconds();
	//Controller->SetControlRotation(NewRotation);
}

// Called every frame
void AMovingPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AMovingPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (AMyPlayerController* PlayerController = Cast<AMyPlayerController>(GetController()))
		{
			UE_LOG(LogTemp, Warning, TEXT("PlayerController Cast Success!"));
			if (PlayerController->MoveForwardAction)
			{
				UE_LOG(LogTemp, Warning, TEXT("MoveForwardAction binding Success!"));
				EnhancedInput->BindAction(
					PlayerController->MoveForwardAction,
					ETriggerEvent::Triggered,
					this,
					&AMovingPawn::MoveForward);
			}
			if (PlayerController->MoveRightAction)
			{
				UE_LOG(LogTemp, Warning, TEXT("MoveRightAction binding Success!"));
				EnhancedInput->BindAction(
					PlayerController->MoveRightAction,
					ETriggerEvent::Triggered,
					this,
					&AMovingPawn::MoveRight);
			}
			if (PlayerController->LookAction)
			{
				UE_LOG(LogTemp, Warning, TEXT("LookAction binding Success!"));
				EnhancedInput->BindAction(
					PlayerController->LookAction,
					ETriggerEvent::Triggered,
					this,
					&AMovingPawn::Look);
			}
		}
	}
}

