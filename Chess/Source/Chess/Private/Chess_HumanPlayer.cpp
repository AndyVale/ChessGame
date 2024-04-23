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
    PlayerColor = WHITE;
    bIsMyTurn = false;
}

// Called when the game starts or when spawned
void AChess_HumanPlayer::BeginPlay()
{
    Super::BeginPlay();
    if (AChess_GameMode* gm = Cast<AChess_GameMode>(GetWorld()->GetAuthGameMode()))
    {
        gm->OnReplayMove.AddDynamic(this, &AChess_HumanPlayer::ReplayHandler);
    }
}

void AChess_HumanPlayer::ReplayHandler(int32 moveNumber) {
    bIsMyTurn = MyColor == ChessColor::WHITE ? moveNumber % 2 == 0 : moveNumber % 2 == 1;
    SelectedPiece = nullptr;
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
    if (AChess_GameMode* GameMode = Cast<AChess_GameMode>(GetWorld()->GetAuthGameMode()))
    {
        GameMode->PlayerSwapNotify(true);
    }
    GameInstanceRef->SetTurnMessage(TEXT("Human Turn"));
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

    if (Hit.bBlockingHit)//if I click on something and the board is not showing a replay TODO: trova un'altra soluzione
    {
        if (SelectedPiece == nullptr)//Piece is not selected yet
        {
            //Select piece
            if (AChessPiece* CurrPiece = Cast<AChessPiece>(Hit.GetActor()))
            {
                if (CurrPiece->PieceColor == MyColor) {
                    SelectedPiece = CurrPiece;
                    ActiveMoves = Board->GetMovesAndShowHints(CurrPiece, true);
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
            if (!CurrClicked)
            {
                return;//clicked on background
            }
            FVector2D oldLoc = Board->GetXYPositionByRelativeLocation(SelectedPiece->GetActorLocation());
            FVector2D newLoc = Board->GetXYPositionByRelativeLocation(CurrClicked->GetActorLocation());
            TSharedPtr<Chess_Move> move = nullptr;

            if (oldLoc == newLoc)//undo move
            {
                GameMode->Board->CancelFeasibleSquares();
                ActiveMoves.Empty();
                SelectedPiece = nullptr;
                return;
            }

            for (TSharedPtr<Chess_Move> tmpMove : ActiveMoves)
            {
                if (tmpMove->From == oldLoc && tmpMove->To == newLoc)
                {
                    move = tmpMove;
                    break;
                }
            }


            if (GameMode->bIsOnReplay && move)
            {
                GameMode->GoBackToActualMove();//Remove history board after the performed move
            }

            if (CurrClicked && move)
            {//if MakeASafeMove return the move is done, otherwise the piece is deselected
                Board->HandledMakeAMove(move, false);
                GameMode->UpdateLastMove(move);
                bIsMyTurn = false;
            }

            GameMode->Board->CancelFeasibleSquares();
            ActiveMoves.Empty();
            SelectedPiece = nullptr;
            if (!bIsMyTurn)
            {
                GameMode->TurnNextPlayer();
            }
        }
    }
}
