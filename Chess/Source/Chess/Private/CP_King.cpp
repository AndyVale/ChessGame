// Fill out your copyright notice in the Description page of Project Settings.


#include "CP_King.h"

TArray<Chess_Move> ACP_King::GetPieceMoves()
{
    TArray<Chess_Move> moves = TArray<Chess_Move>();

    int x = PiecePosition[0], y = PiecePosition[1];
    int max = ReferredBoard->BoardSize;
    //Possible moves:
    FVector2D kingMoves[8] = {
        FVector2D(0, 1),
        FVector2D(1, 1),
        FVector2D(-1, 1),
        FVector2D(0, -1),
        FVector2D(1, -1),
        FVector2D(-1, -1),
        FVector2D(1, 0),
        FVector2D(-1, 0)
    };


    //standard moves
    for (const FVector2D& move : kingMoves) {
        FVector2D newLoc = FVector2D(x + move.X, y + move.Y);
        if (newLoc.X >= 0 && newLoc.X < max && newLoc.Y >= 0 && newLoc.Y < max && ReferredBoard->GetPieceColorFromXY(newLoc) != PieceColor) {
            moves.Add(Chess_Move(PiecePosition, newLoc, ReferredBoard));
        }
    }
    
    //castles
    bool queenSideCastle = false, kingSideCastle;
    int j = 4;
    queenSideCastle = PieceColor == ChessColor::WHITE ? ReferredBoard->bCastleWhiteLong : ReferredBoard->bCastleBlackLong;
    kingSideCastle = PieceColor == ChessColor::WHITE ? ReferredBoard->bCastleWhiteShort : ReferredBoard->bCastleBlackShort;
   
    if(PiecePosition.Y == 7 && PieceColor == WHITE || PiecePosition.Y == 0 && PieceColor == BLACK)
    if (!ReferredBoard->GetMoveShowedOnBoard() || ReferredBoard->GetMoveShowedOnBoard() && ReferredBoard->GetMoveShowedOnBoard()->PlayerOnCheck != PieceColor)//castle is legal only if the king is not under check
    {
        if (queenSideCastle)
        {
            for (j = 3; j > 0; j --){
                AChessPiece* tmp= ReferredBoard->GetPieceFromXY(FVector2D(j, PiecePosition.Y));
                if (tmp != nullptr)
                {
                    break;
                }
            }
            if (j == 0)
            {
                moves.Add(Chess_Move(PiecePosition, FVector2D(2, PiecePosition.Y), ReferredBoard, true));
            }
        }
        
        if (kingSideCastle)
        {
            for (j = 5; j < 7; j++) {
                AChessPiece* tmp = ReferredBoard->GetPieceFromXY(FVector2D(j, PiecePosition.Y));
                if (tmp != nullptr)
                {
                    break;
                }
            }
            if (j == 7)
            {
                moves.Add(Chess_Move(PiecePosition, FVector2D(6, PiecePosition.Y), ReferredBoard, false));
            }
        }
    }
    
    return moves;
}

float ACP_King::GetPieceValue()
{
    return 900.0f;
}



