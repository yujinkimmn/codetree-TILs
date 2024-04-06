#include <iostream>
#include <queue>
#include <cstring> 
#include <tuple>

using namespace std;

int n, m;
int grid[20][20];
pair<int, int> dice_pos = make_pair(0, 0);  // 주사위 시작 위치
int dice_dir = 0;   // 초기 방향 = 오른쪽

int u = 1, f = 2, r = 3;    // 주사위 초기 상태(위, 앞, 오른쪽)

int dx[4] = {0, 1, 0, -1}; // 우, 하, 좌, 상 순
int dy[4] = {1, 0, -1, 0};

int visited[20][20];
int score;



bool InRange(int x, int y){
    return 0 <= x && x < n && 0 <= y && y < n;
}

void RollDice(){
    int nx = dice_pos.first + dx[dice_dir];
    int ny = dice_pos.second + dy[dice_dir];

    // 이동하려는 칸이 격자 밖이면 방향을 반대로 이동하기
    if (!InRange(nx, ny)){
        dice_dir = (dice_dir + 2) % 4;
        nx = dice_pos.first + dx[dice_dir];
        ny = dice_pos.second + dy[dice_dir];
    }

    // 해당 칸으로 이동하기
    dice_pos = make_pair(nx, ny);

    // 주사위도 알맞게 회전시키기
    if(dice_dir == 0){  // 오른쪽 회전
       tie(u, f, r) = make_tuple(7-r, f, u);
    }
    else if (dice_dir == 1){    // 아래쪽 회전
        tie(u, f, r) = make_tuple(7-f, u, r);
    }
    else if(dice_dir == 2){ // 왼쪽 회전 
        tie(u, f, r) = make_tuple(r, f, 7-u);
    }
    else if(dice_dir == 3){ // 위쪽 회전
        tie(u, f, r) = make_tuple(f, 7-u, r);
    }
}

// v랑 일치하는 값이 인접한 격자에서 몇 개 있는지 세기
int CountValue(int v){
    queue<pair<int, int>> qu;
    memset(visited, 0, sizeof(visited));

    qu.push(dice_pos);
    visited[dice_pos.first][dice_pos.second] = 1;
    int cnt = 1;

    while(!qu.empty()){
        int x, y;
        tie(x, y) = qu.front(); qu.pop();

        for(int d = 0; d < 4; d++){
            int nx = x + dx[d];
            int ny = y + dy[d];

            // 격자 안에서 인접한 곳에 같은 값이 있다면 cnt 추가
            if(InRange(nx, ny) && grid[nx][ny] == v && !visited[nx][ny]){
                cnt += 1;
                qu.push(make_pair(nx, ny));
                visited[nx][ny] = 1;
            }
        }
    }
    return cnt;
}

// Step 2. 인접한 칸 개수만큼 점수 추가
void AddScore(){
    int cnt = 0;
    int value = grid[dice_pos.first][dice_pos.second];

    cnt = CountValue(value);    // 인접한 칸 개수 세기
    score += value * cnt;       // 칸 개수 * 격자 값을 점수에 더하기
}


// Step 3. 주사위 다음 이동방향 정하기
void FindNextDir(){
    // 주사위 아랫면 vs 격자값 비교
    int dice_value = 7 - u;
    int grid_value = grid[dice_pos.first][dice_pos.second];

    if(dice_value > grid_value){
        dice_dir = (dice_dir + 1) % 4;
    }
    else if(dice_value < grid_value){
        dice_dir = (dice_dir - 1 + 4) % 4;
    }
    // 같으면 현재 방향 유지
}

int main(){
    // 입력
    //freopen("input.txt", "r", stdin);
    cin >> n >> m;
    for(int i = 0 ; i < n; i++){
        for(int j = 0; j < n ; j++){
            cin >> grid[i][j];
        }
    }
    
    // m번의 턴을 반복
    while(m--){
        // Step 1. 주사위 정해진 방향으로 한 칸 굴리기
        RollDice();

        // Step 2. 격자의 값 개수 세서 점수 획득
        AddScore();

        // Step 3. 주사위 다음 이동방향 정하기
        FindNextDir();
    }
    cout << score << "\n";
}