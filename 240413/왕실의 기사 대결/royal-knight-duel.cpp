#include<iostream>
#include<queue>
#include<tuple>

using namespace std;

int L, N, Q;

// 체스판
int board[41][41];

// 기사들의 체력
int init_hp[31];
int hp[31];
int damages[31]; 

// 기사의 h, w
pair<int, int> width[31];
// 기사의 x, y
pair<int, int> point[31];

// 탈락 여부
bool dead[31];

// 연쇄 이동에 사용
int visited[31];

int dx[4] = {-1, 0, 1, 0};  // 상우하좌
int dy[4] = {0, 1, 0, -1};

bool InRange(int x, int y){
    return 1 <= x && x <= L && 1 <= y && y <= L;
}

// i번 기사를 dir 방향으로 한 칸 이동하는 명령을 수행할 수 있는지 반환
bool CanMove(int num, int dir){
    // 초기화
    for(int i = 1; i <= N; i++){
        visited[i] = 0;
        damages[i] = 0; // 현재 턴에 얻게 될 데미지
    }

    // queue 에 이동할 기사번호들을 넣어서 차례로 처리
    queue<int> qu;
    visited[num] = 1;
    qu.push(num);

    while(!qu.empty()){
        int cur = qu.front(); qu.pop();

        // dir 방향으로 한 칸 이동했을 때 경계가 어떻게 되는지
        int x, y; tie(x, y) = point[cur];
        int h, w; tie(h, w) = width[cur];

        // start_row, end_row, start_col, end_col 계산
        int sr = x + dx[dir], sc = y + dy[dir];
        int er = sr + h - 1, ec = sc + w - 1;

        // 해당 영역이 격자 밖이거나 벽이 있는지 확인
        for(int i = sr; i <= er; i++){
            for(int j = sc; j <= ec; j++){
                if (!InRange(i, j)) return false;
                if (board[i][j] == 2) return false;
                // 명령을 받은 i번째 기사를 제외하고 함정을 만나면 대미지
                if(cur != num && board[i][j] == 1) damages[cur]++;
            }
        }

        // 겹치는 기사 있는지 확인
        for(int k = 1; k <= N; k++){
            if(dead[k] || visited[k]) continue;
            // 현재 기사의 영역 계산
            int k_sr = point[k].first, k_sc = point[k].second;
            int k_er = k_sr + width[k].first - 1, k_ec = k_sc + width[k].second - 1;
            // 겹치지 않는 조건
            if(sr > k_er || er < k_sr || sc > k_ec || ec < k_sc) continue;

            // 겹친다면 해당 기사를 push
            qu.push(k);
            visited[k] = 1;
        }
    }
    return true;
}

void Move(int i, int d){
    // 해당 명령을 수행 가능하면 실제 이동하기
    for(int k = 1; k <= N; k++){
        if(!visited[k]) continue;
        // 이동할 대상이면 Point 업데이트
        int x, y; tie(x, y) = point[k];

        int nx = x + dx[d], ny = y + dy[d];
        point[k] = {nx, ny};
        // 명령을 받은 기사 i를 제외하고 대미지 깎기
        if(k != i) hp[k] -= damages[k];
        if(hp[k] <= 0){
            hp[k] = 0;
            dead[k] = 1;
        }
    }
}

int main(){
    //freopen("input.txt", "r", stdin);
    cin >> L >> N >> Q;
    // LxL 체스판 정보 입력
    for(int i = 1 ; i <= L ; i++){
        for(int j = 1; j <= L ; j++){
            cin >> board[i][j];
        }
    }
    // N명 기사들의 정보
    for(int i = 1; i <= N; i++){
        int r, c, h, w, k; cin >> r >> c >> h >> w >> k;
        init_hp[i] = k;
        hp[i] = k;
        width[i] = make_pair(h, w);
        point[i] = make_pair(r, c);
    }



    // Q번의 명령을 실행
    while(Q--){
        int i, d; cin >> i >> d;
        // 이미 죽은 기사면 아무 일도 X
        if(dead[i]) continue;

        if(!CanMove(i, d)) continue;

        // 이동 가능하면 실제로 이동을 수행, 체력 깎기
        Move(i, d);
    }

    // 살아남은 기사들의 총 대미지 합 구하기
    int sum = 0;
    for(int i = 1; i <= N; i++){
        if (dead[i]) continue;
        sum += (init_hp[i] - hp[i]);
    }
    cout << sum << "\n";
}