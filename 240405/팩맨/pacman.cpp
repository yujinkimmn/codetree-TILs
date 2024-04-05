#include <iostream>
#include <vector>
#include <tuple>
#include <algorithm>

using namespace std;

int m, t, cur_turn;
pair<int, int> packman_pos;

// 몬스터, 복제된 알, 시체를 표시
vector<int> monster[4][4];
vector<int> egg[4][4];
vector<int> dead[4][4];
vector<int> tmp[4][4];

// 몬스터가 이동할 수 있는 방향 8개
int dx[8] = {-1, -1, 0, 1, 1, 1, 0, -1};
int dy[8] = {0, -1, -1, -1, 0, 1, 1, 1};

// 팩맨 이동에 사용할 변수들
int dx2[4] = {-1, 0, 1, 0};     // 상 - 좌 - 하 - 우 우선
int dy2[4] = {0, -1, 0, 1};



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

tuple<int, int, int> GetNextPos(int x, int y, int move_dir){
    // 8 방향 중 이동 가능한 방향 찾기
    for(int offset = 0 ; offset < 8 ; offset++){
        int new_dir = (move_dir + offset) % 8;
        int nx = x + dx[new_dir];
        int ny = y + dy[new_dir];
        // 이동 가능하면 새로운 칸으로 이동
        if(CanMove(nx, ny)){
            return make_tuple(nx, ny, new_dir);
        }
    }

    // 이동 가능한 칸 못찾았으면 원래 자리로
    return make_tuple(x, y, move_dir);
}


// Step 2. 모든 몬스터를 하나씩 이동
void MonsterMove(){

    // 몬스터 이동 결과를 임시 저장할 벡터 초기화
    for(int i = 0 ; i < 4 ; i++){
        for(int j = 0 ; j < 4; j++){
            tmp[i][j].clear();
        }
    }
    
    for(int i = 0 ; i < 4 ; i++){
        for(int j = 0 ; j < 4; j++){       
            // 몬스터들이 각각 자신의 방향대로 이동
            for(int k = 0 ; k < (int) monster[i][j].size(); k++){
                int dir = monster[i][j][k];
                // 이동할 수 있는 방향 받아오기
                tuple<int, int, int> next_pos = GetNextPos(i, j, dir);
                int x, y, next_dir;
                tie(x, y, next_dir) = next_pos;
                tmp[x][y].push_back(next_dir);
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

// 해당 방향으로 이동했을 때 팩맨이 잡을 수 있는 몬스터의 수
int GetKilledNum(int dir1, int dir2, int dir3){
    int dirs[] = {dir1, dir2, dir3};
    int x = packman_pos.first, y = packman_pos.second;
    int killed_num = 0;

    // 방문한 적 있는 칸인지 표시
    vector<pair <int, int> > v_pos;

    for(int i = 0; i < 3; i++){
        int nx = x + dx2[dirs[i]], ny = y + dy2[dirs[i]];

        if(!InRange(nx, ny)) return -1; // 범위 넘어가면 아예 이동 X

        if (find(v_pos.begin(), v_pos.end(), make_pair(nx, ny)) == v_pos.end()){
            killed_num += (int) monster[nx][ny].size();
            v_pos.push_back({nx, ny});
        }
        x = nx; y = ny;
    }
    return killed_num;
}


// 팩맨에 의해 죽은 몬스터 -> 시체로 바꾸기
void DoKill(tuple<int, int, int> route){
    // 팩맨이 이동한 3칸을 하나씩 탐색
    int dir1, dir2, dir3;
    tie(dir1, dir2, dir3) = route;
    int dirs[3] = {dir1, dir2, dir3};


    for(int i = 0; i < 3; i++){
        int d = dirs[i];
        int nx = packman_pos.first + dx2[d];
        int ny = packman_pos.second + dy2[d];

        // 시체 추가

        // 잡아먹힌 몬스터는 (현재 턴 + 2)을 없어지는 시간으로 기록해서 시체로 바꾸기 
        for(int j = 0; j < (int)monster[nx][ny].size(); j++){
            dead[nx][ny].push_back({cur_turn + 2});   // 시체로 기록
        }
        monster[nx][ny].clear();  // 몬스터는 없애기

        packman_pos = make_pair(nx, ny);
    }
}


// Step 3. 팩맨 이동 
void PackmanMove(){
    // 필요한 변수들 초기화
    int max_eaten = -1;
    tuple<int, int, int> best_route;

    // 우선순위 대로 탐색
    for (int i = 0; i < 4; i++){
        for(int j = 0 ; j < 4; j++){
            for(int k = 0 ; k < 4; k++){
                int m_cnt = GetKilledNum(i, j, k);  // ex. 상상상 조합으로 잡을 수 있는 몬스터의 수

                if(m_cnt > max_eaten){
                    max_eaten = m_cnt;
                    best_route = make_tuple(i, j, k);
                }
            }
        }
    }
    DoKill(best_route);    // 팩맨이 이동한 경로에 있던 몬스터 -> 시체로 돌리기
}

// Step 4. 시체 소멸
// 시체들의 기록 시간 == 현재 턴이면 시체 없애기
void DeleteDead(){
    for(int i = 0; i < 4 ; i++){
        for (int j = 0; j < 4; j++){
            if(dead[i][j].empty()) continue;
            for(auto it = dead[i][j].begin(); it != dead[i][j].end();){
                if(*it == cur_turn) {
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
            // 동일한 방향으로 몬스터를 부화시킴
            for(int e = 0; e < (int) egg[i][j].size(); e++){
                monster[i][j].push_back(egg[i][j][e]);
            }
            egg[i][j].clear(); 
        }
    }

}
int main(){
    // freopen("input.txt", "r", stdin);
    cin >> m >> t;
    int x, y; cin >> x >> y;
    // (0, 0)을 시작으로 
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
        // Step 4. 시체 소멸 
        DeleteDead();
        // Step 5. 알 부화
        MakeMonster();
    }

    // 살아 남은 몬스터 수
    int ans = 0;
    for(int i = 0 ; i < 4 ; i++){
        for(int j = 0 ; j < 4; j++){
            ans += (int) monster[i][j].size();
        }
    }
    cout << ans;
}