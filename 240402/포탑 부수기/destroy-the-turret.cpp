#include <iostream>
#include <vector>
#include <algorithm>
#include <queue>
#include <cstring>

using namespace std;

int N, M, K;
int map[12][12];     // (공격력) 저장
struct Turret{
    int x, y, pow, time;
};
vector<Turret> live_turret;     // 부서지지 않은 포탑 정보만 저장
Turret attacking, attacked;
string final_path;
int half_attack, flag, turn, live_turret_cnt;



struct Node{
    int x, y;
    string path;
};

int dx[] = {0, 1, 0, -1};  // 우하좌상 우선
int dy[] = {1, 0, -1, 0};

int dx2[] = {0, 0, 1, 0, -1, 1, 1, -1, -1};  
int dy2[] = {0, 1, 0, -1, 0, 1, -1, -1, 1};

int visited[12][12];
int tmp[12][12];


bool cmp(const Turret& left, const Turret& right){
    // 공격자가 왼쪽으로 가도록
    // 공격력 작은거 -> 공격시점 큰거 -> 행과열 합 큰거 -> 열 큰거 우선 순위
    // 공격력 비교 
    if (left.pow != right.pow) return left.pow < right.pow;
    // 공격력 같으면 시점이 큰거
    if (left.time != right.time) return left.time > right.time;
    // 행과열 합
    if (left.x + left.y != right.x + right.y) return left.x + left.y > right.x + right.y;
    // 열이 큰거 우선
    return left.y > right.y;
}

// BFS로 최단 경로 찾기
void LaserAttack(){
    final_path = "";
    memset(visited, 0, sizeof(visited));
    visited[attacking.x][attacking.y] = 1;

    queue <Node> qu;    // ((x, y), 경로)
    qu.push({attacking.x, attacking.y, ""});

    while(!qu.empty()){
        auto now = qu.front();
        qu.pop();
        int x = now.x;
        int y = now.y;
        string cur_path = now.path;

        // 목적지에 도달했을 때
        if(x == attacked.x && y == attacked.y) {
            final_path = cur_path;
            break;
        }

        for(int d = 0 ; d < 4 ; d++){
            int new_x = (x + dx[d] + N) % (N);
            int new_y = (y + dy[d] + M) % (M);

            if(tmp[new_x][new_y] == 0) continue;    // 부서진 포탑 위치는 지나갈 수 X
            if(visited[new_x][new_y]) continue;     // 이미 방문한 칸은 패스
            
            visited[new_x][new_y] = 1;
            qu.push({new_x, new_y, cur_path + to_string(d)});
        }
    }


    // 레이저 공격 가능하다면 경로에 있는 포탑들 공격력 깎기
    if(final_path.size() != 0){
        // 공격 당하는 포탑 공격력 깎기
        tmp[attacked.x][attacked.y] -= attacking.pow;
        if(tmp[attacked.x][attacked.y] < 0) {
            tmp[attacked.x][attacked.y] = 0;
            live_turret.erase(live_turret.end()-1);
        }

        // 경로 추적하면서 경로에 있는 모든 포탑에 pow / 2 만큼 공격 진행
        int next_x = attacking.x;
        int next_y = attacking.y;
         
        for(int i = 0; i < final_path.size()-1; i++){
            int dir = final_path[i] - '0';
            next_x = (next_x + dx[dir] + N) % (N);
            next_y = (next_y + dy[dir] + M) % (M);
            
            // 공격력 업데이트
            tmp[next_x][next_y] -= half_attack;
            if(tmp[next_x][next_y] < 0){
                tmp[next_x][next_y] = 0;
            }
        }
    }
}

void BombAttack(){
    for(int d = 0 ; d < 9 ; d++){
        int new_x = (attacked.x + dx2[d] + N) % (N);
        int new_y = (attacked.y + dy2[d] + M) % (M);
        
        if(new_x == attacking.x && new_y == attacking.y) 
            continue; // 공격자는 영향 X

        if(new_x == attacked.x && new_y == attacked.y) {    // 공격대상은 pow 만큼 공격
            tmp[new_x][new_y] -= attacking.pow;
            if(tmp[new_x][new_y] < 0) tmp[new_x][new_y] = 0;
        }

        if(tmp[new_x][new_y] == 0) continue;    // 이미 부서진 포탑은 pass 

        tmp[new_x][new_y] -= half_attack;
        if (tmp[new_x][new_y] < 0) {
            tmp[new_x][new_y] = 0;
        }
    }
}

// 포탑 정비 
void Organize(){
    // 1) tmp 결과 map으로 옮기기
    // + 공격과 무관한 포탑들은 공격력 + 1
    // 2) live_turret 벡터 업데이트  + 공격자는 마지막 공격 시점 업데이트
    for(int i = 0; i < N; i++){
        for(int j = 0 ; j < M; j++){
            if(map[i][j] == 0) continue;
            if(map[i][j] == tmp[i][j]){
                if(!flag) map[i][j] = tmp[i][j] + 1;
            }
            else map[i][j] = tmp[i][j];
        }
    }

    for(auto it = live_turret.begin(); it!= live_turret.end(); ){
        if(map[it->x][it->y] == 0){ // 부서진 포탑은 제거
            it = live_turret.erase(it); 
        }
        else{
            it->pow = map[it->x][it->y];
            it++;
        }
    }
}

int main(){
    //freopen("input.txt", "r", stdin);
    cin >> N >> M >> K;
    for(int i = 0 ; i < N ; i++){
        for(int j = 0 ; j < M; j++){
            int pow; cin >> pow;
            if (pow != 0) {
                live_turret.push_back({i, j, pow, 0});
            }
            map[i][j] = pow;
        }
    }

    // K번의 턴 반복
    turn = 1;
    while(turn <= K){
        // 살아있는 포탑이 1개 이하면 바로 종료
        if (live_turret.size() <= 1) break;

        // 공격 결과를 저장할 임시 변수 초기화 
        for(int i = 0; i < N; i++){
            for(int j = 0; j < M; j++){
                tmp[i][j] = map[i][j];
            }
        }

        // 1. 공격자(가장 약한 포탑) & 공격 받을 포탑(가장 강한 포탑) 선정
        sort(live_turret.begin(), live_turret.end(), cmp);
        attacking = live_turret.front();
        attacked = live_turret.back();

        // 공격자 공격력 증가 & 공격시점 업데이트
        tmp[attacking.x][attacking.y] += (N + M);
        attacking.pow = tmp[attacking.x][attacking.y];
        attacking.time = turn;
        live_turret[0] = attacking;

        half_attack = tmp[attacking.x][attacking.y] / 2;
        // 공격자 공격력의 절반이 1 미만이면 아무 영향 X
        if(half_attack >= 1){  
            // 2. 레이저 공격 가능한지
            // 3. 안되면 포탄 공격
            LaserAttack();
            if (final_path.size() == 0) {
                BombAttack();
            }
        }

        // 포탑 재정비
        Organize();
        turn++;
    }

    sort(live_turret.begin(), live_turret.end(), cmp);
    cout << live_turret.back().pow ;
    return 0;
}