#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>


// Variables to track 
int sunkShips1 = 0, sunkShips2 = 0;
int radarSweep1 = 3, radarSweep2 = 3;
int smokeScreen1 = 0 , smokeScreen2 = 0; 
int readyArtilleries1 = 0, readyArtilleries2 = 0;
int readyTorpedo1 = 0, readyTorpedo2 = 0;
int usedTorpedo1 = 0, usedTorpedo2 = 0;
int hits1[4] = {0,0,0,0}; 
int hits2[4] = {0,0,0,0};
int sunkShipsFlags1[4] = {0, 0, 0, 0};
int sunkShipsFlags2[4] = {0, 0, 0, 0};


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

int getDifficulty() {
    char input[10];
    int choice;
    puts("Welcome to our Battleship Game.\n");
    puts("Select the difficulty level:\n. Easy\n. Hard\n");
    scanf("%s", input);

    for (int i = 0; input[i]; i++) {
        input[i] = tolower(input[i]);
    }

    if (strcmp(input, "easy") == 0) {
        puts("Hits(*) and Misses(o) will be shown.\n");
        choice = 1;
    } else if (strcmp(input, "hard") == 0) {
        puts("Only Hits(*) will be shown.\n");
        choice = 2;
    } else {
        puts("Invalid choice, defaulting to Easy.\n");
        choice = 1; 
    }

    return choice;
}

void getNames(char name1[10],char name2[10]) {
    printf("Player 1:\nEnter your name: ");
    scanf("%s", name1);
    printf("\n");

    printf("Player 2:\nEnter your name: ");
    scanf("%s", name2);
    printf("\n");
}

int randomChooser(char name1[10],char name2[10]) {
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

void clearScreen() {
    for (int i = 0; i < 50; i++) {
        printf("\n");
    }
}

void placeShip(char grid[10][10],struct Ship ship,char playerName[10]) {
    char input[20];
    int validPlacement = 0;

    while (validPlacement != 1) {
        printf("%s, place %s of size %d on the grid\n", playerName, ship.name, ship.size);
        printf("Enter the starting position and direction (example: A5,Horizontal): ");
        if (fgets(input, sizeof(input), stdin) == NULL) {
            printf("Error reading input. Please try again.\n");
            continue; 
        }

        input[strcspn(input, "\n")] = 0;

        char *token = strtok(input, ",");
        if (token == NULL) {
            printf("Invalid input format. Please use the format: B3,Horizontal\n");
            continue;
        }

        char col = token[0]; 
        int x = atoi(token + 1); 
        col = toupper(col) - 'A'; 
        x--; 

        token = strtok(NULL, ",");
        if (token == NULL) {
            printf("Invalid input format. Please specify the direction (Horizontal or Vertical).\n");
            continue; 
        }
        char *direction = token; 

        int isHorizontal = (strcmp(direction, "Horizontal") == 0);
        int isVertical = (strcmp(direction, "Vertical") == 0);

        if (isHorizontal) {
            if (col + ship.size > 10) { 
                printf("Invalid placement, out of bounds horizontally!\n");
                continue; 
            }
            int overlap = 0;
            for (int i = 0; i < ship.size; i++) {
                if (grid[x][col + i] != '~') { 
                    overlap = 1;
                    break;
                }
            }
            if (!overlap) {
                for (int i = 0; i < ship.size; i++) {
                    grid[x][col + i] = ship.name[0]; 
                }
                validPlacement = 1; 
            } else {
                printf("Invalid placement, overlapping with another ship!\n");
            }
        } else if (isVertical) {
            if (x + ship.size > 10) { 
                printf("Invalid placement, out of bounds vertically!\n");
                continue; 
            }
            int overlap = 0;
            for (int i = 0; i < ship.size; i++) {
                if (grid[x + i][col] != '~') { 
                    overlap = 1;
                    break;
                }
            }
            if (!overlap) {
                for (int i = 0; i < ship.size; i++) {
                    grid[x + i][col] = ship.name[0]; 
                }
                validPlacement = 1; 
            } else {
                printf("Invalid placement, overlapping with another ship!\n");
            }
        } else {
            printf("Invalid direction! Please enter 'Horizontal' or 'Vertical'.\n");
        }
    }
}

void playerPlaceShips(char grid[10][10], char playerName[10]) {
    printf("\n");

    for (int i = 0; i < 4; i++) { 
        placeShip(grid, ships[i], playerName);
        printf("\n");
        displayGrid(grid); 
    }
    sleep(1);
    clearScreen();  
}


void fire(char grid[10][10],char opponentGrid[10][10],char playerName[10],int row,int col,int difficulty){
    if(grid[row][col] == '~'){
        if(difficulty == 1){
            opponentGrid[row][col] = 'o';
        }
        printf("Miss!\n");
    }else if(grid[row][col] == '*'){
        printf("You already hit this spot.\n");
    }else{
        opponentGrid[row][col] = '*';
        printf("Hit!\n");
    }
}

void radar(char grid[10][10],int row,int col,int *radarSweep){
    int found = 0;
    for(int i = row; i< row+2 && i<10;i++){
        for(int j = col; j< col+2 && j<10;j++){
            if(grid[row][col] == 'C' || grid[row][col] == 'B' || grid[row][col] == 'D' || grid[row][col] == 'S'){
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
}

void smoke(char grid[10][10],int row,int col,int *smokeScreen){
    for(int i = row; i< row+2 && i<10;i++){
        for(int j = col; j< col+2 && j<10;j++){
            grid[i][j] = 'X';
        }
    }
    (*smokeScreen)--;
}

void artillery(char grid[10][10],char opponentGrid[10][10],char playerName[10],int row,int col,int *artilleryReady,int difficulty) {
    for (int i = row; i < row + 2 && i<10; i++) {
        for (int j = col; j < col + 2 && j<10; j++) {
           fire(grid,opponentGrid,playerName,i,j,difficulty);            
        }
    }
}

void torpedo(char grid[10][10],char opponentGrid[10][10],char playerName[10],char target[],int *readyTorpedo,int difficulty){
    if(target[0]>='A' &&  target[0]<='J'){
        int col = target[0] -'A';
        for(int i = 0;i<10;i++){
            fire(grid,opponentGrid,playerName,i,col,difficulty);
        }
    }else {
        int row = atoi(target);
        if (row >= 1 && row <= 10){
            row--;
            for(int i = 0;i<10;i++){
                fire(grid,opponentGrid,playerName,row,i,difficulty);
            }
        } 
    }
}

void Checkifsunk(char grid[10][10],char opponentGrid[10][10],struct Ship ships[],int *sunkShips,int *smokeScreen,int *readyArtilleries,int *readyTorpedo,char playerName[10],char opponentName[10],int hits[4],int sunkShipsFlag[4]){
    for(int i = 0;i<10;i++){
        for(int j = 0; j<10;j++){
            if(opponentGrid[i][j]=='*'){
                switch(grid[i][j]){
                    case 'C':
                        hits[0]++;
                        break;
                    case 'B':
                        hits[1]++;
                        break;
                    case 'D':
                        hits[2]++;
                        break;
                    case 'S':
                        hits[3]++;
                        break;
                }
                    grid[i][j] = '*';
                }
            }
        }
    
    for(int k = 0;k<4; k++){
        if(sunkShipsFlag[k]==0){
            if(hits[k]==ships[k].size){
                printf("%s: You have succeded in sinking %s's %s Ship\n\n",playerName, opponentName, ships[k].name);
                (*sunkShips)++;
                (*smokeScreen)++;
                (*readyArtilleries) = 1;
                sunkShipsFlag[k] = 1;
                if(*sunkShips == 3){
                    (*readyTorpedo) = 1;
                }
            }
        }
    }
}



void processMove(char grid[10][10], char opponentGrid[10][10],char myGrid[10][10],char playerName[10], char opponentName[10], int difficulty, int *sunkShips, int *radarSweep, int *smokeScreen, int *readyArtilleries, int *readyTorpedo,int *usedTorpedo,int hits[4], int sunkShipsFlag[4]) {
    displayGrid(opponentGrid);
    printf("The possible moves are:\n. Fire [coordinates]\n");
    if (*radarSweep > 0) {
        printf(". Radar [top-left coordinate]\n");
    }
    if (*smokeScreen > 0) {
        printf(". Smoke [top-left coordinate]\n");
    }
    if (*readyArtilleries == 1) {
        printf(". Artillery [top-left coordinate]\n");
    }
    if (*readyTorpedo == 1) {
        printf(". Torpedo [row/column]\n");
    }

    char command[50];
    printf("%s, your move: ", playerName);
    fgets(command, sizeof(command), stdin);

    int length = strlen(command);
    for (int i = 0; i < length; i++) {
        command[i] = toupper(command[i]);
    }

    char move[20];
    char target[5];
    int row, col;

    if (sscanf(command, "%s %s", move, target) == 2) {
        if (strcmp(move, "TORPEDO") == 0 && strlen(target) == 1) {
            if ((*readyTorpedo) == 1 && (*usedTorpedo) == 0) {
                torpedo(grid, opponentGrid, playerName, target, readyTorpedo, difficulty);
                (*usedTorpedo) = 1;
            } else {
                printf("Torpedo not available.\n");
            }
        } else {
            col = target[0] - 'A';

            if (strlen(target) == 2) {
                row = target[1] - '1';
            } else if (strlen(target) == 3 && target[1] == '1' && target[2] == '0') {
                row = 9;
            } else {
                printf("Invalid target format. Use a letter and number (ex. B5) or single letter for torpedo.\n");
                (*readyArtilleries) = 0;
                (*readyTorpedo) = 0;
                return;
            }

            if (row < 0 || row >= 10 || col < 0 || col >= 10) {
                printf("Coordinates out of bounds.\n");
                (*readyArtilleries) = 0;
                (*readyTorpedo) = 0;
                return;
            }

            if (strcmp(move, "FIRE") == 0) {
                fire(grid, opponentGrid, playerName, row, col, difficulty);
            } else if (strcmp(move, "RADAR") == 0) {
                if ((*radarSweep) > 0) {
                    radar(grid, row, col, radarSweep);
                } else {
                    printf("No radar sweeps available.\n");
                }
            } else if (strcmp(move, "SMOKE") == 0) {
                if ((*smokeScreen) > 0) {
                    smoke(myGrid, row, col, smokeScreen);
                } else {
                    printf("No smoke screens available.\n");
                }
            } else if (strcmp(move, "ARTILLERY") == 0) {
                if ((*readyArtilleries) == 1) {
                    artillery(grid, opponentGrid, playerName, row, col, readyArtilleries, difficulty);
                } else {
                    printf("Artillery not available.\n");
                }
            } else {
                printf("Invalid move. You lost your turn.\n");
            }
        }  
        displayGrid(opponentGrid);
        (*readyArtilleries) = 0;
        (*readyTorpedo) = 0;
        Checkifsunk(grid, opponentGrid, ships, sunkShips, smokeScreen, readyArtilleries, readyTorpedo, playerName, opponentName, hits, sunkShipsFlag);
    } else {
        printf("Invalid command format.\n");
        (*readyArtilleries) = 0;
        (*readyTorpedo) = 0;
    }
}

int main() {
    char name1[10], name2[10];
    char grid1[10][10], grid2[10][10];// Grids for placing ships
    char grid3[10][10], grid4[10][10];// Grids for playing the game
 
    int difficulty = getDifficulty();
    getNames(name1, name2);

    int startingPlayer = randomChooser(name1, name2);

    initializeGrid(grid1);// Player 1 hidden grid
    initializeGrid(grid2);// Player 2 hidden grid
    initializeGrid(grid3);// Player 1 view of Player 2 grid
    initializeGrid(grid4);// Player 2 view of Player 1 grid

   getchar();
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
            processMove(grid2,grid3,grid1,name1,name2,difficulty,&sunkShips1,&radarSweep1,&smokeScreen1,&readyArtilleries1,&readyTorpedo1,&usedTorpedo1,hits1,sunkShipsFlags1);
            currentPlayer = 1;
        }else{
            processMove(grid1,grid4,grid2,name2,name1,difficulty,&sunkShips2,&radarSweep2,&smokeScreen2,&readyArtilleries2,&readyTorpedo2,&usedTorpedo2,hits2,sunkShipsFlags2);
            currentPlayer = 0;
        }
    }

    if(sunkShips1 == 4){
        printf("%s wins.\n", name1);
    }else{
        printf("%s wins.\n", name2);
    }
    
    return 0;
}