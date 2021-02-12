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
private:
	bool MultiplePiecesCanGoHere(PieceColor color, const Ember::IVec2& attacking_square);
private:
	std::vector<std::string> chess_notations;

	ChessBoard* chess_board;
	ChessPiece* latest_moved_piece;

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
