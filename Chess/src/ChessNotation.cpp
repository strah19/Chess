#include "ChessNotation.h"
#include <sstream>

constexpr char CAPTURE_IDENTIFIER = 'x';

char ColToChessFileConverter(int col) {
	if (col >= 1 && col <= 26)
		return "abcdefghijklmnopqrstuvwxyz"[col - 1];
	return ' ';
}

void ChessNotator::BeforeMove(ChessBoard& chess_board, ChessPiece* latest_moved_piece, const Ember::IVec2& new_location) {
	this->chess_board = &chess_board;
	this->latest_moved_piece = latest_moved_piece;

	specify_file = MultiplePiecesCanGoHere(latest_moved_piece->GetColor(), new_location);
}

void ChessNotator::AfterMove() {
	Ember::IVec2 latest_move_end_spot = chess_board->GetLatestMove().size;
	Ember::IVec2 latest_move_start_spot = chess_board->GetLatestMove().pos;

	char piece_type = chess_board->board[latest_move_end_spot.y][latest_move_end_spot.x];
	piece_type = toupper(piece_type);

	char before_move_file = ColToChessFileConverter(BOARD_WIDTH - (latest_move_start_spot.x + 1) + 1);

	std::stringstream notation_buffer;

	if (specify_file) {
		notation_buffer << before_move_file;
	}

	if (chess_board->Captured()) {
		notation_buffer << CAPTURE_IDENTIFIER;
		chess_board->ResetCaptureFlag();
	}

	if (piece_type != 'P' && piece_type != ' ')
		notation_buffer << piece_type;

	if (chess_board->GetSideOnTop() == PieceColor::WHITE) {
		notation_buffer << ColToChessFileConverter(BOARD_WIDTH - (latest_move_end_spot.x + 1) + 1);
		int t = BOARD_WIDTH - (latest_move_end_spot.y + 1) + 1;
		notation_buffer << BOARD_WIDTH - (t + 1) + 2;
	}
	else {
		notation_buffer << ColToChessFileConverter(latest_move_end_spot.x + 1);
		notation_buffer << BOARD_WIDTH - (latest_move_end_spot.y + 1) + 1;
	}

	const Ember::IVec2 start_rook_pos[4] = { {0, 0}, {7, 7}, {0, 7}, {7, 0} };

	ChessPiece* king = chess_board->GetPieceFromCharacter(GetTypeCharacterFromColor('k', latest_moved_piece->GetColor()));
	if(king) {
		for (int i = 0; i < 4; i++) {
			if (latest_move_start_spot == start_rook_pos[i] && (latest_move_end_spot.x == 5 || latest_move_end_spot.x == 1) && abs(king->GetBoardPosition().x - latest_move_end_spot.x) == 1) {
				notation_buffer = std::stringstream(std::string());
				notation_buffer << "0-0";
				break;
			}
			else if (latest_move_start_spot == start_rook_pos[i] && (latest_move_end_spot.x == 3 || latest_move_end_spot.x == 4) && abs(king->GetBoardPosition().x - latest_move_end_spot.x) == 1) {
				notation_buffer = std::stringstream(std::string());
				notation_buffer << "0-0-0";
				break;
			}
		}
	}


	chess_notations.push_back(notation_buffer.str());

	if (piece_type == 'P')
		CheckForPromotion(latest_move_end_spot);
}

void ChessNotator::CheckForPromotion(const Ember::IVec2& latest_move) {
	if (latest_move.y == 8 || latest_move.y == 0)
		promotion_possible = true;
}

bool ChessNotator::MultiplePiecesCanGoHere(PieceColor color, const Ember::IVec2& attacking_square) {
	std::vector<ChessPiece*> attackers;
	for (auto& piece : chess_board->GetPieces()) {
		if (piece->GetColor() == color) {
			auto l = chess_board->GenerateLegalMoves(piece);
			for (auto& move : l) {
				if (move == attacking_square && chess_board->GetPiece(attacking_square)) 
					attackers.push_back(piece);
			}
		}
	}
	if (!attackers.empty()) {
		char check_duplicate = chess_board->board[attackers[0]->GetBoardPosition().y][attackers[0]->GetBoardPosition().x];
		for (size_t i = 1; i < attackers.size(); i++) {
			if (chess_board->board[attackers[i]->GetBoardPosition().y][attackers[i]->GetBoardPosition().x] == check_duplicate)
				return true;
			
			check_duplicate = chess_board->board[attackers[i]->GetBoardPosition().y][attackers[i]->GetBoardPosition().x];
		}
	}
	return false;
}