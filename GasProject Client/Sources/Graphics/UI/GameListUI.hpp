#pragma once
#include "UI.hpp"

struct GameRow {
	int id;
	string title;
	int num_of_players;

	std::shared_ptr<sfg::Box> box;

	GameRow(int id, string title, int num_of_players);

	void join();
};

class GameListUI : UIModule {
private:
	sfg::Window::Ptr gamelistWindow;
	sfg::Box::Ptr gamesBox;

	std::list<uptr<GameRow>> games;

	void generateGamelistWindow();

	void update();

public:
	GameListUI(UI *ui);
	GameListUI(const GameListUI &) = delete;
	GameListUI &operator=(const GameListUI &) = delete;
	virtual ~GameListUI() = default;

	void AddGame(int id, string title, int num_of_players);
	void Clear();
	virtual void Draw() final {};

	virtual void Hide() final;
	virtual void Show() final;
};