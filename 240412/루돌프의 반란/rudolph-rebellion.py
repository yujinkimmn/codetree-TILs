N, M, P, C, D = map(int, input().split())
v = [[0]*N for _ in range(N)]

ri, rj = map(lambda x:int(x)-1, input().split())
v[ri][rj]=-1                                # 루돌프표시(-1)

score = [0]*(P+1)
alive = [1]*(P+1)
alive[0] = 0                                # 첫 번째는 없는 산타
wakeup_turn = [1]*(P+1)

santa = [[N]*2 for _ in range(P+1)]         # 빈 자리, 번호 맞추기
for _ in range(1, P + 1):
    n,i,j = map(int, input().split())
    santa[n]=[i-1,j-1]                      # i, j
    v[i-1][j-1] = n

def move_santa(cur,si,sj,di,dj,mul):
    q = [(cur,si,sj,mul)]           # cur번 산타를 si,sj에서 di,dj방향으로 mul칸 이동

    while q:
        cur,ci,cj,mul=q.pop(0)
        # 진행방향 mul칸만큼 이동시켜서 범위내이고 산타있으면 q삽입/범위밖 처리
        ni,nj=ci+di*mul, cj+dj*mul
        if 0<=ni<N and 0<=nj<N:     # 범위내 => 산타 O, X
            if v[ni][nj]==0:        # 빈 칸 => 이동처리
                v[ni][nj]=cur
                santa[cur]=[ni,nj]
                return
            else:                   # 산타 O => 연쇄이동
                q.append((v[ni][nj],ni,nj,1))   # 한칸 이동, v[ni][nj]: 다음 산타번호
                v[ni][nj]=cur
                santa[cur]=[ni,nj]
        else:                       # 범위밖 => 탈락 => 끝
            alive[cur]=0
            return

for turn in range(1, M+1):
    # [0] 모두 탈락 시(alive 모두 0) => break
    if alive.count(1)==0:
        break

    # [1-1] 루돌프 이동: 가장 가까운 산타찾기
    mn = 2*N**2
    for idx in range(1, P+1):
        if alive[idx]==0:   continue    # 타락한 산타 => skip..

        si,sj=santa[idx]
        dist=(ri-si)**2+(rj-sj)**2      # 현재거리
        if mn>dist:
            mn=dist
            mlst=[(si,sj,idx)]          # 최소거리=>새리스트
        elif mn==dist:                  # 같은최소=>추가
            mlst.append((si,sj,idx))
    mlst.sort(reverse=True)             # 행 큰>열 큰
    si,sj,mn_idx = mlst[0]              # 돌격 목표산타!

    # [1-2] 대상 산타 방향으로 루돌프 이동
    rdi = rdj = 0
    if ri>si:   rdi=-1  # 산타가 좌표 작은값 => -1방향 이동
    elif ri<si: rdi=1

    if rj>sj:   rdj=-1
    elif rj<sj: rdj=1

    v[ri][rj]=0             # 루돌프 현재자리 지우기
    ri,rj = ri+rdi, rj+rdj  # 루돌프 이동
    v[ri][rj]=-1            # 이동한 자리에 표시

    # [1-3] 루돌프와 산타가 충돌한 경우 산타 밀리는 처리
    if (ri,rj)==(si,sj):            # 충돌!
        score[mn_idx]+=C            # 산타는 C점 획득
        wakeup_turn[mn_idx]=turn+2  # 깨어날 턴 번호를 저장
        move_santa(mn_idx,si,sj,rdi,rdj,C)  # 산타 C칸이동

    # [2-1] 순서대로 산타이동: 기절하지 않은 경우(산타의 턴 <= turn)
    for idx in range(1, P+1):
        if alive[idx]==0:           continue    # 탈락한 경우 skip
        if wakeup_turn[idx]>turn:   continue    # 깨어날 턴이 아직 안된경우

        si,sj = santa[idx]
        mn_dist = (ri-si)**2 + (rj-sj)**2
        tlst = []
        # 상우하좌 순으로 최소거리 찾기
        for di,dj in ((-1,0),(0,1),(1,0),(0,-1)):
            ni,nj=si+di,sj+dj
            dist = (ri-ni)**2 + (rj-nj)**2
            # 범위내, 산타 없고(<=0),더 짧은 거리인 경우
            if 0<=ni<N and 0<=nj<N and v[ni][nj]<=0 and mn_dist>dist:
                mn_dist = dist
                tlst.append((ni,nj,di,dj))
        if len(tlst)==0:    continue    # 이동할 위치 없음
        ni,nj,di,dj = tlst[-1]          # 마지막에 추가된(더 짧은 거리)

        # [2-2] 루돌프와 충돌시 처리
        if (ri,rj)==(ni,nj):            # 루돌프와 충돌: 반대로 튕겨나감!
            score[idx]+=D
            wakeup_turn[idx]=turn+2
            v[si][sj]=0
            move_santa(idx,ni,nj,-di,-dj,D)
        else:                           # 빈 칸: 좌표갱신, 이동처리
            v[si][sj]=0
            v[ni][nj]=idx
            santa[idx]=[ni,nj]

    # [3] 점수획득: alive 산타는 +1점
    for i in range(1,P+1):
        if alive[i]==1:
            score[i]+=1

print(*score[1:])