#include <iostream>
#include <vector>
#include <cmath>
#include <tuple>

#define MAX_N 100
using namespace std;

int n, m, h, k;
int turn, ans;

pair<int, int> tagger_pos;  // 술래 현재 위치
bool forward_facing;        // 술래가 움직이는 방향이 정방향이면 true, 아니라면 false

//  현재 위치에서 술래가 움직여야 할 방향
int tagger_next_dir[MAX_N][MAX_N];  // 정방향
int tagger_rev_dir[MAX_N][MAX_N];   // 역방향 

bool tree[MAX_N][MAX_N];      // 나무 정보

struct Hider{
    int x, y, dir;
};
vector <Hider> hider;   // 도망자 정보 저장

int dx[] = {-1, 0, 1, 0};   // 상-우-하-좌
int dy[] = {0, 1, 0, -1};


bool InRange(int x, int y){
    return x >= 1 && y >= 1 && x <= n && y <= n;
}


// 정중앙 ~ 끝까지 움직이는 경로 계산
void InitTaggerPath(){
    // 시작 위치
    int curr_x = n/2 + 1, curr_y = n/2 + 1;
    // 방향, 해당 방향으로 이동할 횟수
    int move_dir = 0, move_num = 1; 

    while(curr_x > 1 || curr_y > 1){
        for(int i = 0 ; i < move_num ; i++){
            tagger_next_dir[curr_x][curr_y] = move_dir;     // 방향 저장
            curr_x += dx[move_dir];     // 한 칸 이동 
            curr_y += dy[move_dir]; 
            tagger_rev_dir[curr_x][curr_y] = (move_dir < 2) ? (move_dir + 2) : (move_dir -2);  // 역방향에서는 정방향의 반대로 저장

            // 이동하다가 (1, 1)로 오면 이동 종료
            if(curr_x == 1 && curr_y == 1)
                break;
        }

        // 방향 회전
        move_dir = (move_dir + 1) % 4;
        // 만약 현재 방향이 아래 혹은 위가 된 경우
        // 특정 방향으로 움직여야 할 횟수를 1 증가
        if (move_dir == 0 || move_dir == 2) move_num++;
    }
}

void MoveHider(){
    // 도망자 한 명씩 이동
    for(int i = 0; i < hider.size() ; i++){
        int x = hider[i].x, y = hider[i].y; 
        int dir = hider[i].dir;

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
            hider[i].dir = dir;
        }
        // 이동하려는 칸에 술래가 있으면 이동 X
        if (make_pair(nx, ny) == tagger_pos) continue;

        // 술래 없으면 한 칸 이동하기 
        else {
            hider[i].x = nx;
            hider[i].y = ny;
        }
    }
}

void MoveTagger(){
    // 이동할 방향 찾기
    int x, y;
    tie(x, y) = tagger_pos;

    int move_dir;
    if(forward_facing) move_dir = tagger_next_dir[x][y];
    else move_dir = tagger_rev_dir[x][y];

    // 다음 칸으로 이동
    tagger_pos = make_pair(x + dx[move_dir], y + dy[move_dir]);
     
    if (!InRange(tagger_pos.first , tagger_pos.second)){
        cout<< "tagger 이동 범위 벗어남" << "\n";
    }
    // 이동한 칸이 (1, 1) 혹은 중앙이면 이동 방식 반대로 바꾸기
    if(tagger_pos == make_pair(1, 1) && forward_facing){
        forward_facing = false;
    }
    if (tagger_pos == make_pair( n / 2 + 1, n / 2 + 1) && !forward_facing){
        forward_facing = true;
    }
}

void CatchHider(){
    // 현재 술래의 시야에서 현재칸 포함 3칸에 있는 도망자 잡기
    int x, y;
    int catch_cnt = 0;
    tie(x, y) = tagger_pos;

    int move_dir;
    if(forward_facing) move_dir = tagger_next_dir[x][y];
    else move_dir = tagger_rev_dir[x][y];

    for(int i = 0 ; i < 3 ; i++){
        if(!InRange(x, y)) continue;

        if(!tree[x][y]) {
            // 나무가 없고, 해당 칸에 도망자가 있으면 잡기
            for(auto it = hider.begin(); it != hider.end(); ){
                if(it->x == x && it->y == y){   
                    it = hider.erase(it);
                    catch_cnt++;
                }
                else it++;
            }
        }

        x = x + dx[move_dir];
        y = y + dy[move_dir];
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
        hider.push_back({x, y, d});
    }
    for(int i = 0 ; i < h ; i++){   // 나무 정보
        int x, y;
        cin >> x >> y;
        tree[x][y] = true;
    }

    // 술래 시작 위치 & 경로를 미리 저장
    tagger_pos = make_pair(n/2+1, n/2+1);
    forward_facing = true;
    InitTaggerPath();


    // k턴 동안 반복 
    while(k--){
        turn++;

        // Step 1. 도망자 움직이기 
        MoveHider();

        // Step 2. 술래 움직이기
        MoveTagger();

        // Step 3. 술래가 도망자 잡기
        CatchHider();
    }
    cout << ans; 
}