#include <iostream>
#include <vector>
#include <tuple>
#include <algorithm>

#define MAX_N 21
#define MAX_M 30
using namespace std;

int n, m, k;

// 플레이어 정보들
pair<int, int> pos[MAX_M];  
int power[MAX_M];
int dir[MAX_M];
int gun[MAX_M];
int point[MAX_M];

// 격자에 있는 총의 정보들 
vector<int> grid[MAX_N][MAX_N]; 


int dx[] = {-1, 0, 1, 0};
int dy[] = {0, 1, 0, -1};

bool InRange(int x, int y){
    return x >= 1 && y >= 1 && x <= n && y <= n;
}

// 같은 위치에 존재하는지 확인
int IfExist(pair<int, int> cur_pos){
    for(int i = 0 ; i < m ; i++){
        int x, y;
        tie(x, y) = pos[i];
        if (cur_pos == make_pair(x, y)) {
            return i;
        }
    }
    return -1;  // 아무도 존재하지 않으면 -1을 리턴
}

void HaveGun(int num, int x, int y){
    // 가장 좋은 총으로 갱신
    grid[x][y].push_back(gun[num]);
    sort(grid[x][y].begin(), grid[x][y].end(), greater<int>());
    
    int best = grid[x][y][0];
    grid[x][y].erase(grid[x][y].begin());

    gun[num] = best;
}

// x, y 위치에서 새로 들어온 num과 싸우기
void Fight(int num, int existing_num, int x, int y){
    int winner = -1, loser = -1;
    int gap = (power[num] + gun[num]) - (power[existing_num] + gun[existing_num]);
    // 초기 능력치 + 총 공격력 합이 더 큰 플레이어가 승리
    if(gap > 0){
        winner = num;
        loser = existing_num;
    }
    // 같은 경우 초기 능력치가 높으면 승리
    else if(gap == 0){
        if(power[num] > power[existing_num]) {
            winner = num;
            loser = existing_num;
        }
        else {
            winner = existing_num; 
            loser = num;
        }
    }
    else{
        winner = existing_num;
        loser = num;
    }

    // 이긴 플레이어는 포인트 얻기, 해당칸 머물기
    point[winner] += abs(gap);
    pos[winner] = make_pair(x, y);
    
    // 진 플레이어는 총 갖고있는거 내려놓고
    if(gun[loser]){
        grid[x][y].push_back(gun[loser]);
        gun[loser] = 0;
    }
    // 본인의 방향대로 한 칸 이동
    int nx = x + dx[dir[loser]];
    int ny = y + dy[dir[loser]];

    // 격자 밖이거나 다른 플레이어가 있다면 오른쪽으로 90도 회전해서 갈 수 있는 곳 찾기
    while (!InRange(nx, ny) || IfExist(make_pair(nx, ny)) != -1){
       dir[loser] = (dir[loser] + 1) % 4;
       nx = x + dx[dir[loser]];
       ny = y + dy[dir[loser]];
    }
    // 새로운 칸으로 이동 
    pos[loser] = make_pair(nx, ny);

    // 새로운 칸에 총 있으면 갖기
    HaveGun(loser, pos[loser].first, pos[loser].second);
    
    // 이긴 플레이어는 해당 칸에 있는 총 가지기
    HaveGun(winner, pos[winner].first, pos[winner].second);
}

int main(){
    // 입력
    //freopen("input.txt", "r", stdin);
    cin >> n >> m >> k;
    for(int i = 1 ; i <= n ; i++){
        for(int j = 1; j <= n ; j++){
            int g; cin >> g;
            if(g > 0) grid[i][j].push_back(g);
        }
    }
    for(int i = 0; i < m ; i++){    // 플레이어 정보
        int x, y, d, s;
        cin >> x >> y >> d >> s;
        pos[i] = make_pair(x, y);
        dir[i] = d;
        power[i] = s;
    }

    // k번의 턴 동안 반복
    while(k--){
        // 0 ~ m-1번 플레이어 차례로 실행
        for(int i = 0; i < m ; i++){
            // Step 1-1. 현재 플레이어가 움직일 다음 위치와 방향 구하기
            int nx = pos[i].first + dx[dir[i]];
            int ny = pos[i].second + dy[dir[i]];

            if(!InRange(nx, ny)){
                dir[i] = (dir[i] + 2) % 4;  // 격자 벗어나면 정반대로 방향 전환
                nx = pos[i].first + dx[dir[i]];
                ny = pos[i].second + dy[dir[i]];
            }
            
            // 이동할 칸에 다른 플레이어가 있는지 확인
            int existing_num = IfExist(make_pair(nx, ny));

            // 해당 칸으로 이동
            pos[i] = make_pair(nx, ny);

            if(existing_num != -1){
                // 플레이어가 존재한다면 싸우기
                Fight(i, existing_num, nx, ny);
            }

            // 다른 플레이어 없다면 총 가지기
            else {
                HaveGun(i, pos[i].first, pos[i].second);
            }
        }
        int dum = -1;
    }
    // 결과 출력
    for(int i = 0; i < m ; i++){
        cout << point[i] << " ";
    }
}