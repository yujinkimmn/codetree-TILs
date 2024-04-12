#include<iostream>
#include<vector>
#include<cmath>
#include<tuple>

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

void RudolfMove(){
    // 가장 가까운 산타 찾기
    // 거리 작은 순 -> r 큰 순 -> c 큰 순
    // tuple<int, int, int> closest_santa = make_tuple(1e9, 0, 0); // (거리, r, c)
    int closest_index = 0;
    int closest_dist = 1e9;

    // 탈락하지 않은 산타 중 가장 가까운 산타 찾기
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
    // for(int i = 1 ; i <= P ; i++){
    //     if(dead[i]) continue;
    //     int x, y; tie(x, y) = santa_pos[i];

    //     int dist = (int) pow(rx - x, 2) + (int) pow(ry - y, 2);
    //     tuple<int, int, int> cur = make_tuple(dist, -x, -y);

    //     if(cur < closest_santa){
    //         closest_santa = cur;
    //         closest_index = i;
    //     }
    // }
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
    if(dir == -1) cout << "루돌프 이동 방향 오류\n";

    // 원래 있던 자리는 0으로
    grid[rx][ry] = 0;
    // 루돌프 새로운 위치로 이동
    tie(rx, ry) = make_pair(rx + dx[dir], ry + dy[dir]);

    // 산타와 충돌했다면 충돌 처리
    if(grid[rx][ry] > 0){
        int s1 = grid[rx][ry];
        // turn+1 까지 기절 
        sleep[s1] = turn + 1;
        // C만큼 점수 추가
        scores[s1] += C;
        // s1 산타는 루돌프의 이동방향 dir으로 C칸 움직임
        int nx = rx + C * dx[dir], ny = ry + C * dy[dir];

        // 격자 밖이면 탈락
        if(!InRange(nx, ny)){
            dead[s1] = true;
        }

        // 산타가 움직인 칸에 다른 산타가 있으면 연쇄 충돌 -> 연쇄 충돌 일어나는 마지막 산타 찾기
        else if(grid[nx][ny] > 0){
            int end_x = nx, end_y = ny;
            // 격자 내에서 다른 산타가 있는 동안은 계속 이동
            while(InRange(end_x, end_y) && grid[end_x][end_y] > 0){
                end_x += dx[dir], end_y += dy[dir];
            }
            // 마지막 칸에 이전 산타들 하나씩 넣어주기 
            while(end_x != nx || end_y != ny){
                int before_santa = grid[end_x - dx[dir]][end_y - dy[dir]];
                if (InRange(end_x, end_y)){
                    grid[end_x][end_y] = before_santa;
                    santa_pos[before_santa] = {end_x, end_y};
                }
                else 
                    dead[before_santa] = true;
                end_x -= dx[dir], end_y -= dy[dir];
            }
            // 연쇄충돌 처음 일어난 산타 넣어주기
            grid[nx][ny] = s1;
            santa_pos[s1] = make_pair(nx, ny);
        }

        // 빈 칸이면 그냥 이동
        else{
            // 원래 있던 칸은 루돌프가 채울거임
            grid[nx][ny] = s1;
            santa_pos[s1] = make_pair(nx, ny);
        }
    }
    // 산타 모두 처리 후 격자에서 루돌프 위치 이동
    grid[rx][ry] = -1;
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
        
        // 루돌프랑 충돌 없으면 그냥 이동(산타랑 충돌은 X. 어짜피 산타 있는 칸 못감)
        if(make_pair(rx, ry) != new_pos) {
            // 원래 있던 칸 초기화
            grid[sx][sy] = 0;
            grid[new_pos.first][new_pos.second] = i;
            santa_pos[i] = new_pos;
        }

        // 루돌프가 있으면 충돌
        else {
            // 해당 산타는 D점 얻기
            scores[i] += D;
            // turn + 1까지 기절
            sleep[i] = turn + 1;

            // D == 1이면 산타는 원래 자기 자리를 유지한다. 
            if(D == 1) continue;
            
            // 아니면 산타는 자신의 이동방향 반대로 D칸 이동
            dir = (dir + 2) % 4;
            int nx = rx + D * dx[dir], ny = ry + D * dy[dir];

            // 격자 밖이면 원래 있던 칸 초기화만 하고 탈락
            if (!InRange(nx, ny)){
                dead[i] = true;
                grid[sx][sy] = 0;
            }
            // (nx, ny)에 다른 산타가 있으면 연쇄 충돌 -> 연쇄 충돌 일어나는 마지막 산타 찾기
            else {
                int end_x = nx, end_y = ny;
                // 격자 내에서 다른 산타가 있는 동안은 계속 이동
                while(InRange(end_x, end_y) && grid[end_x][end_y] > 0){
                    end_x += dx[dir], end_y += dy[dir];
                }
                // 마지막 칸에 이전 산타들 하나씩 넣어주기 
                while(end_x != nx || end_y != ny){
                    int before_santa = grid[end_x - dx[dir]][end_y - dy[dir]];
                    if (InRange(end_x, end_y)){
                        grid[end_x][end_y] = before_santa;
                        santa_pos[before_santa] = {end_x, end_y};
                    }
                    else 
                        dead[before_santa] = true;
                    end_x -= dx[dir], end_y -= dy[dir];
                }
                // 연쇄충돌 처음 일어난 산타 처리
                grid[nx][ny] = i;
                santa_pos[i] = {nx, ny};
                // 원래 있던 자리는 0으로
                grid[sx][sy] = 0;
            }
        }
    }
}

void AddScore(){
    // 살아남은 산타들 점수 추가
    for(int i = 1; i <= P; i++){
        if(!dead[i]) scores[i] += 1;
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

        // Step 2. 산타 움직임
        SantaMove();

        // 살아 있는 산타들은 1점씩 추가하기
        AddScore();

    }

    // 산타들이 얻은 점수 차례로 출력
    for(int i = 1; i <= P; i++){
        cout << scores[i] << " ";
    }
}