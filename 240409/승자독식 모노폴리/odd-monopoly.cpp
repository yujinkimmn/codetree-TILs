#include <iostream>
#include <vector>
#include <cstring>
#include <set>

using namespace std;

int n, m, k;
int ans = -1;
int turn;

// 독점 계약 정보
// (독점한 플레이어 번호, 계약이 끝나는 턴 수)
pair<int, int> contact[20][20];

// 플레이어 위치 -> 플레이어 번호로 표시 
int grid[20][20];
// 플레이어 이동시킬 때 사용할 임시 변수
int tmp[20][20];

// 플레이어 정보
// (플레이어 번호, 바라보고 있는 방향)
vector<pair<int, int>> player;

// 각 플레이어 별 바라보고 있는 방향에 따른 우선순위
int priority[401][5][4];

int dx[5] = {0, -1, 1, 0, 0};   // 상하좌우
int dy[5] = {0, 0, 0, -1, 1};

void Init(){
    for(int i = 0; i < n ; i++){
        for(int j = 0; j < n ; j++){
            // 플레이어가 있는 자리에 계약 표시하기 
            if(grid[i][j]){
                contact[i][j] = make_pair(grid[i][j], k);
            }
        }
    }
}

// num 번째 플레이어가 격자에서 어디 있는지 반환
pair<int, int> FindPlayerPos(int num){
    for(int i = 0 ; i < n; i++){
        for(int j = 0; j < n ; j++){
            if(grid[i][j] == num) return make_pair(i, j);
        }
    }
    return make_pair(-1, -1);
}

// 해당 칸이 격자 벗어나지 않고, 계약이 없는지 확인
bool CanGo(int x, int y){
    return (contact[x][y] == make_pair(0, 0) && 
            0 <= x && x < n && 0 <= y && y < n);
}

void MovePlayer(){
    // tmp 초기화
    memset(tmp, 0, sizeof(tmp));

    // 각 플레이어 이동
    for(auto it = player.begin(); it != player.end(); it++){
        int num = it->first;
        int dir = it->second;
        int flag = 0;   // 계약 없는칸 찾았는지 여부
        pair<int, int> pos = FindPlayerPos(num);

        pair<int, int> mine = make_pair(-1, -1);
        int mine_dir = -1;

        // 바라보고 있는 방향의 우선순위에 따라 이동할 수 있는 칸 찾기
        for(int d = 0; d < 4; d++){
            int new_dir = priority[num][dir][d];
            int nx = pos.first + dx[new_dir];
            int ny = pos.second + dy[new_dir];

            // 본인이 독점 계약 한 곳이면 저장해놓기
            if(contact[nx][ny].first == num){
                mine_dir = new_dir;
                mine = make_pair(nx, ny);
            }
            // 격자 벗어나지 않고, 계약 없는 칸이면 
            else if(CanGo(nx, ny)){
                flag = 1;
                it->second = new_dir;

                // 아무도 없는 칸이면 그냥 이동 
                if (!tmp[nx][ny]) tmp[nx][ny] = num;
                else{
                    // 누가 있다면 작은 번호가 남기
                    if(num < tmp[nx][ny])
                        tmp[nx][ny] = num;
                }
                break;
            }
        }
        // 빈 칸이 없으면 인접한 4방향중 본인이 독점 계약한 곳으로 이동
        if (!flag && mine != make_pair(-1, -1)){
            // 아무도 없는 칸이면 그냥 이동
            if(!tmp[mine.first][mine.second]) 
                tmp[mine.first][mine.second] = num;
            else // 누가 있다면 작은 번호가 남기
                if(num < tmp[mine.first][mine.second])
                    tmp[mine.first][mine.second] = num;
            it->second = mine_dir;
        }
    }

    int dum = -1;

    // 이동 결과를 원본에 업데이트 
    for(int i = 0; i < n; i++){
        for(int j = 0; j < n ; j++){
            grid[i][j] = tmp[i][j];
        }
    }
    // 남은 플레이어들 벡터에 업데이트
    vector<pair<int, int>> result;
    for(auto it = player.begin(); it != player.end(); it++){
        if(FindPlayerPos(it->first) != make_pair(-1, -1)){
            result.push_back(*it);
        }
    }
    player = result;
}

// 남은 플레이어들의 계약 만들기
void MakeContact(){
    for(int i = 0; i < n; i++){
        for(int j = 0; j < n ; j++){
            // 플레이어 없는 칸은 패스
            if(!grid[i][j]) continue;
            contact[i][j] = make_pair(grid[i][j], turn + k);
        }
    }
}


void DeleteCotact(){
    for(int i = 0; i < n; i++){
        for(int j = 0; j < n ; j++){
            // 계약 없는 칸은 패스
            if(contact[i][j] == make_pair(0,0)) continue;
            if(contact[i][j].second == turn){
                contact[i][j] = make_pair(0,0);
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
            cin >> grid[i][j];
        }
    }
    // 플레이어 초기 방향
    for(int num = 1; num <= m; num++){
        int dir; cin >> dir;
        player.push_back(make_pair(num, dir));
    }

    // 각 플레이어 방향에 따른 우선순위
    for(int num = 1; num <= m ; num++){
        for(int dir = 1 ; dir <= 4; dir++){
            for(int i = 0; i < 4; i++){
                cin >> priority[num][dir][i];   // num번째 플레이어가 dir 를 바라보고 있을 때 우선순위 4개
            }
        }
    }

    // 초기 상태 계약
    Init();

    while(turn <= 1000){
        turn++;

        // 플레이어를 우선순위에 따라 이동
        MovePlayer();

        // 남은 플레이어들이 독점 계약 맺기
        MakeContact();

        // 턴수 지난 계약은 없애기
        DeleteCotact();

        // 플레이어 1번만 남았다면 종료
        if(player.size() == 1 && player[0].first == 1) break;
    }
    if (turn < 1000) ans = turn;
    cout << ans << '\n';
    return 0;    
}