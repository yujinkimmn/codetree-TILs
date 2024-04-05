#include <iostream>
#include <vector>
#include <cstring>

using namespace std;

int m, t, cur_turn;
pair<int, int> packman_pos;

// 몬스터, 복제된 알, 시체를 표시
vector<int> monster[4][4];
vector<int> egg[4][4];
vector<int> dead[4][4];

// 몬스터가 이동할 수 있는 방향 8개
int dx[8] = {-1, -1, 0, 1, 1, 1, 0, -1};
int dy[8] = {0, -1, -1, -1, 0, 1, 1, 1};

// DFS에 사용할 변수들
int dx2[4] = {-1, 0, 1, 0};     // 상 - 좌 - 하 - 우 우선
int dy2[4] = {0, -1, 0, 1};
vector<pair<int,int>> path;     // 팩맨이 이동할 경로
int max_eaten;                  // 먹은 몬스터의 수 
int visited[4][4];

bool InRange(int x, int y){
    return 0 <= x && x < 4 && 0 <= y && y < 4;
}

// Step 1. 몬스터 복제
// 현재 자신의 방향과 동일한 알 낳기
void Duplicate(){
    for(int i = 0 ; i < 4 ; i++){
        for(int j = 0 ; j < 4; j++){
            if(monster[i][j].empty()) continue;
            egg[i][j] = monster[i][j];
        }
    }
}

// 몬스터가 이동할 수 있는지를 판단
// 움직이려는 칸이 격자를 벗어나지 않고, 시체가 없고, 팩맨이 없어야지 true를 반환
bool CanMove(int x, int y){
    if (InRange(x, y) && dead[x][y].empty() && make_pair(x, y) != packman_pos) 
        return true;
    else 
        return false;
}

// Step 2. 모든 몬스터를 하나씩 이동
void MonsterMove(){
    // 몬스터 이동 결과를 임시 저장
    vector<int> tmp[4][4];

    // 몬스터들이 각각 자신의 방향대로 이동
    for(int i = 0 ; i < 4 ; i++){
        for(int j = 0 ; j < 4; j++){

            if(monster[i][j].empty()) continue;
            
            for(int k = 0 ; k < monster[i][j].size(); k++){
                int dir = monster[i][j][k];
                int nx = i + dx[dir];
                int ny = j + dy[dir];

                // 이동 가능하면 새로운 칸으로 이동
                if(CanMove(nx, ny)){
                    tmp[nx][ny].push_back(dir);
                    continue;
                }

                // 격자를 벗어났거나, 다음칸에 시체 있거나 , 팩맨이 존재하면
                // 이동 가능한 칸이 나올 때 까지 반시계 방향으로 회전
                int new_dir = dir;
                for(int d = 0 ; d < 8 ; d++){
                    new_dir = (new_dir + 1) % 8;
                    nx = i + dx[new_dir];
                    ny = j + dy[new_dir];
                    if(CanMove(nx, ny)) break;
                }
                // 이동 가능한 칸 못찾고 원래 방향으로 돌아왔으면 원래 자리에 넣기
                if (new_dir == dir){
                    tmp[i][j].push_back(dir);
                }  
                // 이동 가능한 칸 찾았으면 새로운 칸으로 이동 
                else{
                    tmp[nx][ny].push_back(new_dir);
                } 
            }
        }
    }

    // 이동 결과를 원본에 복사
    for(int i = 0; i < 4; i++){
        for(int j = 0; j < 4; j++){
            monster[i][j] = tmp[i][j];
        }
    }
}

// 최대 수의 몬스터를 먹을 수 있는 팩맨 경로 찾기 
void DFS(int cnt, vector<pair<int, int>> p, int eaten){
    if(cnt == 3){   // 종료 조건
        if (eaten > max_eaten){
            path = p;
            max_eaten = eaten;
        }
        return;
    }

    // 4 방향 탐색하며 탐색
    for(int d = 0; d < 4; d++){
        int nx = p.back().first + dx2[d];
        int ny = p.back().second + dy2[d];

        if(!InRange(nx, ny)) continue;
        if(visited[nx][ny]) continue;

        visited[nx][ny] = 1;
        p.push_back({nx, ny});
        if(!monster[nx][ny].empty()) 
            eaten += monster[nx][ny].size();

        DFS(cnt+1, p, eaten);

        // 원상복구
        visited[nx][ny] = 0;
        p.pop_back();
        if(!monster[nx][ny].empty()) eaten -= monster[nx][ny].size();
    }
}

// Step 3. 팩맨 이동 
void PackmanMove(){
    // 필요한 변수들 초기화
    max_eaten = 0;
    path.clear();
    memset(visited, 0, sizeof(visited));
    
    // 몬스터를 가장 많이 먹을 수 있는 방법 dfs로 찾기
    vector<pair<int, int>> start;
    start.push_back(packman_pos);
    visited[packman_pos.first][packman_pos.second] = 1;

    DFS(0, start, 0);
    int dum = -1;
}

// 팩맨에 의해 죽은 몬스터 -> 시체로 바꾸기
void DeleteMonster(){
    // 팩맨이 이동한 3칸을 하나씩 탐색
    // 단, 팩맨이 시작한 칸은 제외
    for(int i = 1; i <= 3; i++){
        int x = path[i].first;
        int y = path[i].second;

        if(monster[x][y].empty()) continue;

        // 잡아먹힌 몬스터는 현재 턴 + 2 시간을 기록해서 시체로 바꾸기 
        for(int j = 0; j < monster[x][y].size(); j++){
            dead[x][y].push_back({cur_turn + 2});   // 시체로 기록
        }
        monster[x][y].clear();  // 몬스터는 없애기
    }
    // 팩맨의 위치를 마지막 이동 위치로 바꿔주기
    packman_pos = path.back();
}

// Step 4. 시체 소멸
// 시체들의 기록 시간 == 현재 턴이면 시체 없애기
void DeleteDead(){
    for(int i = 0; i < 4 ; i++){
        for (int j = 0; j < 4; j++){
            if(dead[i][j].empty()) continue;
            for(auto it = dead[i][j].begin(); it != dead[i][j].end();){
                if(cur_turn == *it) {
                    it = dead[i][j].erase(it);
                }
                else it++;
            }
        }
    }
}

// Step 5. 몬스터 복제 완성
// 알이었던 몬스터들 부화시키기
void MakeMonster(){
    for(int i = 0; i < 4 ; i++){
        for (int j = 0; j < 4; j++){
            if(egg[i][j].empty()) continue;

            // 동일한 방향으로 몬스터를 부화시킴
            for(int e = 0; e < egg[i][j].size(); e++){
                monster[i][j].push_back(egg[i][j][e]);
            }
            egg[i][j].clear();  // 부활 완료
        }
    }

}
int main(){
    //freopen("input.txt", "r", stdin);
    cin >> m >> t;
    int x, y; cin >> x >> y;
    packman_pos = make_pair(x-1, y-1);
    for(int i = 0; i < m ; i++){
        int x, y, d;
        cin >> x >> y >> d;
        monster[x-1][y-1].push_back(d-1);
    }

    while(t--){
        cur_turn++;

        //Step 1. 몬스터 복제
        Duplicate();

        // Step 2. 몬스터 이동
        MonsterMove();

        // Step 3. 팩맨 이동 
        PackmanMove();
        DeleteMonster();    // 팩맨이 이동한 경로에 있던 몬스터 -> 시체로 돌리기

        // Step 4. 시체 소멸 
        DeleteDead();

        // Step 5. 알 부화
        MakeMonster();
    }
    // 살아 남은 몬스터 수
    int ans = 0;
    for(int i = 0 ; i < 4 ; i++){
        for(int j = 0 ; j < 4; j++){
            ans += monster[i][j].size();
        }
    }
    cout << ans;
}