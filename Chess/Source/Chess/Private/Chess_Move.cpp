// Fill out your copyright notice in the Description page of Project Settings.

#include "Chess_Move.h"
#include "ChessPiece.h"
#include "Square.h"
#include "Chessboard.h"
#include "CP_Bishop.h"
#include "CP_King.h"
#include "CP_Knight.h"
#include "CP_Pawn.h"
#include "CP_Queen.h"
#include "CP_Rook.h"

Chess_Move::Chess_Move(FVector2D f, FVector2D t, AChessboard* board) : PlayerOnCheck(ChessColor::NAC), PlayerOnCheckMate(ChessColor::NAC), PlayerOnStall(ChessColor::NAC), From(f), To(t), ReferredBoard(board)
{
    PieceFrom = board->GetPieceFromXY(f);
    PieceTo = board->GetPieceFromXY(t);
    //PieceLetter = GetPieceLetter(PieceFrom);
    //if the piece get the opposite end of the board
    if (GetMoveColor() == ChessColor::BLACK && To.Y == ReferredBoard->BoardSize - 1 || GetMoveColor() == ChessColor::WHITE && To.Y == 0)
    {
        if (ACP_Pawn* pawn = Cast<ACP_Pawn>(PieceFrom))//and is a pawn->needs a promotion
        {
            bPromotionAfterMove = true;
        }
    }
    MoveValue = MoveValueCalculation();
}

void Chess_Move::CalculateResult()
{
    AChessPiece* XOpponent = PieceFrom;
    ChessColor OpponentColor = XOpponent->PieceColor == WHITE ? BLACK : WHITE;

    if (ReferredBoard->CheckControl(OpponentColor))
    {
        PlayerOnCheck = OpponentColor;
        if (ReferredBoard->MateControl(OpponentColor))
        {
            PlayerOnCheckMate = OpponentColor;
        }
    }
    else
    {
        if (ReferredBoard->StallControl(OpponentColor))
        {
            PlayerOnStall = OpponentColor;
        }
    }
}

void Chess_Move::MakeMove(bool simulate)
{
    //TODO CONTROLLA SIA TUTTO GIUSTO
    if (!PieceFrom)
    {
        UE_LOG(LogTemp, Error, TEXT("MakeAMove:Impossible move, no pieces in old position"));
        return;
    }

    //Making the move:
    ReferredBoard->RemovePiece(PieceFrom);
    if (PieceTo)
    {
        ReferredBoard->RemovePiece(PieceTo);//remove NewPiece
    }
    ReferredBoard->SetPieceFromXY(To, PieceFrom);

    //Moving the piece
    if (!simulate)//not a simulation, move the piece
    {
        if (PieceTo)
        {
            if (PieceTo->PieceColor == BLACK)//TODO:RIARRANGIA
                PieceTo->SetActorLocation(ReferredBoard->GetRelativeLocationByXYPosition(-1, -1));
            else
                PieceTo->SetActorLocation(ReferredBoard->GetRelativeLocationByXYPosition(8, 8));
            PieceTo->SetActorHiddenInGame(true);
        }
        (PieceFrom)->SetActorLocation(ReferredBoard->GetRelativeLocationByXYPosition(To[0], To[1]));
    }

    //CalculateResult test for check, checkmate and stall. Is importat to avoid this procedure in case it's a simulative move (for performance reasons)
    if (bPromotionAfterMove)
    {
        PromotePawn(simulate, nullptr);
    }
    if (!simulate)
    {
        CalculateResult();
    }
}


void Chess_Move::RollbackMove(bool simulate)
{

    if (bPromotionAfterMove && PawnPromotionAusRef)//if move was a promotion and was already done
    {
        PromoteRollbackPawn(simulate);
    }

    ReferredBoard->SetPieceFromXY(To, PieceTo);
    ReferredBoard->SetPieceFromXY(From, PieceFrom);


    if (PieceTo)
    {
        if (!simulate)
        {
            PieceTo->SetActorHiddenInGame(false);
            PieceTo->SetActorLocation(ReferredBoard->GetRelativeLocationByXYPosition(To[0], To[1]));
        }
    }

    if (PieceFrom) {

        if (!simulate)
        {
            PieceFrom->SetActorHiddenInGame(false);
            PieceFrom->SetActorLocation(ReferredBoard->GetRelativeLocationByXYPosition(From[0], From[1]));
        }
    }
}

void Chess_Move::PromotePawn(bool simulate, TSubclassOf<AChessPiece> selectedPiece)
{
    if (selectedPiece != nullptr)//first promotion (not executed if is a replay)
    {
        FVector position = ReferredBoard->GetRelativeLocationByXYPosition(To.X, To.Y) + FVector(0, 0, 10);
        FRotator rotation = FRotator(0, 0, 0);
        ChessColor pColor = GetMoveColor();
        if (PawnPromotionAusRef != nullptr) {//never reached (in theory)
            PawnPromotionAusRef->Destroy();
        }
        PawnPromotionAusRef = ReferredBoard->GetWorld()->SpawnActor<AChessPiece>(selectedPiece, position, rotation);
        PawnPromotionAusRef->SetColorAndMaterial(pColor);
        PawnPromotionAusRef->SetActorHiddenInGame(true);
    }

    if (PawnPromotionAusRef) {//swap pieces
        AChessPiece* swapVar = PieceFrom;
        if (!simulate) {//show the effective piece
            PieceFrom->SetActorHiddenInGame(true);
            PawnPromotionAusRef->SetActorHiddenInGame(false);
            PieceFrom->SetActorLocation(ReferredBoard->GetRelativeLocationByXYPosition(-1, -1));//Move chesspiece away from chessboard to avoid fake click
        }

        ReferredBoard->RemovePiece(PieceFrom);
        PieceFrom = PawnPromotionAusRef;
        PawnPromotionAusRef = swapVar;
        ReferredBoard->SetPieceFromXY(To, PieceFrom);
    }
    CalculateResult();//update attributes for "toString" method
}

void Chess_Move::PromoteRollbackPawn(bool simulate)
{
    //if (simulate) return;

    if (PawnPromotionAusRef == nullptr)
    {
        UE_LOG(LogTemp, Error, TEXT("PromoteRollbackPawn:Impossible rollback, no promoted piece found"));
        return;
    }
    if (!ReferredBoard->GetXYFromPiece(PieceFrom))
    {
        return;
    }
    //swap pieces:
    AChessPiece* swapVar = PieceFrom;
    ReferredBoard->RemovePiece(PieceFrom);
    PieceFrom = PawnPromotionAusRef;
    PawnPromotionAusRef = swapVar;
    ReferredBoard->SetPieceFromXY(To, PieceFrom);

    if (!simulate)
    {
        PieceFrom->SetActorHiddenInGame(false);
        PieceFrom->SetActorLocation(ReferredBoard->GetRelativeLocationByXYPosition(To[0], To[1]));//put the piece in the position
        PawnPromotionAusRef->SetActorHiddenInGame(true);
    }
    else {//if was a simulate move destory the promoted piece to avoid memory leak (TODO: controlla se unreal lo fa già da solo)
        PawnPromotionAusRef->Destroy();
    }
}

float Chess_Move::MoveValueCalculation()
{
    float value = 0;
    if (PieceFrom)
    {
        if (GetMoveColor() == ChessColor::WHITE)
        {//positive counting for white
            value -= PieceFrom->GetPositionValue(From);//move value = newPiecePositionValue - oldPiecePositionValue
            value += PieceFrom->GetPositionValue(To);
            if (PieceTo)
            {
                value += PieceTo->GetCorrectedPieceValue();
            }
        }
        else
        {//negative counting for black
            value += PieceFrom->GetPositionValue(From);//move value = newPiecePositionValue - oldPiecePositionValue
            value -= PieceFrom->GetPositionValue(To);
            if (PieceTo)
            {
                value -= PieceTo->GetCorrectedPieceValue();
            }
        }
    }
    return value;
}

FString Chess_Move::ToString() const
{
    FString AlgebraicNotation;
    FString Columns = "abcdefgh";
    FString Rows = "87654321";
    // Converti le coordinate 'From' e 'To' in notazione algebrica
    AlgebraicNotation += PawnPromotionAusRef == nullptr ? GetPieceLetter(PieceFrom) : GetPieceLetter(PawnPromotionAusRef);
    AlgebraicNotation += Columns[From.X];
    AlgebraicNotation += Rows[From.Y];
    if (PieceTo != nullptr)//eat
    {
        AlgebraicNotation += 'x';
    }
    else {
        AlgebraicNotation += '-';
    }
    AlgebraicNotation += Columns[To.X];
    AlgebraicNotation += Rows[To.Y];

    AlgebraicNotation += PawnPromotionAusRef != nullptr ? "=" + GetPieceLetter(PieceFrom) : "";//for pawn promotion TODO: controlla se è giusta la notazione

    if (PlayerOnCheck != NAC)
    {
        if (PlayerOnCheckMate == NAC)//it's only a check
        {
            AlgebraicNotation += '+';
        }
        else//it's checkmate
        {
            AlgebraicNotation += '#';
        }
    }
    return AlgebraicNotation;
}

ChessColor Chess_Move::GetMoveColor() const
{
    if (PieceFrom)
    {
        return PieceFrom->PieceColor;
    }
    return ChessColor::NAC;
}

Chess_Move::~Chess_Move()
{
}

FString Chess_Move::GetPieceLetter(AChessPiece* piece)
{
    if (const ACP_Bishop* tmp = Cast<ACP_Bishop>(piece)) {
        return FString("B");
    }
    if (const ACP_King* tmp = Cast<ACP_King>(piece)) {
        return FString("K");
    }
    if (const ACP_Knight* tmp = Cast<ACP_Knight>(piece)) {
        return FString("N");
    }
    if (const ACP_Pawn* tmp = Cast<ACP_Pawn>(piece)) {
        return FString("");
    }
    if (const ACP_Queen* tmp = Cast<ACP_Queen>(piece)) {
        return FString("Q");
    }
    if (const ACP_Rook* tmp = Cast<ACP_Rook>(piece)) {
        return FString("R");
    }
    return FString("");
}

bool operator<(const Chess_Move& a, const Chess_Move& b) {
    return a.MoveValue < b.MoveValue;
}