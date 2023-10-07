#include <pong.h>

char winMessage[] = {"PLAYER   WON! CONGRATULATIONS!"};

struct paddle{
    int x;
    int y;
    int prev_y ;
};

struct ball{ //internamente la pelota es un cuadrado, aunque la dibujemos como un circulo
    int  x;
    int  y;
    int prev_x ;
    int prev_y ;
    int speed_x;
    int speed_y;
};


char keys[11];
char rkeys[11];
int l_u;
int l_d;
int r_u;
int r_d;

uint32_t width;
uint32_t height;

int scoreL;
int scoreR;
int bounces = 0;
int running;
int startingDirection = 1;

struct paddle leftPaddle;
struct paddle rightPaddle;
struct ball ball;

static void draw();
static void movePaddles();
static void moveBall();
static void checkCollisionsAndGoals();
static void setStart();
static void mainGame();
static void winScreen(int winner);
static void drawMiddleLine();

void pong(){
    screenInfo(&width,&height);
    ball.prev_x = 0;
    ball.prev_y = 0;
    leftPaddle.prev_y = 0;
    rightPaddle.prev_y = 0;
    running=1;
    clearScreen();
    setStart();

    mainGame();
    clearScreen();
    textPosition(0, height);
}

static void movePaddles(){
    leftPaddle.prev_y = leftPaddle.y;
    rightPaddle.prev_y = rightPaddle.y;
    if (l_d){
        leftPaddle.y += PADDLE_SPEED;
        if (leftPaddle.y + PADDLE_HEIGHT > height){
            leftPaddle.y = height - PADDLE_HEIGHT;
        }
    }
    if (l_u){
        leftPaddle.y -= PADDLE_SPEED;
        if (leftPaddle.y < 0){
            leftPaddle.y = 0;
        }
    }
    if (r_d){
        rightPaddle.y += PADDLE_SPEED;
        if (rightPaddle.y + PADDLE_HEIGHT > height){
            rightPaddle.y = height - PADDLE_HEIGHT;
        }
    }
    if (r_u){
        rightPaddle.y -= PADDLE_SPEED;
        if (rightPaddle.y < 0){
            rightPaddle.y = 0;
        }
    }
}

static void moveBall(){
    if (bounces==5){
        ball.speed_x *= 1.25;
        ball.speed_y *= 1.25;
        bounces=0;
    }
    ball.prev_x = ball.x;
    ball.prev_y = ball.y;
    ball.x += ball.speed_x;
    ball.y += ball.speed_y;

}


static void checkCollisionsAndGoals(){
    if (ball.y <= 0 || ball.y + BALL_SIZE >= height){
        ball.speed_y = -ball.speed_y;
    }

    //LADO IZQUIERDO
    if (ball.x <= leftPaddle.x + PADDLE_WIDTH){
        if (ball.y + BALL_SIZE >= leftPaddle.y && ball.y <= leftPaddle.y+PADDLE_HEIGHT) {
            ball.speed_x = -ball.speed_x;
            bounces++;
        }
        else{
            scoreR++;
            beep((uint8_t)0x0777, 1);
            if(scoreR>9){
                winScreen(2);
            }
            setStart();
        }
    }

    //LADO DERECHO
    if (ball.x >= rightPaddle.x - PADDLE_WIDTH){
        if ( ball.y + BALL_SIZE >= rightPaddle.y && ball.y <= rightPaddle.y+PADDLE_HEIGHT) {
            ball.speed_x = -ball.speed_x;
            bounces++;
        }
        else if (ball.x >= rightPaddle.x - PADDLE_WIDTH){
            scoreL++;
            beep((uint8_t)0x0777, 1);
            if(scoreL>9){
                winScreen(1);
            }
            setStart();
        }
    }


}


static void draw() {
    clearRectangle(leftPaddle.x, leftPaddle.prev_y, PADDLE_WIDTH, PADDLE_HEIGHT);
    clearRectangle(rightPaddle.x, rightPaddle.prev_y, PADDLE_WIDTH, PADDLE_HEIGHT);
    clearCircle(ball.prev_x + BALL_SIZE / 2, ball.prev_y + BALL_SIZE / 2, BALL_SIZE / 2);

    drawRectangle(leftPaddle.x, leftPaddle.y, PADDLE_WIDTH, PADDLE_HEIGHT);
    drawRectangle(rightPaddle.x, rightPaddle.y, PADDLE_WIDTH, PADDLE_HEIGHT);
    drawMiddleLine();
    drawBigNumber(scoreL,NUMBER_SIZE,(width/2)-100-CHAR_WIDTH*NUMBER_SIZE,75);
    drawBigNumber(scoreR,NUMBER_SIZE,(width/2)+100,75);
    drawCircle(ball.x + BALL_SIZE / 2, ball.y + BALL_SIZE / 2, BALL_SIZE / 2);
}

static void setStart(){
    startingDirection = -startingDirection;
    ball.y =height/2-BALL_SIZE/2;
    ball.x=width/2-BALL_SIZE/2;
    ball.speed_x= BALL_INITIAL_SPEED_X * startingDirection;
    ball.speed_y= BALL_INITIAL_SPEED_Y;

    leftPaddle.x = width/10;
    leftPaddle.y = height/2-PADDLE_HEIGHT/2;

    rightPaddle.x = width*9/10;
    rightPaddle.y=height/2-PADDLE_HEIGHT/2;

    bounces = 0;

    l_u = 0;
    l_d = 0;
    r_u = 0;
    r_d = 0;

    draw();

    char c;
    do{
        c = getChar();
    } while( c != ' ' && c != '\n' && c!=ESC);
    if  (c==ESC){
        running = 0;
    }
}


static void mainGame(){
    scoreL = 0;
    scoreR = 0;
    while(running){
        draw();
        sleep(1);//para que el juego no vaya a velocidades absurdas

        getCurrentKeyPress(keys);
        for(int i = 0; keys[i]; i++){
            switch (keys[i]){
                case ESC:
                    running = 0;
                case 'w':
                    l_u = 1;
                    break;
                case 's':
                    l_d =1;
                    break;
                case 'i':
                    r_u = 1;
                    break;
                case 'k':
                    r_d = 1;
                    break;
            }
        }
        getCurrentReleasedKeys(rkeys);
        for(int i = 0; rkeys[i]; i++){
            switch (rkeys[i]){
                case 'w':
                    l_u = 0;
                    break;
                case 's':
                    l_d =0;
                    break;
                case 'i':
                    r_u = 0;
                    break;
                case 'k':
                    r_d = 0;
                    break;
            }
        }
        movePaddles();
        moveBall();
        checkCollisionsAndGoals();
    }
}

static void winScreen(int winner){
    clearScreen();
    scoreL = 0;
    scoreR = 0;
    winMessage[7]= winner+'0';
    textPosition(width/2 - strlen(winMessage)*4,height/2);
    colorPrint(winMessage);
    char c;
    do{
        c = getChar();
    } while( c != ' ' && c != '\n' && c!=ESC);
    if  (c==ESC){
        running = 0;
    }
    clearScreen();
}


static void drawMiddleLine(){
    for(int i = 0; i<height;i+=MID_LINE_HEIGHT*3/2){
        drawRectangle(width/2-MID_LINE_WIDTH/2,i, MID_LINE_WIDTH, MID_LINE_HEIGHT);
    }
}