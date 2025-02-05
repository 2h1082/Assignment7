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

	//ĸ�� ������Ʈ ����
	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	CapsuleComponent->InitCapsuleSize(42.0f,96.0f);
	SetRootComponent(CapsuleComponent);

	//���̷�Ż �޽� ����
	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	SkeletalMesh->SetupAttachment(RootComponent);

	//������ �� ����
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SprintArm"));
	//��Ʈ ������Ʈ�� ����
	SpringArmComp->SetupAttachment(RootComponent);
	//���� ī�޶� ���� �Ÿ� ����
	SpringArmComp->TargetArmLength = 300.0f;
	//��Ʈ�ѷ� ȸ���� ���� ������ �� ���� ȸ��
	SpringArmComp->bUsePawnControlRotation = false;
	//Pawn�� Ptich ȸ���� ������ ����
	SpringArmComp->bInheritPitch = false;
	//ī�޶� ����
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	//������ ���� ���� ��ġ�� ����
	CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);
	//ī�޶�� ������ �ϰ� ���� ȸ���ϴ� PawnControlRotation Off
	CameraComp->bUsePawnControlRotation = false;

	MoveSpeed = 600.0f;
	RotationSpeed = 100.0f;
	RollSpeed = 100.0f;
	Gravity = -980.0f;
	VerticalVelocity = 0.0f;
	AirControlFactor = 0.5f;
	bIsOnGround = true;
	
	//Pawn�� ��Ʈ�ѷ��� ȸ���� ������ ����
	//bUseControllerRotationYaw = true;
}

// Called when the game starts or when spawned
void AMovingPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

void AMovingPawn::MoveForward(const FInputActionValue& Value)
{
	//�Է� �� ��ȯ
	float AxisValue = Value.Get<float>(); // 1(����), -1(�ĸ�)
	if (AxisValue != 0.0f)
	{
		//�� ���� ���� ���� ȹ��
		FVector ForwardVector = GetActorForwardVector();
		float SpeedMultiplier = bIsOnGround ? 1.0f : AirControlFactor;

		//true-> �浹 �����Ͽ� ������� �̵�
		//MoveSpeed*DeltaSeconds�� �Ͽ� ������ ������ ����
		AddActorWorldOffset(ForwardVector * AxisValue * MoveSpeed * SpeedMultiplier * GetWorld()->GetDeltaSeconds(), true); 
		//��� �߿��� �߷��� ������ �� �޵��� �߷� ���� �ӵ��� �̵� �ӵ��� ����
		VerticalVelocity = MoveSpeed * ForwardVector.Z * AxisValue;
	}
}

void AMovingPawn::MoveRight(const FInputActionValue& Value)
{
	//�Է� �� ��ȯ
	float AxisValue = Value.Get<float>(); // 1(����), -1(����)
	if (AxisValue != 0.0f)
	{
		//�� ���� ���� ���� ȹ��
		FVector RightVector = GetActorRightVector();
		float SpeedMultiplier = bIsOnGround ? 1.0f : AirControlFactor;

		//true -> �浹 �����Ͽ� �������� �̵�
		//MoveSpeed*DeltaSeconds�� �Ͽ� ������ ������ ����
		AddActorWorldOffset(RightVector * AxisValue * MoveSpeed * SpeedMultiplier * GetWorld()->GetDeltaSeconds(), true);
		//��� �߿��� �߷��� ������ �� �޵��� �߷� ���� �ӵ��� �̵� �ӵ��� ����
		VerticalVelocity = MoveSpeed * RightVector.Z * AxisValue;
	}
}

void AMovingPawn::MoveUp(const FInputActionValue& Value)
{
	//�Է� �� ��ȯ
	float AxisValue = Value.Get<float>(); // 1(����), -1(����)
	if (AxisValue != 0.0f)
	{
		//�� ���� ���� ���� ȹ��
		FVector UpVector = GetActorUpVector();
		float SpeedMultiplier = bIsOnGround ? 1.0f : AirControlFactor;

		//true -> �浹 �����Ͽ� �������� �̵�
		//MoveSpeed*DeltaSeconds�� �Ͽ� ������ ������ ����
		AddActorWorldOffset(UpVector * AxisValue * MoveSpeed * SpeedMultiplier * GetWorld()->GetDeltaSeconds(), true);
		//��� �߿��� �߷��� ������ �� �޵��� �߷� ���� �ӵ��� �̵� �ӵ��� ����
		VerticalVelocity = MoveSpeed*UpVector.Z*AxisValue;
	}
}

void AMovingPawn::Look(const FInputActionValue& Value)
{
	//�Է� �� ��ȯ (X: �¿�, Y: ����)
	FVector2D LookAxis = Value.Get<FVector2D>();

	//Yaw ȸ�� (�� ȸ��)
	FRotator YawRotation(0.0f, LookAxis.X * RotationSpeed * GetWorld()->GetDeltaSeconds(), 0.0f);
	AddActorLocalRotation(YawRotation);

	//Pitch ȸ�� (�� ȸ��)
	FRotator PitchRotation(LookAxis.Y * RotationSpeed * GetWorld()->GetDeltaSeconds(), 0.0f, 0.0f);
	AddActorLocalRotation(PitchRotation);

	//�Ʒ� �ּ��� �ʼ� ���� �κп� �ش��ϴ� Pitch ȸ�� (���� �״��, ������ ���� ȸ���ؼ� ���� ī�޶� ���� ȸ��)
	/* 
	//������ �� ȸ��(Yaw,Pitch)
	FRotator SpringArmRotation = SpringArmComp->GetRelativeRotation();
	//RotationSpeed*DeltaSeconds�� �Ͽ� ������ ������ ����
	//Clamp�� ȸ�� ���� -80~80 ���� (Pitch: ���ϰ���)
	SpringArmRotation.Pitch = FMath::Clamp(SpringArmRotation.Pitch + LookAxis.Y * RotationSpeed * GetWorld()->GetDeltaSeconds(), -80.0f, 80.0f);
	SpringArmRotation.Yaw = 0.0f;
	//������ �� ȸ�� ����
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

	// ���� ���� Ȯ��
	if (bIsOnGround)
	{
		VerticalVelocity = 0.0f; // ���� �� �ӵ� �ʱ�ȭ
	}
	else
	{
		//��� ���̸� ���ϰ� ����
		float GravityValue = (VerticalVelocity > 0) ? Gravity * 0.3f : Gravity;
		VerticalVelocity += GravityValue * DeltaTime; // �߷� ���ӵ� ����
	}

	//�����̰� ��� ������ ������ �߷� ����
	if (!bIsOnGround)
	{
		//���� ���
		FVector FallVector = FVector(0.0f, 0.0f, VerticalVelocity * DeltaTime);
		//�̵� ����
		AddActorWorldOffset(FallVector, true);
	}
}

bool AMovingPawn::IsOnGround() const
{
	//�浹 ���� ���� ���� ����
	FHitResult Hit;
	FVector Start = GetActorLocation() + FVector(CapsuleComponent->GetUnscaledCapsuleHalfHeight());
	FVector End = GetActorLocation() - FVector(0.0f, 0.0f, CapsuleComponent->GetUnscaledCapsuleHalfHeight()); //�浹 ������ ���� ����
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	//Sweep���� ���� �浹 Ȯ��
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
	//�߷� ����
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

