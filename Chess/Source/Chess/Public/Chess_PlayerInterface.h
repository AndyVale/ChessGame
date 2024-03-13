// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Chess_PlayerInterface.generated.h"

// This class does not need to be modified.
UENUM()
enum class PlayerColor : uint8
{
	WHITE,
	BLACK
};

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
	PlayerColor Color;
	virtual void OnTurn() {};
	virtual void OnWin() {};
	virtual void OnLose() {};
};