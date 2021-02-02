#include "ChessPieces.h"
#include "ChessBoard.h"
#include <typeinfo>

ChessPiece::ChessPiece() { 
	spritesheet_location = 0;
}

bool ChessPiece::Capture(const Ember::IVec2& new_position) {
	if (board->IsTherePieceOn(new_position)) {
		if (board->GetPiece(new_position)->GetColor() != color)
			board->CapturePiece(new_position);
		else
			return false;
	}
	board->MovePiece(this, new_position);
	SetBoardPosition(new_position);
	return true;
}

void Pawn::Initialize() {
	spritesheet_location = 3;
	SeekSpriteLocation();

	two_step = false;
}

bool Pawn::MovePiece(const Ember::IVec2& new_position) {
	int direction = (color == PieceColor::BLACK) ? 1 : -1;

	Ember::Rect latest = board->GetLatestMove();
	if ((latest.pos.y == 1 || latest.pos.y == 6) && (latest.size.y == 3 || latest.size.y == 4)
		&& board_position.y == latest.size.y && new_position.x == latest .size.x &&
		new_position.y == latest.size.y + direction) {
		if (!board->IsTherePieceOn(new_position)) {
			board->MovePiece(this, new_position);
			board->CapturePiece(latest.size);
			SetBoardPosition(new_position);
			return true;
		}
	}

	if (board_position.x == new_position.x) {
		if (!two_step && new_position.y == board_position.y + direction + direction)
			return Move(new_position);
		if (new_position.y == board_position.y + direction) 
			return Move(new_position);
	}

	if (new_position.x == board_position.x + 1 || new_position.x == board_position.x - 1) {
		if (new_position.y == board_position.y + direction) {
			if (board->IsTherePieceOn(new_position)) {
				if (board->GetPiece(new_position)->GetColor() != color) {
					board->MovePiece(this, new_position);
					board->CapturePiece(new_position);
					SetBoardPosition(new_position);
					return true;
				}
				else
					return false;
			}
		}

	}

	return false;
}

bool Pawn::Move(const Ember::IVec2& new_position) {
	if (!board->IsTherePieceOn(new_position)) {
		board->MovePiece(this, new_position);
		SetBoardPosition(new_position);
		two_step = true;
		return true;
	}
	return false;
}

void Rook::Initialize() {
	spritesheet_location = 2;
	SeekSpriteLocation();
}

bool Rook::MovePiece(const Ember::IVec2& new_position) {
	if (new_position.y == board_position.y) {
		bool can_move = true;
		int low = std::min(new_position.x, board_position.x);
		int high = std::max(new_position.x, board_position.x);
		for (int i = low + 1; i < high; i++) {
			ChessPiece* piece = board->GetPiece({ i, new_position.y });
			if (piece != nullptr && piece != this) {
				can_move = false;
				break;
			}
		}

		if (can_move) {
			can_castle = false;
			return Capture(new_position);
		}
	}
	else if (new_position.x == board_position.x) {
		bool can_move = true;
		int low = std::min(new_position.y, board_position.y);
		int high = std::max(new_position.y, board_position.y);
		for (int i = low + 1; i < high; i++) {
			ChessPiece* piece = board->GetPiece({ new_position.x, i });
			if (piece != nullptr && piece != this) {
				can_move = false;
				break;
			}
		}

		if (can_move) {
			can_castle = false;
			return Capture(new_position);
		}
	}

	return false;
}

bool MoveAlongTheLines(const Ember::IVec2& range, int opposite_base, ChessPiece* piece) {
	int low = std::min(range.x, range.y);
	int high = std::max(range.x, range.y);
	for (int i = low; i < high; i++) {
		if (piece->GetBoard()->IsTherePieceOn({ i, opposite_base }))
			return false;
	}
	return true;
}

void Knight::Initialize() {
	spritesheet_location = 1;
	SeekSpriteLocation();
}

bool Knight::MovePiece(const Ember::IVec2& new_position) {
	for (int i = 0; i < 8; i++) {
		if (board_position.x + knight_move_offsets[i].x == new_position.x && board_position.y + knight_move_offsets[i].y == new_position.y)
			return Capture(new_position);
	}

	return false;
}

void Bishop::Initialize() {
	spritesheet_location = 0;
	SeekSpriteLocation();
}

bool Bishop::MovePiece(const Ember::IVec2& new_position) {
	Ember::IVec2 difference = new_position - board_position;

	difference.x = abs(difference.x);
	difference.y = abs(difference.y);

	Ember::IVec2 direction = { -1, -1 };
	if (new_position.x > board_position.x)
		direction.x = 1;
	if (new_position.y > board_position.y)
		direction.y = 1;

	if (difference.x == difference.y) {
		bool can_move = true;
		Ember::IVec2 test_spots = board_position;
		while(test_spots != new_position) {
			ChessPiece* piece = board->GetPiece(test_spots);
			if (piece != nullptr && piece != this) {
				can_move = false;
				break;
			}
			test_spots += direction;
		}
		if(can_move)
			return Capture(new_position);
	}

	return false;
}

void Queen::Initialize() {
	spritesheet_location = 5;
	SeekSpriteLocation();
}

bool Queen::MovePiece(const Ember::IVec2& new_position) {
	Ember::IVec2 difference = new_position - board_position;

	difference.x = abs(difference.x);
	difference.y = abs(difference.y);

	Ember::IVec2 direction = { -1, -1 };
	if (new_position.x > board_position.x)
		direction.x = 1;
	if (new_position.y > board_position.y)
		direction.y = 1;

	if (difference.x == difference.y) {
		bool can_move = true;
		Ember::IVec2 test_spots = board_position;
		while (test_spots != new_position) {
			ChessPiece* piece = board->GetPiece(test_spots);
			if (piece != nullptr && piece != this) {
				can_move = false;
				break;
			}
			test_spots += direction;
		}
		if (can_move)
			return Capture(new_position);
	}

	if (new_position.y == board_position.y) {
		bool can_move = true;
		int low = std::min(new_position.x, board_position.x);
		int high = std::max(new_position.x, board_position.x);
		for (int i = low + 1; i < high; i++) {
			ChessPiece* piece = board->GetPiece({ i, new_position.y });
			if (piece != nullptr && piece != this) {
				can_move = false;
				break;
			}
		}

		if (can_move)
			return Capture(new_position);
	}
	else if (new_position.x == board_position.x) {
		bool can_move = true;
		int low = std::min(new_position.y, board_position.y);
		int high = std::max(new_position.y, board_position.y);
		for (int i = low + 1; i < high; i++) {
			ChessPiece* piece = board->GetPiece({ new_position.x, i });
			if (piece != nullptr && piece != this) {
				can_move = false;
				break;
			}
		}

		if (can_move)
			return Capture(new_position);
	}

	return false;
}

void King::Initialize() {
	spritesheet_location = 4;
	SeekSpriteLocation();
}

bool King::MovePiece(const Ember::IVec2& new_position) {
	Ember::IVec2 difference = new_position - board_position;

	difference.x = abs(difference.x);
	difference.y = abs(difference.y);

	if (can_castle) {
		Rook* left_rook = nullptr;
		Rook* right_rook = nullptr;
		ChessPiece* p = board->GetPieceFromLocAndType({ 0, new_position.y }, GetTypeCharacterFromColor('r', color));
		if(p != nullptr)
			left_rook = static_cast<Rook*>(p);
		ChessPiece* p2 = board->GetPieceFromLocAndType({ BOARD_WIDTH - 1, new_position.y }, GetTypeCharacterFromColor('r', color));
		if(p2 != nullptr)
			right_rook = static_cast<Rook*>(p2);

		bool can_move = true;
		int low = std::min(new_position.y, board_position.y);
		int high = std::max(new_position.y, board_position.y);
		for (int i = low; i < high - 1; i++) {
			for (int i = low + 1; i < high; i++) {
				ChessPiece* piece = board->GetPiece({ new_position.x, i });
				if (piece != nullptr && piece != this) {
					can_move = false;
					break;
				}
			}
		}
		if (left_rook != nullptr) {
			if (left_rook->can_castle && can_castle && can_move && left_rook->board_position == new_position) {
				board->MovePiece(this, { 1, board_position.y });
				board->MovePiece(left_rook, { 2, left_rook->board_position.y });

				board_position.x = 1;
				left_rook->board_position.x = 2;
				left_rook->can_castle = false;
				can_castle = false;

				return true;
			}
		}
		if (right_rook != nullptr) {
			if (right_rook->can_castle && can_castle && can_move && right_rook->board_position == new_position) {
				board->MovePiece(this, { 5, board_position.y });
				board->MovePiece(right_rook, { 4, right_rook->board_position.y });

				board_position.x = 5;
				right_rook->board_position.x = 4;
				right_rook->can_castle = false;
				can_castle = false;

				return true;
			}
		}
	}

	if (difference.x == 1 || difference.y == 1) {
		can_castle = false;
		return Capture(new_position);
	}

	return false;
}

ChessPieceFactory::ChessPieceFactory() {
	Register('p', &Pawn::Create);
	Register('r', &Rook::Create);
	Register('b', &Bishop::Create);
	Register('n', &Knight::Create);
	Register('q', &Queen::Create);
	Register('k', &King::Create);

	Register('P', &Pawn::Create);
	Register('R', &Rook::Create);
	Register('B', &Bishop::Create);
	Register('N', &Knight::Create);
	Register('Q', &Queen::Create);
	Register('K', &King::Create);
}

void ChessPieceFactory::Register(char piece_character, CreateChessPieceFn piece_creation) {
	factory_map[piece_character] = piece_creation;
}

ChessPiece* ChessPieceFactory::CreatePiece(char piece_character) {
	FactoryMap::iterator it = factory_map.find(piece_character);
	if (it != factory_map.end()) 
		return it->second();
	return NULL;
}