// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Chess_PlayerController.h"
#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Chess_HumanPlayer.h"
#include "InputActionValue.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

AChess_PlayerController::AChess_PlayerController()
{
	bShowMouseCursor = true;
	bEnableClickEvents = true;
}


void AChess_PlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		if (ChessContext) {
			Subsystem->AddMappingContext(ChessContext, 0);
		}
	}
}

void AChess_PlayerController::ClickOnGrid()
{
	const auto HumanPlayer = Cast<AChess_HumanPlayer>(GetPawn());
	if (IsValid(HumanPlayer))
	{
		HumanPlayer->OnClick();
	}
}

// Called to bind functionality to input
void AChess_PlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		EnhancedInputComponent->BindAction(ClickAction, ETriggerEvent::Triggered, this, &AChess_PlayerController::ClickOnGrid);
	}
}


