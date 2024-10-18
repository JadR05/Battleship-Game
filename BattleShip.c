#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define GRID_SIZE 10
#define WATER '~'
#define HIT '*'
#define MISS 'o'
#define CARRIER_SIZE 5
#define BATTLESHIP_SIZE 4
#define DESTROYER_SIZE 3
#define SUBMARINE_SIZE 2
#define EASY 1
#define HARD 2

// Variables to track 
int radarSweep1 = 3, radarSweep2 = 3;
int sunkShips1 = 0, sunkShips2 = 0;
int readyArtilleries1 = 0, readyArtilleries2 = 0;
int readyTorpedo1 = 0, readyTorpedo2 = 0;

struct Ship {
    char name[20];
    int size;
};

struct Ship ships[] = {
    {"Carrier", 5},
    {"Battleship", 4},
    {"Destroyer", 3},
    {"Submarine", 2},
};

void initializeGrid(char grid[10][10]) {
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            grid[i][j] = '~';
        }
    }
}

void displayGrid(char grid[10][10]) {
    printf("   A B C D E F G H I J\n");
    for (int i = 0; i < 10; i++) {
        if (i == 9) {
            printf("%d ", i + 1);
        } else {
            printf("%2d ", i + 1);
        }
        for (int j = 0; j < 10; j++) {
            printf("%c ", grid[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

void clearScreen() {
    for (int i = 0; i < 50; i++) {
        printf("\n");
    }
}

void placeShip(char grid[10][10], struct Ship ship, char playerName[20]) {
    int x;
    int direction;
    int validPlacement = 0;

    while (validPlacement != 1) {
        printf("%s, place %s of size: %d on the grid\n", playerName, ship.name, ship.size);
        printf("Enter starting position (example: A5): ");
        char col;
        scanf(" %c%d", &col, &x);

        col = col - 'A'; 
        x--;              

        printf("Choose a direction (0 for horizontal, 1 for vertical): ");
        scanf("%d", &direction);

        if (direction == 0 && col + ship.size <= 10) {  
            int overlap = 0;
            for (int i = 0; i < ship.size; i++) {
                if (grid[x][col + i] != '~') {
                    overlap = 1;
                    break;
                }
            }
            if (!overlap) {
                for (int i = 0; i < ship.size; i++) {
                    grid[x][col + i] = 'S';
                }
                validPlacement = 1;
            } else {
                printf("Invalid placement, overlapping with another ship!\n");
            }
        } else if (direction == 1 && x + ship.size <= 10) {  
            int overlap = 0;
            for (int i = 0; i < ship.size; i++) {
                if (grid[x + i][col] != '~') {
                    overlap = 1;
                    break;
                }
            }
            if (!overlap) {
                for (int i = 0; i < ship.size; i++) {
                    grid[x + i][col] = 'S';
                }
                validPlacement = 1;
            } else {
                printf("Invalid placement, overlapping with another ship!\n");
            }
        } else {
            printf("Invalid placement, out of bounds!\n");
        }
    }
}

void playerPlaceShips(char grid[10][10], char playerName[20]) {
    printf("\n");

    for (int i = 0; i < 4; i++) {
        displayGrid(grid);  
        placeShip(grid, ships[i], playerName);
    }
    clearScreen();  
}

int getDifficulty() {
    int choice;
    puts("Welcome to our Battleship Game.\n");
    puts("Select the difficulty level:\n1 (Easy)\n2 (Hard)\n");
    scanf("%d", &choice);

    if (choice == 1) {
        puts("Hits(*) and Misses(o) will be shown.\n");
    } else {
        puts("Only Hits(*) will be shown.\n");
    }
    return choice;
}

void getNames(char name1[20], char name2[20]) {
    printf("Player 1:\nEnter your name: ");
    scanf("%s", name1);

    printf("\n");

    printf("Player 2:\nEnter your name: ");
    scanf("%s", name2);

    printf("\n");
}

int randomChooser(char name1[20], char name2[20]) {
    srand(time(0));
    int x = rand() % 2;
    if (x == 0) {
        printf("%s starts first.\n\n", name1);
        return 0;
    } else {
        printf("%s starts first.\n\n", name2);
        return 1;
    }
}

void processMove(char grid[10][10], char opponentGrid[10][10], char playerName[20],int difficulty, int *radarSweep, int *sunkShips, int *readyArtilleries, int *readyTorpedo){

}

void fire(char grid[10][10], char playerName[20], int x, int y, int difficulty){
    if(grid[x][y] == 'S'){
        grid[x][y] = '*';
        printf("%s fired at %c%d. Hit!\n", playerName, y + 'A', x+1);
    }else{
        if(difficulty == 1){
            grid[x][y] = 'o';
        }
    }
}

void radarSweep(char grid[10][10], char playerName[20], int x, int y, int *radarSweep){
    if(*radarSweep){
        int found = 0;
        for(int i = x; i< x+2 && i<10;i++){
            for(int j = y; j< y+2 && j<10;j++){
                if(grid[i][j] == 'S'){
                    found = 1;
                    break;
                }
            }
        }
        (*radarSweep)--;
        if(found){
            printf("Enemy ships found in radar area.\n");
        }else{
            printf("No enemy ships found.\n");
        }
    }else{
        printf("No radar uses left.\n");
    }
}

void smokeScreen(char grid[10][10], int *sunkShips, int x, int y){
    if(*sunkShips > 0){
        for(int i = x; i< x+2 && i<10;i++){
            for(int j = y; j< y+2 && j<10;j++){
                grid[i][j] == 'X';
            }
        }
        (*sunkShips)--;
        clearScreen();
    }else{
        printf("No available smoke screens.\n");
    }
}

void artillery(char grid[10][10], char playerName[20], int row, int col, int *artilleryReady, int difficulty) {
    if (*artilleryReady == 0) {
        printf("Artillery is not ready.\n");
        return;
    }

    int hit = 0;
    printf("%s fires artillery at %c%d!\n", playerName, 'A' + col, row + 1);
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            int tempRow = row + i;
            int tempCol = col + j;

            if (row < 10 && col < 10) {
                fire(grid, playerName, tempRow, tempCol, difficulty);            
            }
        }
    }

    if (!hit) {
        printf("%s's artillery missed.\n", playerName);
    }

    *artilleryReady = 0;
}


int main() {
    char name1[20], name2[20];
    char grid1[10][10], grid2[10][10];

    int difficulty = getDifficulty();
    getNames(name1, name2);

    int startingPlayer = randomChooser(name1, name2);

    initializeGrid(grid1);
    initializeGrid(grid2);

    if(startingPlayer==0){
        playerPlaceShips(grid1, name1);
        playerPlaceShips(grid2, name2);
    }else{
        playerPlaceShips(grid1, name2);
        playerPlaceShips(grid2, name1);
    }

    int currentPlayer = startingPlayer;
    while(sunkShips1 < 4 && sunkShips2 < 4){
        if(currentPlayer == 0){
            processMove(grid1, grid2, name1, difficulty, &radarSweep1, &sunkShips1, &readyArtilleries1,&readyTorpedo1);
            currentPlayer = 1;
        }else{
            processMove(grid2, grid1, name2, difficulty, &radarSweep2, &sunkShips2, &readyArtilleries2,&readyTorpedo2);
            currentPlayer = 0;
        }
    }

    if(sunkShips1 == 4){
        printf("%s wins.\n", name2);
    }else{
        printf("%s wins.\n", name1);
    }
    
    return 0;
}
