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
            GameInstanceRef->OnMessageChange.AddDynamic(this, &UChess_UserWidget::MessageChangeHandler);
            GameMode->OnShowPromotionWidget.BindUObject(this, &UChess_UserWidget::SetPromotionWidgetVisible);
            GameMode->OnTurnGoBack.AddUObject(this, &UChess_UserWidget::RemoveMovesUntil);
            GameMode->OnTurnSwap.BindUObject(this, &UChess_UserWidget::SetIsEnabledButtons);
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
        if (GameMode)
        {
            GameMode->OnMoveUpdate.BindUObject(this, &UChess_UserWidget::MoveHandler);
        }
    }
    if (PawnPromotionWidget) {
        PawnPromotionWidget->SetVisibility(ESlateVisibility::Hidden);
    }

    ScoreUpdateHandler();//Show points
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

void UChess_UserWidget::RemoveMovesUntil(int32 moveNumber)
{
    //moveNumber = number of moves to mantain
    int32 numberOfEntriesToRemove = TurnsHistory.Num() - moveNumber / 2;
    while (numberOfEntriesToRemove > 0)
    {
        RemoveLastMove();
        numberOfEntriesToRemove--;
    }
    if (moveNumber % 2 != 0)
    {
        if (!TurnsHistory.IsEmpty())
        {
            TurnsHistory.Top()->SetBlackMoveText(FString("--"));
        }
    }
}

void UChess_UserWidget::SetPromotionWidgetVisible(ChessColor playerColor)
{
    FLinearColor color = playerColor == ChessColor::WHITE ? FLinearColor(999, 999, 999) : FLinearColor(FVector(0, 0, 0));
    if (PawnPromotionWidget) {
        PawnPromotionWidget->SetVisibility(ESlateVisibility::Visible);
        PawnPromotionWidget->SetColorAndOpacity(color);
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
    //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("ScoreUpdateHandler HUD"));

    if (GameInstanceRef)
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
   // GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("Reset HUD"));
    if (StoryBoardScrollBox)
    {
        TurnsHistory = TArray<UChess_StoryBoardEntry*>();
        StoryBoardScrollBox->ClearChildren();
    }
}

void UChess_UserWidget::MoveHandler(FString stringMove, int32 moveNumber)
{
    //Get the textbox:
    UChess_StoryBoardEntry* moveTurnEntry;
    if (TurnsHistory.Num() * 2 >= moveNumber)//2 moves in each turn
    {//no need to create new turn entry
        moveTurnEntry = TurnsHistory[(moveNumber - 1) / 2];
    }
    else
    {//create a new entry
        moveTurnEntry = CreateWidget<UChess_StoryBoardEntry>(GetWorld(), StoryBoardEntry);
        if (StoryBoardScrollBox)//add to scrollbox
        {
            StoryBoardScrollBox->AddChild(moveTurnEntry);
            StoryBoardScrollBox->ScrollToEnd();
        }
        TurnsHistory.Add(moveTurnEntry);
        if (GameMode)
        {
            moveTurnEntry->SetTurnNumberText(FString::Printf(TEXT("%d"), GameMode->TurnNumber));
        }
    }
    UButton* selectedButton = (moveNumber - 1) % 2 == 0 ? moveTurnEntry->WhiteBackButton : moveTurnEntry->BlackBackButton;
    if (selectedButton && !selectedButton->OnClicked.IsBound())
    {
        (moveNumber - 1) % 2 == 0 ? selectedButton->OnClicked.AddDynamic(moveTurnEntry, &UChess_StoryBoardEntry::WhiteButtonClickHandler) : selectedButton->OnClicked.AddDynamic(moveTurnEntry, &UChess_StoryBoardEntry::BlackButtonClickHandler);
    }
    (moveNumber - 1) % 2 == 0 ? moveTurnEntry->SetWhiteMoveText(stringMove) : moveTurnEntry->SetBlackMoveText(stringMove);
}

void UChess_UserWidget::MessageChangeHandler()
{
    if (GameInstanceRef && CurrentPlayerText)
    {
        CurrentPlayerText->SetText(FText::FromString(GameInstanceRef->CurrentTurnMessage));
    }
    //if (CurrentPlayerText)
    //{
    //    if (CurrentPlayerText->GetText().ToString().Compare("White turn") == 0)
    //    {
    //        CurrentPlayerText->SetText(FText::FromString("Black turn"));
    //    }
    //    else
    //    {
    //        CurrentPlayerText->SetText(FText::FromString("White turn"));
    //    }
    //}
}

void UChess_UserWidget::SetIsEnabledButtons(bool s)
{
    ChangeOpponentButton->SetIsEnabled(s);
    for (UChess_StoryBoardEntry* entry : TurnsHistory)
    {
        entry->SetAllButtons(s);
    }
}