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

	virtual void Initialize() { }
	virtual bool MovePiece(const Ember::IVec2& new_position) { return false; }
	bool Capture(const Ember::IVec2& new_position);
protected:
	ChessBoard* board;
	PieceColor color = PieceColor::NONE;
	size_t spritesheet_location;
	Ember::IVec2 board_position;

	void SeekSpriteLocation() {
		if (color == PieceColor::BLACK)
			spritesheet_location += 6;
	}

};

typedef ChessPiece* (__stdcall* CreateChessPieceFn)(void);

class Pawn : public ChessPiece {
public:
	virtual void Initialize() override;
	virtual bool MovePiece(const Ember::IVec2& new_position) override;

	static ChessPiece* __stdcall Create() { return new Pawn(); }
private:
	bool two_step;
	bool Move(const Ember::IVec2& new_position);
};

class King;

class Rook : public ChessPiece {
public:
	virtual void Initialize() override;
	virtual bool MovePiece(const Ember::IVec2& new_position) override;

	static ChessPiece* __stdcall Create() { return new Rook(); }
private:
	bool can_castle = true;
	friend class King;
};

class Knight : public ChessPiece {
public:
	virtual void Initialize() override;
	virtual bool MovePiece(const Ember::IVec2& new_position) override;

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
	virtual bool MovePiece(const Ember::IVec2& new_position) override;

	static ChessPiece* __stdcall Create() { return new Bishop(); }
private:
};

class Queen : public ChessPiece {
public:
	virtual void Initialize() override;
	virtual bool MovePiece(const Ember::IVec2& new_position) override;

	static ChessPiece* __stdcall Create() { return new Queen(); }
private:
};

class King : public ChessPiece {
public:
	virtual void Initialize() override;
	virtual bool MovePiece(const Ember::IVec2& new_position) override;

	static ChessPiece* __stdcall Create() { return new King(); }
private: 
	bool can_castle = true;
};

bool MoveAlongTheLines(const Ember::IVec2& range, int opposite_base, ChessPiece* piece);

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
