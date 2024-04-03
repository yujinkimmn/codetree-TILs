#include <iostream>
#include <vector>
#include <algorithm>
#include <queue>
#include <tuple>

using namespace std;

int N, M, K;
int turn;
int map[10][10];     // (공격력) 저장
bool is_active[10][10]; // 공격에 관련 있는 포탑인지 여부를 저장
int rec[10][10];    // 마지막 공격 시점

struct Turret{
    int x, y, power, time;
};
vector<Turret> live_turret;     // 부서지지 않은 포탑 정보만 저장
Turret attacking, attacked;

int dx[4] = {0, 1, 0, -1};  // 우하좌상 우선
int dy[4] = {1, 0, -1, 0};
int dx2[9] = {0, 0, 1, 0, -1, 1, 1, -1, -1};  // 우하좌상 우선
int dy2[9] = {0, 1, 0, -1, 0, 1, -1, -1, 1};

// BFS에 사용할 변수
bool visited[10][10];
int back_x[10][10], back_y[10][10];


bool cmp(const Turret& left, const Turret& right){
    // 공격자가 왼쪽으로 가도록
    // 공격력 작은거 -> 공격시점 큰거 -> 행과열 합 큰거 -> 열 큰거 우선 순위
    // 공격력 비교 
    if (left.power != right.power) return left.power < right.power;
    // 공격력 같으면 시점이 큰거
    if (left.time != right.time) return left.time > right.time;
    // 행과열 합
    if (left.x + left.y != right.x + right.y) return left.x + left.y > right.x + right.y;
    // 열이 큰거 우선
    return left.y > right.y;
}




bool LaserAttack(){
    int sx = attacking.x , sy = attacking.y;
    int ex = attacked.x, ey = attacked.y;
    int pow = attacking.power;

    queue <pair<int, int>> qu;    // ((x, y), 경로)
    qu.push(make_pair(sx, sy));
    visited[sx][sy] = true;

    bool can_attack = false;

    // BFS로 최단 경로 찾기
    while(!qu.empty()){
        int x, y;
        tie(x, y)= qu.front(); qu.pop();

        // 목적지에 도달했을 때
        if(x == ex && y == ey) {
            can_attack = true;
            break;
        }
        
        for(int d = 0 ; d < 4 ; d++){
            int nx = (x + dx[d] + N) % N;
            int ny = (y + dy[d] + M) % M;

            if(visited[nx][ny]) continue;     // 이미 방문한 칸은 패스
            if(map[nx][ny] == 0) continue;    // 부서진 포탑 위치는 지나갈 수 X
            
            visited[nx][ny] = true;
            back_x[nx][ny] = x;
            back_y[nx][ny] = y; 
            qu.push(make_pair(nx, ny));
        }
    }


    // 레이저 공격 가능하다면 경로에 있는 포탑들 공격력 깎기
    if(can_attack){
        map[ex][ey] -= pow;
        if(map[ex][ey] < 0) map[ex][ey] = 0;
        is_active[ex][ey] = true;

        // back_x, back_y 를 이용해 역추적하면서 경로 상의 포탑들 pow/2 만큼 공격하기
        int cx = back_x[ex][ey];
        int cy = back_y[ex][ey]; 
         
        while(!(cx == sx && cy == sy)){
            map[cx][cy] -= pow / 2;
            if(map[cx][cy] < 0) map[cx][cy] = 0;
            is_active[cx][cy] = true;

            int next_cx = back_x[cx][cy];
            int next_cy = back_y[cx][cy];

            cx = next_cx;
            cy = next_cy;
        }
    }
    return can_attack;
}

void BombAttack(){
    int sx = attacking.x , sy = attacking.y;
    int ex = attacked.x, ey = attacked.y;
    int pow = attacking.power;

    for(int d = 0 ; d < 9 ; d++){
        int nx = (ex + dx2[d] + N) % N;
        int ny = (ey + dy2[d] + M) % M;
        
        if(nx == sx && ny == sy) continue; // 공격자 자신은 영향 X

        if(nx == ex && ny == ey){   // 공격 당하는 포탑이면 pow 만큼 감소
            map[nx][ny] -= pow;
            if (map[nx][ny] < 0) {
                map[nx][ny] = 0;
            }
            is_active[nx][ny] = true;
        }
        else {
            map[nx][ny] -= pow / 2;
            if (map[nx][ny] < 0) {
                map[nx][ny] = 0;
            }
            is_active[nx][ny] = true;
        }
            
    }
}

// 포탑 정비 
void Reserve(){
   // 공격과 무관한 포탑들은 공격력 + 1
    for(int i = 0; i < N ; i++){
        for(int j = 0 ; j < M; j++){
            if(is_active[i][j]) continue;
            if(map[i][j] == 0) continue;
            map[i][j]++;
        }
    }
}

int main(){
    //freopen("input.txt", "r", stdin);
    cin >> N >> M >> K;
    for(int i = 0 ; i < N ; i++){
        for(int j = 0 ; j < M; j++){
            cin >> map[i][j];
        }
    }

    // K번의 턴 반복
    while(turn < K){
        turn++;
        live_turret.clear();
        for(int i = 0 ; i < N ;i++){
            for(int j = 0 ; j < M ;j++){
                if(map[i][j]){
                    live_turret.push_back({i, j, map[i][j], rec[i][j]});
                }
            }
        }
        // 살아있는 포탑이 1개 이하라면 바로 종료
        if (live_turret.size() <= 1) break;

        // 턴 진행에 필요한 변수 초기화 
       
        for(int i = 0; i < N; i++){
            for(int j = 0; j < M; j++){
                is_active[i][j] = false;
                visited[i][j] = false;
            }
        }

        // 1. 공격자(가장 약한 포탑) & 공격 받을 포탑(가장 강한 포탑) 선정
        sort(live_turret.begin(), live_turret.end(), cmp);

        attacking = live_turret.front();
        attacked = live_turret.back();
        int x = attacking.x, y = attacking.y;

        map[x][y] += N + M; // 공격자 공격력 증가
        rec[x][y] = turn;

        attacking.power = map[x][y];
        attacking.time = rec[x][y];
        is_active[x][y] = true;

        live_turret[0] = attacking;

        
        // 2. 레이저 공격 가능한지
        bool is_suc = LaserAttack();

        // 3. 안되면 포탄 공격
        if(!is_suc) BombAttack();

        Reserve();
    }
    // sort(live_turret.begin(), live_turret.end(), cmp);
    // cout << live_turret.back().power ;
    int ans = 0;
    for(int i = 0; i < N; i++)
        for(int j = 0; j < M; j++)
            ans = max(ans, map[i][j]);

    cout << ans;
    return 0;
}