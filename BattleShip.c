#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>

#define Grid_Size 10
#define Easy 1
#define Hard 2
#define player 0
#define numOfShips 4

int sunkShips1 = 0, sunkShips2 = 0;
int radarSweep1 = 3, radarSweep2 = 3;
int smokeScreen1 = 0, smokeScreen2 = 0;
int readyArtilleries1 = 0, readyArtilleries2 = 0;
int readyTorpedo1 = 0, readyTorpedo2 = 0;
int usedTorpedo1 = 0, usedTorpedo2 = 0;
int hits1[4] = {0, 0, 0, 0};
int hits2[4] = {0, 0, 0, 0};
int sunkShipsFlags1[4] = {0, 0, 0, 0};
int sunkShipsFlags2[4] = {0, 0, 0, 0};

struct Ship{
    char name[20];
    int size;
};

struct Ship ships[] = 
{
    {"Carrier", 5},
    {"Battleship", 4},
    {"Destroyer", 3},
    {"Submarine", 2},
};

typedef struct coordinates Coordinates;
struct coordinates{
    int row;
    int col;
};

void initializeGrid(char grid[Grid_Size][Grid_Size]){
    for(int i = 0; i < Grid_Size; i++){
        for(int j = 0; j < Grid_Size; j++){
            grid[i][j] = '~';
        }
    }
}

void displayGrid(char grid[Grid_Size][Grid_Size]){
    printf("   A B C D E F G H I J\n");
    for(int i = 0; i < Grid_Size; i++){
        if(i == 9){
            printf("%d ", i + 1);
        }else{
            printf("%2d ", i + 1);
        }
        for(int j = 0; j < Grid_Size; j++){
            printf("%c ", grid[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

int getDifficulty(){
    char input[10];
    int choice;
    puts("Welcome to our Battleship Game.\nYou will be playing against a bot.\n");
    puts("First, select the difficulty level:\n. Easy\n. Hard\n");
    scanf("%s", input);

    for(int i = 0; input[i]; i++){
        input[i] = tolower(input[i]);
    }

    if(strcmp(input, "easy") == 0){
        puts("Hits(*) and Misses(o) will be shown.\n");
        choice = Easy;
    }else if(strcmp(input, "hard") == 0){
        puts("Only Hits(*) will be shown.\n");
        choice = Hard;
    }else{
        puts("Invalid choice, defaulting to Easy.\n");
        choice = Easy;
    }

    return choice;
}

void getPlayerName(char playerName[10]){
    printf("Second, enter your name: ");
    scanf("%s", playerName);
    printf("\n");
}

int randomChooser(char playerName[10]){
    int x = rand() % 2;
    if(x == player){
        printf("%s, you will start first.\n\n", playerName);
        return 0;
    }else{
        printf("Bot will start first.\n\n");
        return 1;
    }
}

void clearScreen(){
    for(int i = 0; i < 50; i++){
        printf("\n");
    }
}

void placeShip(char grid[Grid_Size][Grid_Size], struct Ship ship, char playerName[10]){
    char input[20];
    int validPlacement = 0;

    while(validPlacement != 1){
        printf("%s, place %s of size %d on the grid\n", playerName, ship.name, ship.size);
        printf("Enter the starting position and direction (example: A5,Horizontal): ");
        if(fgets(input, sizeof(input), stdin) == NULL){
            printf("Error reading input. Please try again.\n");
            continue;
        }

        input[strcspn(input, "\n")] = 0;

        char *token = strtok(input, ",");
        if(token == NULL){
            printf("Invalid input format. Please use the format: B3,Horizontal\n");
            continue;
        }

        char col = token[0];
        int x = atoi(token + 1);
        col = toupper(col) - 'A';
        x--;

        token = strtok(NULL, ",");
        if(token == NULL){
            printf("Invalid input format. Please specify the direction (Horizontal or Vertical).\n");
            continue;
        }
        char *direction = token;

        int isHorizontal = (strcmp(direction, "Horizontal") == 0);
        int isVertical = (strcmp(direction, "Vertical") == 0);

        if(isHorizontal){
            if((col + ship.size) > Grid_Size){
                printf("Invalid placement, out of bounds horizontally!\n");
                continue;
            }
            int overlap = 0;
            for(int i = 0; i < ship.size; i++){
                if (grid[x][col + i] != '~'){
                    overlap = 1;
                    break;
                }
            }
            if(!overlap){
                for(int i = 0; i < ship.size; i++){
                    grid[x][col + i] = ship.name[0];
                }
                validPlacement = 1;
            }else{
                printf("Invalid placement, overlapping with another ship!\n");
            }
        }
        else if(isVertical){
            if((x + ship.size) > Grid_Size){
                printf("Invalid placement, out of bounds vertically!\n");
                continue;
            }
            int overlap = 0;
            for(int i = 0; i < ship.size; i++){
                if(grid[x + i][col] != '~'){
                    overlap = 1;
                    break;
                }
            }
            if(!overlap){
                for(int i = 0; i < ship.size; i++){
                    grid[x + i][col] = ship.name[0];
                }
                validPlacement = 1;
            }else{
                printf("Invalid placement, overlapping with another ship!\n");
            }
        }else{
            printf("Invalid direction! Please enter 'Horizontal' or 'Vertical'.\n");
        }
    }
}

void playerPlaceShips(char grid[Grid_Size][Grid_Size], char playerName[10]){
    printf("\n");

    for(int i = 0; i < numOfShips; i++){
        placeShip(grid, ships[i], playerName);
        printf("\n");
        displayGrid(grid);
    }
    sleep(1);
    clearScreen();
}

void botPlaceShips(char grid[Grid_Size][Grid_Size]){
    srand(time(0));
    for(int i = 0; i < numOfShips; i++){
        int validPlacement = 0;
        while(validPlacement != 1){
            int row = rand() % Grid_Size;
            int col = rand() % Grid_Size;
            int isHorizontal = rand() % 2;

            if(isHorizontal && (col + ships[i].size) <= Grid_Size){
                int overlap = 0;
                for (int j = 0; j < ships[i].size; j++){
                    if(grid[row][col+j] != '~'){
                        overlap = 1;
                        break;
                    }
                }

                if(!overlap){
                    for (int j = 0; j < ships[i].size; j++){
                        grid[row][col+j] = ships[i].name[0];
                    }
                    validPlacement = 1;
                }
            }else if(!isHorizontal && (row + ships[i].size) <= Grid_Size){
                int overlap = 0;
                for(int j = 0; j < ships[i].size; j++){
                    if(grid[row+j][col] != '~'){
                        overlap = 1;
                        break;
                    }
                }

                if(!overlap){
                    for(int j = 0; j < ships[i].size; j++){
                        grid[row+j][col] = ships[i].name[0];
                    }
                    validPlacement = 1;
                }
            }
        }
    }
}

void fire(char shipGrid[Grid_Size][Grid_Size], char viewGrid[Grid_Size][Grid_Size], int row, int col, int difficulty){
    if(shipGrid[row][col] == '~'){
        if(difficulty == 1){
            viewGrid[row][col] = 'o';
        }
        printf("Miss!\n");
    }else if(shipGrid[row][col] == '*'){
        printf("You already hit this spot.\n");
    }else{
        viewGrid[row][col] = '*';
        printf("Hit!\n");
    }
}

void radar(char shipGrid[Grid_Size][Grid_Size], int row, int col, int *radarSweep){
    int found = 0;
    for(int i = row; i < row + 2 && i < Grid_Size; i++){
        for(int j = col; j < col + 2 && j < Grid_Size; j++){
            if(shipGrid[row][col] == 'C' || shipGrid[row][col] == 'B' || shipGrid[row][col] == 'D' || shipGrid[row][col] == 'S'){
                found = 1;
                break;
            }
        }
    }
    (*radarSweep)--;
    if (found){
        printf("Enemy ships found in radar area.\n");
    }else{
        printf("No enemy ships found.\n");
    }
}

void smoke(char grid[Grid_Size][Grid_Size], int row, int col, int *smokeScreen){
    for(int i = row; i < row + 2 && i < Grid_Size; i++){
        for(int j = col; j < col + 2 && j < Grid_Size; j++){
            grid[i][j] = 'X';
        }
    }
    (*smokeScreen)--;
}

void artillery(char shipGrid[Grid_Size][Grid_Size], char viewGrid[Grid_Size][Grid_Size], int row, int col, int difficulty){
    for(int i = row; i < row + 2 && i < Grid_Size; i++){
        for(int j = col; j < col + 2 && j < Grid_Size; j++){
            fire(shipGrid, viewGrid, i, j, difficulty);
        }
    }
}

void torpedo(char shipGrid[Grid_Size][Grid_Size], char viewGrid[Grid_Size][Grid_Size], char target[], int difficulty){
    if(target[0] >= 'A' && target[0] <= 'J'){
        int col = target[0] - 'A';
        for(int i = 0; i < Grid_Size; i++){
            fire(shipGrid, viewGrid, i, col, difficulty);
        }
    }else{
        int row = atoi(target);
        if(row >= 1 && row <= Grid_Size){
            row--;
            for(int i = 0; i < Grid_Size; i++){
                fire(shipGrid, viewGrid, row, i, difficulty);
            }
        }
    }
}

void Checkifsunk(char shipGrid[Grid_Size][Grid_Size], char viewGrid[Grid_Size][Grid_Size], struct Ship ships[], int *sunkShips, int *smokeScreen, int *readyArtilleries, int *readyTorpedo, char playerName[10], char opponentName[10], int hits[4], int sunkShipsFlag[4]){
    for(int i = 0; i < Grid_Size; i++){
        for(int j = 0; j < Grid_Size; j++){
            if (viewGrid[i][j] == '*'){
                switch (shipGrid[i][j]){
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
                shipGrid[i][j] = '*';
            }
        }
    }

    for(int k = 0; k < numOfShips; k++){
        if(sunkShipsFlag[k] == 0){
            if(hits[k] == ships[k].size){
                printf("%s: You have succeded in sinking %s's %s Ship\n\n", playerName, opponentName, ships[k].name);
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

void processMove(char shipGrid[Grid_Size][Grid_Size], char viewGrid[Grid_Size][Grid_Size], char myGrid[Grid_Size][Grid_Size], char playerName[10], char opponentName[10], int difficulty, int *sunkShips, int *radarSweep, int *smokeScreen, int *readyArtilleries, int *readyTorpedo, int *usedTorpedo, int hits[4], int sunkShipsFlag[4]){
    displayGrid(viewGrid);
    printf("The possible moves are:\n. Fire [coordinates]\n");
    if(*radarSweep > 0){
        printf(". Radar [top-left coordinate]\n");
    }
    if(*smokeScreen > 0){
        printf(". Smoke [top-left coordinate]\n");
    }
    if(*readyArtilleries == 1){
        printf(". Artillery [top-left coordinate]\n");
    }
    if(*readyTorpedo == 1){
        printf(". Torpedo [row/column]\n");
    }

    char command[50];
    printf("%s, your move: ", playerName);
    fgets(command, sizeof(command), stdin);

    int length = strlen(command);
    for(int i = 0; i < length; i++){
        command[i] = toupper(command[i]);
    }

    char move[20];
    char target[5];
    int row, col;

    if(sscanf(command, "%s %s", move, target) == 2){
        if(strcmp(move, "TORPEDO") == 0 && (strlen(target) == 1 || strlen(target) == 2)){
            if((*readyTorpedo) == 1 && (*usedTorpedo) == 0){
                torpedo(shipGrid, viewGrid, target, difficulty);
                (*usedTorpedo) = 1;
            }else{
                printf("Torpedo not available.\n");
            }
        }else{
            col = target[0] - 'A';

            if(strlen(target) == 2){
                row = target[1] - '1';
            }else if(strlen(target) == 3 && target[1] == '1' && target[2] == '0'){
                row = 9;
            }else{
                printf("Invalid target format. Use a letter and number (ex. B5) or single letter for torpedo.\n");
                (*readyArtilleries) = 0;
                (*readyTorpedo) = 0;
                return;
            }

            if(row < 0 || row >= Grid_Size || col < 0 || col >= Grid_Size){
                printf("Coordinates out of bounds.\n");
                (*readyArtilleries) = 0;
                (*readyTorpedo) = 0;
                return;
            }

            if(strcmp(move, "FIRE") == 0){
                fire(shipGrid, viewGrid, row, col, difficulty);
            }else if(strcmp(move, "RADAR") == 0){
                if((*radarSweep) > 0){
                    radar(shipGrid, row, col, radarSweep);
                }else{
                    printf("No radar sweeps available.\n");
                }
            }else if(strcmp(move, "SMOKE") == 0){
                if((*smokeScreen) > 0){
                    smoke(myGrid, row, col, smokeScreen);
                }else{
                    printf("No smoke screens available.\n");
                }
            }else if(strcmp(move, "ARTILLERY") == 0){
                if((*readyArtilleries) == 1){
                    artillery(shipGrid, viewGrid, row, col, difficulty);
                }else{
                    printf("Artillery not available.\n");
                }
            }else{
                printf("Invalid move. You lost your turn.\n");
            }
        }
        displayGrid(viewGrid);
        (*readyArtilleries) = 0;
        (*readyTorpedo) = 0;
        Checkifsunk(shipGrid, viewGrid, ships, sunkShips, smokeScreen, readyArtilleries, readyTorpedo, playerName, opponentName, hits, sunkShipsFlag);
    }else{
        printf("Invalid command format.\n");
        (*readyArtilleries) = 0;
        (*readyTorpedo) = 0;
    }
}

void initializeUnfiredCells(Coordinates unfiredCells[], int *unfiredcount){
    for(int row = 0; row < Grid_Size; row++){
        for(int col = 0; col < Grid_Size; col++){
            unfiredCells[*unfiredcount].row = row;
            unfiredCells[*unfiredcount].col = col;
            (*unfiredcount)++;
        }
    }
}

void addHit(Coordinates hitstack[], int *hitcount, int row, int col){
    hitstack[*hitcount].row = row;
    hitstack[*hitcount].col = col;
    (*hitcount)++;
}

void removeHit(Coordinates hitstack[], int *hitcount){
    if(*hitcount>0){
        (*hitcount)--;
    }
}

void markFired(Coordinates unfiredCells[], int *unfiredcount, int row, int col){
    for(int i = 0; i < (*unfiredcount); i++){
        if(unfiredCells[i].row == row && unfiredCells[i].col == col){
            unfiredCells[i] = unfiredCells[--(*unfiredcount)];
            break;
        }
    }
}

void removeHitsForSunkShip(Coordinates hitstack[],int *hitcount,char shipGrid[Grid_Size][Grid_Size],int sunkShipsFlag[4]){
    char shipID;
    for(int i = 0;i < numOfShips;i++){
        if(sunkShipsFlag[i] == 1){
            shipID = ships[i].name[0];
            int j = 0;
            while(j < *hitcount){
                if(shipGrid[hitstack[j].row][hitstack[j].col]==shipID){
                    for (int k = j; k < *hitcount - 1; k++) {
                        hitstack[k] = hitstack[k + 1];
                    }
                    (*hitcount)--;
                }else{
                    j++;
                }
            }
            sunkShipsFlag[i] = 2;
        }
    }
}

void botRandomFire(char shipGrid[Grid_Size][Grid_Size],char viewGrid[Grid_Size][Grid_Size],Coordinates hitstack[],int *hitcount,Coordinates unfiredCells[],int *unfiredcount,int difficulty){
    int randomindex = rand() % (*unfiredcount);
    Coordinates target = unfiredCells[randomindex];
    int row = target.row;
    int col = target.col;

    fire(shipGrid,viewGrid,row,col,difficulty);
    markFired(unfiredCells,unfiredcount,row,col);
    if(viewGrid[row][col] == '*'){
        addHit(hitstack,hitcount,row,col);
    }
}

void botAdvancedFire(char shipGrid[Grid_Size][Grid_Size],char viewGrid[Grid_Size][Grid_Size],Coordinates hitstack[],int *hitcount,Coordinates unfiredCells[],int *unfiredcount,int *firingdirection,Coordinates *initialHit,int difficulty){
    int fired = 0;

    Coordinates target;
    if((*firingdirection) == -1){
        (*initialHit)  = hitstack[*hitcount-1];
        target = *initialHit;
    }else{
        target = hitstack[*hitcount-1];
    }

    int direction[4][2] = {{-1,0},{1,0},{0,-1},{0,1}};//Up,Down,Left,Right
    int oppositeDirections[4] = {1,0,3,2};

    if((*firingdirection) != -1){
        int newrow = target.row + direction[*firingdirection][0];
        int newcol = target.col + direction[*firingdirection][1];
        if(newrow >= 0 && newrow < Grid_Size && newcol >= 0 && newcol < Grid_Size && viewGrid[newrow][newcol] == '~'){
            fire(shipGrid,viewGrid,newrow,newcol,difficulty);
            markFired(unfiredCells,unfiredcount,newrow,newcol);
            fired = 1;

            if(viewGrid[newrow][newcol] == '*'){
                addHit(hitstack,hitcount,newrow,newcol);
            }else{
                *firingdirection = -1;
            }
        }else{
            *firingdirection = -1;
        }
    }
    if(!fired){
        for(int i = 0;i < 4;i++){
            if(i == (*firingdirection)){
                continue;
            }
                
            int newrow = (*initialHit).row + direction[i][0];
            int newcol = (*initialHit).col + direction[i][1];
            if(newrow >= 0 && newrow < Grid_Size && newcol >= 0 && newcol < Grid_Size && viewGrid[newrow][newcol] == '~'){
                fire(shipGrid,viewGrid,newrow,newcol,difficulty);
                markFired(unfiredCells,unfiredcount,newrow,newcol);
                fired = 1;

                if(viewGrid[newrow][newcol] == '*'){
                    addHit(hitstack,hitcount,newrow,newcol);
                    (*firingdirection) = i;
                }
                break;
            }
        }
    }
}

void botArtilleryAttack(char shipGrid[Grid_Size][Grid_Size],char viewGrid[Grid_Size][Grid_Size],Coordinates hitstack[],int *hitcount,Coordinates unfiredCells[],int *unfiredcount,int row,int col,int difficulty){
    for(int i = row; i < row + 2 && i < Grid_Size; i++){
        for(int j = col; j < col + 2 && j < Grid_Size; j++){
            fire(shipGrid, viewGrid, i, j, difficulty);
            markFired(unfiredCells,unfiredcount,i,j);
            if(viewGrid[i][j] == '*'){
                addHit(hitstack,hitcount,i,j);
            }
        }
    }
}

void botTorpedoAttack(char shipGrid[Grid_Size][Grid_Size], char viewGrid[Grid_Size][Grid_Size],Coordinates hitstack[],int *hitcount,Coordinates unfiredCells[],int *unfiredcount,int roworcol,int isrow,int difficulty){
    if(isrow){
        for(int i = 0;i<Grid_Size;i++){
            fire(shipGrid,viewGrid,roworcol,i,difficulty);
            markFired(unfiredCells,unfiredcount,roworcol,i);
            if(viewGrid[roworcol][i] == '*'){
                addHit(hitstack,hitcount,roworcol,i);
            }
        }
    }else{
        for(int i = 0;i<Grid_Size;i++){
            fire(shipGrid,viewGrid,i,roworcol,difficulty);
            markFired(unfiredCells,unfiredcount,i,roworcol);
            if(viewGrid[i][roworcol] == '*'){
                addHit(hitstack,hitcount,i,roworcol);
            }
        }
    }
}

void botMove(char shipGrid[Grid_Size][Grid_Size],char viewGrid[Grid_Size][Grid_Size],char myGrid[Grid_Size][Grid_Size],char copyPlayerGrid[Grid_Size][Grid_Size], char botName[10], char opponentName[10], int difficulty, int *sunkShips, int *radarSweep, int *smokeScreen, int *readyArtilleries, int *readyTorpedo, int *usedTorpedo, int hits[4], int sunkShipsFlag[4]){
    displayGrid(viewGrid);

    static Coordinates hitstack[Grid_Size*Grid_Size];
    static int hitcount = 0;

    static Coordinates unfiredCells[Grid_Size*Grid_Size];
    static int unfiredcount = 0;

    static int firingdirection = -1;//-1(Not set),0(Up),1(Down),2(Left),3(Right)
    static Coordinates initialHit;

    if(unfiredcount == 0){
        initializeUnfiredCells(unfiredCells,&unfiredcount);
    }

    if(*readyTorpedo){
        printf("Bot uses Artillery!\n\n");
        int row,col;
        int isRow = rand()%2;
        if(hitcount>0){
            row = hitstack[0].row;
            col = hitstack[0].col;
        }else{
            int randomindex = rand() % unfiredcount;
            Coordinates target = unfiredCells[randomindex];
            row = target.row;
            col = target.col;
        }
        if(isRow){
            botTorpedoAttack(shipGrid,viewGrid,hitstack,&hitcount,unfiredCells,&unfiredcount,row,isRow,difficulty);
        }else{
            botTorpedoAttack(shipGrid,viewGrid,hitstack,&hitcount,unfiredCells,&unfiredcount,col,isRow,difficulty);
        }
    }else if(*readyArtilleries){
        printf("Bot uses Artillery!\n\n");
        int row,col;
        if(hitcount>0){
            row = hitstack[0].row;
            col = hitstack[0].col;
        }else{
            int randomindex = rand() % unfiredcount;
            Coordinates target = unfiredCells[randomindex];
            row = target.row;
            col = target.col;
        }
        if(col == 9){
            col = col-2;
        }else{
            col++;
        }
        botArtilleryAttack(shipGrid,viewGrid,hitstack,&hitcount,unfiredCells,&unfiredcount,row,col,difficulty);
    }else if(hitcount>0){
        printf("Bot uses advanced fire.\n");
        botAdvancedFire(shipGrid,viewGrid,hitstack,&hitcount,unfiredCells,&unfiredcount,&firingdirection,&initialHit,difficulty);    
    }else if(unfiredcount > 0){
        printf("Bot uses fire.\n");
        botRandomFire(shipGrid,viewGrid,hitstack,&hitcount,unfiredCells,&unfiredcount,difficulty);
    }
    displayGrid(viewGrid);
    (*readyArtilleries) = 0;
    (*readyTorpedo) = 0;
    Checkifsunk(shipGrid,viewGrid,ships,sunkShips,smokeScreen,readyArtilleries,readyTorpedo,botName,opponentName,hits,sunkShipsFlag); 
    removeHitsForSunkShip(hitstack,&hitcount,copyPlayerGrid,sunkShipsFlag);
}

int main(){
    char playerName[10];
    char botName[10] = "Bot";
    char playerGrid[Grid_Size][Grid_Size], botGrid[Grid_Size][Grid_Size];// Grids showing the ships
    char playerViewGrid[Grid_Size][Grid_Size], botViewGrid[Grid_Size][Grid_Size]; // Grids for playing the game

    int difficulty = getDifficulty();
    getPlayerName(playerName);

    int startingPlayer = randomChooser(playerName);

    initializeGrid(playerGrid);
    initializeGrid(botGrid);
    initializeGrid(playerViewGrid);
    initializeGrid(botViewGrid);

    getchar();
    if (startingPlayer == player){
        playerPlaceShips(playerGrid, playerName);
        botPlaceShips(botGrid);
        sleep(1);
        printf("Bot have placed his ships.\n\n");
    }else{
        botPlaceShips(botGrid);
        sleep(1);
        printf("Bot have placed his ships.\nNow, it is your turn %s.\n",playerName);
        playerPlaceShips(playerGrid, playerName);
    }

    char copyPlayerGrid [Grid_Size][Grid_Size];
    for(int i = 0;i<Grid_Size;i++){
        for(int j = 0;j<Grid_Size;j++){
            copyPlayerGrid[i][j] = playerGrid[i][j];
        }
    }

    int currentPlayer = startingPlayer;
    while (sunkShips1 < numOfShips && sunkShips2 < numOfShips){
        if (currentPlayer == 0){
            processMove(botGrid, playerViewGrid, playerGrid, playerName, botName, difficulty, &sunkShips1, &radarSweep1, &smokeScreen1, &readyArtilleries1, &readyTorpedo1, &usedTorpedo1, hits1, sunkShipsFlags1);
            currentPlayer = 1;
        }else{
            printf("%s's turn:\n",botName);
            botMove(playerGrid, botViewGrid, botGrid, copyPlayerGrid, botName, playerName, difficulty, &sunkShips2, &radarSweep2, &smokeScreen2, &readyArtilleries2, &readyTorpedo2, &usedTorpedo2, hits2, sunkShipsFlags2);
            currentPlayer = 0;
        }
    }

    if (sunkShips1 == numOfShips){
        printf("%s wins.\n", playerName);
    }else{
        printf("Bot wins.\n");
    }

    return 0;
}