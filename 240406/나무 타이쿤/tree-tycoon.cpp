#include <iostream>
#include <vector>
#include <tuple>
#include <cstring>

using namespace std;

int n, m;
int grid[15][15];

// 특수 영양제를 주입한 자리를 표시 
int injected[15][15];    
// 특수 영양제의 위치
vector<pair<int, int>> nutrition;   

// 영양제가 이동할 방향들 
int dx1[8] = {0, -1, -1, -1, 0, 1, 1, 1};  // 오른쪽부터 시작해서 반시계방향 순서
int dy1[8] = {1, 1, 0, -1, -1, -1, 0, 1};

// 영양제의 대각선 살피기 위함
int dx2[4] = {-1, -1, 1, 1};    // 우측상단부터 반시계방향 순서
int dy2[4] = {1, -1, -1, 1};


bool InRange(int x, int y){
    return 0 <= x && x < n && 0 <= y && y < n;
}

void MoveNutrition(int dir, int amount){
    // 영양제 각각을 명령에 맞게 이동
    for(auto it = nutrition.begin(); it != nutrition.end(); it++){
        int x, y; 
        tie(x, y) = *it;

        int nx = (x + amount * dx1[dir] + n) % n;
        int ny = (y + amount * dy1[dir] + n) % n;
        if(!InRange(nx, ny)) cout << "영양제 범위 벗어남 \n";

        *it = make_pair(nx, ny);
    }
}

void Inject(){
    // 영양제 주입한 리브로수 위치 표시하기 위한 변수 초기화
    memset(injected, 0, sizeof(injected));

    // 영양제가 있는 칸 높이 1씩 증가 
    for(auto it = nutrition.begin(); it != nutrition.end(); it++){
        int x, y; 
        tie(x, y) = *it;

        grid[x][y] += 1;
        injected[x][y] = 1; 
    }

    // 1칸씩 높이 증가한 다음에, 
    for(auto it = nutrition.begin(); it != nutrition.end(); it++){
        int x, y; 
        tie(x, y) = *it;

        // 해당 칸에서 4 방향의 대각선에서 높이 1 이상인 나무 개수 세기
        int cnt = 0;
        for(int d = 0; d < 4; d++){
            int nx = x + dx2[d];
            int ny = y + dy2[d];
            if(!InRange(nx, ny)) continue;
            if(grid[nx][ny] >= 1) cnt++;
        }

        // cnt 만큼 해당 칸의 높이 증가
        grid[x][y] += cnt;
    }
    // 영양제는 삭제하기
    nutrition.clear();
}

void CutTrees(){
    for(int i = 0; i < n ; i++){
        for(int j = 0 ; j < n; j++){
            if(injected[i][j]) continue;
            if(grid[i][j] < 2) continue;
            grid[i][j] -= 2;                        // 나무 높이 2만큼 자르기
            nutrition.push_back(make_pair(i, j));   // 영양제로 전환    
        }
    }
}

int main(){
    //freopen("input.txt", "r", stdin);
    cin >> n >> m;
    for(int i = 0 ; i < n; i++){
        for(int j = 0 ; j < n ; j++){
            cin >> grid[i][j];
        }
    }

    // 초기 특수 영양제는 좌하단 4칸
    for(int i = n-1; i >= n-2; i--){
        for(int j = 0; j <= 1; j++){
            nutrition.push_back(make_pair(i, j));
        }
    }

    // m년 동안 반복 
    for(int i = 1 ; i <= m; i++){
        int d, p; cin >> d >> p;
        
        // Step 1. 규칙에 따라 영양제 위치 이동
        MoveNutrition(d-1, p);

        // Step 2. 영양제 주입
        Inject();

        // Step 3. 영양제 땅 제외하고 높이 2 이상인 나무 잘라서 특수 영양제로 만들기
        CutTrees();
    }
    // 남은 나무의 높이 총 합 
    int ans = 0; 
    for(int i = 0; i < n ; i++){
        for(int j = 0; j < n ; j++){
            ans += grid[i][j];
        }
    }
    cout << ans << '\n';
}