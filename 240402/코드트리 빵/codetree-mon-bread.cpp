#include <iostream>
#include <tuple>
#include <queue>
#include <climits>

#define EMPTY make_pair(-1, -1)

using namespace std;


int n, m; 
int map[16][16];    // 0이면 빈칸, 1이면 베이스 캠프, -1이면 아무도 갈 수 없는 곳
pair<int,int> cvs_list[31]; // 편의점 좌표  // (0,0)부터 시작하도록!
pair<int, int> people[31];  // 사람들 좌표

int dx[] = {-1, 0, 0, 1};
int dy[] = {0, -1, 1, 0};   // 상-좌-우-하 우선

// BFS에 사용되는 변수들
int visited[16][16];  // 방문 여부 표시
int step[16][16];   // 최단거리 결과

int curr_t;

bool InRange(int x, int y){
    return 0<= x && x < n && 0 <= y && y < n;
}

bool CanGo(int x, int y){
    return InRange(x, y) &&    // 범위를 벗어나지 않으면서
    !visited[x][y] &&          // 방문한 적이 없으면서
    map[x][y] != -1;           // 이동 가능한 곳
}

void BFS(pair<int, int> start){
    // 초기화
    for(int i = 0 ; i < n; i++){
        for(int j = 0 ; j < n; j++){
            visited[i][j] = false;
            step[i][j] = 0;
        }
    }

    queue<pair<int, int>> q;
    int sx = start.first, sy = start.second;
    q.push(start);
    visited[sx][sy] = true;
    step[sx][sy] = 0;

    while(!q.empty()){
        pair<int, int> curr_pos = q.front();
        q.pop();

        int x= curr_pos.first, y = curr_pos.second;
        for(int i = 0 ; i < 4; i++){
            int nx = x + dx[i], ny = y + dy[i];
            if (CanGo(nx, ny)){
                visited[nx][ny] = true;
                step[nx][ny] = step[x][y] + 1;
                q.push(make_pair(nx, ny));
            }
        }
    }
}

void Simulate(){
    // Step 1. 격자에 있는 사람들에 한하여 편의점 방향을 향해 1칸 움직이기
    for(int i = 0 ; i < m ; i++){
        // 아직 격자 밖에 있는 사람이거나 편의점에 도착한 사람은 제외
        if(people[i] == EMPTY || people[i] == cvs_list[i]) continue;    // (0,0)부터 시작하도록!

        // 편의점 위치를 시작으로 하는 BFS를 구해서 현재 사람에서 움직일 다음위치를 더 쉽게 구할 수 있다. 
        BFS(cvs_list[i]);   // (0,0)부터 시작하도록!

        int px, py;
        tie(px, py) = people[i];
        // 편의점 ~ 사람까지 거꾸로 BFS를 진행했으므로, 사람의 위치에서 상좌우하 중 거리 값이 가장 작은 곳을 고르면 된다!
        int min_dist = INT_MAX;
        int min_x = -1, min_y = -1;
        for(int j = 0 ; j < 4; j++){
            int nx = px + dx[j], ny = py + dy[j];
            if (InRange(nx, ny) && visited[nx][ny] && step[nx][ny] < min_dist){
                min_dist = step[nx][ny];
                min_x = nx; min_y = ny;
            }
        }

        // 사람 해당 칸으로 한 칸 이동
        people[i] = make_pair(min_x, min_y);
    }

    // Step 2. 편의점에 도착한 사람에 한하여 이동 불가능 하다는 표시로 -1로 바꾸기!
    for(int i = 0 ; i < m; i++){
        if(people[i] == cvs_list[i]){   // (0,0)부터 시작하도록!
            int px, py; 
            tie(px, py) = people[i];
            map[px][py] = -1;
        }
    }

    // Step 3. 현재 시간 curr_t에 대해 curr_t<=m 이라면, t번 사람이 베이스 캠프로 이동
    if (curr_t <= m){
        // Step 3-1. m번 편의점에서 가장 가까운 베이스캠프 찾기
        BFS(cvs_list[curr_t - 1]);  // (0,0)부터 시작하도록!

        int min_dist = INT_MAX;
        int min_x = -1, min_y = -1;
        for (int i = 0; i < n ; i++){
            for(int j = 0; j < n ; j++){
                if(map[i][j] == 1 && visited[i][j] && step[i][j] < min_dist){
                    min_dist = step[i][j];
                    min_x = i, min_y = j;
                }
            }
        }

        // 해당 베이스 캠프로 사람을 이동 
        people[curr_t - 1] = make_pair(min_x, min_y);
        map[min_x][min_y] = -1; 
    }
}

// 모든 사람이 편의점에 도착했는지 확인
bool End(){ 
    for(int i = 0; i < m; i++){
        if (people[i] != cvs_list[i]) return false; // (0,0)부터 시작하도록!
    }
    return true;
}

int main(){
    //freopen("input.txt", "r", stdin);
    cin >> n >> m;
    // (0, 0) 부터 시작
    for(int i = 0 ; i < n ; i++){
        for(int j = 0 ; j < n; j++){
            cin >> map[i][j];
        }
    }
    for(int i = 0 ; i < m; i++){   
        int x, y;
        cin >> x >> y;
        cvs_list[i] = make_pair(x - 1, y - 1);  // (0,0)부터 시작하도록!
    }

    // 초기 사람들의 위치는 모두 격자 밖 
    for (int i = 0 ; i < m ; i++){
        people[i] = EMPTY;
    }

    // 편의점 도착하지 못한 사람이 있는 동안 반복
    while(true){
        curr_t++;
        Simulate();
        if (End()) break;
    }
    cout << curr_t << '\n';
    return 0;
}