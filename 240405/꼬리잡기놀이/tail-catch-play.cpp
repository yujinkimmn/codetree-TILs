#include <iostream>
#include <vector>
#include <queue>

#define MAX_N 21
#define MAX_M 6
using namespace std;

int n, m, k;

int board[MAX_N][MAX_N];

// 각 팀별 레일 위치
vector<pair<int, int>> v[MAX_M];
// 각 팀별 tail이 레일에서 몇 번째에 있는지
int tail[MAX_M];
bool visited[MAX_N][MAX_N];

// 격자 내 레일에 각 팀 번호 
int board_idx[MAX_N][MAX_N];

int ans;

int dx[] = {0, -1, 0, 1};
int dy[] = {1, 0, -1, 0};

bool InRange(int x, int y){
    return x >= 1 && y >= 1 && x <= n && y <= n;
}

void DFS(int x, int y, int idx){
    visited[x][y] = true;
    board_idx[x][y] = idx;  // 격자에 이동선 표시

    for(int i = 0 ; i < 4; i++){
        int nx = x + dx[i]; 
        int ny = y + dy[i];

        if(!InRange(nx, ny)) continue;  // 격자 벗어나면 패스
        if(board[nx][ny] == 0) continue;    // 빈칸이면 패스
        if (visited[nx][ny]) continue;      // 이미 지나간 경로

        // 현재가 머리 사람인데 다음이 2가 아니면 패스
        if((int) v[idx].size() == 1 && board[nx][ny] != 2) continue;

        v[idx].push_back({nx, ny});
        if (board[nx][ny] == 3) tail[idx] = (int) v[idx].size();
        DFS(nx, ny, idx);
    }
}

void Init(){
    int cnt = 1;
    // 레일을 벡터에 저장
    for(int i = 1; i <= n ; i++){
        for(int j = 1; j <= n ; j++){
            if (board[i][j] == 1) v[cnt++].push_back({i, j});   // 머리 사람을 우서 앞에 넣기
        }
    }

    // dfs를 통해 팀별 레일을 벡터에 순서대로 넣어주기 
    for(int i = 1 ; i <= m ; i++){
        DFS(v[i][0].first, v[i][0].second, i);  // (머리 사람 위치, 팀 번호)
    }
}

// 각 팀을 한 칸씩 머리사람 따라 이동하기
void MoveAll(){
    for(int i = 1; i <= m ; i++){
        // 각 팀에 대해 레일을 한 칸씩 뒤로 이동 
        // 맨 뒤의 레일부터 
        pair<int, int> tmp = v[i].back();
        for(int j = (int) v[i].size() - 1; j >= 1; j--){
            v[i][j] = v[i][j-1];
        }
        v[i][0] = tmp;
    }

    // 벡터 정보에 따라 보드의 표기도 바꿔주기
    for(int i = 1; i <= m ; i++){
        for(int j = 0; j < (int) v[i].size(); j++){
            pair<int, int> x = v[i][j];
            
            // 머리 사람이면 1로 표기
            if (j == 0){
                board[x.first][x.second] = 1;
            }
            else if (j < tail[i] - 1){  // 중간 사람이면
                board[x.first][x.second] = 2;
            }
            else if (j == tail[i]-1){   // 꼬리 사람
                board[x.first][x.second] = 3;
            }
            else{
                board[x.first][x.second] = 4;
            }
        }
    }
}

// (x, y) 지점에서 공을 맞은 사람 팀 내에서 찾기
void GetPoint(int x, int y){
    int idx = board_idx[x][y];
    int cnt = 0;
    for(int i = 0; i < (int) v[idx].size(); i++){
        if(v[idx][i].first == x && v[idx][i].second == y) cnt = i;
    }
    ans += (cnt + 1) * (cnt + 1);   // 해당 순서의 제곱만큼 포인트 얻기
}

// turn 번째 라운드의 공을 던집니다.
// 공을 던졌을 때 이를 받은 팀의 번호를 반환합니다.
int ThrowBall(int turn) {
    int t = (turn - 1) % (4 * n) + 1;

    if(t <= n) {
        // 1 ~ n 라운드의 경우 왼쪽에서 오른쪽 방향으로 공을 던집니다.
        for(int i = 1; i <= n; i++) {
            if(1 <= board[t][i] && board[t][i] <= 3) {
                // 사람이 있는 첫 번째 지점을 찾습니다.
                // 찾게 되면 점수를 체크한 뒤 찾은 사람의 팀 번호를 저장합니다.
                GetPoint(t, i);
                return board_idx[t][i];
            }
        }
    }
    else if(t <= 2 * n) {
        // n+1 ~ 2n 라운드 : 아래 -> 위
        t -= n;
        for(int i = n; i >= 1; i--) {
            if(1 <= board[i][t] && board[i][t] <= 3) {
                // 사람이 있는 첫 번째 지점을 찾습니다.
                // 찾게 되면 점수를 체크한 뒤 찾은 사람의 팀 번호를 저장합니다.
                GetPoint(i, t);
                return board_idx[i][t];
            }
        }
    }
    else if(t <= 3 * n) {
        // 2n+1 ~ 3n 라운드: 오른쪽 -> 왼쪽
        t -= (2 * n);
        for(int i = n; i >= 1; i--) {
            if(1 <= board[n + 1 - t][i] && board[n + 1 - t][i] <= 3) {
                // 사람이 있는 첫 번째 지점을 찾습니다.
                // 찾게 되면 점수를 체크한 뒤 찾은 사람의 팀 번호를 저장합니다.
                GetPoint(n + 1 - t, i);
                return board_idx[n + 1 - t][i];
            }
        }
    }
    else {
        // 3n+1 ~ 4n 라운드: 위 -> 아래 
        t -= (3 * n);
        for(int i = 1; i <= n; i++) {
            if(1 <= board[i][n + 1 - t] && board[i][n + 1 - t] <= 3) {
                // 사람이 있는 첫 번째 지점을 찾습니다.
                // 찾게 되면 점수를 체크한 뒤 찾은 사람의 팀 번호를 저장합니다.
                GetPoint(i, n + 1 - t);
                return board_idx[i][n + 1 - t];
            }
        }
    }
    // 공이 그대로 지나간다면 0을 반환합니다.
    return 0;
}


// 공을 획득한 팀은 이동 방향을 반대로
void Reverse(int got_ball_idx){

    // 아무도 공을 받지 못했으면 넘어가기
    if(got_ball_idx == 0) return;

    int idx = got_ball_idx;
    vector<pair<int, int>> new_v;

    // 순서에 맞게 new_v에 레일 넣어주기 
    // tail ~ head까지
    // tail <-> head 역할을 서로 바꾸기
    // 주의 ! 실제 tail은 tail[idx] - 1 에 들어있다. (벡터 원소 0부터 시작해서)
    for(int i = tail[idx] - 1 ; i >= 0; i--){
        pair<int, int> x = v[idx][i];
        new_v.push_back(x);
    }
    // 나머지 레일들 처리
    for(int i = (int) v[idx].size() - 1 ; i >= tail[idx]; i--){
        pair<int, int> x = v[idx][i];
        new_v.push_back(x);
    }
    v[idx] = new_v;

    // 벡터에 저장한 정보를 바탕으로 보드 표기도 바꿔주기
    for(int j = 0 ; j < (int) v[idx].size(); j++){
        pair<int, int> x = v[idx][j];
        if(j == 0) 
            board[x.first][x.second] = 1;
        else if(j < tail[idx] - 1) 
            board[x.first][x.second] = 2;
        else if (j == tail[idx] -1)
            board[x.first][x.second] = 3;
        else
            board[x.first][x.second] = 4;
    }
}

int main(){
    //freopen("input.txt", "r", stdin);
    cin >> n >> m >> k;
    // n x n 줄 격자 정보 입력
    for(int i = 1 ; i <= n ; i ++){
        for(int j = 1 ; j <= n; j++){
            cin >> board[i][j];
        }
    }
    // 각 팀의 레일 정보, 이동선 등을 저장
    Init();

    int dum = -1;
    // K 번의 라운드 반복
    for(int i = 1 ; i <= k ; i++){
        // 각 팀을 머리사람을 따라 한칸씩 이동 
        MoveAll();

        // i 번째 라운드의 공을 던지기.
        // 공을 받은 사람을 찾아 점수를 추가 
        int got_ball_idx = ThrowBall(i);    // 공을 맞은 팀의 번호를 저장

        // 공을 맞은 팀은 이동 방향을 반대로 바꾼다.
        Reverse(got_ball_idx);
    }   

    cout << ans;
    return 0;
}