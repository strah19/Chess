#ifndef CHESS_NOTATION_H
#define CHESS_NOTATION_H

#include <vector>
#include <string>

#include "ChessBoard.h"

char ColToChessFileConverter(int col);

class ChessNotator {
public:
	void BeforeMove(ChessBoard& chess_board, ChessPiece* latest_moved_piece, const Ember::IVec2& new_location);
	void AfterMove();

	std::vector<std::string>& GetNotations() { return chess_notations; }
	inline bool PromotionPossible() const { return promotion_possible; }
	inline void AddPromotionTypeId(char type) { chess_notations.back() += toupper(int(type)); promotion_possible = false; }
private:
	bool MultiplePiecesCanGoHere(PieceColor color, const Ember::IVec2& attacking_square);
	void CheckForPromotion(const Ember::IVec2& latest_move);
private:
	std::vector<std::string> chess_notations;

	ChessBoard* chess_board;
	ChessPiece* latest_moved_piece;

	bool promotion_possible = false;
	bool specify_file = false;
};

class ChessNotationSerializer {
public:
	void Save(ChessNotator& notator, const std::string& desired_file_location) { }
private:
};

class ChessNotationDeserializer {
public:
	std::vector<std::string> Open(const std::string& desired_file_location) { return std::vector<std::string>(); }
private:
};

class NotationExecutor {
public:
	void ExecuteNotations(const std::vector<std::string>& notations, ChessBoard& chess_board) { }
private:
};

#endif // !CHESS_NOTATION_H
