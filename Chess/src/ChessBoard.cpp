#include "ChessBoard.h"
#include <cctype>

ChessBoard::ChessBoard() {
	for (int i = 0; i < BOARD_WIDTH; i++) {
		for (int j = 0; j < BOARD_HEIGHT; j++) {
			if (board[i][j] != ' ') {
				chess_pieces.push_back(ChessPieceFactory::Get()->CreatePiece(board[i][j]));
				if (chess_pieces.back() != nullptr) {
					chess_pieces.back()->SetColor(FindPieceColor(board[i][j]));
					chess_pieces.back()->SetBoardPosition(Ember::IVec2(j, i));
					chess_pieces.back()->SetParentBoard(this);
					chess_pieces.back()->Initialize();
				}
			}
		}
	}
}

void ChessBoard::MovePiece(ChessPiece* piece, const Ember::IVec2& new_position) {
	char current_piece_type = board[piece->GetBoardPosition().y][piece->GetBoardPosition().x];
	board[piece->GetBoardPosition().y][piece->GetBoardPosition().x] = ' ';
	board[new_position.y][new_position.x] = current_piece_type;
	
	latest_move.pos = piece->GetBoardPosition();
	latest_move.size = new_position;
}

ChessPiece* ChessBoard::GetPieceFromLocAndType(const Ember::IVec2& pos, char charc) {
	for (int i = 0; i < BOARD_WIDTH; i++) {
		for (int j = 0; j < BOARD_HEIGHT; j++) {
			if (board[i][j] == charc) {
				ChessPiece* p = GetPiece(pos);
				if (p != nullptr) {
					if (p->GetBoardPosition() == Ember::IVec2(j, i))
						return p;
				}
			}
		}
	}
	return nullptr;
}

ChessPiece* ChessBoard::GetPieceFromCharacter(char piece) {
	for (int i = 0; i < BOARD_WIDTH; i++) {
		for (int j = 0; j < BOARD_HEIGHT; j++) {
			if (board[i][j] == piece) {
				return GetPiece({ j, i });
			}
		}
	}
	return nullptr;
}

ChessPiece* ChessBoard::GetPiece(const Ember::IVec2& position) {
	for (auto& piece : chess_pieces)
		if (piece->GetBoardPosition() == position)
			return piece;
	return nullptr;
}

bool ChessBoard::CapturePiece(const Ember::IVec2& capture_position) {
	for (size_t i = 0; i < chess_pieces.size(); i++) 
		if (chess_pieces[i]->GetBoardPosition() == capture_position) {
			board[capture_position.x][capture_position.y] = ' ';
			chess_pieces.erase(chess_pieces.begin() + i);
			return true;
		}

	return false;
}

void ChessBoard::Log() {
	for (int i = 0; i < BOARD_WIDTH; i++) {
		for (int j = 0; j < BOARD_HEIGHT; j++) {
			std::cout << board[i][j];
		}
		std::cout << std::endl;
	}
}

bool ChessBoard::IsTherePieceOn(const Ember::IVec2& position) {
	for (auto& piece : chess_pieces) 
		if (piece->GetBoardPosition() == position)
			return true;
	return false;
}

PieceColor FindPieceColor(char piece) {
	return((isupper((int)piece) ? PieceColor::BLACK : PieceColor::WHITE));
}

char GetTypeCharacterFromColor(char piece, PieceColor color) {
	if (FindPieceColor(piece) == color)
		return piece;
	else {
		if (isupper((int)piece))
			return tolower((int)piece);
		if (islower((int)piece))
			return toupper((int)piece);
	}

	return piece;
}