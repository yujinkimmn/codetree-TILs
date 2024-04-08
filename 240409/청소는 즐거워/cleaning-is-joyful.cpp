#include <iostream>

using namespace std;

int n;
int grid[500][500];
int ans;

int mop_dx[4] = {0, 1, 0, -1};   // 좌하우상
int mop_dy[4] = {-1, 0, 1, 0};

// 빗자루 위치를 중앙으로 가정하고,
// 좌, 하, 우, 상에 따른 먼지 이동 비율
int dust_ratio[4][5][5] = {
    {   // 좌
        {0, 0, 2, 0, 0},
        {0, 10, 7, 1, 0},
        {5, 0, 0, 0, 0},
        {0, 10, 7, 1, 0},
        {0, 0, 2, 0, 0}
    },
    {   // 하
        {0, 0, 0, 0, 0},
        {0, 1, 0, 1, 0},
        {2, 7, 0, 7, 2},
        {0, 10, 0, 10, 0},
        {0, 0, 5, 0, 0}
    },
    {   // 우
        {0, 0, 2, 0, 0},
        {0, 1, 7, 10, 0},
        {0, 0, 0, 0, 5},
        {0, 1, 7, 10, 0},
        {0, 0, 2, 0, 0}
    },
    {   // 상
        {0, 0, 5, 0, 0},
        {0, 10, 0, 10, 0},
        {2, 7, 0, 7, 2},
        {0, 1, 0, 1, 0},
        {0, 0, 0, 0, 0}
    }
};

bool InRange(int x, int y){
    return 0 <= x && x < n && 0 <= y && y < n;
}

// 격자 안/밖 이동 여부를 결정해서 실제 이동
// 격자 안으로 이동한다면, 그 양을 반환
void AddDust(int x, int y, int amount){
    // 격자 밖이면 ans에 누적
    if(!InRange(x, y)) {
        ans += amount;
    }
    // 격자 안으로 이동 
    else {
        grid[x][y] += amount; 
    }  
}

void Clean(){
    int x = (n-1) / 2;
    int y = (n-1) / 2;

    int move_num = 1;
    int dir = 0;    // 왼쪽부터 시작
    
    while (true){
        for(int i = 0 ; i < move_num; i++){
            // 빗자루 이동할 칸
            x = x + mop_dx[dir];
            y = y + mop_dy[dir];
            int cur_dust = grid[x][y];
            int sum = 0;

            // 5 x 5 먼지 퍼센트 배열 돌기
            for(int r = 0 ; r < 5 ; r++){
                for(int c = 0; c < 5; c++){
                    int dust = cur_dust * dust_ratio[dir][r][c] / 100;
                    // 현재 (x, y) 빗자루 칸에서 (x-2, y-2)를 해서 
                    // dust_ratio 배열의 (0,0) 자리로 간다고 생각하면 됨!
                    // 각 비율이 곱해질 칸의 위치와 더할 먼지 양을 전달 
                    AddDust(x - 2 + r, y - 2 + c, dust);
                    // a% 계산을 위해
                    sum += dust;    
                }
            }
            // 마지막 a%칸 처리
            AddDust(x + mop_dx[dir], y + mop_dy[dir], cur_dust - sum);

            if(x == 0 && y == 0) return; // 좌측 최상단 도달하면 종료
        }
        dir = (dir + 1) % 4;
        // 이동 방향이 좌 혹은 우가 되면 move_num 증가
        if(dir == 0 || dir == 2) move_num++;
    }
}

int main(){
    //freopen("input.txt", "r", stdin);
    cin >> n;
    for(int i = 0 ; i < n; i++){
        for(int j = 0 ; j < n ; j++){
            cin >> grid[i][j];
        }
    }

    // 중앙에서 시작해서 달팽이 모양으로 돌면서 청소
    Clean();
    cout << ans << '\n';
}