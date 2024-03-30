#include <iostream>
#include <vector>
#include <cstring>

using namespace std;

int N, M, K, left_people;
int map[11][11];
int exit_x, exit_y;
pair<int, int> pos[11];
int sum[11];
int ifExit[11];
int most_left_x, most_left_y, square_size;

int dx[] = {-1, 1, 0, 0};   // 상 - 하 - 좌 - 우
int dy[] = {0, 0, -1, 1};

void peopleMove(){
    for(int i = 1 ; i<= M ; i++){
        // 이미 탈출한 참가자 제외
        if (ifExit[i]) continue;

        // 참가자 현위치 ~ 출구까지 가까워 질 수 있는 상하좌우 찾기
        // 단, 현위치 보다 가까워야 함 
        int min_dist = abs(pos[i].first - exit_x) + abs(pos[i].second - exit_y);
        int dir = -1;
        for(int d = 0; d < 4 ; d++){
            int new_x = pos[i].first + dx[d];
            int new_y = pos[i].second + dy[d];

            if (new_x < 1 || new_y < 1 || new_x > N || new_y > N) continue; // 격자 범위 체크
            if (map[new_x][new_y] > 0) continue;   // 벽으로 이동 X

            int cur_dist = abs(new_x - exit_x) + abs(new_y - exit_y);
            if (cur_dist < min_dist){
                dir = d;
                min_dist = cur_dist;
            }
        }

        if (dir == -1) continue;    // 이동할 칸이 없으면 이동 X

        // 실제 이동
        pos[i].first += dx[dir];
        pos[i].second += dy[dir];
        sum[i]++;

        // 출구 도착시 탈출
        if (pos[i].first == exit_x && pos[i].second == exit_y) {
            ifExit[i] = true;
            left_people--;
        }
    }
}

void FindSquare(){
    // 가장 작은 정사각형부터 하나씩 만들어보기
    for(int size = 2 ; size <= N; size++){
        // 가장 좌상단 r 좌표가 작은 것 부터 하나씩 만들어보기
        for(int x1 = 1; x1 <= N - size + 1; x1++){
            // 가장 좌상단 c 좌표가 작은 것부터 하나씩 만들어보기
            for(int y1 = 1 ; y1 <= N - size + 1 ; y1++){
                int x2 = x1 + size - 1; // 가장 우측 상단 좌표들
                int y2 = y1 + size - 1;

                // 출구가 해당 정사각형 안에 없다면 패스
                if (!(x1 <= exit_x && exit_x <= x2 && y1 <= exit_y && exit_y <= y2)) continue;

                // 한 명 이상의 참가자가 정사각형 안에 있는지 확인
                bool is_people_in = false;
                for(int l = 1 ; l <= M ; l++){
                    if(ifExit[l]) continue; // 탈출한 사람은 제외하기 
                    if(x1 <= pos[l].first && pos[l].first <= x2 && y1 <= pos[l].second && pos[l].second <= y2){
                        is_people_in = true;
                    }
                }
                
                // 한 명 이상이라도 있으면 정사각형 찾기 완료!
                if (is_people_in){
                    most_left_x = x1;
                    most_left_y = y1;
                    square_size = size;
                    return;
                }

            }
        }
    }
}

void RotateMaze(){
    // 정사각형을 시계방향으로 90도 회전
    // 해당 칸이 벽이면 내구도 -1
    int tmp[11][11];
    memset(tmp, 0, sizeof(tmp));
    for(int x = most_left_x; x < most_left_x + square_size; x++){
        for(int y = most_left_y ; y < most_left_y + square_size; y++){
            int num = map[x][y];
            // 벽일 경우 내구도 -1
            if (num > 0) num--;

            // step 1. (most_left_x, most_left_y)를 (0,0)으로 변환시키는 작업
            int ox = x - most_left_x;
            int oy = y - most_left_y;

            // step 2. 변환된 상태에서는 90도 시계방향 회전 시 (x, y) -> (y, 정사각형길이 - x - 1)
            int rx = oy, ry = square_size - ox - 1;

            // step 3. 다시 (most_left_x, most_left_y)를 더해준다. 
            tmp[rx + most_left_x][ry + most_left_y] = num;

            // 출구는 좌표 업데이트 해주기
            if (num == -1) {
                exit_x = rx + most_left_x;
                exit_y = ry + most_left_y;
            }
        }
    }

    // 원본 지도에 업데이트 정보 옮기기
    for(int i = most_left_x; i < most_left_x + square_size; i++){
        for(int j = most_left_y ; j < most_left_y + square_size; j++){
            map[i][j] = tmp[i][j];   
        }
    }

    // 회전된 사람 위치도 옮겨주기
    for(int i = 1; i <= M; i++){
        int x = pos[i].first;
        int y = pos[i].second;
        if(ifExit[i]) continue;
        if(most_left_x <= x && x < most_left_x + square_size){
            if(most_left_y <= y && y < most_left_y + square_size){
                int ox = x - most_left_x;
                int oy = y - most_left_y;

                int rx = oy, ry = square_size - ox - 1;
                pos[i].first = rx + most_left_x;
                pos[i].second = ry + most_left_y;
            }
        }
    }
}


int main(){
    //freopen("input.txt", "r", stdin);

    cin >> N >> M >> K;
    for(int i = 1; i <= N; i++){
        for(int j = 1; j <= N; j++){
            cin >> map[i][j];
        }
    }
    for(int i = 1; i<= M; i++){
        int x, y; cin >> x >> y;
        pos[i] = {x, y};
    }
    cin >> exit_x >> exit_y;
    map[exit_x][exit_y] = -1;   // 출구 좌표 -1로 표시


    int time = 1;
    left_people = M; 
    while(time <= K && left_people > 0){
        // 사람 이동
        peopleMove();
        if(left_people <= 0) break;
        
        // 정사각형 찾아 회전
        FindSquare();
        RotateMaze();
        time++;
    }

    int ans = 0 ;
    for(int i = 1; i<= M; i++){
        ans += sum[i];
    }
    cout << ans << '\n';
    cout << exit_x << " " << exit_y;

}