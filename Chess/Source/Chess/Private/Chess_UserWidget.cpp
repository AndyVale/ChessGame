// Fill out your copyright notice in the Description page of Project Settings.


#include "Chess_UserWidget.h"
#include <string.h>
#include "Chess_GameMode.h"

void UChess_UserWidget::NativeConstruct()
{
    GameInstanceRef = Cast<UChess_GameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
    if (GameInstanceRef)
    {
        GameInstanceRef->OnScoreUpdate.AddDynamic(this, &UChess_UserWidget::ScoreUpdateHandler);
        GameInstanceRef->OnResetEvent.AddDynamic(this, &UChess_UserWidget::ResetHandler);
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
        StoryBoardScrollBox->ClearChildren();
    }
}