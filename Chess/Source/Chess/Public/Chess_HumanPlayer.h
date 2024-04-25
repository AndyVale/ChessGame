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
	virtual void Tick(float DeltaTime) override;//TODO:maybe useless

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//Overrides from IChess_PlayerInterface
	virtual void OnTurn() override;
	virtual void OnWin() override;

	UFUNCTION()
	//Method to handle the click event
	void OnClick();

	UPROPERTY(Transient)
	//Reference to the selected piece if any
	AChessPiece* SelectedPiece;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	//Color of the player (work in progress)
	ChessColor PlayerColor = WHITE;
private:
	UFUNCTION()
	//Method to handle the reset event
	void ReplayHandler(int32 moveNumber);

	//List of active moves for the selected piece, used remember the possible moves of the selected piece
	TArray<TSharedPtr<Chess_Move>> ActiveMoves;
};
