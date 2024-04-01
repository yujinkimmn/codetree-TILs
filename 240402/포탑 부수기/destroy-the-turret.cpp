#include <iostream>
#include <vector>
#include <algorithm>
#include <queue>
#include <cstring>

using namespace std;

int N, M, K;
int map[12][12];     // (공격력) 저장
struct Turret{
    int x, y, power, time;
};
vector<Turret> turrets;     // 부서지지 않은 포탑 정보만 저장
Turret attacking, attacked;
string final_path;
int attack_half, flag, turn, turrets_cnt;



struct Node{
    int x, y, dist;
    string path;
};

int dx[] = {0, 1, 0, -1, 1, 1, -1, -1};  // 우하좌상 우선
int dy[] = {1, 0, -1, 0, 1, -1, -1, 1};

int visited[12][12];
int tmp[12][12];


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

// BFS로 최단 경로 찾기
void LaiserAttack(Turret start, Turret dst){
    final_path = "";
    int shortest_dist = 1e9;
    memset(visited, 0, sizeof(visited));
    visited[start.x][start.y] = 1;

    queue <Node> qu;    // ((x, y), 경로)
    qu.push({start.x, start.y, 0, ""});

    while(!qu.empty()){
        auto now = qu.front();
        qu.pop();
        int x = now.x;
        int y = now.y;
        int dist = now.dist;
        string cur_path = now.path;

        for(int d = 0 ; d < 4 ; d++){
            int new_x = x + dx[d];
            int new_y = y + dy[d];
            int new_dist = dist + 1;

            if(new_x < 1) new_x = N;    // 격자 벗어나면 반대편으로 이동하기
            else if(new_x > N) new_x = 1;
            if(new_y < 1) new_y = M;
            else if(new_y > M) new_y = 1;

            if(tmp[new_x][new_y] <= 0) continue;    // 부서진 포탑 위치는 지나갈 수 X
            if(visited[new_x][new_y]) continue;     // 이미 방문한 칸은 패스

            // 목적지에 도달했을 때
            if(new_x == dst.x && new_y == dst.y) {
                if (new_dist < shortest_dist){
                    shortest_dist = new_dist;
                    final_path = cur_path;
                }
                else if(new_dist == shortest_dist){ // 경로 길이 같을 때는 우하좌상 우선
                    final_path = min(final_path, cur_path);
                }
            }
            
            visited[new_x][new_y] = 1;
            qu.push({new_x, new_y, new_dist, cur_path + to_string(d)});
        }
    }


    // 레이저 공격 가능하다면 경로에 있는 포탑들 공격력 깎기
    if(final_path.size() != 0){
        int next_x = start.x;
        int next_y = start.y;
         
        for(int i = 0; i < final_path.size(); i++){
            int dir = final_path[i] - '0';
            next_x += dx[dir];
            next_y += dy[dir];

            if(next_x < 1) next_x = N;    // 격자 벗어나면 반대편으로 이동하기
            else if(next_x > N) next_x = 1;
            if(next_y < 1) next_y = M;
            else if(next_y > M) next_y = 1; 

            // 공격력 업데이트
            tmp[next_x][next_y] -= attack_half;
            if(tmp[next_x][next_y] <= 0){
                turrets_cnt--;
                if(turrets_cnt < 1) {
                    flag = 1;
                    return;
                }
            }
        }
    }
}

void BombAttack(Turret start, Turret target){
    for(int d = 0 ; d < 8 ; d++){
        int new_x = target.x + dx[d];
        int new_y = target.y + dy[d];

        if(new_x < 1) new_x = N;    // 격자 벗어나면 반대편으로 이동하기
        else if(new_x > N) new_x = 1;
        if(new_y < 1) new_y = M;
        else if(new_y > M) new_y = 1;
        
        
        if(new_x == start.x && new_y == start.y) continue; // 공격자는 영향 X
        if(new_x == target.x && new_y == target.y) continue;
        if(tmp[new_x][new_y] <= 0) continue;    // 이미 부서진 포탑은 pass 

        tmp[new_x][new_y] -= attack_half;
        if (tmp[new_x][new_y] <= 0) {
            turrets_cnt--;
            if(turrets_cnt < 1) {
                flag = 1;
                return;
            }
        }
    }
}

// 포탑 정비 
void Organize(){
    // 1) tmp 결과 map으로 옮기기
    // + 공격과 무관한 포탑들은 공격력 + 1
    // 2) turrets 벡터 업데이트  + 공격자는 마지막 공격 시점 업데이트
    for(int i = 1; i <= N; i++){
        for(int j = 1 ; j <= M; j++){
            if(map[i][j] <= 0) continue;
            if(map[i][j] == tmp[i][j]){
                if(!flag) map[i][j] = tmp[i][j] + 1;
            }
            else map[i][j] = tmp[i][j];
        }
    }

    for(auto it = turrets.begin(); it!= turrets.end(); ){
        if(map[it->x][it->y] <= 0){ // 부서진 포탑은 제거
            it = turrets.erase(it); 
        }
        else{
            it->power = map[it->x][it->y];
            if(it->x == attacking.x && it->y == attacking.y){   // 공격자는 마지막 공격 시점 업데이트
                it->time = turn;
            }
            it++;
        }
    }
}

int main(){
    //freopen("input.txt", "r", stdin);
    cin >> N >> M >> K;
    for(int i = 1 ; i <= N ; i++){
        for(int j = 1 ; j <= M; j++){
            int t; cin >> t;
            if (t != 0) {
                turrets.push_back({i, j, t, 0});
            }
            map[i][j] = t;
        }
    }
    

    // K번의 턴 반복
    turn = 1;
    while(turn <= K && turrets.size() > 1){
        turrets_cnt = turrets.size();
        // 공격 결과를 저장할 임시 변수 초기화 
        for(int i = 1; i <= N; i++){
            for(int j =1; j <= M; j++){
                tmp[i][j] = map[i][j];
            }
        }

        // 1. 공격자(가장 약한 포탑) & 공격 받을 포탑(가장 강한 포탑) 선정
        sort(turrets.begin(), turrets.end(), cmp);
        attacking = turrets.front();
        attacked = turrets.back();
        tmp[attacking.x][attacking.y] += (N + M); // 공격자 공격력 증가
        tmp[attacked.x][attacked.y] -= tmp[attacking.x][attacking.y];   // 공격 대상은 공격자 만큼의 피해
        if(tmp[attacked.x][attacked.y] <= 0) turrets_cnt--;
        if(turrets_cnt < 1) {
            flag = 1;
            Organize();
            break;
        }

        attack_half = tmp[attacking.x][attacking.y] / 2;
        // 공격자 공격력의 절반이 1 미만이면 아무 영향 X
        if(attack_half >= 1){  
            // 2. 레이저 공격 가능한지
            // 3. 안되면 포탄 공격
            LaiserAttack(attacking, attacked);
            if (final_path.size() == 0) {
                BombAttack(attacking, attacked);
            }
        }

        Organize();
        if(flag) break;
        turn++;
    }
    sort(turrets.begin(), turrets.end(), cmp);
    cout << turrets.back().power ;
    return 0;
}