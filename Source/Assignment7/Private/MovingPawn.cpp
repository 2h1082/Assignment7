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
	PrimaryActorTick.bCanEverTick = true;

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
	RollSpeed = 100.0f;
	Gravity = -980.0f;
	VerticalVelocity = 0.0f;
	AirControlFactor = 0.5f;
	bIsOnGround = true;
	
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
		float SpeedMultiplier = bIsOnGround ? 1.0f : AirControlFactor;

		//true-> 충돌 감안하여 정면방향 이동
		//MoveSpeed*DeltaSeconds를 하여 프레임 독립성 보장
		AddActorWorldOffset(ForwardVector * AxisValue * MoveSpeed * SpeedMultiplier * GetWorld()->GetDeltaSeconds(), true); 
		//상승 중에는 중력의 영향을 안 받도록 중력 관련 속도를 이동 속도로 고정
		VerticalVelocity = MoveSpeed * ForwardVector.Z * AxisValue;
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
		float SpeedMultiplier = bIsOnGround ? 1.0f : AirControlFactor;

		//true -> 충돌 감안하여 우측방향 이동
		//MoveSpeed*DeltaSeconds를 하여 프레임 독립성 보장
		AddActorWorldOffset(RightVector * AxisValue * MoveSpeed * SpeedMultiplier * GetWorld()->GetDeltaSeconds(), true);
		//상승 중에는 중력의 영향을 안 받도록 중력 관련 속도를 이동 속도로 고정
		VerticalVelocity = MoveSpeed * RightVector.Z * AxisValue;
	}
}

void AMovingPawn::MoveUp(const FInputActionValue& Value)
{
	//입력 값 변환
	float AxisValue = Value.Get<float>(); // 1(우측), -1(좌측)
	if (AxisValue != 0.0f)
	{
		//폰 위쪽 방향 벡터 획득
		FVector UpVector = GetActorUpVector();
		float SpeedMultiplier = bIsOnGround ? 1.0f : AirControlFactor;

		//true -> 충돌 감안하여 우측방향 이동
		//MoveSpeed*DeltaSeconds를 하여 프레임 독립성 보장
		AddActorWorldOffset(UpVector * AxisValue * MoveSpeed * SpeedMultiplier * GetWorld()->GetDeltaSeconds(), true);
		//상승 중에는 중력의 영향을 안 받도록 중력 관련 속도를 이동 속도로 고정
		VerticalVelocity = MoveSpeed*UpVector.Z*AxisValue;
	}
}

void AMovingPawn::Look(const FInputActionValue& Value)
{
	//입력 값 변환 (X: 좌우, Y: 상하)
	FVector2D LookAxis = Value.Get<FVector2D>();

	//Yaw 회전 (폰 회전)
	FRotator YawRotation(0.0f, LookAxis.X * RotationSpeed * GetWorld()->GetDeltaSeconds(), 0.0f);
	AddActorLocalRotation(YawRotation);

	//Pitch 회전 (폰 회전)
	FRotator PitchRotation(LookAxis.Y * RotationSpeed * GetWorld()->GetDeltaSeconds(), 0.0f, 0.0f);
	AddActorLocalRotation(PitchRotation);

	//아래 주석은 필수 과제 부분에 해당하는 Pitch 회전 (폰은 그대로, 스프링 암이 회전해서 상하 카메라 시점 회전)
	/* 
	//스프링 암 회전(Yaw,Pitch)
	FRotator SpringArmRotation = SpringArmComp->GetRelativeRotation();
	//RotationSpeed*DeltaSeconds를 하여 프레임 독립성 보장
	//Clamp로 회전 각도 -80~80 제한 (Pitch: 상하값만)
	SpringArmRotation.Pitch = FMath::Clamp(SpringArmRotation.Pitch + LookAxis.Y * RotationSpeed * GetWorld()->GetDeltaSeconds(), -80.0f, 80.0f);
	SpringArmRotation.Yaw = 0.0f;
	//스프링 암 회전 적용
	SpringArmComp->SetRelativeRotation(SpringArmRotation);
	*/
}

void AMovingPawn::Roll(const FInputActionValue& Value)
{
	float AxisValue = Value.Get<float>();
	if (AxisValue != 0.0f)
	{
		FRotator RollRotation(0.0f, 0.0f, AxisValue * RollSpeed * GetWorld()->GetDeltaSeconds());
		AddActorLocalRotation(RollRotation);
	}
}

void AMovingPawn::ApplyGravity(float DeltaTime)
{
	bIsOnGround = IsOnGround();

	// 지면 여부 확인
	if (bIsOnGround)
	{
		VerticalVelocity = 0.0f; // 착지 시 속도 초기화
	}
	else
	{
		//상승 중이면 약하게 적용
		float GravityValue = (VerticalVelocity > 0) ? Gravity * 0.3f : Gravity;
		VerticalVelocity += GravityValue * DeltaTime; // 중력 가속도 적용
	}

	//공중이고 상승 중이지 않으면 중력 적용
	if (!bIsOnGround)
	{
		//벡터 계산
		FVector FallVector = FVector(0.0f, 0.0f, VerticalVelocity * DeltaTime);
		//이동 적용
		AddActorWorldOffset(FallVector, true);
	}
}

bool AMovingPawn::IsOnGround() const
{
	//충돌 감지 관련 변수 선언
	FHitResult Hit;
	FVector Start = GetActorLocation() + FVector(CapsuleComponent->GetUnscaledCapsuleHalfHeight());
	FVector End = GetActorLocation() - FVector(0.0f, 0.0f, CapsuleComponent->GetUnscaledCapsuleHalfHeight()); //충돌 감지할 범위 설정
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	//Sweep으로 지면 충돌 확인
	bool bHitGround = GetWorld()->SweepSingleByChannel(
		Hit,
		Start,
		End,
		FQuat::Identity,
		ECC_Visibility,
		FCollisionShape::MakeCapsule(1.0f,0.1f),
		QueryParams
	);
	return bHitGround && (VerticalVelocity <= 0.0f);
}

// Called every frame
void AMovingPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//중력 적용
	ApplyGravity(DeltaTime);
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
			if (PlayerController->MoveUpAction)
			{
				UE_LOG(LogTemp, Warning, TEXT("MoveUpAction binding Success!"));
				EnhancedInput->BindAction(
					PlayerController->MoveUpAction,
					ETriggerEvent::Triggered,
					this,
					&AMovingPawn::MoveUp);
			}
			if (PlayerController->RollAction)
			{
				UE_LOG(LogTemp, Warning, TEXT("RollAction binding Success!"));
				EnhancedInput->BindAction(
					PlayerController->RollAction,
					ETriggerEvent::Triggered,
					this,
					&AMovingPawn::Roll);
			}
		}
	}
}

