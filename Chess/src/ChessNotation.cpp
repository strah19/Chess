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

	char before_move_file = ColToChessFileConverter(latest_move_start_spot.x + 1);

	std::stringstream notation_buffer;

	if (specify_file) {
		notation_buffer << before_move_file;
	}

	if (chess_board->Captured()) {
		notation_buffer << CAPTURE_IDENTIFIER;
		chess_board->ResetCaptureFlag();
	}

	if (piece_type != 'P')
		notation_buffer << piece_type;

	notation_buffer << ColToChessFileConverter(latest_move_end_spot.x + 1);
	notation_buffer << BOARD_WIDTH - (latest_move_end_spot.y + 1) + 1;

	chess_notations.push_back(notation_buffer.str());

	if (piece_type == 'P')
		CheckForPromotion(latest_move_end_spot);
}

void ChessNotator::CheckForPromotion(const Ember::IVec2& latest_move) {
	if (latest_move.y == 8 || latest_move.y == 0)
		promotion_possible = true;
}

bool ChessNotator::MultiplePiecesCanGoHere(PieceColor color, const Ember::IVec2& attacking_square) {
	int num_of_attackers = 0;
	for (auto& piece : chess_board->GetPieces()) {
		if (piece->GetColor() == color) {
			auto l = chess_board->GenerateLegalMoves(piece);
			for (auto& move : l) {
				if (move == attacking_square && chess_board->GetPiece(attacking_square) != nullptr) {
					num_of_attackers++;
				}
			}
		}
	}

	return (num_of_attackers > 1);
}