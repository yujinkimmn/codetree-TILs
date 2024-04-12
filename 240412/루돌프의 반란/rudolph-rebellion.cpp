#include<iostream>
#include<vector>
#include<cmath>
#include<tuple>
#include<queue>

using namespace std;

int N, M, P, C, D;
// 현재 턴 수 
int turn;
// 격자(산타들의 위치를 번호로 표시)
int grid[51][51];
int tmp[51][51];

// 산타들의 위치를 표시
pair<int, int> santa_pos[31];   
// 산타들이 몇 번 턴까지 기절인지
int sleep[31];
// 탈락한 산타 정보 
bool dead[31];
// 이동 방향들
int dx[8] = {-1, 0, 1, 0, -1, 1, 1, -1};    // 상우하좌  - 나머지 대각선 시계방향 순
int dy[8] = {0, 1, 0, -1, 1, 1, -1, -1};
// 산타들의 점수 
int scores[31];

// 루돌프 위치
int rx, ry;

// 종료 조건
bool End(){
    if (turn >= M) return true;
    for(int i = 1; i <= P ; i++){
        // 한 명이라도 살아 있는 산타가 있으면 아직 종료 X
        if(!dead[i]) return false;
    }
    return true;
}

int InRange(int x, int y){
    return 1 <= x && x <= N && 1 <= y && y <= N;
}

// 산타를 실제로 이동시키는 명령
// s_num번 산타를 (x, y)에서 dir 방향으로 amount 만큼 이동
void Move(int s_num, int x, int y, int dir, int amount){

    queue<tuple<int, int, int, int>> qu;
    qu.push(make_tuple(s_num, x, y, amount));

    while(!qu.empty()){
        int num, r, c, a; 
        tie(num, r, c, a) = qu.front(); qu.pop();
        int nx = r + a * dx[dir];
        int ny = c + a * dy[dir];

        // 밖으로 튕겨나감 -> 탈락
        if (!InRange(nx, ny)){
            dead[num] = true;
            return;   
        }
        // 빈 칸이면 그냥 이동
        else if(grid[nx][ny] == 0){
            grid[nx][ny] = num;
            santa_pos[num] = make_pair(nx, ny);
            return;
        }
        // 다른 산타와 충돌 => 큐로 연쇄 충돌 처리
        else{
            // 다른 산타는 1칸 이동
            qu.push(make_tuple(grid[nx][ny], nx, ny, 1)); 
            // 자기 자신은 위치 업데이트
            grid[nx][ny] = num;
            santa_pos[num] = make_pair(nx, ny);   
        }
    }
}

void RudolfMove(){
    // 가장 가까운 산타 찾기
    int closest_index = 0;
    int closest_dist = 1e9;

    // 탈락하지 않은 산타 중 가장 가까운 산타 찾기
    // 거리 작은 순 -> r 큰 순 -> c 큰 순
    for(int i = N; i >= 1; i--){
        for(int j = N; j >= 1; j--){
            int s = grid[i][j];
            if(s <= 0) continue;
            if(dead[s]) continue;

            int dist = (int) pow(rx - i, 2) + (int) pow(ry - j, 2);
            if (dist < closest_dist){
                closest_dist = dist;
                closest_index = s;
            }
        }
    }

    // 이동할 산타가 더 이상 없으면
    if(closest_index == 0) return;

    // 8개 산타 중 해당 타겟 산타로 가까워지는 방향으로 이동
    int dir = -1;
    int cur_best = 1e9;
    int target_x, target_y; tie(target_x, target_y) = santa_pos[closest_index];

    for(int d = 0 ; d < 8 ; d++){
        int nx = rx + dx[d];
        int ny = ry + dy[d];
        if(!InRange(nx, ny)) continue;

        int dist = (int) pow(nx - target_x, 2) + (int) pow(ny - target_y, 2);
        if(dist < cur_best){
            cur_best = dist;
            dir = d;
        }
    }

    // 원래 있던 자리는 0으로(루돌프는 무조건 해당 자리로 이동한다)
    grid[rx][ry] = 0;
    // 루돌프 새로운 위치로 이동
    tie(rx, ry) = make_pair(rx + dx[dir], ry + dy[dir]);
    // 격자에서 루돌프 위치 이동
    grid[rx][ry] = -1;

    // 루돌프가 이동한 위치에서 산타와 충돌 => 산타 이동 
    if(make_pair(rx, ry) == make_pair(target_x, target_y)){
        int s = closest_index;
        // 산타 C점 얻고, turn + 1까지 기절하고, 루돌프 이동 방향으로 C칸
        scores[s] += C;
        sleep[s] = turn + 1;
        // 산타 있던 자리는 어짜피 루돌프가 차지함
        Move(s, rx, ry, dir, C);    // 산타 s가 (rx, ry)에서 dir 방향으로 C칸 이동
    }
}


// 1번부터 P번까지 기절 안했고, 탈락하지 않은 산타만 이동
void SantaMove(){
    for (int i = 1; i <= P; i++){
        if(dead[i] || sleep[i] >= turn) continue;

        // 루돌프에게 가장 가까워지는 방향을 찾기
        int sx, sy; tie(sx, sy) = santa_pos[i];
        // 현재 위치보다 가까워져야 함
        int cur_dist = (int) pow(rx - sx, 2) + (int) pow(ry - sy ,2);
        int dir = -1;

        for(int d = 0 ; d < 4; d++){    // 상우하좌 우선
            int nx = sx + dx[d], ny = sy + dy[d];
            // 격자 밖이나 다른 산타가 있는 칸은 움직일 수 없음
            if(!InRange(nx, ny) || grid[nx][ny] > 0) continue;

            int dist = (int) pow(rx - nx, 2) + (int) pow(ry - ny, 2);
            if(dist < cur_dist){
                dir = d;
                cur_dist = dist;
            }
        }
        
        // 그런 칸이 없으면 이동하지 않음. 원래 위치 유지
        if(dir == -1) continue;

        // 이동 가능한 위치
        pair<int, int> new_pos = make_pair(sx + dx[dir], sy + dy[dir]);
        // 원래 자리는 0으로
        grid[sx][sy] = 0;

        // 루돌프와 충돌했는지?
        if(new_pos == make_pair(rx, ry)){
            // D점 획득, turn+1까지 기절
            scores[i] += D;
            sleep[i] = turn + 1;

            // 산타를 자신의 이동방향 반대로 D칸 이동
            Move(i, new_pos.first, new_pos.second, (dir+2)%4, D);
        }

        // 충돌 안했으면 그 자리 그대로 이동
        else{   
            // 새로운 자리로 업데이트
            grid[new_pos.first][new_pos.second] = i;
            santa_pos[i] = new_pos;
        }
    }
}



void AddScore(){
    // 살아남은 산타들 점수 추가
    for(int i = 1; i <= P; i++){
        if(!dead[i]) scores[i] += 1;
    }
}

void print(){
    // 디버깅용
    cout << "\n";
    for(int i = 1; i <= N; i++){
        for(int j = 1; j <= N; j++){
            cout << grid[i][j] << " ";
        }
        cout << "\n";
    }
}

int main(){
    // 입력
    //freopen("input.txt", "r", stdin);
    cin >> N >> M >> P >> C >> D;
    cin >> rx >> ry;
    grid[rx][ry] = -1;
    for(int i = 1; i <= P ; i++){
        int num, x, y; cin >> num >> x >> y;
        santa_pos[num] = make_pair(x, y);
        grid[x][y] = num;
    }

    // M번 턴을 진행
    while(!End()){
        turn++; 

        // Step 1. 루돌프 움직임 1칸
        RudolfMove();
        //print();

        // Step 2. 산타 움직임
        SantaMove();
        //print();

        // 살아 있는 산타들은 1점씩 추가하기
        AddScore();
    }

    // 산타들이 얻은 점수 차례로 출력
    for(int i = 1; i <= P; i++){
        cout << scores[i] << " ";
    }
}