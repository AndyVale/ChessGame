// Fill out your copyright notice in the Description page of Project Settings.


#include "Chess_UserWidget.h"
#include <string.h>
#include "Chess_GameMode.h"

void UChess_UserWidget::NativeConstruct()
{
    TurnsHistory = TArray<UChess_StoryBoardEntry*>();
    GameInstanceRef = Cast<UChess_GameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
    GameMode = Cast<AChess_GameMode>(GetWorld()->GetAuthGameMode());
    //Binding receiver:
    if (GameInstanceRef)
    {
        GameInstanceRef->OnScoreUpdate.AddDynamic(this, &UChess_UserWidget::ScoreUpdateHandler);
        GameInstanceRef->OnResetEvent.AddDynamic(this, &UChess_UserWidget::ResetHandler);
        if (GameMode)
        {
            if (CurrentPlayerText)
            {
                CurrentPlayerText->SetText(FText::FromString("White turn"));
            }
            GameMode->OnPlayerSwap.AddDynamic(this, &UChess_UserWidget::ShowPlayerHandler);
        }
    }

    //Binding Events:
    if (ResetButton)
    {
        ResetButton->OnClicked.AddDynamic(this, &UChess_UserWidget::ResetOnClick);
    }
    if (RematchButton)
    {
        RematchButton->OnClicked.AddDynamic(this, &UChess_UserWidget::RematchOnClick);
    }
    if (StoryBoardScrollBox)
    {
        if (GameMode && GameMode->Board)
        {
            GameMode->Board->OnMove.AddDynamic(this, &UChess_UserWidget::MoveHandler);
        }
    }
}

void UChess_UserWidget::ReplayMove(int32 moveNumber)
{
    //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("UNDO Move n: %d"), moveNumber));
    //bool bIsWhiteMove = moveNumber % 2 != 0;
    //int32 turnNumber = (moveNumber-1) / 2;
    //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("At turn n: %d"), turnNumber));
    if (AChess_GameMode* GameModeRef = Cast<AChess_GameMode>(UGameplayStatics::GetGameMode(GetWorld()))) {
        GameModeRef->OnReplayMove.Broadcast(moveNumber);
    }
}

void UChess_UserWidget::RemoveLastMove()
{
    if (StoryBoardScrollBox->GetChildrenCount() - 1 >= 0)
    {
        TurnsHistory.Pop();
        StoryBoardScrollBox->RemoveChildAt((StoryBoardScrollBox->GetChildrenCount() - 1));
    }
}

void UChess_UserWidget::ResetOnClick()
{
    if (GameInstanceRef)
    {
        GameInstanceRef->ResetPointsAndGame();
    }
}

void UChess_UserWidget::RematchOnClick()
{
    if (GameInstanceRef)
    {
        GameInstanceRef->ResetGame();
    }
}

void UChess_UserWidget::ScoreUpdateHandler() {
    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("ScoreUpdateHandler HUD"));

    if(GameInstanceRef)
    {
        if (HumanScoreText)
        {
            HumanScoreText->SetText(FText::AsNumber(GameInstanceRef->GetScoreHumanPlayer()));
        }

        if (AIScoreText)
        {
            AIScoreText->SetText(FText::AsNumber(GameInstanceRef->GetScoreAIPlayer()));
        }
    }
}

void UChess_UserWidget::ResetHandler() {
    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("Reset HUD"));
    if (StoryBoardScrollBox)
    {
        TurnsHistory = TArray<UChess_StoryBoardEntry*>();
        StoryBoardScrollBox->ClearChildren();
    }
}

void UChess_UserWidget::MoveHandler(FString stringMove)
{
    UChess_StoryBoardEntry* lastTurnRow;
    if (TurnsHistory.Num() > 0)//if is not the first move
    {
        lastTurnRow = TurnsHistory.Last();//get the last couple of moves
    }
    else {
        lastTurnRow = CreateWidget<UChess_StoryBoardEntry>(GetWorld(), StoryBoardEntry);//else create a new couple
    }

    //if all the move are 
    if (lastTurnRow->bWhiteMoveDone && !lastTurnRow->bBlackMoveDone)
    {//just add black move
        lastTurnRow->SetBlackMoveText(stringMove);
        lastTurnRow->BlackBackButton->OnClicked.AddDynamic(lastTurnRow, &UChess_StoryBoardEntry::BlackButtonClickHandler);
    }
    else
    {//add replay button, turn number and white move
        lastTurnRow = CreateWidget<UChess_StoryBoardEntry>(GetWorld(), StoryBoardEntry);//create new row
        //lastTurnRow->HudRef = this;
        lastTurnRow->WhiteBackButton->OnClicked.AddDynamic(lastTurnRow, &UChess_StoryBoardEntry::WhiteButtonClickHandler);
        if (StoryBoardScrollBox)//add to scrollbox
        {
            StoryBoardScrollBox->AddChild(lastTurnRow);
            StoryBoardScrollBox->ScrollToEnd();
        }

        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, stringMove);//new turn and white move
        if (GameMode)
        {
            lastTurnRow->SetTurnNumberText(FString::Printf(TEXT("%d"), GameMode->TurnNumber));
        }
        lastTurnRow->SetWhiteMoveText(stringMove);

        TurnsHistory.Add(lastTurnRow);
    }
}

void UChess_UserWidget::ShowPlayerHandler()
{
    if(CurrentPlayerText)
    {
        if(CurrentPlayerText->GetText().ToString().Compare("White turn") == 0)
        {
            CurrentPlayerText->SetText(FText::FromString("Black turn"));
        }
        else
        {
            CurrentPlayerText->SetText(FText::FromString("White turn"));
        }
    }
}
