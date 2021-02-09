#include "Core/Application.h"
#include "Assets/Texture.h"
#include "Animation/Spritesheet.h"

#include "ChessPieces.h"
#include "ChessBoard.h"
#include "SpritesheetParser.h"

int BOARD_SQUARE_WIDTH = 64;
int BOARD_SQUARE_HEIGHT = 64;
const std::string application_name = "Chess Engine";
const Ember::IVec2 side_padding = { 200, 200 };
const Ember::IVec2 top_down_padding = { 10, 200 };

class Chess : public Ember::Application {
public:
	void OnCreate() {
		chess_piece_texture.Initialize("ChessPixelArt/Figures1.png", renderer);

		SpritesheetParser parser;
		chess_piece_sheet.SetDividers(parser.Read("ChessPixelArt/spritesheetconfig.txt"));
		properties->name = application_name + " - Player 1";
		SDL_Surface* icon = Ember::Texture::LoadSurface("chess/black_king.png");
		SDL_SetWindowIcon(window->GetNativeWindow(), icon);
		SDL_FreeSurface(icon);
	}

	void ConsterVec(Ember::IVec2& vec) {
		if (vec.x < 0)
			vec.x = 0;
		if (vec.y < 0)
			vec.y = 0;
		if (vec.x > BOARD_WIDTH - 1)
			vec.x = BOARD_WIDTH - 1;
		if (vec.y > BOARD_HEIGHT - 1)
			vec.y = BOARD_HEIGHT - 1;
	}

	void OnUserUpdate() {
		window->Update();

		renderer->Clear(background_color);

		if (!game_over && !stalemate) {
			Ember::IVec2 cursor = events->MousePosition();
			if (events->Clicked() && events->ButtonId() == Ember::ButtonIds::LeftMouseButton) {
				UpdatePieces();
				selected = !selected;
			}
			if (!selected) {
				selected_square = { (cursor.x - side_padding.x) / BOARD_SQUARE_WIDTH, cursor.y / BOARD_SQUARE_HEIGHT };
				selected_square_for_new_piece = { (cursor.x - side_padding.x) / BOARD_SQUARE_WIDTH, (cursor.y - top_down_padding.x) / BOARD_SQUARE_HEIGHT };
			}
			if (selected)
				selected_square_for_new_piece = { (cursor.x - side_padding.x) / BOARD_SQUARE_WIDTH, (cursor.y - top_down_padding.x) / BOARD_SQUARE_HEIGHT };

			ConsterVec(selected_square);
			ConsterVec(selected_square_for_new_piece);
		}

		bool light_square = true;
		for (int i = 0; i < BOARD_WIDTH; i++) {
			for (int j = 0; j < BOARD_HEIGHT; j++) {
				Ember::Color square_color = (light_square) ? Ember::Color({ 173,189,143, 255 }) : Ember::Color({ 111,143,114, 255 });
				light_square = !light_square;

				renderer->Rectangle(Ember::Rect({ side_padding.x + i * BOARD_SQUARE_WIDTH,  top_down_padding.x + j * BOARD_SQUARE_HEIGHT, BOARD_SQUARE_WIDTH, BOARD_SQUARE_HEIGHT }), square_color);
				renderer->Border(Ember::Rect({ side_padding.x + i * BOARD_SQUARE_WIDTH,  top_down_padding.x + j * BOARD_SQUARE_HEIGHT, BOARD_SQUARE_WIDTH, BOARD_SQUARE_HEIGHT }), { 0, 0, 0, 100 });
			}
			light_square = !light_square;
		}

		for(auto& piece : chess_board.GetPieces())
			DrawPiece(*piece);

		renderer->Rectangle(Ember::Rect({ side_padding.x + selected_square.x * BOARD_SQUARE_WIDTH,
			top_down_padding.x + selected_square.y * BOARD_SQUARE_HEIGHT,
			BOARD_SQUARE_WIDTH, BOARD_SQUARE_HEIGHT }), { 240, 240, 50, 50 });
		
		renderer->Rectangle(Ember::Rect({ side_padding.x + selected_square_for_new_piece.x * BOARD_SQUARE_WIDTH,  top_down_padding.x + selected_square_for_new_piece.y * BOARD_SQUARE_HEIGHT,
									BOARD_SQUARE_WIDTH, BOARD_SQUARE_HEIGHT }), { 240, 240, 50, 50 });


		for (auto& move : all_possible_moves_for_current_selected_piece) {
			renderer->Rectangle(Ember::Rect({ side_padding.x + move.x * BOARD_SQUARE_WIDTH, top_down_padding.x + move.y * BOARD_SQUARE_HEIGHT, BOARD_SQUARE_WIDTH, BOARD_SQUARE_HEIGHT }), { 66, 184, 182, 100 });
		}

		if (game_over) {
			renderer->Rectangle(Ember::Rect({ 0, 0, properties->width, properties->height }), { 0, 0, 0, 100 });
			properties->name = "Checkmate!";
		}
		if (stalemate) {
			renderer->Rectangle(Ember::Rect({ 0, 0, properties->width, properties->height }), { 0, 0, 0, 100 });
			properties->name = "Stalemate!";
		}

		renderer->Show();
	}

	void UpdatePieces() {
		ChessPiece* current_selected_piece = chess_board.GetPiece(selected_square);
		if (current_selected_piece != nullptr) {
			if (current_selected_piece->GetColor() == current_player_color) {
				CalculatePiecesAction(current_selected_piece);

				if (chess_board.Checkmate(PieceColor::WHITE) || chess_board.Checkmate(PieceColor::BLACK))
					game_over = true;
				if (!game_over) {
					Stalemate();
				}
			}
		}
	}

	void Stalemate() {
		int count = 0;

		for (auto& piece : chess_board.GetPieces())
			if (piece->GetColor() == current_player_color) {
				if (chess_board.LegalMovesInCheck(piece).empty()) {
					count++;
				}
			}
		if (chess_board.GetNumberOfPieces(current_player_color) == count)
			stalemate = true;
	}

	void CalculatePiecesAction(ChessPiece* current_selected_piece) {
		all_possible_moves_for_current_selected_piece = chess_board.LegalMovesInCheck(current_selected_piece);
		if (current_selected_piece->CanPieceGoHere(selected_square_for_new_piece)) {
			for (auto& move : all_possible_moves_for_current_selected_piece) {
				if (selected_square_for_new_piece == move) {
					current_selected_piece->SetMovingLocation(selected_square_for_new_piece);
					current_selected_piece->MovePiece();
					OnPieceMovement();
					break;
				}
			}
		}
	}

	void OnPieceMovement() {
		player = !player;
		all_possible_moves_for_current_selected_piece.clear(); 
		if (player) {
			properties->name = application_name + " - Player 2";
			current_player_color = PieceColor::BLACK;
		}
		else {
			properties->name = application_name + " - Player 1";
			current_player_color = PieceColor::WHITE;
		}
	}

	void Reset() {
		game_over = false;
		properties->name = "Chess - Player 1";
		current_player_color = PieceColor::WHITE;
		chess_board.Reset();
		player = false;
		selected = false;
		stalemate = false;
		selected_square = { 0, 0 };
		selected_square_for_new_piece = { 0, 0 };
	}

	void DrawPiece(ChessPiece& chess_piece) {
		chess_piece_sheet.SetSelected(chess_piece.GetSpritesheetLocation());
		chess_piece_texture.Draw(Ember::Rect({ side_padding.x + chess_piece.GetBoardPosition().x * BOARD_SQUARE_WIDTH,
											   top_down_padding.x + chess_piece.GetBoardPosition().y * BOARD_SQUARE_HEIGHT,
											   BOARD_SQUARE_WIDTH, BOARD_SQUARE_HEIGHT }), 
			                                   chess_piece_sheet.ReturnSourceRect());
	}

	bool Keyboard(Ember::KeyboardEvents& keyboard) {
		if (keyboard.scancode == Ember::EmberKeyCode::R) {
			Reset();
		}
		if (keyboard.scancode == Ember::EmberKeyCode::LeftAlt)
			chess_board.Log();
		return true;
	}

	void UserDefEvent(Ember::Event& event) {
		Ember::EventDispatcher dispatch(&event);
		dispatch.Dispatch<Ember::KeyboardEvents>(EMBER_BIND_FUNC(Keyboard));
	}
private:
	Ember::Color background_color = { 50, 55, 55, 200 };

	ChessBoard chess_board;

	Ember::Texture chess_piece_texture;
	UserDividedSpritesheet chess_piece_sheet;

	bool selected = false;
	Ember::IVec2 selected_square = { 0, 0 };
	Ember::IVec2 selected_square_for_new_piece = { 0, 0 };

	std::vector<Ember::IVec2> all_possible_moves_for_current_selected_piece;
	PieceColor current_player_color = PieceColor::WHITE;
	bool player = false;
	bool game_over = false;
	bool stalemate = false;
};

int main(int argc, char** argv) {
	Chess chess;
	chess.Initialize(application_name, false, BOARD_WIDTH * BOARD_SQUARE_WIDTH + side_padding.x + side_padding.y, BOARD_HEIGHT * BOARD_SQUARE_HEIGHT + top_down_padding.x + top_down_padding.y);

	chess.Run();

	return 0;
}