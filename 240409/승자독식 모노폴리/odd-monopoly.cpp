#include <iostream>
#include <vector>
#include <cstring>
#include <set>

#define EMPTY make_pair(401, 401)

using namespace std;

int n, m, k;
int ans = -1;
int turn;

// 독점 계약 정보
// (독점한 플레이어 번호, 계약이 끝나는 턴 수)
pair<int, int> contract[20][20];

int given_map[20][20];
// 플레이어 정보(번호, 방향)을 포함
pair<int, int> player[20][20];
// 플레이어 이동시킬 때 사용할 임시 변수
pair<int, int> tmp[20][20];

// 각 플레이어 별 바라보고 있는 방향에 따른 우선순위
int priority[401][5][4];

int dx[5] = {0, -1, 1, 0, 0};   // 상하좌우
int dy[5] = {0, 0, 0, -1, 1};

// 해당 칸이 격자 벗어나지 않고, 계약이 없는지 확인
bool CanGo(int x, int y){
    return (0 <= x && x < n && 0 <= y && y < n) && (contract[x][y] == EMPTY);
}

// 종료 조건
bool End(){
    if(turn >= 1000) return true;

    // 플레이어 1만 남으면 종료
    for(int i = 0; i < n ; i++){
        for(int j = 0 ; j < n ; j++){
            if(player[i][j] == EMPTY) continue;
            // 한 명이라도 1번이 아닌 플레이어가 있으면 바로 false
            if(player[i][j].first != 1) return false;
        }
    }
    return true;
}

tuple<int, int, int> NextPos(int x, int y, int curr_dir){
    int num = player[x][y].first;

    // 바라보고 있는 방향의 우선순위에 따라 이동할 수 있는 칸 찾기
    for(int d = 0; d < 4; d++){
        int new_dir = priority[num][curr_dir][d];
        int nx = x + dx[new_dir];
        int ny = y + dy[new_dir];
        // 격자 벗어나지 않고, 계약 없는 칸이면 
        if(CanGo(nx, ny)){
            return make_tuple(nx, ny, new_dir);
        }
    }
    // 없다면 자신이 독점 계약 한 땅으로 이동
    for(int d = 0; d < 4; d++){
        int new_dir = priority[num][curr_dir][d];
        int nx = x + dx[new_dir];
        int ny = y + dy[new_dir];

        if(contract[nx][ny].first == num){
            return make_tuple(nx, ny, new_dir);
        }
    }
}

// (x, y)에 새로운 플레이어(num, dir)가 들어 왔을 때
void Update(int x, int y, pair<int, int> player){
    // 빈 자리는 401이므로 무조건 업데이트 됨
    if(tmp[x][y] > player){
        tmp[x][y] = player;
    }
}

void MovePlayer(){
    // tmp 초기화
    // 초기 값을 401로 해서 항상 update가 되도록함
    for(int i = 0; i < n ; i++){
        for(int j = 0; j < n; j++){
            tmp[i][j] = EMPTY;
        }
    }

    // 각 플레이어 한 칸씩 이동
    for(int i = 0; i < n ; i++){
        for(int j = 0; j < n; j++){
            if(player[i][j] == EMPTY) continue;
            int num = player[i][j].first;
            int dir =  player[i][j].second;
            // 다음 위치와 방향 구하기
            int nx, ny, next_dir;
            tie(nx, ny, next_dir) = NextPos(i, j, dir);
            // 해당 위치로 플레이어 이동
            Update(nx, ny, make_pair(num, next_dir));
        }
    }
    
    // 이동 결과를 원본에 업데이트 
    for(int i = 0; i < n; i++){
        for(int j = 0; j < n ; j++){
            player[i][j] = tmp[i][j];
        }
    }
}

// 남은 플레이어들의 계약 만들기
void MakeContract(){
    for(int i = 0; i < n; i++){
        for(int j = 0; j < n ; j++){
            // 플레이어 없는 칸은 패스
            if(player[i][j] == EMPTY) continue;
            contract[i][j] = make_pair(player[i][j].first, turn + k);
        }
    }
}


void DeContract(){
    for(int i = 0; i < n; i++){
        for(int j = 0; j < n ; j++){
            // 계약 없는 칸은 패스
            if(contract[i][j] == EMPTY) continue;
            // k 턴이 지나게 되면 주인 없는 칸으로 돌아감
            if(contract[i][j].second == turn){
                contract[i][j] = EMPTY;
            }
        }
    }
}


int main(){
    //freopen("input.txt", "r", stdin);
    cin >> n >> m >> k;
    // 격자 정보 입력 
    for(int i = 0 ; i < n; i++){
        for(int j = 0; j < n ; j++){
            cin >> given_map[i][j];
            if (given_map[i][j] == 0){
                player[i][j] = EMPTY;
                contract[i][j] = EMPTY;
            }
        }
    }
    // 플레이어 초기 방향
    for(int num = 1; num <= m; num++){
        int dir; cin >> dir;
        // 플레이어 위치를 찾아서 격자에 넣어주기
        for(int i = 0 ; i < n ; i++){
            for(int j = 0; j < n; j++){
                if(given_map[i][j] == num){
                    player[i][j] = make_pair(num, dir);
                    contract[i][j] = make_pair(num, k);
                }
            }
        }
    }
    // 각 플레이어 방향에 따른 우선순위
    for(int num = 1; num <= m ; num++){
        for(int dir = 1 ; dir <= 4; dir++){
            for(int i = 0; i < 4; i++){
                cin >> priority[num][dir][i];   // num번째 플레이어가 dir 를 바라보고 있을 때 우선순위 4개
            }
        }
    }

    while(!End()){
        turn++;

        // 플레이어를 우선순위에 따라 이동
        MovePlayer();

        // 남은 플레이어들이 독점 계약 맺기
        MakeContract();

        // 턴수 지난 계약은 없애기
        DeContract();
    }

    if (turn < 1000) ans = turn;
    cout << ans << '\n';
    return 0;    
}