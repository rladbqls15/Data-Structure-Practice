#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_CHAR   256
#define MAX_CODE   256
#define QUEUE_SIZE 512

/* =====================================================
   자료구조 정의
===================================================== */

// 트리 노드
typedef struct Node {
    unsigned char ch;
    int freq;
    struct Node *left, *right;
} Node;

// 원형 큐
typedef struct {
    Node *data[QUEUE_SIZE];
    int front, rear, size;
} Queue;

// 스택 (비트 코드 생성용)
typedef struct {
    int data[MAX_CODE];
    int top;
} Stack;

/* =====================================================
   원형 큐 함수
===================================================== */

void queue_init(Queue *q) {
    q->front = 0;
    q->rear  = 0;
    q->size  = 0;
}

int queue_empty(Queue *q) {
    return q->size == 0;
}

void enqueue(Queue *q, Node *node) {
    q->data[q->rear] = node;
    q->rear = (q->rear + 1) % QUEUE_SIZE;
    q->size++;
}

Node *dequeue(Queue *q) {
    Node *node = q->data[q->front];
    q->front = (q->front + 1) % QUEUE_SIZE;
    q->size--;
    return node;
}

Node *queue_front(Queue *q) {
    return q->data[q->front];
}

/* =====================================================
   스택 함수
===================================================== */

void stack_init(Stack *s) {
    s->top = -1;
}

void stack_push(Stack *s, int val) {
    s->data[++(s->top)] = val;
}

int stack_pop(Stack *s) {
    return s->data[(s->top)--];
}

int stack_empty(Stack *s) {
    return s->top == -1;
}

/* =====================================================
   노드 생성
===================================================== */

Node *new_node(unsigned char ch, int freq, Node *left, Node *right) {
    Node *node = (Node *)malloc(sizeof(Node));
    node->ch    = ch;
    node->freq  = freq;
    node->left  = left;
    node->right = right;
    return node;
}

/* =====================================================
   PART 2: 허프만 트리 빌드 구성
   - 3단계: build_huffman_tree() + pick_min()
   - 4단계: generate_code_table()
===================================================== */

// 전역: 부호표 (전체 코드에서 공유)
char code_table[MAX_CHAR][MAX_CODE];

/* ─────────────────────────────────────────────────
   3단계: 허프만 트리 빌드 (build_huffman_tree)

   [동작 원리]
   - Queue A: 초기 리프 노드들 (빈도수 오름차순 정렬)
   - Queue B: 합쳐진 내부 노드들 (자동으로 오름차순 유지)
   - 매 반복마다 두 큐에서 최솟값 2개를 꺼내 합산 후 Queue B에 삽입
   - 큐에 노드가 1개 남으면 그것이 루트 노드

   [pick_min]
   - 두 큐의 front 값을 비교해 더 작은 쪽 dequeue
───────────────────────────────────────────────── */

Node *pick_min(Queue *qA, Queue *qB) {
    if (queue_empty(qA) && queue_empty(qB)) return NULL;
    if (queue_empty(qA)) return dequeue(qB);
    if (queue_empty(qB)) return dequeue(qA);
    if (queue_front(qA)->freq <= queue_front(qB)->freq)
        return dequeue(qA);
    return dequeue(qB);
}

Node *build_huffman_tree(Queue *qA, Queue *qB) {
    while (1) {
        int total = qA->size + qB->size;
        if (total == 1) break;  // 노드가 1개만 남으면 루트

        Node *left   = pick_min(qA, qB);
        Node *right  = pick_min(qA, qB);
        Node *parent = new_node(0, left->freq + right->freq, left, right);
        enqueue(qB, parent);    // 합산 노드는 Queue B로
    }

    // 마지막 남은 노드 = 루트
    if (!queue_empty(qA)) return dequeue(qA);
    return dequeue(qB);
}

/* ─────────────────────────────────────────────────
   4단계: 부호표 생성 (generate_code_table)

   [동작 원리]
   - 트리를 재귀적으로 탐색
   - 왼쪽 자식으로 갈 때 스택에 0 push
   - 오른쪽 자식으로 갈 때 스택에 1 push
   - 리프 노드 도달 시 → 스택에 쌓인 비트열 = 해당 문자의 허프만 코드
   - 돌아올 때 stack_pop 으로 경로 되돌림 (백트래킹)
───────────────────────────────────────────────── */

void generate_code_table(Node *node, Stack *s) {
    if (!node) return;

    // 리프 노드 도달 → 스택 내용을 문자열 코드로 저장
    if (!node->left && !node->right) {
        for (int i = 0; i <= s->top; i++)
            code_table[node->ch][i] = '0' + s->data[i];
        code_table[node->ch][s->top + 1] = '\0';
        return;
    }

    // 왼쪽: 0 push → 재귀 탐색 → pop
    stack_push(s, 0);
    generate_code_table(node->left, s);
    stack_pop(s);

    // 오른쪽: 1 push → 재귀 탐색 → pop
    stack_push(s, 1);
    generate_code_table(node->right, s);
    stack_pop(s);
}
