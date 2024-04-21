// Fill out your copyright notice in the Description page of Project Settings.

#include "Chess_StoryBoardEntry.h"
#include "Chess_GameMode.h"
#include "ChessPiece.h"
#include "Chess_UserWidget.h"
#include "Blueprint/WidgetTree.h"

void UChess_StoryBoardEntry::NativeConstruct()
{
	Super::NativeConstruct();
}

void UChess_StoryBoardEntry::WhiteButtonClickHandler()
{
	bool isWhiteButton = true;
	ButtonClickHandler(isWhiteButton);
}

void UChess_StoryBoardEntry::BlackButtonClickHandler()
{
	bool isWhiteButton = false;
	ButtonClickHandler(isWhiteButton);
}

void UChess_StoryBoardEntry::ButtonClickHandler(bool isWhiteButton)
{
	FText tmpText = this->TurnNumberText->GetText();
	FString tmpStr = tmpText.ToString();
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, tmpStr);
	int32 turnClicked = FCString::Atoi(*tmpStr);
	int32 moveNumber = isWhiteButton ? turnClicked * 2 - 1 : turnClicked * 2;
	if (AChess_GameMode* GameModeRef = Cast<AChess_GameMode>(UGameplayStatics::GetGameMode(GetWorld()))) {
		GameModeRef->ReplayMove(moveNumber);
	}
}

void UChess_StoryBoardEntry::SetTurnNumberText(FString t)
{
	if (TurnNumberText)
	{
		TurnNumberText->SetText(FText::FromString(t));
	}
	else
	{

	}
}

void UChess_StoryBoardEntry::SetWhiteMoveText(FString t)
{
	if (WhiteMoveText)
	{
		WhiteMoveText->SetText(FText::FromString(t));
		bWhiteMoveDone = true;
	}
	else
	{
	}
}

void UChess_StoryBoardEntry::SetBlackMoveText(FString t)
{
	if (BlackMoveText) {
		BlackMoveText->SetText(FText::FromString(t));
		bBlackMoveDone = true;
	}
	else
	{
	}
}

void UChess_StoryBoardEntry::SetAllButtons(bool s)
{
	WhiteBackButton->SetIsEnabled(s);
	BlackBackButton->SetIsEnabled(s);
}

