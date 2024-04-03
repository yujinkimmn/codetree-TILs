#include <iostream>
#include <set>

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
// set으로 제일 큰 총이 앞으로 오도록 함
set<int, greater<int>> grid[MAX_N][MAX_N]; 

int dx[] = {-1, 0, 1, 0};
int dy[] = {0, 1, 0, -1};


int existing_num = -1;

bool InRange(int x, int y){
    return x >= 1 && y >= 1 && x <= n && y <= n;
}

bool IfExist(int num, int x, int y){
    for(int i = 0 ; i < m ; i++){
        if(i == num) continue;
        if (pos[i].first == x && pos[i].second == y) {
            existing_num = i;
            return true;
        }
    }
    return false;
}

void HaveGun(int num, int x, int y){
    // 해당 칸에 총이 있으면 
    if(grid[x][y].size() > 0){
        // num 플레이어가 총이 없으면 그냥 제일 센 총 가지기
        int max_gun = *grid[x][y].begin();
        if(!gun[num]){
            gun[num] = max_gun;
            grid[x][y].erase(max_gun);  // 격자에선 해당 총 없애기
        }
        else{
            // 갖고 있는 총 vs 격차에 있는 총 중 센걸로 가지기
            if(max_gun > gun[num]){
                grid[x][y].insert(gun[num]);    // 갖고 있던 총 내려놓기
                grid[x][y].erase(max_gun);      // 격자에서 총 없애기
                gun[num] = max_gun;
            }
        }
    }
}

// x, y 위치에서 새로 들어온 num과 싸우기
void Fight(int num, int x, int y){
    int winner = -1;
    int loser = -1;
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
        grid[x][y].insert(gun[loser]);
        gun[loser] = 0;
    }
    // 본인의 방향대로 한 칸 이동
    int nx = pos[loser].first + dx[dir[loser]];
    int ny = pos[loser].second + dy[dir[loser]];

    // 격자 밖이거나 다른 플레이어가 있다면 오른쪽으로 90도 회전해서 갈 수 있는 곳 찾기
    while (!InRange(nx, ny) || IfExist(loser, nx, ny)){
       dir[loser] = (dir[loser] + 1) % 4;
       nx = pos[loser].first + dx[dir[loser]];
       ny = pos[loser].second + dy[dir[loser]];
    }
    // 새로운 칸으로 이동 
    pos[loser] = make_pair(nx, ny);

    // 새로운 칸에 총 있으면 갖기
    HaveGun(loser, nx, ny);
    
    // 이긴 플레이어는 해당 칸에 있는 총 가지기
    HaveGun(winner, x, y);
}

int main(){
    // 입력
    //freopen("input.txt", "r", stdin);
    cin >> n >> m >> k;
    for(int i = 1 ; i <= n ; i++){
        for(int j = 1; j <= n ; j++){
            int g; cin >> g;
            if(g > 0) grid[i][j].insert(g);
        }
    }
    for(int i = 0; i < m ; i++){    // 플레이어 정보
        int x, y, d, s;
        cin >> x >> y >> d >> s;
        pos[i] = make_pair(x, y);
        dir[i] = d;
        power[i] = s;
    }
    int dum = -1;
    // k번의 턴 동안 반복
    while(k--){
        // 0 ~ n-1번 플레이어 차례로 실행
        for(int i = 0; i < m ; i++){
            int nx = pos[i].first + dx[dir[i]];
            int ny = pos[i].second + dy[dir[i]];

            if(!InRange(nx, ny)){
                dir[i] = (dir[i] + 2) % 4;  // 격자 벗어나면 정반대로 방향 전환
                nx = pos[i].first + dx[dir[i]];
                ny = pos[i].second + dy[dir[i]];
            }
            
            // 해당 칸으로 이동
            pos[i] = make_pair(nx, ny);

            // 이동한 칸에 다른 플레이어가 있는지 확인
            if(IfExist(i, nx, ny)){
                // 존재한다면 싸우기
                Fight(i, nx, ny);
            }

            // 다른 플레이어 없다면 총 가지기
            else{
                HaveGun(i, nx, ny);
            }
        }
        int dum = -1;
    }
    for(int i = 0; i < m ; i++){
        cout << point[i] << " ";
    }
}