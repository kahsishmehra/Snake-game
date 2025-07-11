#include <iostream>
#include <raylib.h>
#include <deque>
#include <raymath.h>

using namespace std;

/*
STEPS:
1. creating a blank canvas an game loop
2. creating the food:
creating a grid, invisible to user but useful for us
3. creating the snake
4. moving the snake : we remove one cell from the bottom and add one cell in the beginning in the desired direction
5. making the snake eat the food: 
if the snake head cell == food cell, new food in a random location
6. making the snake grow taller: adding an additional segment to the beginning of the snake's body
7. checking for collision with edges and tails
8. adding title and frame
DrawRectangleLinesEx(): used to draw the outline of a rectangle with customizable thickness, color
9. keeping scores
10. adding scores
*/

Color green = {173, 204, 96, 255};
Color darkGreen = {43, 51, 24, 255};


// for the grid
int cellSize = 30;
int cellCount = 25; // grid will cover an area of 750 by 750 px
int offset = 75; // determine the size of the border

double lastUpdateTime = 0; // used to keep the track of time, intialiased to zero when the game starts

bool ElementInDeque(Vector2 element, deque<Vector2>& snakeBody)
{
    for(unsigned int i =0; i < snakeBody.size(); i ++)
    {
        if(Vector2Equals(snakeBody[i], element))
        {
            return true;
        }
    }
    return false;
}


bool eventTriggered(double interval)
{
    double currentTime = GetTime();
    // compare the difference between the current time and the last updated time
    if((currentTime - lastUpdateTime) >= interval)
    {
        lastUpdateTime = currentTime;
        return true;
    }
    return false;
}

class Food 
{
    // position on the grid, use vector2 struct by rayliv instead of x and y
public:
    Vector2 position; // 5 ad 6 a re the x, y coordinates, position.x and position.y, they represent cells and not px
    Texture2D texture;

    Food(deque <Vector2> snakeBody)
    {
        Image image = LoadImage("Graphics/food.png");
        // LoadTextureFromImage() : Texture2D, optimized datatype
        texture = LoadTextureFromImage(image);
        UnloadImage(image);
        position = GenerateRandomPos(snakeBody);
    }

    ~Food()
    {
        UnloadTexture(texture);
    }
    void Draw(){
        DrawTexture(texture, offset + position.x * cellSize, offset + position.y * cellSize, WHITE);
       
    }

    Vector2 GenerateRandomCell()
    {
        float x = GetRandomValue(0, cellCount -1);
        float y = GetRandomValue(0, cellCount -1);
        return Vector2{x,y};
    }

    Vector2 GenerateRandomPos(deque<Vector2> snakeBody)
    {
        float x = GetRandomValue(0, cellCount -1);
        float y = GetRandomValue(0, cellCount -1);
        Vector2 position = {x,y};
        while(ElementInDeque(position, snakeBody))
        {
            position = GenerateRandomCell();
        }
        return position;
    }
};

class Snake 
{
    /*Deque: is a data structure that allows you to add and remove elemets from the both ends.*/
    public:
    deque <Vector2> body = {Vector2{6,9}, Vector2{5,9}, Vector2{4,9}};
    Vector2 direction = {1,0};
    bool addSegment = false;

    void Draw()
    {
        for(unsigned int i =0; i < body.size(); i ++)
        {
            float x = body[i].x;
            float y = body[i].y;
            /*DrawRectangleRounded(rectangle rec, float roundness, int segments, color color)

            struct Rectangle {
            float x;
            float y; 
            float width;
            float height;
            } Rectangle
            */
           Rectangle segment = Rectangle{offset + x*cellSize, offset + y*cellSize, (float)cellSize, (float)cellSize};
            DrawRectangleRounded(segment, 0.5, 6, darkGreen);

        }
    }

    void Update()
    {
        body.push_front(Vector2Add(body[0],direction));
        if(addSegment == true)
        {
            addSegment = false;
        }
        else
        {
        body.pop_back();
        }

    }

    void Reset()
    {
        body = {Vector2{6,9}, Vector2{5,9}, Vector2{4,9}};
        direction = {1,0};
    }
}; 

class Game 
{
    public:
    Snake snake = Snake();
    Food food = Food(snake.body);
    bool running = true; //if the games runs and stops, tp pause the game on game over
    void Draw()
    {
        food.Draw();
        snake.Draw();

    }

    void Update()
    {
        if (running)
        {
        snake.Update();
        CheckCollisionWithFood();
        CheckCollisionWithEdges();
        CheckCollisionWithTail();
        }
    }

    void CheckCollisionWithFood()
    {
        if(Vector2Equals(snake.body[0], food.position))
        {
            food.position = food.GenerateRandomPos(snake.body);
            snake.addSegment = true;

        }
    }

    void CheckCollisionWithEdges()
    {
        if(snake.body[0].x == cellCount || snake.body[0].x == -1)
        {
            GameOver();
        }
        if(snake.body[0].y == cellCount || snake.body[0].y == -1)
        {
            GameOver();
        }
    }

    
    void GameOver()
    {
        snake.Reset();
        food.position = food.GenerateRandomPos(snake.body);
        running = false;
    }


    void CheckCollisionWithTail()
    {
        deque<Vector2> headlessBody = snake.body; // creating copy of the snake body
        headlessBody.pop_front();
        if (ElementInDeque(snake.body[0], headlessBody))
        {
            GameOver();
        }
    }

};

int main () {

    cout << "Starting the game..." <<endl;
    // GAME WINDOW
    // specify width and the height for the game window 
    InitWindow(2*offset + cellSize*cellCount, 2* offset + cellSize*cellCount, "Snake Game");
    SetTargetFPS(60); // defining the frame rate, if we dont computer will make it as fast as it can

   Game game = Game();

    // GAME LOOP
    while(WindowShouldClose() == false)
    {
        BeginDrawing(); // creates a blank canvas to draw

        if(eventTriggered(0.2))
        {
         game.Update(); 
        }// updating snake's location every 0.2 seconds


// adding keyboard keys
       if(IsKeyPressed(KEY_UP) && game.snake.direction.y != 1)
       {
        game.snake.direction = {0,-1};
        game.running = true;
       }
       if(IsKeyPressed(KEY_DOWN) && game.snake.direction.y != -1)
       {
        game.snake.direction = {0,1};
        game.running = true;
       }
       if(IsKeyPressed(KEY_LEFT) && game.snake.direction.x != 1)
       {
        game.snake.direction = {-1,0};
        game.running = true;
       }
       if(IsKeyPressed(KEY_RIGHT) && game.snake.direction.x != -1)
       {
        game.snake.direction = {1,0};
        game.running = true;
       }
       

        // drawing
        ClearBackground(green);
        DrawRectangleLinesEx(Rectangle{(float)offset-5, (float)offset-5, (float)cellSize*cellCount+10,(float) cellSize*cellCount+ 10}, 5, darkGreen);
        // DrawText(text, posX, posY, fontSize, darkGreen)
        game.Draw();
        EndDrawing(); // ends 
    }
    CloseWindow();
return 0;
}

// colors in raylib
// struct Color {red, green, blue, alpha}, a lil similar to html
// aplha is the transparency of the color