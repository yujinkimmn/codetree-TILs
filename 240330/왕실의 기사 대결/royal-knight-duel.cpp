#include <iostream>
#include <queue>
#include<cstring>

using namespace std;

#define MAX_L 50
#define MAX_N 40

int L, N, Q;

int info[MAX_L][MAX_L];      // 체스판 상태
int damage[MAX_N];      // 기사들이 받은 대미지 수
int first_hp[MAX_N];            // 기사들의 현재 체력
int hp[MAX_N];            // 기사들의 현재 체력
int is_moved[MAX_N];    // 이동 여부

pair<int, int> pos[MAX_N];  // 기사들의 위치
pair<int, int> tmp_pos[MAX_N]; 
pair<int, int> knight_h_w[MAX_N];   // 기사들의 방패 크기 

int dx[] = {-1, 0, 1, 0};   // 위, 오, 아래, 왼
int dy[] = {0, 1, 0, -1};


// 명령 받은 기사가 이동할 영역이 벽이거나 격자 밖이진 않은지 & 충돌 발생한 기사 확인
bool CheckIfMove(int start_num, int dir){
    // 초기화
    for(int i = 1; i <= N ; i++){
        is_moved[i] = false;
        damage[i] = 0;
        tmp_pos[i] = pos[i];
    }
    
    queue<int> qu; 

    qu.push(start_num);
    is_moved[start_num] = true;

    // 연쇄 이동 중에 벽에 부딪히지 않는지 확인하기 -> 모든 이동 무산
    while(!qu.empty()){
        int cur_num = qu.front();
        qu.pop();

        tmp_pos[cur_num].first += dx[dir];
        tmp_pos[cur_num].second += dy[dir];

        int new_x = tmp_pos[cur_num].first;
        int new_y = tmp_pos[cur_num].second;
        int h =  knight_h_w[cur_num].first;
        int w =  knight_h_w[cur_num].second;

        // 함정 & 벽 체크
        for(int i = new_x; i < new_x + h; i++){
            for(int j = new_y; j < new_y + w; j++){
                
                // 벽이 있다면 모든 이동 무산!
                if(i < 1 || j < 1 || i > L || j > L) return false;
                if (info[i][j] == 2)    return false;

                // 함정이면 대미지 기록
                if (info[i][j] == 1) {
                    damage[cur_num] += 1;
                }
            }
        }

        // 다른 기사와 충돌하는지 확인
        for(int i = 1 ; i <= N; i++){

            // 이미 충돌한 기사거나, 죽은 기사는 패스 
            if(is_moved[i] || hp[i] <= 0) continue;

            // 겹치지 않는 경우를 따져서 패스 
            // 상하가 겹치지 않는 경우
            // 1. 다른 기사(pos[i].first)의 윗변보다 해당 기사의 아랫변(new_x + h - 1)이 더 위에 있거나
            // 2. 다른 기사의 아랫변보다 해당 기사의 윗변이 더 아래에 있는 경우 
            if (pos[i].first > new_x + h - 1 || pos[i].first + knight_h_w[i].first - 1 < new_x) continue;
            
            // 좌우가 겹치지 않는 경우
            // 1. 다른 기사의 좌측 변보다 해당 기사의 우측변이 더 작은 경우
            // 2. 다른 기사의 우측변보다 해당 기사의 좌측 변이 더 큰 경우 
            if (pos[i].second > new_y + w - 1 || pos[i].second + knight_h_w[i].second - 1 < new_y) continue;

            // 겹치면 큐에 넣기
            is_moved[i] = true;
            qu.push(i);
        }
    }
    damage[start_num] = 0;
    return true;
}


void MoveKnights(){

    for(int i = 1; i <= N ; i++){
        pos[i] = tmp_pos[i];
        hp[i] -= damage[i];
    }
}

int main(){
    //freopen("input.txt", "r", stdin);

    cin >> L >> N >> Q;
    // 체스판 정보
    for(int i = 1; i <= L; i++){
        for(int j = 1; j <= L ; j++){
            cin >> info[i][j];
       }
    }
    // 1번 ~ N명의 기사 정보
    for(int i = 1; i <= N; i++){
        int r, c, h, w, k;
        cin >> r >> c >> h >> w >> k;
        pos[i] = {r, c};
        knight_h_w[i] = {h, w};
        hp[i] = k;
        first_hp[i] = k;
    }

    // Q개의 명령
    for(int q = 1; q <= Q; q++){
        int first_knight_num, dir;
        cin >> first_knight_num >> dir;

        if(hp[first_knight_num] <= 0) continue;     // 죽은기사에게 명령 내리면 nothing 
 
        if(CheckIfMove(first_knight_num, dir)){
            // 이동 가능하면 실제 이동을 수행
            MoveKnights();
        }
    }

    // 결과 출력
    int ans = 0;
    for(int i = 1; i <= N; i++){
        if(hp[i] > 0) ans += (first_hp[i] - hp[i]);
    }

    cout << ans;
}