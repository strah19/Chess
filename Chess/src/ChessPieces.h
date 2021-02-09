#ifndef CHESS_PIECES_H
#define CHESS_PIECES_H

#include <Ember.h>
#include <map>

enum class PieceColor {
	NONE = 0, BLACK = 1, WHITE = 2
};

class ChessBoard;

class ChessPiece {
public:
	ChessPiece();

	size_t GetSpritesheetLocation() const { return spritesheet_location; }
	Ember::IVec2 GetBoardPosition() const { return board_position; }
	PieceColor GetColor() const { return color; }
	ChessBoard* GetBoard() const { return board; }

	void SetColor(PieceColor c) { color = c; }
	void SetBoardPosition(const Ember::IVec2& position) { board_position = position; }
	void SetParentBoard(ChessBoard* board) { this->board = board; }
	void SetMovingLocation(const Ember::IVec2& position) { wanting_position = position; }

	virtual void Initialize() { }
	virtual bool CanPieceGoHere(const Ember::IVec2& new_position) { return false; }
	virtual void MovePiece() { }
	virtual std::vector<Ember::IVec2> GenerateAllPossibleMoves();
	bool Capture(const Ember::IVec2& new_position);
protected:
	ChessBoard* board;
	PieceColor color = PieceColor::NONE;
	size_t spritesheet_location;
	Ember::IVec2 board_position;
	Ember::IVec2 wanting_position;

	void SeekSpriteLocation() {
		if (color == PieceColor::BLACK)
			spritesheet_location += 6;
	}

};

typedef ChessPiece* (__stdcall* CreateChessPieceFn)(void);

class Pawn : public ChessPiece {
public:
	virtual void Initialize() override;
	virtual void MovePiece() override;
	virtual bool CanPieceGoHere(const Ember::IVec2& new_position) override;

	static ChessPiece* __stdcall Create() { return new Pawn(); }
private:
	bool en_passant = false;
};

class King;

class Rook : public ChessPiece {
public:
	virtual void Initialize() override;
	virtual void MovePiece() override;
	virtual bool CanPieceGoHere(const Ember::IVec2& new_position) override;

	static ChessPiece* __stdcall Create() { return new Rook(); }
private:
	bool can_castle = true;
	friend class King;
};

class Knight : public ChessPiece {
public:
	virtual void Initialize() override;
	virtual void MovePiece() override;
	virtual bool CanPieceGoHere(const Ember::IVec2& new_position) override;

	static ChessPiece* __stdcall Create() { return new Knight(); }
private:
	const Ember::IVec2 knight_move_offsets[8] = {
		{ 1, -2 }, { 2, -1 }, { 2, 1 }, { 1, 2 },
		{ -1, 2 }, { -2, 1 }, { -2, -1 }, { -1, -2}
	};
};

class Bishop : public ChessPiece {
public:
	virtual void Initialize() override;
	virtual void MovePiece() override;
	virtual bool CanPieceGoHere(const Ember::IVec2& new_position) override;

	static ChessPiece* __stdcall Create() { return new Bishop(); }
private:
};

class Queen : public ChessPiece {
public:
	virtual void Initialize() override;
	virtual void MovePiece() override;
	virtual bool CanPieceGoHere(const Ember::IVec2& new_position) override;

	static ChessPiece* __stdcall Create() { return new Queen(); }
private:
};

class King : public ChessPiece {
public:
	virtual void Initialize() override;
	virtual void MovePiece() override;
	virtual bool CanPieceGoHere(const Ember::IVec2& new_position) override;

	static ChessPiece* __stdcall Create() { return new King(); }
private: 
	bool can_castle = true;

	const int NOT_CASTLE = 0;
	const int LEFT_CASTLE = 1;
	const int RIGHT_CASTLE = 2;

	int is_going_to_castle = NOT_CASTLE;

	Rook* GetRook(const Ember::IVec2& rook_position);
};

class ChessPieceFactory {
public:
	~ChessPieceFactory() { factory_map.clear(); }
	static ChessPieceFactory* Get() {
		static ChessPieceFactory instance;
		return &instance;
	}

	void Register(char piece_character, CreateChessPieceFn piece_creation);
	ChessPiece* CreatePiece(char piece_character);
private:
	ChessPieceFactory();
	ChessPieceFactory(const ChessPieceFactory&) { }
	ChessPieceFactory& operator=(const ChessPieceFactory&) { return *this; }

	typedef std::map<char, CreateChessPieceFn> FactoryMap;
	FactoryMap factory_map;
};

#endif // !CHESS_PIECES_H
