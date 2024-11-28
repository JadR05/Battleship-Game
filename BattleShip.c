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
#define bot 1
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

typedef struct ShipTargetingInfo ShipTargetingInfo;
struct  ShipTargetingInfo{
    char shipID;
    Coordinates hitstack[5];
    int hitcount;
    int firingdirection;
};

void initializeGrid(char grid[Grid_Size][Grid_Size]){
    for(int i = 0;i < Grid_Size;i++){
        for(int j = 0;j < Grid_Size;j++){
            grid[i][j] = '~';
        }
    }
}

void displayGrid(char grid[Grid_Size][Grid_Size]){
    printf("   A B C D E F G H I J\n");
    for(int i = 0;i < Grid_Size;i++){
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

    if(strcmp(input,"easy") == 0){
        puts("Hits(*) and Misses(o) will be shown.\n");
        choice = Easy;
    }else if(strcmp(input,"hard") == 0){
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
    scanf("%s",playerName);
    printf("\n");
}

int randomChooser(char playerName[10],char botName[10]){
    srand(time(0));
    int x = rand() % 2;
    if(x == 0){
        printf("%s, you will start first.\n\n",playerName);
        return player;
    }else{
        printf("%s will start first.\n\n",botName);
        return bot;
    }
}

void clearScreen(){
    for(int i = 0;i < 50;i++){
        printf("\n");
    }
}

void placeShip(char grid[Grid_Size][Grid_Size],struct Ship ship,char playerName[10]){
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
            for(int i = 0;i < ship.size;i++){
                if (grid[x][col + i] != '~'){
                    overlap = 1;
                    break;
                }
            }
            if(!overlap){
                for(int i = 0;i < ship.size;i++){
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
            for(int i = 0;i < ship.size;i++){
                if(grid[x + i][col] != '~'){
                    overlap = 1;
                    break;
                }
            }
            if(!overlap){
                for(int i = 0;i < ship.size;i++){
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

void playerPlaceShips(char grid[Grid_Size][Grid_Size],char playerName[10]){
    printf("\n");

    for(int i = 0;i < numOfShips;i++){
        placeShip(grid,ships[i],playerName);
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

void fire(char shipGrid[Grid_Size][Grid_Size],char viewGrid[Grid_Size][Grid_Size],int row,int col,int difficulty){
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

void radar(char shipGrid[Grid_Size][Grid_Size],int smokeScreenGrid[Grid_Size][Grid_Size],int row,int col,int *radarSweep){
    int found = 0;
    for(int i = row; i < row + 2 && i < Grid_Size; i++){
        for(int j = col; j < col + 2 && j < Grid_Size; j++){
            if(smokeScreenGrid[i][j] == 0 && (shipGrid[i][j] == 'C' || shipGrid[i][j] == 'B' || shipGrid[i][j] == 'D' || shipGrid[i][j] == 'S')){
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

void smoke(int smokeScreenGrid[Grid_Size][Grid_Size],int row,int col,int *smokeScreen){
    for(int i = row;i < row + 2 && i < Grid_Size;i++){
        for(int j = col;j < col + 2 && j < Grid_Size;j++){
            smokeScreenGrid[i][j] = 1;
        }
    }
    (*smokeScreen)--;
}

void artillery(char shipGrid[Grid_Size][Grid_Size],char viewGrid[Grid_Size][Grid_Size],int row,int col,int difficulty){
    for(int i = row;i < row + 2 && i < Grid_Size;i++){
        for(int j = col;j < col + 2 && j < Grid_Size;j++){
            fire(shipGrid,viewGrid,i,j,difficulty);
        }
    }
}

void torpedo(char shipGrid[Grid_Size][Grid_Size], char viewGrid[Grid_Size][Grid_Size], char target[], int difficulty){
    if(target[0] >= 'A' && target[0] <= 'J'){
        int col = target[0] - 'A';
        for(int i = 0; i < Grid_Size; i++){
            fire(shipGrid,viewGrid,i,col,difficulty); 
        }
    }else{
        int row = atoi(target);
        if(row >= 1 && row <= Grid_Size){
            row--;
            for(int i = 0; i < Grid_Size; i++){
                fire(shipGrid,viewGrid,row,i,difficulty);
            }
        }
    }
}

void clearSmokeScreen(int smokeScreenGrid[Grid_Size][Grid_Size]){
    for(int i = 0;i < Grid_Size;i++){
        for(int j = 0;j < Grid_Size;j++){
            smokeScreenGrid[i][j] = 0;
        }
    }
}

void Checkifsunk(char shipGrid[Grid_Size][Grid_Size],char viewGrid[Grid_Size][Grid_Size],struct Ship ships[],int *sunkShips,int *smokeScreen,int *readyArtilleries,int *readyTorpedo,char playerName[10],char opponentName[10],int hits[4],int sunkShipsFlag[4]){
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

    for(int k = 0;k < numOfShips;k++){
        if(sunkShipsFlag[k] == 0){
            if(hits[k] == ships[k].size){
                printf("%s: You have succeded in sinking %s's %s Ship\n\n", playerName,opponentName,ships[k].name);
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

void processMove(char shipGrid[Grid_Size][Grid_Size],char viewGrid[Grid_Size][Grid_Size],char myGrid[Grid_Size][Grid_Size],int smokeScreenGrid[Grid_Size][Grid_Size],char playerName[10],char opponentName[10],int difficulty,int *sunkShips,int *radarSweep,int *smokeScreen,int *readyArtilleries,int *readyTorpedo,int *usedTorpedo,int hits[4],int sunkShipsFlag[4]){
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
                torpedo(shipGrid,viewGrid,target,difficulty);
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
                    radar(shipGrid,smokeScreenGrid,row,col,radarSweep);
                }else{
                    printf("No radar sweeps available.\n");
                }
            }else if(strcmp(move, "SMOKE") == 0){
                if((*smokeScreen) > 0){
                    smoke(smokeScreenGrid,row,col,smokeScreen);
                }else{
                    printf("No smoke screens available.\n");
                }
            }else if(strcmp(move, "ARTILLERY") == 0){
                if((*readyArtilleries) == 1){
                    artillery(shipGrid,viewGrid,row,col,difficulty);
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
        Checkifsunk(shipGrid,viewGrid,ships,sunkShips,smokeScreen,readyArtilleries,readyTorpedo,playerName,opponentName,hits,sunkShipsFlag);
    }else{
        printf("Invalid command format.\n");
        (*readyArtilleries) = 0;
        (*readyTorpedo) = 0;
    }
}

void initializeUnfiredCells(Coordinates unfiredCells[],int *unfiredcount){
    for(int row = 0;row < Grid_Size;row++){
        for(int col = 0;col < Grid_Size;col++){
            unfiredCells[*unfiredcount].row = row;
            unfiredCells[*unfiredcount].col = col;
            (*unfiredcount)++;
        }
    }
}

void initializeShips(ShipTargetingInfo Ships[numOfShips],int *isInitialized){
    for (int i = 0;i < numOfShips;i++) {
        if (i == 0){
            Ships[i].shipID = 'C';
        }else if (i == 1){
            Ships[i].shipID = 'B';
        }else if (i == 2){
            Ships[i].shipID = 'D';
        }else if (i == 3){
            Ships[i].shipID = 'S';
        }

        Ships[i].hitcount = 0;        
        Ships[i].firingdirection = -1;
    }
    *isInitialized = 1;
}

void addHit(Coordinates hitstack[],int *hitcount,int row,int col){
    hitstack[*hitcount].row = row;
    hitstack[*hitcount].col = col;
    (*hitcount)++;
}

void markFired(Coordinates unfiredCells[],int *unfiredcount,int row,int col){
    for(int i = 0;i < (*unfiredcount);i++){
        if(unfiredCells[i].row == row && unfiredCells[i].col == col){
            unfiredCells[i] = unfiredCells[--(*unfiredcount)];
            break;
        }
    }
}

int checkTypeOfShip(char grid[Grid_Size][Grid_Size],int row,int col){
    if(grid[row][col] == 'C'){
        return 0;
    }else if(grid[row][col] == 'B'){
        return 1;
    }else if(grid[row][col] == 'D'){
        return 2;
    }else{
        return 3;
    }
}

int checkHitCount(ShipTargetingInfo Ships[numOfShips]){
    for(int i = 0;i < numOfShips;i++){
        if(Ships[i].hitcount>0){
            return 1;
        }
    }
    return 0;
}

void botRandomFire(char shipGrid[Grid_Size][Grid_Size],char viewGrid[Grid_Size][Grid_Size],char copyPlayerGrid[Grid_Size][Grid_Size],ShipTargetingInfo Ships[numOfShips],Coordinates unfiredCells[],int *unfiredcount,int difficulty){
    int randomindex = rand() % (*unfiredcount);
    Coordinates target = unfiredCells[randomindex];
    int row = target.row;
    int col = target.col;

    fire(shipGrid,viewGrid,row,col,difficulty);
    markFired(unfiredCells,unfiredcount,row,col);
    if(viewGrid[row][col] == '*'){
        int i = checkTypeOfShip(copyPlayerGrid,row,col);
        addHit(Ships[i].hitstack,&Ships[i].hitcount,row,col);
    }
}

int botAdvancedFire(char shipGrid[Grid_Size][Grid_Size],char viewGrid[Grid_Size][Grid_Size],char copyPlayerGrid[Grid_Size][Grid_Size],ShipTargetingInfo Ships[numOfShips],Coordinates unfiredCells[],int *unfiredcount,int difficulty){
    int fired = 0;
    
    for (int i = 0;i < numOfShips;i++){
        if (Ships[i].hitcount > 0 && (Ships[i].hitcount < ships[i].size)){
            Coordinates initialHit = Ships[i].hitstack[0];
            int firingdirection = Ships[i].firingdirection;

            int direction[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

            if(firingdirection != -1) {
                Coordinates target = Ships[i].hitstack[(Ships[i].hitcount)-1];
                int newRow = target.row + direction[firingdirection][0];
                int newCol = target.col + direction[firingdirection][1];
                
                if (newRow >= 0 && newRow < Grid_Size && newCol >= 0 && newCol < Grid_Size && viewGrid[newRow][newCol] == '~') {
                    fire(shipGrid, viewGrid, newRow, newCol, difficulty);
                    fired = 1;
                    markFired(unfiredCells, unfiredcount, newRow, newCol);
                    if (viewGrid[newRow][newCol] == '*') {
                        int j = checkTypeOfShip(copyPlayerGrid,newRow,newCol);
                        addHit(Ships[j].hitstack,&Ships[j].hitcount,newRow,newCol);
                    }
                }
            }
            if (!fired){
                for (int dir = 0;dir < 4;dir++){
                    if(dir == firingdirection){
                        continue;
                    }
                    int newRow = initialHit.row + direction[dir][0];
                    int newCol = initialHit.col + direction[dir][1];
                    
                    if (newRow >= 0 && newRow < Grid_Size && newCol >= 0 && newCol < Grid_Size && viewGrid[newRow][newCol] == '~') {
                        fire(shipGrid, viewGrid, newRow, newCol, difficulty);
                        fired = 1;
                        markFired(unfiredCells, unfiredcount, newRow, newCol);
                        if (viewGrid[newRow][newCol] == '*') {
                            int j = checkTypeOfShip(copyPlayerGrid,newRow,newCol);
                            addHit(Ships[j].hitstack,&Ships[j].hitcount,newRow,newCol);
                            Ships[j].firingdirection = dir;
                        }
                        break;
                    }
                }
            }
        }
        if (fired) {
            break;
        }
    }

    return fired;
}

void botArtilleryAttack(char shipGrid[Grid_Size][Grid_Size],char viewGrid[Grid_Size][Grid_Size],char copyPlayerGrid[Grid_Size][Grid_Size],ShipTargetingInfo Ships[numOfShips],Coordinates unfiredCells[],int *unfiredcount,int row,int col,int difficulty){
    for(int i = row; i < row + 2 && i < Grid_Size; i++){
        for(int j = col; j < col + 2 && j < Grid_Size; j++){
            fire(shipGrid,viewGrid,i,j,difficulty);
            markFired(unfiredCells,unfiredcount,i,j);
            if(viewGrid[i][j] == '*'){
                int k = checkTypeOfShip(copyPlayerGrid,i,j);
                addHit(Ships[k].hitstack,&Ships[k].hitcount,i,j);
            }
        }
    }
}

void botTorpedoAttack(char shipGrid[Grid_Size][Grid_Size],char viewGrid[Grid_Size][Grid_Size],char copyPlayerGrid[Grid_Size][Grid_Size],ShipTargetingInfo Ships[numOfShips],Coordinates unfiredCells[],int *unfiredcount,int roworcol,int isrow,int difficulty){
    if(isrow){
        for(int i = 0;i<Grid_Size;i++){
            fire(shipGrid,viewGrid,roworcol,i,difficulty);
            markFired(unfiredCells,unfiredcount,roworcol,i);
            if(viewGrid[roworcol][i] == '*'){
                int k = checkTypeOfShip(copyPlayerGrid,roworcol,i);
                addHit(Ships[k].hitstack,&Ships[k].hitcount,roworcol,i);
            }
        }
    }else{
        for(int i = 0;i<Grid_Size;i++){
            fire(shipGrid,viewGrid,i,roworcol,difficulty);
            markFired(unfiredCells,unfiredcount,i,roworcol);
            if(viewGrid[i][roworcol] == '*'){
                int k = checkTypeOfShip(copyPlayerGrid,i,roworcol);
                addHit(Ships[k].hitstack,&Ships[k].hitcount,i,roworcol);
            }
        }
    }
}

void botMove(char shipGrid[Grid_Size][Grid_Size],char viewGrid[Grid_Size][Grid_Size],char myGrid[Grid_Size][Grid_Size],char copyPlayerGrid[Grid_Size][Grid_Size],char botName[10],char opponentName[10],int difficulty,int *sunkShips,int *radarSweep,int *smokeScreen,int *readyArtilleries,int *readyTorpedo,int *usedTorpedo,int hits[4],int sunkShipsFlag[4]){
    displayGrid(viewGrid);
    int fired;

    static Coordinates unfiredCells[Grid_Size*Grid_Size];
    static int unfiredcount = 0;

    static ShipTargetingInfo ShipsTargetingInfo[numOfShips];
    static int isInitialized = 0;

    if(unfiredcount == 0){
        initializeUnfiredCells(unfiredCells,&unfiredcount);
    }

    if(!isInitialized){
        initializeShips(ShipsTargetingInfo,&isInitialized);
    }

    if(*readyTorpedo){
        printf("Bot uses Artillery!\n\n");
        int row,col;
        int isRow = rand()%2;
        if(checkHitCount(ShipsTargetingInfo)){
            // add implementation
        }else{
            int randomindex = rand() % unfiredcount;
            Coordinates target = unfiredCells[randomindex];
            row = target.row;
            col = target.col;
        }
        if(isRow){
            botTorpedoAttack(shipGrid,viewGrid,copyPlayerGrid,ShipsTargetingInfo,unfiredCells,&unfiredcount,row,isRow,difficulty);
        }else{
            botTorpedoAttack(shipGrid,viewGrid,copyPlayerGrid,ShipsTargetingInfo,unfiredCells,&unfiredcount,col,isRow,difficulty);
        }
    }else if(*readyArtilleries){
        printf("Bot uses Artillery!\n\n");
        int row,col;
        if(checkHitCount(ShipsTargetingInfo)){
            // add implementation
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

        if(row == 9){
            row--;
        }
        botArtilleryAttack(shipGrid,viewGrid,copyPlayerGrid,ShipsTargetingInfo,unfiredCells,&unfiredcount,row,col,difficulty);
    }else if(checkHitCount(ShipsTargetingInfo)){
        printf("Bot uses advanced fire.\n");
        fired = botAdvancedFire(shipGrid,viewGrid,copyPlayerGrid,ShipsTargetingInfo,unfiredCells,&unfiredcount,difficulty);    
    }
    if(!fired && unfiredcount > 0){
        printf("Bot uses fire.\n");
        botRandomFire(shipGrid,viewGrid,copyPlayerGrid,ShipsTargetingInfo,unfiredCells,&unfiredcount,difficulty);
        }
    displayGrid(viewGrid);
    (*readyArtilleries) = 0;
    (*readyTorpedo) = 0;
    Checkifsunk(shipGrid,viewGrid,ships,sunkShips,smokeScreen,readyArtilleries,readyTorpedo,botName,opponentName,hits,sunkShipsFlag); 
}

int main(){
    char playerName[10];
    char botName[10] = "Bot";
    char playerGrid[Grid_Size][Grid_Size], botGrid[Grid_Size][Grid_Size];// Grids showing the ships
    char playerViewGrid[Grid_Size][Grid_Size], botViewGrid[Grid_Size][Grid_Size]; // Grids for playing the game
    int  smokeScreenGrid[Grid_Size][Grid_Size] = {0};
    int difficulty = getDifficulty();
    getPlayerName(playerName);

    int startingPlayer = randomChooser(playerName,botName);

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
        printf("Bot have placed his ships.\nNow, it is your turn %s to place your ships.\n",playerName);
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
        if (currentPlayer == player){
            processMove(botGrid,playerViewGrid,playerGrid,smokeScreenGrid,playerName,botName,difficulty, &sunkShips1, &radarSweep1, &smokeScreen1, &readyArtilleries1, &readyTorpedo1, &usedTorpedo1, hits1, sunkShipsFlags1);
            currentPlayer = 1;
        }else{
            printf("%s's turn:\n",botName);
            botMove(playerGrid,botViewGrid,botGrid,copyPlayerGrid,botName,playerName,difficulty, &sunkShips2, &radarSweep2, &smokeScreen2, &readyArtilleries2, &readyTorpedo2, &usedTorpedo2, hits2, sunkShipsFlags2);
            sleep(1);
            currentPlayer = 0;
            clearSmokeScreen(smokeScreenGrid);
        }
    }

    if (sunkShips1 == numOfShips){
        printf("%s wins.\n", playerName);
    }else{
        printf("Bot wins.\n");
    }

    return 0;
}