#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>


// Variables to track 
int sunkShips1 = 0, sunkShips2 = 0;
int radarSweep1 = 3, radarSweep2 = 3;
int smokeScreen1 = 0 , smokeScreen2 = 0; 
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

void placeShip(char grid[10][10], struct Ship ship, char playerName[10]) {
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

void playerPlaceShips(char grid[10][10], char playerName[10]) {
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

void getNames(char name1[10], char name2[10]) {
    printf("Player 1:\nEnter your name: ");
    scanf("%s", name1);

    printf("\n");

    printf("Player 2:\nEnter your name: ");
    scanf("%s", name2);

    printf("\n");
}

int randomChooser(char name1[10], char name2[10]) {
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

int isPartOfShip(char grid[10][10],int row,int col,int shipsize){
    for(int i = 0;i<shipsize;i++){
        if(col + i<10 && grid[row][col+i] == 'S') return 1;
        if(row + i<10 && grid[row+i][col] == 'S') return 1;
    }

    return 0; 
}

void Checkifsunk(char grid[10][10], struct Ship ships[], int *sunkShips,int *smokeScreen, int *readyArtilleries, int *readyTorpedo, char playerName[10]){
    int shipSizes[] = {5,4,3,2};
    int shipcounters[] = {0,0,0,0};

    for(int i = 0;i<10;i++){
        for(int j = 0; j<10;j++){
            if(grid[i][j]=='*'){

                for(int k = 0; k<4;k++){
                    if(shipSizes[k] != 0 && isPartOfShip(grid,i,j,shipSizes[k])){
                        shipcounters[k]++;
                    }
                }

            }
        }
    }

    for(int k = 0;k<4; k++){
        if(shipcounters[k] == shipSizes[k]){
            printf("%s's %s has been sunk!\n", playerName, ships[k].name);
            (*sunkShips)++;
            (*smokeScreen)++;
            (*readyArtilleries) = 1;
            shipSizes[k] = 0;
            if(*sunkShips == 3){
                *readyTorpedo = 1;
            }
        }
    }
}

void processMove(char grid[10][10], char opponentGrid[10][10], char playerName[10],int difficulty,int *sunkShips, int *radarSweep, int *smokeScreen, int *readyArtilleries, int *readyTorpedo){
    displayGrid(opponentGrid);
    printf("The posssible moves are: \n. fire\n");
    if(*radarSweep>0){
        printf(". Radar\n");
    }
    if(*smokeScreen>0){
        printf(". Smoke\n");
    }
    if(*readyArtilleries == 1){
        printf(". Artillery\n");
    }
    if(*readyTorpedo == 1){
        printf(". Torpedo\n");
    }

    (*readyArtilleries) = 0;
    (*readyTorpedo) = 0;
    Checkifsunk(grid,ships,sunkShips,smokeScreen,readyArtilleries,readyTorpedo,playerName);
    displayGrid(opponentGrid);
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

void radarSweep(char grid[10][10], int x, int y, int *radarSweep){
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

void smokeScreen(char grid[10][10], int *smokeScreen, int x, int y){
    if(*smokeScreen > 0){
        for(int i = x; i< x+2 && i<10;i++){
            for(int j = y; j< y+2 && j<10;j++){
                grid[i][j] = 'X';
            }
        }
        (*smokeScreen)--;
        clearScreen();
    }else{
        printf("No available smoke screens.\n");
    }
}

void artillery(char grid[10][10], char playerName[20], int row, int col, int *artilleryReady, int difficulty) {
    if (*artilleryReady) {
        for (int i = 0; i < row + 2 && i<10; i++) {
            for (int j = 0; j < col + 2 && j<10; j++) {
                fire(grid, playerName, i, j, difficulty);            
            }
        }
            *artilleryReady = 0;
    }
    else{
        printf("Artillery not avialable.");
    }
}

void torpedo(char grid[10][10], char playerName[10], int roworcol, int *readyTorpedo, int difficulty){
    if(*readyTorpedo){
        if(roworcol>='A' && roworcol<='J'){
            int col = roworcol -'A';
            for(int i = 0;i<10;i++){
                fire(grid,playerName,i,col,difficulty);
            }
        }else if(roworcol>='1' && roworcol<='9'){
            int row = roworcol-'1';
            for(int i = 0;i<10;i++){
                fire(grid,playerName,row,i,difficulty);
            }
            *readyTorpedo = 0;
        }
    }else{
        printf("No available torpedo.\n");
    }
}


int main() {
    char name1[10], name2[10];
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
            processMove(grid1, grid2, name1, difficulty,&sunkShips1, &radarSweep1, &smokeScreen1, &readyArtilleries1,&readyTorpedo1);
            currentPlayer = 1;
        }else{
            processMove(grid2, grid1, name2, difficulty, &sunkShips2, &radarSweep2, &smokeScreen2, &readyArtilleries2,&readyTorpedo2);
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