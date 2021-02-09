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

std::vector<Ember::IVec2> ChessPiece::GenerateAllPossibleMoves() {
	std::vector<Ember::IVec2> all_possible_moves;
	for (int i = 0; i < BOARD_WIDTH; i++) 
		for (int j = 0; j < BOARD_HEIGHT; j++) 
			if (CanPieceGoHere({ j, i })) 
				all_possible_moves.push_back({ j, i });
	return all_possible_moves;
}

void Pawn::Initialize() {
	en_passant = false;
	spritesheet_location = 3;
	SeekSpriteLocation();
}

void Pawn::MovePiece() {
	int direction = (color == PieceColor::BLACK) ? 1 : -1;
	if (en_passant) {
		board->CapturePiece({ wanting_position.x, wanting_position.y - direction });
		board->MovePiece(this, wanting_position);
		SetBoardPosition(wanting_position);
	}
	if (wanting_position.x == board_position.x + 1 || wanting_position.x == board_position.x - 1) {
		if (wanting_position.y == board_position.y + direction) {
			board->CapturePiece(wanting_position);
			board->MovePiece(this, wanting_position);
			SetBoardPosition(wanting_position);
		}
	}
	else {
		board->MovePiece(this, wanting_position);
		SetBoardPosition(wanting_position);
	}
}

bool Pawn::CanPieceGoHere(const Ember::IVec2& new_position) {
	wanting_position = new_position;
	int direction = (color == PieceColor::BLACK) ? 1 : -1;

	Ember::Rect latest = board->GetLatestMove();
	if ((latest.pos.y == 1 || latest.pos.y == 6) && (latest.size.y == 3 || latest.size.y == 4)
		&& board_position.y == latest.size.y && new_position.x == latest.size.x &&
		new_position.y == latest.size.y + direction) {
		if (!board->IsTherePieceOn(new_position)) {
			en_passant = true;
			return true;
		}
	}

	if (board_position.x == new_position.x) {
		if (!board->IsTherePieceOn(new_position) && board_position.y + direction + direction == new_position.y && (board_position.y == 1 || board_position.y == 6))
			return true; 
		if (!board->IsTherePieceOn(new_position) && board_position.y + direction == new_position.y)
			return true;
	}

	if (new_position.x == board_position.x + 1 || new_position.x == board_position.x - 1) {
		if (new_position.y == board_position.y + direction) {
			if (board->IsTherePieceOn(new_position)) 
				if (board->GetPiece(new_position)->GetColor() != color) 
					return true;
		}

	}

	return false;
}

void Rook::Initialize() {
	can_castle = true;
	spritesheet_location = 2;
	SeekSpriteLocation();
}

void Rook::MovePiece() {
	if (Capture(wanting_position))
		can_castle = false;
}

bool Rook::CanPieceGoHere(const Ember::IVec2& new_position) {
	wanting_position = new_position;
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
			if (board->IsTherePieceOn(new_position)) {
				if (board->GetPiece(new_position)->GetColor() != color)
					return true;
			}
			else
				return true;
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
			if (board->IsTherePieceOn(new_position)) {
				if (board->GetPiece(new_position)->GetColor() != color)
					return true;
			}
			else
				return true;
		}
	}

	return false;
}

void Knight::Initialize() {
	spritesheet_location = 1;
	SeekSpriteLocation();
}

void Knight::MovePiece() {
	Capture(wanting_position);
}

bool Knight::CanPieceGoHere(const Ember::IVec2& new_position) {
	wanting_position = new_position;
	for (int i = 0; i < 8; i++) 
		if (board_position.x + knight_move_offsets[i].x == new_position.x && board_position.y + knight_move_offsets[i].y == new_position.y) {
			if (board->IsTherePieceOn(new_position)) {
				if (board->GetPiece(new_position)->GetColor() != color)
					return true;
			}
			else
				return true;
		}

	return false;
}

void Bishop::Initialize() {
	spritesheet_location = 0;
	SeekSpriteLocation();
}

void Bishop::MovePiece() {
	Capture(wanting_position);
}

bool Bishop::CanPieceGoHere(const Ember::IVec2& new_position) {
	wanting_position = new_position;
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
		if (can_move) {
			if (board->IsTherePieceOn(new_position)) {
				if (board->GetPiece(new_position)->GetColor() != color)
					return true;
			}
			else
				return true;
		}
	}

	return false;
}

void Queen::Initialize() {
	spritesheet_location = 5;
	SeekSpriteLocation();
}

void Queen::MovePiece() {
	Capture(wanting_position);
}

bool Queen::CanPieceGoHere(const Ember::IVec2& new_position) {
	wanting_position = new_position;
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
		if (can_move) {
			if (board->IsTherePieceOn(new_position)) {
				if (board->GetPiece(new_position)->GetColor() != color)
					return true;
			}
			else
				return true;
		}
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

		if (can_move) {
			if (board->IsTherePieceOn(new_position)) {
				if (board->GetPiece(new_position)->GetColor() != color)
					return true;
			}
			else
				return true;
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
			if (board->IsTherePieceOn(new_position)) {
				if (board->GetPiece(new_position)->GetColor() != color)
					return true;
			}
			else
				return true;
		}
	}

	return false;
}

void King::Initialize() {
	can_castle = true;
	spritesheet_location = 4;
	SeekSpriteLocation();
}

void King::MovePiece() {
	if (is_going_to_castle != NOT_CASTLE) {
		if (is_going_to_castle == LEFT_CASTLE) {
			Rook* left_rook = GetRook({ 0, board_position.y });
			board->MovePiece(this, { 1, board_position.y });
			board->MovePiece(left_rook, { 2, left_rook->board_position.y });
			board_position.x = 1;
			left_rook->board_position.x = 2;
			left_rook->can_castle = false;
			can_castle = false;
		}
		else if(is_going_to_castle == RIGHT_CASTLE) {
			Rook* right_rook = GetRook({ BOARD_WIDTH - 1, board_position.y });
			board->MovePiece(this, { 6, board_position.y });
			board->MovePiece(right_rook, { 5, right_rook->board_position.y });

			board_position.x = 6;
			right_rook->board_position.x = 5;
			right_rook->can_castle = false;
			can_castle = false;
		}
		is_going_to_castle = NOT_CASTLE;
	}
	else {
		can_castle = false;
		Capture(wanting_position);
	}
}

bool King::CanPieceGoHere(const Ember::IVec2& new_position) {
	wanting_position = new_position;
	Ember::IVec2 difference = new_position - board_position;

	difference.x = abs(difference.x);
	difference.y = abs(difference.y);

	if (can_castle) {
		Rook* left_rook = GetRook({ 0, new_position.y });
		Rook* right_rook = GetRook({ BOARD_WIDTH - 1, new_position.y });
		
		bool can_move = false;
		if (new_position == Ember::IVec2(0, board_position.y) || new_position == Ember::IVec2(BOARD_WIDTH - 1, board_position.y)) {
			can_move = true;
			int low = std::min(new_position.x, board_position.x);
			int high = std::max(new_position.x, board_position.x);
			for (int i = low + 1; i < high; i++) {
				ChessPiece* piece = board->GetPiece({ i, new_position.y });
				if (piece != nullptr && piece != this) {
					can_move = false;
					break;
				}
			}
		}
		if (left_rook != nullptr) {
			if (left_rook->can_castle && can_castle && can_move && left_rook->board_position == Ember::IVec2(new_position.x, new_position.y)) {
				is_going_to_castle = LEFT_CASTLE;
				return true;
			}
		}
		if (right_rook != nullptr) {
			if (right_rook->can_castle && can_castle && can_move && right_rook->board_position == Ember::IVec2(new_position.x, new_position.y)) {
				is_going_to_castle = RIGHT_CASTLE;
				return true;
			}
		}
	}

	if (difference.x <= 1 && difference.y <= 1 && difference.x >= -1 && difference.y >= -1) {
		if (board->IsTherePieceOn(new_position)) {
			if (board->GetPiece(new_position)->GetColor() != color)
				return true;
		}
		else
			return true;
	}
	return false;
}

Rook* King::GetRook(const Ember::IVec2& rook_position) {
	ChessPiece* piece = board->GetPieceFromLocAndType(rook_position, GetTypeCharacterFromColor('r', color));
	if (piece != nullptr)
		 return static_cast<Rook*>(piece);
	return nullptr;
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