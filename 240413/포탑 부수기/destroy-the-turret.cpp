#include<iostream>
#include<vector>
#include<algorithm>
#include<queue>

using namespace std;

int N, M, K;
int turn;
int grid[10][10];
// 마지막 공격 시점
int attack_time[10][10];   
int visited[10][10];

struct Turret{
    int power, time, x, y;
};

// 공격자 정보
Turret attacker;
// 공격 대상 정보
Turret target;

bool cmp(Turret left, Turret right){
    // 공격력이 가장 낮은 포탑이 왼쪽에
    if(left.power != right.power) return left.power < right.power;
    // 공격력이 같다면, 가장 최근에 공격한 포탑이 왼쪽에
    else if(left.time != right.time) return left.time > right.time;
    // 행과 열의 합이 큰 포탑이 왼쪽에
    else if(left.x + left.y != right.x + right.y) return left.x + left.y > right.x + right.y;
    // 아니면 열 값이 큰 포탑이 왼쪽에
    else return left.y > right.y;
}

int dx[8] = {0, 1, 0, -1, -1, 1, 1, -1};  // 우하좌상 - 나머지 대각선 우측상단부터 시계방향
int dy[8] = {1, 0, -1, 0, 1, 1, -1, -1};

void Init(){
    for(int i = 0 ; i < N; i++){
        for(int j = 0 ; j < M; j++){
            visited[i][j] = 0;
        }
    }
}

// 레이저 공격 시도
// 가능하다면 공격자 ~ 공격대상 최단 경로를 리턴, 불가능하면 빈 vector 경로를 리턴
vector<pair<int, int>> CanLaserAttack(){
    // 초기화
    Init();
    
    queue<vector<pair<int, int>>> qu;
    visited[attacker.x][attacker.y] = 1;
    vector<pair<int, int>> p;
    p.push_back({attacker.x, attacker.y});
    qu.push(p);

    while(!qu.empty()){
        // 현재까지 온 경로
        vector<pair<int, int>> cur_path = qu.front(); qu.pop();
        // 경로의 마지막 칸(현재 칸)
        pair<int, int> cur = cur_path.back();

        // 공격대상까지 도착했으면 리턴
        if(cur == make_pair(target.x, target.y)){
            return cur_path;
        }

        // 우하좌상 순으로 탐색하며 경로 탐색
        for(int d = 0 ; d < 4 ; d++){
            // 반대편으로 이동할 수 있음!!!!
            int nx = (cur.first + dx[d] + N) % N;
            int ny = (cur.second + dy[d] + M) % M;

            // 이미 부서진 포탑 자리면 못감. 패스
            if (grid[nx][ny] == 0) continue;
            if (visited[nx][ny]) continue;
            
            // 현재 경로에 해당 칸을 추가 -> 업데이트 된 경로를 큐에 삽입
            auto cp = cur_path;
            cp.push_back({nx, ny});
            qu.push(cp);
            visited[nx][ny] = 1;
        }
    }
    // 공격 대상에 도달하지 못했으면 빈 path 리턴
    p.clear();
    return p;
}

// 레이저 공격을 path대로 직접 시행
// visited에 공격과 관련/무관 구분해주기
void DoLaserAttack(vector<pair<int, int>> path){
    // 초기화
    Init();

    for(auto cur : path){
        // 공격자는 visited만 표시
        // 공격 대상은 공격자의 공격력 만큼 피해 
        if(cur == make_pair(target.x, target.y)) {
            grid[cur.first][cur.second] -= attacker.power;
        }
        // 경로에 놓인 포탑이라면 공격자의 공격력 / 2 만큼 깎기
        else if(cur != make_pair(attacker.x, attacker.y)){
            grid[cur.first][cur.second] -= attacker.power / 2;
        }
        visited[cur.first][cur.second] = 1;
    }
}

// target 주위 8개 방향 포탑 공격하기
void BombAttack(){
    Init();
    // visited 표시
    visited[attacker.x][attacker.y] = 1;
    // 타겟은 공격자의 공격력만큼 감소
    visited[target.x][target.y] = 1;
    grid[target.x][target.y] -= attacker.power;

    for(int d = 0 ; d < 8 ; d++){
        int nx = (target.x + dx[d] + N) % N;
        int ny = (target.y + dy[d] + M) % M;

        if(visited[nx][ny]) continue;
        // 이미 죽은 포탑이면 패스
        if(grid[nx][ny] == 0) continue;

        // 공격자의 공격력 / 2만큼 공격력 감소
        visited[nx][ny] = 1; 
        grid[nx][ny] -= attacker.power / 2;
    }
}


// 부서지지 않은 포탑 중에서
// 공격과 무관한 포탑들은 공격력을 1씩 증가(visited == 0)
void AddPower(){
    for (int i = 0 ; i < N; i++){
        for(int j = 0; j < M; j++){
            if(grid[i][j] == 0) continue;
            if(visited[i][j] == 1) continue;
            grid[i][j] += 1;
        }
    }
}

int main(){
    // 입력
    //freopen("input.txt", "r", stdin);
    cin >> N >> M >> K;
    for(int i = 0 ; i < N; i++){
        for(int j = 0 ; j < M; j++){
            cin >> grid[i][j];
        }
    }

    // K번의 턴을 진행 
    while(K--){
        turn++;

        // 1. 격자 돌면서 공격자, 공격 대상 선정하기
        vector<Turret> turrets;
        for (int i = 0 ; i < N; i++){
            for(int j = 0 ; j < M; j++){
                if(grid[i][j] == 0) continue;
                turrets.push_back({grid[i][j], attack_time[i][j], i, j});
            }
        }
        // 부서지지 않은 포탑이 1개 남으면 게임 종료
        if(turrets.size() == 1) break;

        sort(turrets.begin(), turrets.end(), cmp);

        // 공격자는 벡터의 맨 앞 원소, 공격 대상은 맨 뒤 요소
        attacker = turrets.front();
        target = turrets.back();
 
        // 공격자 정보 업데이트(배열, 구조체 둘 다 해주기)
        // 공격자의 공격력 증가, 공격 시점 현재 turn으로 설정
        int power = grid[attacker.x][attacker.y] + (N + M);
        grid[attacker.x][attacker.y] = power;
        attack_time[attacker.x][attacker.y] = turn;
        attacker = Turret({power, turn, attacker.x, attacker.y});

        // 2. 레이저 공격이 가능한지 보기
        vector<pair<int, int>> path = CanLaserAttack();

        // 레이저 공격이 가능하면 경로대로 공격 실행
        if(!path.empty()){
            DoLaserAttack(path);
        }
        else{
            BombAttack();
        }

        // 3. 포탑 부서짐 - 공격력 0 이하가 된 애들 0으로 만들어주기 
        for(int i = 0 ; i < N; i++){
            for(int j = 0 ; j < M; j++){
                if(grid[i][j] < 0) grid[i][j] = 0;
            }
        }

        // 4. 포탑 정비
        // 부서지지 않은 포탑 중 공격과 무관한 포탑 공격력 1씩 증가
        AddPower();
    }
    // 남은 포탑 중 가장 강한 포탑 찾기
    int best = 0;
    for(int i = 0; i < N; i++){
        for(int j = 0 ; j < M; j++){
            best = max(best, grid[i][j]); 
        }
    }
    cout << best << "\n";
}