#include "Core/Application.h"
#include "Assets/Texture.h"
#include "Animation/Spritesheet.h"

#include "ChessPieces.h"
#include "ChessBoard.h"
#include "SpritesheetParser.h"

int BOARD_SQUARE_WIDTH = 64;
int BOARD_SQUARE_HEIGHT = 64;

class Chess : public Ember::Application {
public:
	void OnCreate() {
		chess_piece_texture.Initialize("ChessPixelArt/Figures1.png", renderer);

		SpritesheetParser parser;
		chess_piece_sheet.SetDividers(parser.Read("ChessPixelArt/spritesheetconfig.txt"));
		properties->name = "Chess - Player 1";
		SDL_Surface* icon = Ember::Texture::LoadSurface("chess/black_king.png");
		SDL_SetWindowIcon(window->GetNativeWindow(), icon);
		SDL_FreeSurface(icon);
		window->SetResizeable(true);
	}

	void OnUserUpdate() {
		window->Update();

		BOARD_SQUARE_WIDTH = properties->width / BOARD_WIDTH;
		BOARD_SQUARE_HEIGHT = properties->height / BOARD_HEIGHT;

		renderer->Clear(background_color);

		Ember::IVec2 cursor = events->MousePosition();
		if (events->Clicked() && events->ButtonId() == Ember::ButtonIds::LeftMouseButton) {
			if (selected)
				UpdatePieces();
			selected = !selected;
		}
		if (!selected) {
			selected_square = { cursor.x / BOARD_SQUARE_WIDTH, cursor.y / BOARD_SQUARE_HEIGHT };
			selected_square_for_new_piece = { cursor.x / BOARD_SQUARE_WIDTH, cursor.y / BOARD_SQUARE_HEIGHT };
		}
		if (selected)
			selected_square_for_new_piece = { cursor.x / BOARD_SQUARE_WIDTH, cursor.y / BOARD_SQUARE_HEIGHT };

		bool light_square = true;
		for (int i = 0; i < BOARD_WIDTH; i++) {
			for (int j = 0; j < BOARD_HEIGHT; j++) {
				Ember::Color square_color = (light_square) ? Ember::Color({ 89, 89, 89, 255 }) : Ember::Color({ 54, 54, 54, 255 });
				light_square = !light_square;

				renderer->Rectangle(Ember::Rect({ i * BOARD_SQUARE_WIDTH, j * BOARD_SQUARE_HEIGHT, BOARD_SQUARE_WIDTH, BOARD_SQUARE_HEIGHT }), square_color);
				renderer->Border(Ember::Rect({ i * BOARD_SQUARE_WIDTH, j * BOARD_SQUARE_HEIGHT, BOARD_SQUARE_WIDTH, BOARD_SQUARE_HEIGHT }), { 0, 0, 0, 100 });
			}
			light_square = !light_square;
		}

		for(auto& piece : chess_board.GetPieces())
			DrawPiece(*piece);

		renderer->Rectangle(Ember::Rect({ selected_square.x * BOARD_SQUARE_WIDTH, selected_square.y * BOARD_SQUARE_HEIGHT, BOARD_SQUARE_WIDTH, BOARD_SQUARE_HEIGHT }), { 240, 240, 50, 50 });
		renderer->Rectangle(Ember::Rect({ selected_square_for_new_piece.x * BOARD_SQUARE_WIDTH, selected_square_for_new_piece.y * BOARD_SQUARE_HEIGHT, 
										  BOARD_SQUARE_WIDTH, BOARD_SQUARE_HEIGHT }), { 240, 240, 50, 50 });

		renderer->Show();
	}

	void UpdatePieces() {
		ChessPiece* current_selected_piece = chess_board.GetPiece(selected_square);
		if (current_selected_piece != nullptr) {
			if (current_selected_piece->GetColor() == current_player_color) {
				if (current_selected_piece->MovePiece(selected_square_for_new_piece)) {
					player = !player;
					if (player) {
						properties->name = "Chess - Player 2";
						current_player_color = PieceColor::BLACK;
					}
					else {
						properties->name = "Chess - Player 1";
						current_player_color = PieceColor::WHITE;
					}
				}
			}
		}
		
		ChessPiece* king = chess_board.GetPieceFromCharacter(GetTypeCharacterFromColor('k', current_player_color));

		if (king == nullptr) {
			std::cout << "GAME OVER\n";
		}
		else {
			for (auto& piece : chess_board.GetPieces()) {
				Ember::IVec2 original = piece->GetBoardPosition();
				
			}
		}
	}

	void DrawPiece(ChessPiece& chess_piece) {
		chess_piece_sheet.SetSelected(chess_piece.GetSpritesheetLocation());
		chess_piece_texture.Draw(Ember::Rect({ chess_piece.GetBoardPosition().x * BOARD_SQUARE_WIDTH, 
											   chess_piece.GetBoardPosition().y * BOARD_SQUARE_HEIGHT, 
											   BOARD_SQUARE_WIDTH, BOARD_SQUARE_HEIGHT }), 
			                                   chess_piece_sheet.ReturnSourceRect());
		renderer->Border(Ember::Rect({ chess_piece.GetBoardPosition().x * BOARD_SQUARE_WIDTH,
											   chess_piece.GetBoardPosition().y * BOARD_SQUARE_HEIGHT,
											   BOARD_SQUARE_WIDTH, BOARD_SQUARE_HEIGHT }),
												{ 255, 100, 100, 150 });
	}

	void UserDefEvent(Ember::Event& event) {
		Ember::EventDispatcher dispatch(&event);
	}
private:
	Ember::Color background_color = { 0, 0, 0, 255 };

	ChessBoard chess_board;

	Ember::Texture chess_piece_texture;
	UserDividedSpritesheet chess_piece_sheet;

	bool selected = false;
	Ember::IVec2 selected_square = { 0, 0 };
	Ember::IVec2 selected_square_for_new_piece = { 0, 0 };

	PieceColor current_player_color = PieceColor::WHITE;
	bool player = false;
};

int main(int argc, char** argv) {
	Chess chess;
	chess.Initialize("Chess", false, BOARD_WIDTH * BOARD_SQUARE_WIDTH, BOARD_HEIGHT * BOARD_SQUARE_HEIGHT);

	chess.Run();

	return 0;
}