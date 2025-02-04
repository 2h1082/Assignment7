// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "MovingPawn.generated.h"

class USpringArmComponent; //스프링 암 전방 선언
class UCameraComponent; //카메라 전방 선언
class USkeletalMeshComponent; //스켈레탈 메시 전방 선언
class UCapsuleComponent; //캡슐 컴포넌트 전방 선언
struct FInputActionValue; //Enhanced Input에서 액션 값 받을 때 사용할 구조체 전방 선언
UCLASS()
class ASSIGNMENT7_API AMovingPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AMovingPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//이동, 회전 함수
	UFUNCTION()
	void MoveForward(const FInputActionValue& Value);
	UFUNCTION()
	void MoveRight(const FInputActionValue& Value);
	UFUNCTION()
	void Look(const FInputActionValue& Value);

	//스프링 암
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	USpringArmComponent* SpringArmComp;
	//카메라
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* CameraComp;
	//스켈레탈 메시
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USkeletalMeshComponent* SkeletalMesh;
	//캡슐 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UCapsuleComponent* CapsuleComponent;

	//이동 속도, 회전 속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float MoveSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float RotationSpeed;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
