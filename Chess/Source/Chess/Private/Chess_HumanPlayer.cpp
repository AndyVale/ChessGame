// Fill out your copyright notice in the Description page of Project Settings.


#include "Chess_HumanPlayer.h"
#include "Square.h"
#include "Chess_GameMode.h"
#include "ChessPiece.h"
#include <EnhancedInputComponent.h>

// Sets default values
AChess_HumanPlayer::AChess_HumanPlayer()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	AutoPossessPlayer = EAutoReceiveInput::Player0;
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("DAJE"));

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	SetRootComponent(Camera);
    GameInstanceRef = Cast<UChess_GameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	Color = WHITE;
}

// Called when the game starts or when spawned
void AChess_HumanPlayer::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AChess_HumanPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AChess_HumanPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AChess_HumanPlayer::OnTurn()
{
	bIsMyTurn = true;
	//GameInstance->SetTurnMessage(TEXT("Human Turn"));
}

void AChess_HumanPlayer::OnWin()
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("You Win!"));
	GameInstanceRef->SetTurnMessage(TEXT("Human Wins!"));
	GameInstanceRef->IncrementScoreHumanPlayer();
}

void AChess_HumanPlayer::OnLose()
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("You Lose!"));
	GameInstanceRef->SetTurnMessage(TEXT("Human Loses!"));
}

void AChess_HumanPlayer::OnClick()
{
    FHitResult Hit = FHitResult(ForceInit);
    GetWorld()->GetFirstPlayerController()->GetHitResultUnderCursor(ECollisionChannel::ECC_Pawn, true, Hit);

    if (!bIsMyTurn)
        return;

    AChess_GameMode* GameMode = Cast<AChess_GameMode>(GetWorld()->GetAuthGameMode());
    if (GameMode == nullptr) {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("GameMode is null in OnClick function"));
        return;
    }
    
    AChessboard* Board = GameMode->Board;
    if (Board == nullptr) {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Board is null in OnClick function"));
        return;
    }

    if (Hit.bBlockingHit && !Board->IsOnReplay())//if I click on something and the board is not showing a replay TODO: trova un'altra soluzione
    {
        if(SelectedPiece == nullptr)//Piece is not selected yet
        {
            //Select piece
            if (AChessPiece* CurrPiece = Cast<AChessPiece>(Hit.GetActor()))
            {
                if (CurrPiece->PieceColor == MyColor) {
                    SelectedPiece = CurrPiece;
                    Board->GetFeasibleSquares(SelectedPiece, true);
                }
                else {
                    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("You have to select a piece of your color!"));
                }
            }
        }
        else
        {
            //Move piece (or eat)
            AActor* CurrClicked = nullptr;//using an AActor beacuse no 'special' methods are used here
            //If clicked obj is a square I have to move, otherwise if obj is a piece eat it, it's handled by MakeASafeMove
            Cast<AChessPiece>(Hit.GetActor()) ? CurrClicked = Cast<AChessPiece>(Hit.GetActor()) : CurrClicked = Cast<ASquare>(Hit.GetActor());
            FVector2D oldLoc = Board->GetXYPositionByRelativeLocation(SelectedPiece->GetActorLocation());
            FVector2D newLoc = Board->GetXYPositionByRelativeLocation(CurrClicked->GetActorLocation());

            if (CurrClicked && Board->MakeASafeMove(oldLoc, newLoc))
            {//if MakeASafeMove return the move is done, otherwise the piece is deselected
               // Chess_Move succMove = Chess_Move(oldLoc, newLoc);
                bIsMyTurn = false;
            }
            Board->CancelFeasibleSquares();
            SelectedPiece = nullptr;
            if (!bIsMyTurn)
                GameMode->TurnNextPlayer();
        }
    }
}
