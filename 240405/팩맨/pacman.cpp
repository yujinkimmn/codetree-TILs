#include <iostream>
#include <tuple>
#include <vector>
#include <algorithm>

#define MAX_T 25
#define MAX_N 4
#define DIR_NUM 8
#define P_DIR_NUM 4
#define MAX_DECAY 2

using namespace std;

int n = 4;
int m, t;

// map의 상태를 뜻합니다.
// t번째 턴에 (x, y) 위치에 방향 move_dir를 바라보고 있는
// 몬스터의 수를 뜻합니다.
int monster[MAX_T + 1][MAX_N][MAX_N][DIR_NUM];

// 시체를 관리하기 위한 배열입니다.
// 좀 더 자세하게는,
// (x, y)위치에서
// 썩기 t초 전인 시체가
// 몇 개 있는지를 의미합니다.
int dead[MAX_N][MAX_N][MAX_DECAY + 1];

// 문제에서 주어지는 방향 순서대로
// dx, dy 값들을 정의합니다. 
// 몬스터를 위한 방향입니다.
int dx[DIR_NUM] = {-1, -1,  0,  1, 1, 1, 0, -1};
int dy[DIR_NUM] = { 0, -1, -1, -1, 0, 1, 1,  1};

// 팩맨을 위한
// dx, dy를 따로 정의합니다.
// 우선순위에 맞춰
// 상좌하우 순으로 적어줍니다.
int p_dx[P_DIR_NUM] = {-1,  0, 1, 0};
int p_dy[P_DIR_NUM] = { 0, -1, 0, 1};

// 팩맨의 위치를 저장합니다.
int px, py;

// 현재 몇 번째 턴인지를 저장합니다.
int t_num = 1;

// 영역 내에 있는지를 확인합니다.
bool InRange(int x, int y) {
    return 0 <= x && x < n && 0 <= y && y < n;
}

// 나아가려고 하는 위치가 영역 내에 있으며 
// 몬스터 시체가 없고, 팩맨도 없다면
// 이동이 가능합니다.
bool CanGo(int x, int y) {
    return InRange(x, y) && dead[x][y][0] == 0 && dead[x][y][1] == 0 
           && make_pair(x, y) != make_pair(px, py);
}

tuple<int, int, int> GetNextPos(int x, int y, int move_dir) {
    // 현재 위치에서부터
    // 반시계방향으로 45'씩 회전해보며
    // 가능한 곳이 보이면 바로 이동합니다.
    for(int c_dir = 0; c_dir < DIR_NUM; c_dir++) {
        int n_dir = (move_dir + c_dir) % DIR_NUM;
        int nx = x + dx[n_dir];
        int ny = y + dy[n_dir];
        if(CanGo(nx, ny)) {
            return make_tuple(nx, ny, n_dir);
        }
    }

    // 이동이 불가능하다면, 움직이지 않고 기존 상태 그대로 반환합니다.
    return make_tuple(x, y, move_dir);
}

void MoveM() {
    // 각 (i, j)칸에 k 방향을 바라보고 있는 몬스터들이
    // 그 다음으로 이동해야할 위치 및 방향을 구해
    // 전부 (칸, 방향) 단위로 이동시켜 줍니다.
    // 일일이 몬스터 마다 위치를 구해 이동시키면 시간초과가 발생합니다.
    for(int i = 0; i < n; i++)
        for(int j = 0; j < n; j++)
            for(int k = 0; k < DIR_NUM; k++) {
                tuple<int, int, int> next_pos = GetNextPos(i, j, k);
                int x, y, next_dir;
                tie(x, y, next_dir) = next_pos;
                monster[t_num][x][y][next_dir] += monster[t_num - 1][i][j][k];
            }
}

int GetKilledNum(int dir1, int dir2, int dir3) {
    int dirs[] = {dir1, dir2, dir3};
    int x = px, y = py;
    int killed_num = 0;

    // 방문한적이 있는지를 기록합니다.
    vector<pair<int, int> > v_pos;

    for(int i = 0; i < 3; i++) {
        int nx = x + p_dx[dirs[i]], ny = y + p_dy[dirs[i]];
        // 움직이는 도중에 격자를 벗어나는 경우라면, 선택되면 안됩니다.
        if(!InRange(nx, ny))
            return -1;
		// 이미 계산한 곳에 대해서는, 중복 계산하지 않습니다.
        if(find(v_pos.begin(), v_pos.end(), make_pair(nx, ny)) == v_pos.end()) {
            for(int j = 0; j < DIR_NUM; j++)
                killed_num += monster[t_num][nx][ny][j];
            
            v_pos.push_back(make_pair(nx, ny));
        }

        x = nx; y = ny;
    }
    return killed_num;
}

void DoKill(tuple<int, int, int> best_route) {
    int dir1, dir2, dir3;
    tie(dir1, dir2, dir3) = best_route;
	
	// 정해진 dir1, dir2, dir3 순서에 맞춰 이동하며
	// 몬스터를 잡습니다.
    int dirs[] = {dir1, dir2, dir3};
    for(int i = 0; i < 3; i++) {
        int nx = px + p_dx[dirs[i]], ny = py + p_dy[dirs[i]];

        for(int j = 0; j < DIR_NUM; j++) {
			// 시체가 추가됩니다.
            dead[nx][ny][MAX_DECAY] += monster[t_num][nx][ny][j];
            monster[t_num][nx][ny][j] = 0;
        }

        px = nx; py = ny;
    }
}

void MoveP() {
    int max_cnt = -1;
    tuple<int, int, int> best_route;
	
	// 우선순위 순서대로 수행합니다.
    for(int i = 0; i < P_DIR_NUM; i++)
        for(int j = 0; j < P_DIR_NUM; j++)
            for(int k = 0; k < P_DIR_NUM; k++) {
                int m_cnt = GetKilledNum(i, j, k);
				// 가장 많은 수의 몬스터를 잡을 수 있는 경우 중
				// 우선순위가 가장 높은 것을 고릅니다.
                if(m_cnt > max_cnt) {
                    max_cnt = m_cnt;
                    best_route = make_tuple(i, j, k);
                }
            }
    
    // 최선의 루트에 따라 
    // 실제 죽이는 것을 진행합니다.
    // cout << " best route : " << get<0>(best_route) << " " << get<1>(best_route) <<  " " << get<2>(best_route) << "\n";
    DoKill(best_route);
}

void DecayM() {
	// decay를 진행합니다. 턴을 하나씩 깎아주면 됩니다.
    for(int i = 0; i < n; i++)
        for(int j = 0; j < n; j++) {
            for(int k = 0; k < MAX_DECAY; k++)
                dead[i][j][k] = dead[i][j][k + 1];
            dead[i][j][MAX_DECAY] = 0;
        }
}

void AddM() {
	// 몬스터가 복제됩니다.
    for(int i = 0; i < n; i++)
        for(int j = 0; j < n; j++)
            for(int k = 0; k < DIR_NUM; k++)
                monster[t_num][i][j][k] += monster[t_num - 1][i][j][k];
}

void Simulate() {
    // 매 초마다 기록하기 때문에 굳이 copy를 진행할 필요는 없습니다.
    
    // 각 칸에 있는 몬스터를 이동시킵니다.
    MoveM();

    // cout << t_num << "번째 몬스터 이동 결과\n";

    // for(int i = 0; i < n ; i++){
    //     for(int j = 0 ; j < n ; j++){
    //         int cnt = 0;    
    //         for(int k = 0; k < DIR_NUM; k++)
    //             cnt += monster[t_num][i][j][k];
    //         cout << cnt << " ";
    //     }
    //     cout << "\n";
    // }
    // cout << "\n";
    // 팩맨을 이동시킵니다.
    MoveP();

    // 시체들이 썩어갑니다.
    DecayM();

    // 몬스터가 복제됩니다.
    AddM();

    // cout << t_num << "번째 턴 결과\n";

    // for(int i = 0; i < n ; i++){
    //     for(int j = 0 ; j < n ; j++){
    //         int cnt = 0;    
    //         for(int k = 0; k < DIR_NUM; k++)
    //             cnt += monster[t_num][i][j][k];
    //         cout << cnt << " ";
    //     }
    //     cout << "\n";
    // }
    // cout << "\n";
}

int CountMonster() {
    int cnt = 0;

    // 마지막 턴을 마친 이후의 몬스터 수를 셉니다.
    for(int i = 0; i < n; i++)
        for(int j = 0; j < n; j++)
            for(int k = 0; k < DIR_NUM; k++)
                cnt += monster[t][i][j][k];

    return cnt;
}

int main() {
    // 입력:
    //freopen("input.txt","r",stdin);
    cin >> m >> t;
    cin >> px >> py;
    px--; py--;
    for(int i = 0; i < m; i++) {
        int mx, my, mdir;
        cin >> mx >> my >> mdir;
        // 첫 번째 턴의 상태를 기록합니다.
        monster[0][mx - 1][my - 1][mdir - 1]++;
    }
    
	// t번 시뮬레이션을 진행합니다.
    while(t_num <= t) {
        Simulate();
        t_num++;
    }

    cout << CountMonster();
    return 0;
}