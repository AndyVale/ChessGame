// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Chessboard.h"
#include "GameFramework/Pawn.h"
#include "Chess_PlayerInterface.h"
#include "Chess_RandomPlayer.generated.h"

UCLASS()
class CHESS_API AChess_RandomPlayer : public APawn, public IChess_PlayerInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AChess_RandomPlayer();
	void MakeRandomMove();

	UFUNCTION()//ResetHandler and bMyTurn are workaround, remove it before last commit 
	void ResetHandler();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void OnTurn() override;
	virtual void OnWin() override;

private:
};
