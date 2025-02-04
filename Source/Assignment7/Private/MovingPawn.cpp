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
		//true-> �浹 �����Ͽ� ������� �̵�
		//MoveSpeed*DeltaSeconds�� �Ͽ� ������ ������ ����
		AddActorWorldOffset(ForwardVector * AxisValue * MoveSpeed * GetWorld()->GetDeltaSeconds(), true); 
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
		//true -> �浹 �����Ͽ� �������� �̵�
		//MoveSpeed*DeltaSeconds�� �Ͽ� ������ ������ ����
		AddActorWorldOffset(RightVector * AxisValue * MoveSpeed * GetWorld()->GetDeltaSeconds(), true);
	}
}

void AMovingPawn::Look(const FInputActionValue& Value)
{
	//�Է� �� ��ȯ (X: �¿�, Y: ����)
	FVector2D LookAxis = Value.Get<FVector2D>();

	//Yaw ȸ�� (�� ȸ��)
	FRotator YawRotation(0.0f, LookAxis.X * RotationSpeed * GetWorld()->GetDeltaSeconds(), 0.0f);
	AddActorLocalRotation(YawRotation);
	//������ �� ȸ��(Yaw,Pitch)
	FRotator SpringArmRotation = SpringArmComp->GetRelativeRotation();
	//RotationSpeed*DeltaSeconds�� �Ͽ� ������ ������ ����
	//Clamp�� ȸ�� ���� -80~80 ���� (Pitch: ���ϰ���)
	SpringArmRotation.Pitch = FMath::Clamp(SpringArmRotation.Pitch + LookAxis.Y * RotationSpeed * GetWorld()->GetDeltaSeconds(), -80.0f, 80.0f);
	SpringArmRotation.Yaw = 0.0f;
	//������ �� ȸ�� ����
	SpringArmComp->SetRelativeRotation(SpringArmRotation);
	//FRotator NewRotation = Controller->GetControlRotation();
	//RotationSpeed*DeltaSeconds�� �Ͽ� ������ ������ ����
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

