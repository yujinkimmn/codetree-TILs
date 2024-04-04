#include <iostream>
#include <vector>
#include <cmath>
#include <tuple>

#define MAX_N 100
#define MAX_M 10000

using namespace std;

int n, m, h, k;
int cnt;    // 정사각형 개수 세기 용도
int edge[2];    // 정사각형의 모서리 표시 용도
int square_len;
int op, turn, ans;

pair<int, int> tagger_pos;  // 술래 정보 
int tagger_dir;
int back_x[MAX_N][MAX_N];   // 역추적 위해서 이전 칸의 정보를 저장
int back_y[MAX_N][MAX_N];
int dir[MAX_N][MAX_N];      

int trees[MAX_N][MAX_N];      // 나무는 9 (겹치지 않음)

struct Runner{
    int x, y, dir;
};
vector<Runner> runner;   // 도망자 정보 저장

int dx[] = {-1, 0, 1, 0};   // 상-우-하-좌
int dy[] = {0, 1, 0, -1};


bool InRange(int x, int y){
    return x >= 1 && y >= 1 && x <= n && y <= n;
}

bool InSquare(int x, int y){
    int center = n / 2 + 1;
    int edge1 = center + (square_len-1)/2;   // 큰 경계값
    int edge2 = center - (square_len-1)/2;   // 작은 경계값
    return x <= edge1 && edge2 <= x && edge2 <= y && y <= edge1;
}

void MoveRunner(){
    // 도망자 한 명씩 이동
    for(int i = 0; i < m ; i++){
        int x = runner[i].x, y = runner[i].y; 
        int dir = runner[i].dir;

        // 술래와 거리가 3 이하인지 판단
        int dist = abs(tagger_pos.first - x) + abs(tagger_pos.second - y);
        if (dist > 3) continue;

        // 현재 방향에서 한 칸 이동하기
        int nx = x + dx[dir];
        int ny = y + dy[dir];

        // 격자 벗어나는지 체크
        if(!InRange(nx, ny)){
            // 벗어나면 방향 반대로 바꾸기
            dir = (dir + 2) % 4;
            nx =  x + dx[dir];
            ny = y + dy[dir];
            runner[i].dir = dir;
        }
        // 이동하려는 칸에 술래가 있으면 이동 X
        if (make_pair(nx, ny) == tagger_pos) continue;

        // 술래 없으면 한 칸 이동하기 
        else {
            runner[i].x = nx;
            runner[i].y = ny;
        }
    }
}

void MoveTagger(){
    // 다음 칸으로 이동
    int nx = tagger_pos.first + dx[tagger_dir];
    int ny = tagger_pos.second + dy[tagger_dir];
    // 역추적을 위해 이전 칸의 정보 저장
    back_x[nx][ny] = tagger_pos.first;
    back_y[nx][ny] = tagger_pos.second;

    tagger_pos = make_pair(nx, ny);
    dir[nx][ny] = tagger_dir; //역추적 위해 정보 저장(회전하기 전의 방향이 필요)
    cnt ++;

    // 이동한 칸이 (1, 1)이면 이동 방식 반대로 바꾸기
    if(tagger_pos == make_pair(1, 1)){
        op = -1;
        tagger_dir = (tagger_dir + 2) % 4;
        return;
    }

    // 정사각형 하나 채웠으면 정사각형 크기 키우거나 줄이기
    // & 기존 방향 유지
    if(cnt == square_len * square_len){
        square_len += 2;
        return;
    }
    
    // 그 다음 이동하려는 칸이 현재 정사각형 밖이면 미리 방향 회전 시켜 놓기
    int nnx = nx + dx[tagger_dir];
    int nny = ny + dy[tagger_dir];
    if(!InSquare(nnx, nny)){
        tagger_dir = (tagger_dir + 1) % 4;
    }
}


// (1,1)에서 -> 중심 방향으로 
void MoveTaggerReverse(){
    int x, y;
    tie(x, y) = tagger_pos;
    
    // 새로 이동할 칸 = MoveTagger 이동의 역추적 & 방향은 정반대로 
    int nx = back_x[x][y]; 
    int ny = back_y[x][y];
    int d = (dir[nx][ny] + 2) % 4;    

    tagger_pos = make_pair(nx, ny);
    tagger_dir = d;

    // 다시 시작 위치로 돌아왔을 경우
    // 정사각형 3으로 시작
    // cnt = 1로 시작
    // 방향 정 반대로 
    if(tagger_pos == make_pair(n/2+1, n/2+1)){
        cnt = 1;
        square_len = 3;
        op = 1;
        tagger_dir = (d + 2) % 4;
    }

}


void CatchRunner(){
    // 현재 술래의 시야에서 현재칸 포함 3칸에 있는 도망자 잡기
    int x, y;
    int catch_cnt = 0;
    tie(x, y) = tagger_pos;

    for(int i = 0 ; i < 3 ; i++){
        if(!InRange(x, y)) continue;

        if(trees[x][y] != 9) {
            // 나무가 없고, 해당 칸에 도망자가 있으면 잡기
            for(auto it = runner.begin(); it != runner.end(); ){
                if(it->x == x && it->y == y){   
                    it = runner.erase(it);
                    catch_cnt++;
                }
                else it++;
            }
        }

        x = x + dx[tagger_dir];
        y = y + dy[tagger_dir];
    }

    // t턴 x 잡은 도망자 수 만큼 점수 얻기
    ans += turn * catch_cnt;
}

int main(){
    // 입력
    //freopen("input2.txt", "r", stdin);
    cin >> n >> m >> h >> k;
    for(int i = 0; i < m ; i++){    // 도망자 정보
        int x, y, d;
        cin >> x >> y >> d;
        runner.push_back({x, y, d});
    }
    for(int i = 0 ; i < h ; i++){   // 나무 정보
        int x, y;
        cin >> x >> y;
        trees[x][y] = 9;
    }

    // 술래 시작 위치
    tagger_pos = make_pair(n/2+1, n/2+1);
    square_len = 3; 
    cnt = 1;
    op = 1;

    // k턴 동안 반복 
    while(k--){
        turn++;

        // Step 1. 도망자 움직이기 
        MoveRunner();

        // Step 2. 술래 한 칸 이동
        if(op == 1) MoveTagger();
        else if (op == -1) MoveTaggerReverse();

        // Step 3. 술래가 도망자 잡기
        CatchRunner();
    }
    cout << ans; 
}