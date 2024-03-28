#include<iostream>
#include <vector>
#include <cmath>
#include<map>

using namespace std;

int N, M, P, C, D;
struct Santa{
    int num, x, y, score, sleep, fail;   // 산타 번호, 위치, 점수, 기절, 탈락여부, 루돌프까지의 거리
};
map <int, Santa> santas;   // 산타 번호 + 정보 저장
struct Rudolf{
    int x, y;
};
Rudolf rudolf;
int MAP[51][51];    // 루돌프는 -1로, 산타는 1 ~ P번으로 표현
int turn = 1;
int santa_cnt;
void SantaRudolfCrash(int score, int x, int y, int santa_num, int x_to_move, int y_to_move);
void SantaInteract(int x_move, int y_move, int x, int y, int santa_num);


void input(){
    //freopen("input.txt", "r", stdin);
    cin >> N >> M >> P >> C >> D;
    cin >> rudolf.x >> rudolf.y;
    MAP[rudolf.x][rudolf.y] = -1;
    santa_cnt = P;

    for(int i = 1 ; i<= P; i++){
        int p, x, y;
        cin >> p >> x >> y;
        MAP[x][y] = p;
        santas.insert({p, {p, x, y, 0, 0, 0}});
    }
}

// 3. (x, y) 칸에서 santa_num 번째 산타와 충돌 발생
void SantaRudolfCrash(int score, int x, int y, int santa_num, int x_to_move, int y_to_move){
    
    // 산타 -> 루돌프 충돌일 경우(D)
    // 산타점수 + D
    // 산타는 자신 이동의 반대방향으로 D만큼 튕기기
    // 산타는 k+1까지 기절  

    // 루돌프 -> 산타 충돌일경우(C)
    // 산타 점수 + C
    // 산타는 루돌프의 이동방향으로 C칸 밀림
    // 산타는 k+1까지 기절 

    santas[santa_num].score += score;
    int santa_new_x = x + (score * x_to_move);
    int santa_new_y = y + (score * y_to_move);

    // 튕겨서 격자 밖으로 간 산타는 탈락
    if(santa_new_x < 1|| santa_new_y < 1|| santa_new_x > N || santa_new_y > N){
        santa_cnt--;
        santas[santa_num].fail = 1;
    }   

    // 다른 산타와 연쇄 충돌일 경우
    else if(MAP[santa_new_x][santa_new_y] != 0){
        santas[santa_num].sleep = turn + 1; // 기절
        SantaInteract(x_to_move, y_to_move, santa_new_x, santa_new_y, santa_num);
    }

    // 연쇄 충돌 아니면 그 칸으로 이동하고 기절 
    else if(MAP[santa_new_x][santa_new_y] == 0){
        MAP[santa_new_x][santa_new_y] = santa_num;
        santas[santa_num].sleep = turn + 1;
        MAP[santas[santa_num].x][santas[santa_num].y] = 0;  // 산타가 원래 있던 자리는 비우기
        santas[santa_num].x = santa_new_x;
        santas[santa_num].y = santa_new_y;
    }
}

// 산타끼리 충돌 = 상호작용
// santa_num = 이동해온 산타 번호 
void SantaInteract(int x_move, int y_move, int x, int y, int santa_num){
    // 충돌 당한 자리에 원래 있던 산타
    Santa crashed_santa = santas[MAP[x][y]];


    // 충돌 당한 산타는 이동 방향으로 1칸 밀림
    int new_x = crashed_santa.x + x_move;
    int new_y = crashed_santa.y + y_move;

    // 게임판 밖이면 탈락
    if (new_x < 1 || new_y < 1 || new_x > N || new_y > N) {
        crashed_santa.fail = 1;
    }

    // 루돌프와 부딪힌다면 산타 -> 루돌프 충돌 
    else if(MAP[new_x][new_y] == -1){
        SantaRudolfCrash(D, new_x, new_y, crashed_santa.num, x_move, y_move);
    }

    // 다른 산타가 있다면 연쇄적으로 충돌
    else if(MAP[new_x][new_y] != 0){
        SantaInteract(x_move, y_move, new_x, new_y, crashed_santa.num);
    }

    // 아무도 없다면 그 자리로 이동 
    else{
        MAP[new_x][new_y] = crashed_santa.num;
        crashed_santa.x = new_x;
        crashed_santa.y = new_y;
        santas[crashed_santa.num] = crashed_santa;
    }

    // 새로운 산타가 자리 차지
    MAP[x][y] = santa_num;
    santas[santa_num].x = x;
    santas[santa_num].y = y;
}



// 1. 루돌프 움직이기
// 가장 가까운 & 탈락하지 않은 산타를 향해 8방향 중 하나로 1칸 이동
void MoveRudolf(){
    
    int min_dist = 1e9;
    int closest_santa_x = -1;
    int closest_santa_y = -1;

    // 루돌프와 가장 가까운 산타 찾기 
    for(int i = N ; i >= 1; i--){
        for(int j = N; j >= 1 ; j--){
            if(MAP[i][j] == 0 || MAP[i][j] == -1) continue;
            
            int santa_num = MAP[i][j];
            if(santas[santa_num].fail == 1) continue;   // 탈락한 산타는 제외

            int dist = pow(rudolf.x - santas[santa_num].x, 2) + pow(rudolf.y - santas[santa_num].y, 2);
            if(dist < min_dist) {
                closest_santa_x = santas[santa_num].x;
                closest_santa_y = santas[santa_num].y;
                min_dist = dist;
            }
        }
    }

    // 8방향 중 이동할 방향 정하기
    int x_to_move = 0;
    int y_to_move = 0;
    if (rudolf.x < closest_santa_x) x_to_move++;
    else if (rudolf.x > closest_santa_x) x_to_move--;
    if (rudolf.y < closest_santa_y) y_to_move++;
    else if(rudolf.y > closest_santa_y) y_to_move--;

    int new_x = rudolf.x + x_to_move;
    int new_y = rudolf.y + y_to_move;
    MAP[rudolf.x][rudolf.y] = 0;    // 루돌프 원래 있던 자리 0으로

    // 루돌프 이동으로 충돌이 생긴다면
    if(MAP[new_x][new_y] != 0){
        // 산타가 C만큼 점수를 얻고, 루돌프 이동방향으로 C칸 밀리기 
        SantaRudolfCrash(C, new_x, new_y, MAP[new_x][new_y], x_to_move, y_to_move);
    }

    MAP[new_x][new_y] = -1;     // 루돌프 이동
    rudolf.x = new_x;
    rudolf.y = new_y;
}


// 2. 산타 이동 
// 기절하지 않은 산타만 루돌프에게 가까워지는 방향으로
int dx[] = {-1, 0, 1, 0};   // 상 - 우 - 하 - 좌 우선순위 
int dy[] = {0, 1, 0, -1};

void MoveSanta(int cur_turn ){
    for(int i = 1; i<= P ; i++){
        // 탈락한 산타 제외, K턴에 기절 상태인 산타 제외
        if(santas[i].fail || santas[i].sleep >= cur_turn) continue;

        // 상하좌우 중 루돌프와 가장 가까운 쪽이 어딘지 찾기
        int min_dist = pow(rudolf.x - santas[i].x, 2) + pow(rudolf.y - santas[i].y, 2);
        int santa_new_x = -1;
        int santa_new_y = -1;
        int dir = -1;

        for(int d = 0; d < 4; d++){
            int new_x = santas[i].x + dx[d];
            int new_y = santas[i].y + dy[d];

            // 격자 범위 체크
            if (new_x < 1 || new_y < 1 || new_x > N || new_y > N) continue;
            // 다른 산타가 있는 칸은 움직일 수 없음
            if (MAP[new_x][new_y] != -1 && MAP[new_x][new_y] != 0) continue;

            // 현재 거리보다 가까워 질 수 있는 방법만 고려
            int dist = pow(new_x - rudolf.x, 2) + pow(new_y - rudolf.y, 2);
            if (dist < min_dist){
                santa_new_x = new_x;
                santa_new_y = new_y;
                min_dist = dist;
                dir = d;
            }
        }
        // 움직일 수 있는 칸이 없으면 움직이지 않음
        if(dir == -1) continue;

        MAP[santas[i].x][santas[i].y] = 0;  // 원래 산타가 있던 자리 0으로

        // 해당 칸에 루돌프가 있으면 산타->루돌프 충돌
        // 산타는 자신의 이동방향 반대로 밀림
        if(MAP[santa_new_x][santa_new_y] == -1){
            SantaRudolfCrash(D, santa_new_x, santa_new_y, i, -dx[dir], -dy[dir]);
        }

        // 어짜피 다른 칸에 산타가 있으면 이동 안함!
        // // 해당 칸에 산타가 있으면 연쇄 충돌
        // else if(MAP[santa_new_x][santa_new_y] != 0){
        //     SantaInteract(dx[dir], dy[dir], santa_new_x, santa_new_y, i);
        // }

        // 아무도 없으면 그냥 이동
        else {
            MAP[santa_new_x][santa_new_y] = i;
            santas[i].x = santa_new_x;
            santas[i].y = santa_new_y;
        }
    }
}


// 해당 턴에 살아남은 산타들에게 +1 점수
void AddScore(){
    for(int p = 1; p <= P; p++){
        if(santas[p].fail != 1) santas[p].score++;
    }
}

int main(){
    input();

    while(turn <= M){
        if(santa_cnt == 0) break;   // P명의 산타가 모두 탈락시 게임 종료 
        MoveRudolf();
        MoveSanta(turn);
        AddScore();  // 해당 턴에 살아남은 산타들 점수 +1

        // for(int i = 1; i<= P; i++){
        //     cout << santas[i].score << " ";
        // }
        // cout << "\n";
        turn++;
    }
    for(int i = 1; i<= P; i++){
    cout << santas[i].score << " ";
    }
}