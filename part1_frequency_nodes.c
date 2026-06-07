/* =====================================================
   PART 1: 빈도수 측정 및 초기 노드 생성
   - 자료구조 정의 (Node, Queue, Stack)
   - 원형 큐 / 스택 함수
   - 노드 생성
   - 1단계: count_frequency()
   - 2단계: init_nodes()
   ===================================================== */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_CHAR 256
#define MAX_CODE 256
#define QUEUE_SIZE 512

/* ───────────────────────────────
   자료구조 정의
─────────────────────────────── */

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

/* ───────────────────────────────
   원형 큐 함수
─────────────────────────────── */

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

/* ───────────────────────────────
   스택 함수
─────────────────────────────── */

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

/* ───────────────────────────────
   노드 생성
─────────────────────────────── */

Node *new_node(unsigned char ch, int freq, Node *left, Node *right) {
    Node *node = (Node *)malloc(sizeof(Node));
    node->ch    = ch;
    node->freq  = freq;
    node->left  = left;
    node->right = right;
    return node;
}

/* ───────────────────────────────
   1단계: 빈도수 구하기 (count_frequency)
   - 파일을 바이너리로 열어 각 바이트의 등장 횟수 계산
   - freq[256] 배열에 문자별 빈도수 저장
─────────────────────────────── */

void count_frequency(const char *filename, int freq[MAX_CHAR]) {
    FILE *fp = fopen(filename, "rb");
    if (!fp) { perror("파일 열기 실패"); exit(1); }

    memset(freq, 0, sizeof(int) * MAX_CHAR);
    int c;
    while ((c = fgetc(fp)) != EOF)
        freq[(unsigned char)c]++;

    fclose(fp);
}

/* ───────────────────────────────
   2단계: 초기 노드 생성 및 Queue A 삽입 (init_nodes)
   - 빈도수 > 0 인 문자만 노드로 생성
   - 버블 정렬로 빈도수 오름차순 정렬
   - 정렬된 노드를 Queue A에 순서대로 삽입
─────────────────────────────── */

void init_nodes(int freq[MAX_CHAR], Queue *qA) {
    // 빈도수 > 0 인 문자들을 임시 배열에 모음
    Node *tmp[MAX_CHAR];
    int cnt = 0;

    for (int i = 0; i < MAX_CHAR; i++) {
        if (freq[i] > 0)
            tmp[cnt++] = new_node((unsigned char)i, freq[i], NULL, NULL);
    }

    // 버블 정렬 (빈도수 오름차순)
    for (int i = 0; i < cnt - 1; i++)
        for (int j = 0; j < cnt - i - 1; j++)
            if (tmp[j]->freq > tmp[j+1]->freq) {
                Node *t = tmp[j]; tmp[j] = tmp[j+1]; tmp[j+1] = t;
            }

    // Queue A에 삽입
    for (int i = 0; i < cnt; i++)
        enqueue(qA, tmp[i]);
}
