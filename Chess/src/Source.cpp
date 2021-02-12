#include "Core/Application.h"
#include "Assets/Texture.h"
#include "Animation/Spritesheet.h"
#include "Assets/Font.h"
#include "Layer.h"

#include "ChessPieces.h"
#include "ChessBoard.h"
#include "SpritesheetParser.h"
#include "ChessNotation.h"

int BOARD_SQUARE_WIDTH = 64;
int BOARD_SQUARE_HEIGHT = 64;
const std::string application_name = "Chess Engine";
const Ember::IVec2 side_padding = { 200, 200 };
const Ember::IVec2 top_down_padding = { 100, 200 };

class Chess : public Ember::Application {
public:
	void OnCreate() {
		chess_piece_texture.Initialize("res/sprites.png", renderer);

		SpritesheetParser parser;
		chess_piece_sheet.SetDividers(parser.Read("res/properconfig.txt"));
		properties->name = application_name + " - Player 1";
		SDL_Surface* icon = Ember::Texture::LoadSurface("chess/black_king.png");
		SDL_SetWindowIcon(window->GetNativeWindow(), icon);
		SDL_FreeSurface(icon);

		font.Initialize(renderer, "res/Inconsolata-Regular.ttf", 16);

		stack.AddLayer("Menu");
		stack.AddLayer("MainPlay");
		stack.SetCurrentLayer("MainPlay");
	}

	~Chess() {
		if(!notator.GetNotations().empty())
			SaveMoves();
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

		stack.DrawEventToLayer(EMBER_BIND_FUNC(Menu), "Menu");
		stack.DrawEventToLayer(EMBER_BIND_FUNC(MainPlay), "MainPlay");

		renderer->Show();
	}

	void Menu() {
		
	}

	void MainPlay() {
		if (!game_over && !stalemate) {
			Ember::IVec2 cursor = events->MousePosition();
			if (events->Clicked() && events->ButtonId() == Ember::ButtonIds::LeftMouseButton) {
				if (selected)
					all_possible_moves_for_current_selected_piece.clear();
				UpdatePieces();
				selected = !selected;
			}
			if (!selected) {
				selected_square = { (cursor.x - side_padding.x) / BOARD_SQUARE_WIDTH, (cursor.y - top_down_padding.x) / BOARD_SQUARE_HEIGHT };
				selected_square_for_new_piece = { (cursor.x - side_padding.x) / BOARD_SQUARE_WIDTH, (cursor.y - top_down_padding.x) / BOARD_SQUARE_HEIGHT };
			}
			if (selected) {
				selected_square_for_new_piece = { (cursor.x - side_padding.x) / BOARD_SQUARE_WIDTH, (cursor.y - top_down_padding.x) / BOARD_SQUARE_HEIGHT };
			}
			ConsterVec(selected_square);
			ConsterVec(selected_square_for_new_piece);
		}

		bool light_square = true;
		for (int i = 0; i < BOARD_WIDTH; i++) {
			for (int j = 0; j < BOARD_HEIGHT; j++) {
				Ember::Color square_color = (light_square) ? Ember::Color({ 124, 76, 62, 255 }) : Ember::Color({ 81, 42, 42, 255 });
				light_square = !light_square;

				renderer->Rectangle(Ember::Rect({ side_padding.x + i * BOARD_SQUARE_WIDTH,  top_down_padding.x + j * BOARD_SQUARE_HEIGHT, BOARD_SQUARE_WIDTH, BOARD_SQUARE_HEIGHT }), square_color);
				renderer->Border(Ember::Rect({ side_padding.x + i * BOARD_SQUARE_WIDTH,  top_down_padding.x + j * BOARD_SQUARE_HEIGHT, BOARD_SQUARE_WIDTH, BOARD_SQUARE_HEIGHT }), { 0, 0, 0, 100 });
			}
			light_square = !light_square;
		}

		for (auto& piece : chess_board.GetPieces())
			DrawPiece(*piece);

		renderer->Rectangle(Ember::Rect({ side_padding.x + selected_square.x * BOARD_SQUARE_WIDTH,
			top_down_padding.x + selected_square.y * BOARD_SQUARE_HEIGHT,
			BOARD_SQUARE_WIDTH, BOARD_SQUARE_HEIGHT }), { 240, 240, 50, 50 });

		renderer->Rectangle(Ember::Rect({ side_padding.x + selected_square_for_new_piece.x * BOARD_SQUARE_WIDTH,  top_down_padding.x + selected_square_for_new_piece.y * BOARD_SQUARE_HEIGHT,
									BOARD_SQUARE_WIDTH, BOARD_SQUARE_HEIGHT }), { 240, 240, 50, 50 });


		for (auto& move : all_possible_moves_for_current_selected_piece) {
			renderer->Rectangle(Ember::Rect({ side_padding.x + move.x * BOARD_SQUARE_WIDTH, top_down_padding.x + move.y * BOARD_SQUARE_HEIGHT, BOARD_SQUARE_WIDTH, BOARD_SQUARE_HEIGHT }), { 214, 250, 115, 100 });
		}

		if(chess_board.GetSideOnTop() == PieceColor::BLACK)
			for (int i = 0; i < BOARD_HEIGHT; i++) {
				SideSymbols(i, i);
			}
		else {
			int norm_count = 0;
			for (int i = BOARD_HEIGHT - 1; i >= 0; i--) {
				SideSymbols(i, norm_count);
				norm_count++;
			}
		}

		int offset = 0;
		for (auto& chess_note : notator.GetNotations()) {
			font.SetPosition(10, 10 + offset);
			font.UpdateText(chess_note);
			font.UpdateColor({ 255, 255, 255, 255 });
			font.SetStyle(0);
			font.Render();
			offset += 20;
		}
		
		int new_line = 0;
		for (int i = 0; i < BOARD_WIDTH; i++) {
			for (int j = 0; j < BOARD_HEIGHT; j++) {
				font.SetPosition(10 + (j * 10), 400 + new_line);
				font.UpdateText(chess_board.board[i][j]);
				font.UpdateColor({ 255, 255, 255, 255 });
				font.SetStyle(0);
				font.Render();
			}
			new_line += 20;
		}

		if (game_over) {
			renderer->Rectangle(Ember::Rect({ 0, 0, properties->width, properties->height }), { 0, 0, 0, 100 });
			properties->name = "Checkmate!";
		}
		if (stalemate) {
			renderer->Rectangle(Ember::Rect({ 0, 0, properties->width, properties->height }), { 0, 0, 0, 100 });
			properties->name = "Stalemate!";
		}
	}

	void SideSymbols(int i, int text_index) {
		font.SetPosition(side_padding.x - 10, top_down_padding.x + (BOARD_HEIGHT * BOARD_SQUARE_HEIGHT) - (i * BOARD_SQUARE_HEIGHT) - BOARD_SQUARE_HEIGHT);
		font.UpdateText(text_index + 1);
		font.UpdateColor({ 255, 255, 255, 255 });
		font.SetStyle(1);
		font.Render();

		font.SetPosition(side_padding.x + i * BOARD_SQUARE_WIDTH, top_down_padding.x + (BOARD_HEIGHT * BOARD_SQUARE_HEIGHT));
		font.UpdateText(ColToChessFileConverter(text_index + 1));
		font.UpdateColor({ 255, 255, 255, 255 });
		font.SetStyle(1);
		font.Render();
	}

	void UpdatePieces() {
		if (!promote_pawn) {
			ChessPiece* current_selected_piece = chess_board.GetPiece(selected_square);
			if (current_selected_piece != nullptr) {
				if (current_selected_piece->GetColor() == current_player_color) {
					CalculatePiecesAction(current_selected_piece);

					if (chess_board.Checkmate(PieceColor::WHITE) || chess_board.Checkmate(PieceColor::BLACK)) {
						//notator.GetNotations().back().append("#");
						game_over = true;
					}
					else if (!game_over) {
						Stalemate();
					}
				}
			}
		}
	}

	void Stalemate() {
		int count = 0;

		for (auto& piece : chess_board.GetPieces())
			if (piece->GetColor() == current_player_color) {
				if (chess_board.GenerateLegalMoves(piece).empty()) {
					count++;
				}
			}
		if (chess_board.GetNumberOfPieces(current_player_color) == count)
			stalemate = true;
	}

	void CalculatePiecesAction(ChessPiece* current_selected_piece) {
		all_possible_moves_for_current_selected_piece = chess_board.GenerateLegalMoves(current_selected_piece);
		if (current_selected_piece->CanPieceGoHere(selected_square_for_new_piece)) {
			for (auto& move : all_possible_moves_for_current_selected_piece) {
				if (selected_square_for_new_piece == move) {
					notator.BeforeMove(chess_board, current_selected_piece, selected_square_for_new_piece);
					current_selected_piece->SetMovingLocation(selected_square_for_new_piece);
					//char type = chess_board.board[selected_square.y][selected_square.x];
					current_selected_piece->MovePiece();
					notator.AfterMove();

					/*
					char before_move_file = ColToChessFileConverter(selected_square.x + 1);
					std::stringstream ss;
					type = toupper(type);
					if (type != 'P')
						ss << type;
					if (chess_board.Captured()) {
						if (type == 'P')
							ss << before_move_file << 'x';
						else
							ss << 'x';
						chess_board.ResetCaptureFlag();
					}

					ss << ColToChessFileConverter(selected_square_for_new_piece.x + 1);
					int correct = selected_square_for_new_piece.y + 1;
					correct = BOARD_HEIGHT - correct + 1;
					ss << correct;

					chess_notation_moves.push_back(ss.str());

					Ember::Rect late = chess_board.GetLatestMove();
					if (type == 'K') {
						if (late.x == 0  && late.w == 3)
							chess_notation_moves.back() = "0-0-0";
						if (late.x == 7 && late.w == 5)
							chess_notation_moves.back() = "0-0";

					}

					if (type == 'P') {
						if (late.h == 8 || late.h == 0) {
							std::cout << "Promote!\n";
							promote_pawn = true;
							current = current_selected_piece;
						}
					}
					*/

					OnPieceMovement();


					//if (chess_board.Check(current_player_color))
					//	chess_notation_moves.back().append("+");

					break;
				}
			}
		}
	}

	void SaveMoves() {
		Cinder::CFSFile file("moves.txt");
		const int NUM_OF_MOVES_IN_LINE = 10;
		file.Write("NEW GAME: ");
		int c = 0;
		for (auto& move : notator.GetNotations()) {
			file.Write(move);
			file.Write(' ');
			c++;
			if (c == NUM_OF_MOVES_IN_LINE) {
				file.Write('\n');
				c = 0;
			}
		}
		file.Write('\n');
		notator.GetNotations().clear();
		file.CloseFile();
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
		SaveMoves();

		for (auto& n : need_deleting) {
			chess_board.GetPieces().erase(chess_board.GetPieces().begin() + n);
		}
		need_deleting.clear();
	}

	void DrawPiece(ChessPiece& chess_piece) {
		if (chess_piece.GetBoardPosition() != Ember::IVec2(-1, -1)) {
			chess_piece_sheet.SetSelected(chess_piece.GetSpritesheetLocation());
			chess_piece_texture.Draw(Ember::Rect({ side_padding.x + chess_piece.GetBoardPosition().x * BOARD_SQUARE_WIDTH,
												   top_down_padding.x + chess_piece.GetBoardPosition().y * BOARD_SQUARE_HEIGHT,
												   BOARD_SQUARE_WIDTH, BOARD_SQUARE_HEIGHT }),
				chess_piece_sheet.ReturnSourceRect());
		}
	}

	bool Keyboard(Ember::KeyboardEvents& keyboard) {
		if (keyboard.scancode == Ember::EmberKeyCode::S) {
			Reset();
		}
		if (keyboard.scancode == Ember::EmberKeyCode::LeftAlt)
			chess_board.Log();
		if (keyboard.scancode == Ember::EmberKeyCode::Q && promote_pawn) {
			Promote('q', 'Q');
		}
		if (keyboard.scancode == Ember::EmberKeyCode::B && promote_pawn) {
			Promote('b', 'B');
		}
		if (keyboard.scancode == Ember::EmberKeyCode::R && promote_pawn) {
			Promote('r', 'R');
		}
		if (keyboard.scancode == Ember::EmberKeyCode::K && promote_pawn) {
			Promote('n', 'N');
		}
		if (keyboard.scancode == Ember::EmberKeyCode::F) {
			chess_board.Flip();
		}
		return true;
	}

	void Promote(char lower, char upper) {
		if (current != nullptr) {
			current->SetBoardPosition({ -1, -1 });
			chess_board.GetPieces().push_back(ChessPieceFactory::Get()->CreatePiece((current_player_color == PieceColor::WHITE) ? lower : upper));
			chess_board.GetPieces().back()->SetColor((current_player_color == PieceColor::WHITE) ? PieceColor::BLACK : PieceColor::WHITE);
			chess_board.GetPieces().back()->SetBoardPosition(Ember::IVec2(chess_board.GetLatestMove().w, chess_board.GetLatestMove().h));
			chess_board.GetPieces().back()->SetParentBoard(&chess_board);
			chess_board.GetPieces().back()->Initialize();
			chess_board.board[chess_board.GetLatestMove().w][chess_board.GetLatestMove().h] = (current_player_color == PieceColor::WHITE) ? lower : upper;
			need_deleting.push_back(chess_board.GetPieces().size() - 1);
		}
		promote_pawn = false;
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

//	std::vector<std::string> chess_notation_moves;

	ChessPiece* current;
	bool promote_pawn = false;
	Ember::Font font;
	std::vector<int> need_deleting;

	ChessNotator notator;

	std::string menu[4] = { "Play", "Settings", "About", "Quit" };
	Ember::EventStack stack;
};

int main(int argc, char** argv) {
	Chess chess;
	chess.Initialize(application_name, false, BOARD_WIDTH * BOARD_SQUARE_WIDTH + side_padding.x + side_padding.y, BOARD_HEIGHT * BOARD_SQUARE_HEIGHT + top_down_padding.x + top_down_padding.y);

	chess.Run();

	return 0;
}