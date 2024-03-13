// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ChessPiece.h"
#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Chess_PlayerInterface.h"
#include "Chess_GameMode.h"
//#include "MyGameInstance.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Chess_HumanPlayer.generated.h"

UCLASS()
class CHESS_API AChess_HumanPlayer : public APawn, public IChess_PlayerInterface
{
	GENERATED_BODY()
public:
	// Sets default values for this pawn's properties
	AChess_HumanPlayer();
	UCameraComponent* Camera;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void OnTurn() override;
	virtual void OnWin() override;
	virtual void OnLose() override;

	UFUNCTION()
	void OnClick();

	UPROPERTY(Transient)
	AChessPiece* SelectedPiece;

	//UScacchiGameInstance* GameInstance
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	bool bIsMyTurn = false;
	ChessColor MyColor = WHITE;
private:


};
