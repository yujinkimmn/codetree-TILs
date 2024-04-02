#include <iostream>
#include <vector>
#include <queue>
#include <cstring>
#include <tuple>

using namespace std;

int n, m; 
int map[16][16];
int cx[31]; int cy[31]; // 편의점 좌표

struct Person{
    int num, x, y;
    bool is_arrived;
};
vector<Person> on_board;

struct Node{
    int x, y;
    string path;
};
int visited[16][16];
int dx[] = {-1, 0, 0, 1};
int dy[] = {0, -1, 1, 0};   // 상-좌-우-하 우선
int tmp_block[16][16];

// t번째 사람이 t번 편의점을 향해 이동
Person GoToStore(Person p){
    memset(visited, 0, sizeof(visited));

    // m번 사람 ~ m번 편의점까지 최단 경로 찾기
    int sx = p.x; 
    int sy = p.y;
    int ex = cx[p.num];
    int ey = cy[p.num]; 
    string final_path = "";

    queue<Node> qu;
    qu.push({sx, sy, ""});
    visited[sx][sy] = 1;

    while(!qu.empty()){
        Node now = qu.front();
        qu.pop();

        // 편의점 도착
        if(now.x == ex && now.y == ey){
            final_path = now.path;
            break;
        }

        for(int d = 0 ; d < 4 ; d++){
            int new_x = now.x + dx[d];
            int new_y = now.y + dy[d];

            // 격자 벗어나면 pass 
            if (new_x < 1 || new_y < 1 || new_x > n || new_y > n) continue;
            // block된 칸이면 pass
            if (map[new_x][new_y] == -1) continue;  
            // visited면 Pass
            if (visited[new_x][new_y] == 1) continue;

            qu.push({new_x, new_y, now.path + to_string(d)});
            visited[new_x][new_y] = 1;
        }
    }

    // 최단 경로의 path[0] 대로 이동
    p.x = sx + dx[final_path[0] - '0'];
    p.y = sy + dy[final_path[0] - '0'];
    
    // 편의점 도착시
    if(p.x == ex && p.y == ey){
        tmp_block[ex][ey] = -1;   // 해당 칸 block
        p.is_arrived = true;
    }
    return p;
}

// t번째 사람이 t번 편의점에서 가장 가까운 베이스 캠프로 이동 
void GoToBaseCamp(int t){
    queue<pair<int, int>> qu;
    memset(visited, 0, sizeof(visited));

    int start_x = cx[t];
    int start_y = cy[t];
    int base_x, base_y;

    visited[start_x][start_y] = 1;
    qu.push({start_x, start_y});

    // t번 편의점에서 가장 가까운 베이스 캠프 찾기
    while(!qu.empty()){
        int x, y;
        tie(x, y) = qu.front();
        qu.pop();

        // 베이스 캠프 도달시 종료
        if(map[x][y] == 1){
            base_x = x; base_y = y;
            break;
        }

        for(int d = 0 ; d < 4 ; d++){
            int new_x = x + dx[d];
            int new_y = y + dy[d];

            // 격자 벗어나면 pass 
            if (new_x < 1 || new_y < 1 || new_x > n || new_y > n) continue;
            // block된 칸이면 pass
            if (map[new_x][new_y] == -1) continue;  
            // visited면 Pass
            if (visited[new_x][new_y] == 1) continue;

            qu.push({new_x, new_y});
            visited[new_x][new_y] = 1;
        }
    }
    int dum = -1;
    // t번째 사람을 해당 베이스 캠프로 이동
    on_board.push_back({t, base_x, base_y, false});
    // 해당 베이스 캠프는 앞으로 지나갈 수 없음
    map[base_x][base_y] = -1;
}


int main(){
    //freopen("input.txt", "r", stdin);
    cin >> n >> m;
    // (1, 1) 부터 시작
    for(int i = 1 ; i <= n ; i++){
        for(int j = 1 ; j <= n; j++){
            cin >> map[i][j];
        }
    }

    for(int i = 1 ; i <= m; i++){   
        cin >> cx[i] >> cy[i];  // 편의점 좌표
    }

    int t = 0;
    // 편의점 도착하지 못한 사람이 있는 동안 반복
    while(t <= m || on_board.size() > 0){
        t++;
        // STEP 1. 격자 위에 있는 사람 -> 편의점 이동
        if(on_board.size() > 0){

            // 다음 턴부터 block 될 정보 저장할 배열 초기화
            for(int i = 1 ; i <= n ; i++){
                for(int j = 1; j <= n ; j++){
                    tmp_block[i][j] = map[i][j];
                }
    }
            // m번째 사람이 m번 편의점을 향해 1칸 이동
            for(auto it = on_board.begin(); it != on_board.end(); ){
                Person result = GoToStore(*it);

                if(result.is_arrived) {
                    it = on_board.erase(it);
                }
                else {
                    it->x = result.x; 
                    it->y = result.y;
                    it++;
                }
            }
            // block 된 칸 정보 옮기기
            for(int i = 1; i <= n ; i++){
                for (int j = 1; j <= n; j++){
                    map[i][j] = tmp_block[i][j];
                }
            }
        }


        // STEP 2. t분에 t번째 사람 베이스캠프로 출발
        if(t <= m){
            GoToBaseCamp(t);
        }
    }
    cout << t << '\n';
}