#include "Game.h"
Game::Game()
{
	hero = new Player(Vector2f(2.5f, 2.0f),    //position
		              Vector2f(0.0f, 1.0f),    //direction
		              Vector2f(-0.66f, 0.0f)); //plane


	Vector2i screen_size = renderer.get_screen_size();
	window = new RenderWindow(sf::VideoMode(screen_size.x + 1, screen_size.y), "Adventure 3D");
	window->setFramerateLimit(60);


	hasFocus = true;

	check_map_for_errors();
}
Game::~Game()
{
	delete window;
	delete hero;
}
void Game::check_map_for_errors()
{
	//if the map is not correct, we can have segmentation faults. So check it.
	if (!level.check_for_errors()) {
		cout << stderr << "map is invalid!";
		Time sleep_t = seconds(5.0f);
		sleep(sleep_t);
		exit(-1);
	}
}
void Game::handle_SFML_events()
{
	sf::Event event;
	while (window->pollEvent(event))
	{
		switch (event.type) {
		case sf::Event::Closed:
			window->close();
			break;
		case sf::Event::LostFocus:
			hasFocus = false;
			break;
		case sf::Event::GainedFocus:
			hasFocus = true;
			break;
		default:
			break;
		}
	}
}
void Game::run()
{
	while (window->isOpen()) 
	{
		fps_counter.count();
		handle_SFML_events();

		float dt = fps_counter.get_delta_time();
		hero->move(hasFocus, dt, level); //also check can hero move
		hero->rotate(hasFocus, dt);


		window->clear();
		renderer.draw(*hero, level, window);
		fps_counter.draw(window);
		fps_counter.update_frame_time();
		window->display();
	}
}