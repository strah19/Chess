#ifndef CHESS_BOARD_H
#define CHESS_BOARD_H

#include <vector>

#include "ChessPieces.h"

constexpr int CHESS_PIECE_NUM = 16;

constexpr int BOARD_WIDTH = 8;
constexpr int BOARD_HEIGHT = 8;

constexpr char start_board[BOARD_WIDTH][BOARD_HEIGHT] = {
		{ 'R', 'N', 'B', 'Q', 'K', 'B', 'N', 'R' },
		{ 'P', 'P', 'P', 'P', 'P', 'P', 'P', 'P' },
		{ ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' },
		{ ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' },
		{ ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' },
		{ ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' },
		{ 'p', 'p', 'p', 'p', 'p', 'p', 'p', 'p' },
		{ 'r', 'n', 'b', 'q', 'k', 'b', 'n', 'r' }
};

class ChessBoard {
public:
	ChessBoard();
	bool CapturePiece(const Ember::IVec2& capture_position);
	bool IsTherePieceOn(const Ember::IVec2& position);
	ChessPiece* GetPiece(const Ember::IVec2& position);
	void MovePiece(ChessPiece* piece, const Ember::IVec2& new_position);
	ChessPiece* GetPieceFromCharacter(char piece);
	ChessPiece* GetPieceFromLocAndType(const Ember::IVec2& pos, char charc);
	int GetNumberOfPieces(PieceColor color);

	std::vector<ChessPiece*>& GetPieces() { return chess_pieces; }
	void Log();
	void Reset();

	bool Checkmate(PieceColor current_player_color);
	std::vector<Ember::IVec2> LegalMovesInCheck(ChessPiece* piece);
	bool Check(PieceColor current_player_color);
	void AddPiece(ChessPiece* piece) { chess_pieces.push_back(piece); }
	Ember::Rect GetLatestMove() const { return latest_move; }
	char GetTypeFromPosition(const Ember::IVec2& position) { return board[position.y][position.x]; }
	char board[BOARD_WIDTH][BOARD_HEIGHT];
private:

	std::vector<ChessPiece*> chess_pieces;
	Ember::Rect latest_move = { 0, 0, 0, 0 };
};

PieceColor FindPieceColor(char piece);
char GetTypeCharacterFromColor(char piece, PieceColor color);

#endif // !CHESS_BOARD_H
