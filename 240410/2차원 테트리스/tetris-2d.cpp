#include <iostream>

using namespace std;

int k;
int score;

// 노란색 보드는 board[0], 빨간색 보드는 board[1]
int board[2][6][4];

int shape[4][2][2] = {
    {},

    // type 1
    {{1, 0},
    {0, 0}},

    // type 2
    {{1, 1},
    {0, 0}},

    // type 3
    {{1, 0},
    {1, 0}},
};


// 필요한 부분만 잘라서 보드를 만듦
bool InRange(int x, int y){
    return 0 <= x && x < 6 && 0 <= y && y < 4;
}

// type 블록을 b_num 보드의 (x, y) 칸에 놓을 수 있는지
bool CanPut(int b_num, int type, int x, int y){
    // 타일의 모든 칸을 놓을 수 있는지 확인
    for(int i = 0; i < 2 ; i++){
        for(int j = 0; j < 2; j++){
            if(shape[type][i][j]){
                int nx = x + i;
                int ny = y + j;
                // 하나라도 범위 초과하거나 블록이 이미 있는 경우는 false
                if(!InRange(nx, ny) || board[b_num][nx][ny]) 
                    return false;
            }
        }
    }
    return true;
}

void Put(int b_num, int t, int x, int y){
    // 해당 (x, y)에서 시작해서 t 타입의 블록 놓기 
    for(int i = 0; i < 2; i++){
        for(int j = 0 ; j < 2; j++){
            if(shape[t][i][j]){
                int nx = x + i;
                int ny = y + j;
                board[b_num][nx][ny] = 1;
            }
        }
    }
}

bool CheckIfFull(int b_num, int r){
    // 해당 행이 블록으로 꽉 찼는지 확인
    for(int j = 0; j < 4; j++){
        // 하나라도 빈 칸이 있으면 false
        if(board[b_num][r][j] == 0) return false;   
    }
    return true;
}

void DownRow(int b_num, int row){
    // row를 지우기 위함
    // 0번째 행 ~ row-1 번째 행을 한 줄 씩 밑으로 내림 
    for(int r = row; r > 0; r--){
        for(int j = 0; j < 4; j++){
            board[b_num][r][j] = board[b_num][r-1][j];
            board[b_num][r-1][j] = 0;
        }
        int dum = -1;
    }
}

// 해당 보드에서 짙은 부분에 꽉 찬 행이 있으면 지우기
void ProcessDark(int b_num){
    int row = 5;    // 밑에서부터 탐색
    while (row >= 2){
        if(CheckIfFull(b_num, row)){
            score++;
            DownRow(b_num, row);
        }
        else row--;
    }
}
// 연한 부분에 블록이 있는 행의 개수만큼 행 지우기
void ProcessLight(int b_num){
    int cnt = 0;
    for(int r = 0; r < 2 ; r++){
        for(int c = 0 ; c < 4; c++){
            // 하나라도 블록이 있으면 
            if(board[b_num][r][c] == 1){
                cnt++;
                break;
            }
        }
    }

    while(cnt--){
        // 마지막 행을 지움
        DownRow(b_num, 5);
    }
}

void Drop(int b_num, int y, int type){
    // 해당 열 y를 고정하고
    // 맨 위 행부터 타일이 들어갈 수 있는 행을 찾는다
    for(int r = 0; r < 6; r++){
        if(!CanPut(b_num, type, r + 1, y)){
            // 다음 행에 놓을 수 없다면 해당 행에 블록 놓기
            Put(b_num, type, r, y);
            break;
        }
    }

    // 짙은 부분에 꽉 찬 행이 있으면 처리
    ProcessDark(b_num);
    // 연한 부분 처리
    ProcessLight(b_num);
}

void Print(int b_num){
    for(int i = 0; i < 6; i++){
        for(int j = 0 ; j < 4 ; j++){
            cout << board[b_num][i][j] << " ";
        }
        cout << "\n";
    }
}

// 보드에 남은 블록 세기
int CountBlock(int b_num){
    int cnt = 0 ;
    for(int i = 0 ; i < 6 ; i++){
        for(int j = 0 ; j < 4; j++){
            if(board[b_num][i][j] == 1) cnt ++;
        }
    }
    return cnt;
}


int main(){
    //freopen("input.txt", "r", stdin);
    cin >> k;

    // k번 블록을 추가
    for(int i = 0; i < k; i++){
        int t, x, y;
        cin >> t >> x >> y;

        // 몇 번째 열에 떨어지는지가 중요 
        // (보드 번호, 열, 타일 번호)
        // 노란색 보드 실행 
        Drop(0, y, t);

        // 빨간 보드는 90도 회전해서 실행하기
        if(t == 1) Drop(1, 4 - 1 - x, 1);
        if(t == 2) Drop(1, 4 - 1 - x, 3);
        if(t == 3) Drop(1, 4 - 1 - (x + 1), 2);
    }
    
    cout << score << "\n";
    int ans = 0;
    ans += CountBlock(0);
    ans += CountBlock(1);
    cout << ans << "\n";
}