// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include <Chess_GameInstance.h>
#include <Kismet/GameplayStatics.h>
#include "Chess_PlayerInterface.generated.h"



UINTERFACE(MinimalAPI)
class UChess_PlayerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 *
 */
class CHESS_API IChess_PlayerInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	ChessColor PlayerColor;
	UChess_GameInstance* GameInstanceRef;
	bool bIsMyTurn;//here to avoid "reset-event-problems"

	virtual void OnTurn() {};
	virtual void OnWin() {};
};
