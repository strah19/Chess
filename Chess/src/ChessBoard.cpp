#include "ChessBoard.h"
#include <cctype>
#include <algorithm>

ChessBoard::ChessBoard() {
	for (int i = 0; i < BOARD_WIDTH; i++)
		for (int j = 0; j < BOARD_HEIGHT; j++)
			board[i][j] = start_board[i][j];

	latest_move = { 0, 0, 0, 0 };
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

void ChessBoard::Reset() {
	for (int i = 0; i < BOARD_WIDTH; i++)
		for (int j = 0; j < BOARD_HEIGHT; j++)
			board[i][j] = start_board[i][j];

	int p_counter = 0;
	for (int i = 0; i < BOARD_WIDTH; i++) {
		for (int j = 0; j < BOARD_HEIGHT; j++) {
			if (board[i][j] != ' ') {
				chess_pieces[p_counter]->SetColor(FindPieceColor(board[i][j]));
				chess_pieces[p_counter]->SetBoardPosition(Ember::IVec2(j, i));
				chess_pieces[p_counter]->Initialize();
				p_counter++;
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
			chess_pieces[i]->SetBoardPosition({ -1, -1 });
			captured_flag = true;
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

bool ChessBoard::Check(PieceColor current_player_color) {
	ChessPiece* king = GetPieceFromCharacter(GetTypeCharacterFromColor('k', current_player_color));

	if (king != nullptr) {
		for (auto& piece : GetPieces()) {
			if (piece->GetColor() != king->GetColor()) {
				if (piece->CanPieceGoHere(king->GetBoardPosition())) {
					return true;
				}
			}
		}
	}
	return false;
}

int ChessBoard::GetNumberOfPieces(PieceColor color) {
	int piece_count = 0;
	for (auto& piece : GetPieces()) 
		if (piece->GetColor() == color && piece->GetBoardPosition() != Ember::IVec2(-1, -1))
			piece_count++;

	return piece_count;
}

bool ChessBoard::Checkmate(PieceColor current_player_color) {
	bool check_mate = true;
	
	if (Check(current_player_color)) {
		for (auto& piece : chess_pieces) {
			if (piece->GetColor() == current_player_color) {
				if (!LegalMovesInCheck(piece).empty()) {
					check_mate = false;
					break;
				}
			}
		}
	}
	else {
		check_mate = false;
	}

	return check_mate;
}

std::vector<Ember::IVec2> ChessBoard::LegalMovesInCheck(ChessPiece* piece) {
	std::vector<Ember::IVec2> legal_moves;

	auto moves = piece->GenerateAllPossibleMoves();
	for (auto& move : moves) {
		Ember::IVec2 original = piece->GetBoardPosition();

		char deleting_spot_type = ' ';
		char original_type = ' ';
		original_type = GetTypeFromPosition(original);
		ChessPiece* moving_piece = GetPiece(move);
		if (moving_piece != nullptr) {
			moving_piece->SetBoardPosition({ -1, -1 });
			deleting_spot_type = board[move.y][move.x];
		}
		board[move.y][move.x] = original_type;
		board[original.y][original.x] = ' ';

		piece->SetBoardPosition(move);
		if (!Check(piece->GetColor())) 
			legal_moves.push_back(move);

		piece->SetBoardPosition(original);
		board[original.y][original.x] = original_type;
		if (moving_piece != nullptr) {
			moving_piece->SetBoardPosition(move);
			board[move.y][move.x] = deleting_spot_type;
		}
		else {
			board[move.y][move.x] = ' ';
		}
	}

	return legal_moves;
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