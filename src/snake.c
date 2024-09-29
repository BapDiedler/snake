#include <SDL2/SDL.h>
#include <stdlib.h>
#include <time.h>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define GRID_SIZE 20
#define INITIAL_SNAKE_LENGTH 5

// Directions
enum Direction { UP, DOWN, LEFT, RIGHT };

// Segment du serpent
typedef struct {
    int x, y;
} Segment;

typedef struct {
    Segment* body;
    int length;
    enum Direction dir;
} Snake;

// Créer la nourriture
Segment food;

// Initialiser le serpent
void initSnake(Snake* snake) {
    snake->length = INITIAL_SNAKE_LENGTH;
    snake->dir = RIGHT;
    snake->body = malloc(snake->length * sizeof(Segment));
    
    // Positionner le serpent au milieu de l'écran
    for (int i = 0; i < snake->length; i++) {
        snake->body[i].x = snake->length - i - 1;
        snake->body[i].y = 0;
    }
}

// Placer la nourriture à un endroit aléatoire
void placeFood() {
    food.x = rand() % (SCREEN_WIDTH / GRID_SIZE);
    food.y = rand() % (SCREEN_HEIGHT / GRID_SIZE);
}

// Déplacer le serpent
void moveSnake(Snake* snake) {
    // Déplacer chaque segment vers la position du précédent
    for (int i = snake->length - 1; i > 0; i--) {
        snake->body[i] = snake->body[i - 1];
    }

    // Déplacer la tête du serpent dans la direction actuelle
    switch (snake->dir) {
        case UP:
            snake->body[0].y--;
            break;
        case DOWN:
            snake->body[0].y++;
            break;
        case LEFT:
            snake->body[0].x--;
            break;
        case RIGHT:
            snake->body[0].x++;
            break;
    }
}

// Vérifier les collisions avec les murs ou le serpent lui-même
int checkCollision(Snake* snake) {
    Segment head = snake->body[0];
    
    // Vérifier les collisions avec les murs
    if (head.x < 0 || head.x >= SCREEN_WIDTH / GRID_SIZE || head.y < 0 || head.y >= SCREEN_HEIGHT / GRID_SIZE) {
        return 1; // Collision détectée
    }

    // Vérifier les collisions avec le corps
    for (int i = 1; i < snake->length; i++) {
        if (head.x == snake->body[i].x && head.y == snake->body[i].y) {
            return 1; // Collision détectée
        }
    }

    return 0;
}

// Manger la nourriture
int eatFood(Snake* snake) {
    Segment head = snake->body[0];
    if (head.x == food.x && head.y == food.y) {
        snake->length++;
        snake->body = realloc(snake->body, snake->length * sizeof(Segment));
        return 1; // Nourriture mangée
    }
    return 0;
}

// Nettoyer les ressources
void cleanup(Snake* snake) {
    free(snake->body);
}

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window* window = SDL_CreateWindow("Snake SDL", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    srand(time(NULL));

    Snake snake;
    initSnake(&snake);
    placeFood();

    int running = 1;
    SDL_Event event;
    int frameDelay = 100; // Délai de chaque frame (en ms)
    Uint32 frameStart, frameTime;

    while (running) {
        frameStart = SDL_GetTicks();

        // Gérer les événements
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            } else if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_UP:
                        if (snake.dir != DOWN) snake.dir = UP;
                        break;
                    case SDLK_DOWN:
                        if (snake.dir != UP) snake.dir = DOWN;
                        break;
                    case SDLK_LEFT:
                        if (snake.dir != RIGHT) snake.dir = LEFT;
                        break;
                    case SDLK_RIGHT:
                        if (snake.dir != LEFT) snake.dir = RIGHT;
                        break;
                }
            }
        }

        // Déplacer le serpent
        moveSnake(&snake);

        // Vérifier les collisions
        if (checkCollision(&snake)) {
            running = 0;
            break;
        }

        // Vérifier si le serpent mange la nourriture
        if (eatFood(&snake)) {
            placeFood();
        }

        // Dessiner
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Effacer l'écran (noir)
        SDL_RenderClear(renderer);

        // Dessiner le serpent
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // Couleur verte pour le serpent
        for (int i = 0; i < snake.length; i++) {
            SDL_Rect rect = { snake.body[i].x * GRID_SIZE, snake.body[i].y * GRID_SIZE, GRID_SIZE, GRID_SIZE };
            SDL_RenderFillRect(renderer, &rect);
        }

        // Dessiner la nourriture
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Couleur rouge pour la nourriture
        SDL_Rect foodRect = { food.x * GRID_SIZE, food.y * GRID_SIZE, GRID_SIZE, GRID_SIZE };
        SDL_RenderFillRect(renderer, &foodRect);

        // Afficher l'écran
        SDL_RenderPresent(renderer);

        // Délai pour maintenir une vitesse constante
        frameTime = SDL_GetTicks() - frameStart;
        if (frameDelay > frameTime) {
            SDL_Delay(frameDelay - frameTime);
        }
    }

    // Nettoyer les ressources
    cleanup(&snake);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
