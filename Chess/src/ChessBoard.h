#ifndef CHESS_BOARD_H
#define CHESS_BOARD_H

#include <vector>
#include <unordered_map>

#include "ChessPieces.h"

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
	std::vector<Ember::IVec2> GenerateLegalMoves(ChessPiece* piece);
	bool Check(PieceColor current_player_color);
	Ember::Rect GetLatestMove() const { return latest_move; }
	char GetTypeFromPosition(const Ember::IVec2& position) { return board[position.y][position.x]; }
	char board[BOARD_WIDTH][BOARD_HEIGHT];

	bool Captured() const { return captured_flag; }
	void ResetCaptureFlag() { captured_flag = false; }

	void Flip();
	PieceColor GetSideOnTop() const { return side_starting_on_top; }
private:
	bool captured_flag = false;

	PieceColor side_starting_on_top= PieceColor::BLACK;

	std::vector<ChessPiece*> chess_pieces;
	Ember::Rect latest_move = { 0, 0, 0, 0 };
};

class Promoter {
public:
	inline void SetPromotionPawn(ChessPiece* piece) { current_piece = piece; }
	void Promote(ChessBoard* chess_board, char promotion_type);
	void Reset(ChessBoard* chess_board);
private:
	ChessPiece* current_piece;
	std::vector<int> need_deleting;
};

PieceColor FindPieceColor(char piece);
char GetTypeCharacterFromColor(char piece, PieceColor color);

#endif // !CHESS_BOARD_H
