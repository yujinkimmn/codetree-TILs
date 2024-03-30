#include <iostream>
#include <unordered_set>
#include <queue>

using namespace std;

#define MAX_L 50
#define MAX_N 40

int L, N, Q;

int info[MAX_L][MAX_L];      // 체스판 상태
int tmp[MAX_L][MAX_L];  // 디버깅으로 동작 확인용
int knight_map[MAX_L][MAX_L];   // 기사 영역
int damage[MAX_N];      // 기사들이 받은 대미지 수
int first_hp[MAX_N];            // 기사들의 현재 체력
int hp[MAX_N];            // 기사들의 현재 체력
int is_moved[MAX_N];    // 이동 여부

pair<int, int> knight_pos[MAX_N];  // 기사들의 위치
pair<int, int> tmp_knight_pos[MAX_N]; 

pair<int, int> knight_h_w[MAX_N];   // 기사들의 방패 크기 

int dx[] = {-1, 0, 1, 0};   // 위, 오, 아래, 왼
int dy[] = {0, 1, 0, -1};


// 명령 받은 기사가 이동할 영역이 벽이거나 격자 밖이진 않은지 & 충돌 발생한 기사 확인
bool CheckIfMove(int start_num, int dir){
    // 초기화
    for(int i = 1; i <= N ; i++){
        tmp_knight_pos[i] = knight_pos[i];
        is_moved[i] = false;
        damage[i] = 0;
    }

    // 시작 기사 넣기
    queue<int> crashed_knights;     
    crashed_knights.push(start_num);
    is_moved[start_num] = true;

    // 연쇄 이동 중에 벽에 부딪히지 않는지 확인하기 -> 모든 이동 무산
    while(!crashed_knights.empty()){
        int cur_num = crashed_knights.front();
        crashed_knights.pop();

        tmp_knight_pos[cur_num].first += dx[dir];
        tmp_knight_pos[cur_num].second += dy[dir];

        int new_x = tmp_knight_pos[cur_num].first;
        int new_y = tmp_knight_pos[cur_num].second;
        int h =  knight_h_w[cur_num].first;
        int w =  knight_h_w[cur_num].second;

        // 영역에서 충돌이 발생하는지 확인하기
        for(int i = new_x; i < new_x + h; i++){
            for(int j = new_y; j < new_y + w; j++){
                
                // 벽이 있다면 모든 이동 무산!
                if(i < 1 || j < 1 || i > L || j > L) return false;
                if (info[i][j] == 2)    return false;

                // 함정이면 대미지 기록
                if (info[i][j] == 1) {
                    damage[cur_num] += 1;
                }

                // 자기 자신이 아닌 또 다른 기사와 충돌
                int other_knight_num = knight_map[i][j];
                if (other_knight_num > 0 && other_knight_num != cur_num){
                    if (!is_moved[other_knight_num]){
                        crashed_knights.push(other_knight_num);
                        is_moved[other_knight_num] = true;
                    }
                }
            }
        }
    }
    damage[start_num] = 0;
    return true;
}


void MoveKnights(){
    // 기사 지도 초기화
    memset(knight_map, 0, sizeof(knight_map));

    for(int i = 1; i <= N ; i++){
        // 이동한 기사는 새로운 위치 & 대미지 업데이트
        if(is_moved[i]){
            knight_pos[i] = tmp_knight_pos[i];
            hp[i] -= damage[i];
        }

        // 죽은 기사는 격자에서 없애기
        if (hp[i] <= 0) continue;

        int x = knight_pos[i].first;
        int y = knight_pos[i].second;
        int h = knight_h_w[i].first;
        int w = knight_h_w[i].second;

        for(int r = x ; r < x + h; r++){
            for(int c = y ; c < y + w; c++){
                knight_map[r][c] = i;
            }
        }
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
        knight_pos[i] = {r, c};
        knight_h_w[i] = {h, w};
        hp[i] = k;
        first_hp[i] = k;
        for(int x = r; x < r + h ; x++){
            for(int y = c; y < c + w; y++){
                knight_map[x][y] = i;
            }
        }
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
    return 0;
}