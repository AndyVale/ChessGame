// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Chess_HumanPlayer.h"
#include "Chess_PlayerController.h"
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
			UE_LOG(LogTemp, Error, TEXT("ChessContext Bene"));
			Subsystem->AddMappingContext(ChessContext, 0);
		}
		else
			UE_LOG(LogTemp, Error, TEXT("ChessContext MALE"));
	}
}

void AChess_PlayerController::ClickOnGrid()
{
	// Logica specifica per il click sulla griglia
	UE_LOG(LogTemp, Error, TEXT("ClickOnGrid rilevato"));


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

	// Associa la funzione delegata all'azione specificata
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		UE_LOG(LogTemp, Error, TEXT("Binding Bene"));
		EnhancedInputComponent->BindAction(ClickAction, ETriggerEvent::Triggered, this, &AChess_PlayerController::ClickOnGrid);
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("Binding Male"));

	}
}


