#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

typedef struct TreeNode {
    int data;
    struct TreeNode* left;
    struct TreeNode* right;
} TreeNode;

TreeNode* createNode(int data) {
    TreeNode* newNode = (TreeNode*)malloc(sizeof(TreeNode));
    if (newNode == NULL) {
        fprintf(stderr, "Erro ao alocar memória para o nó da árvore\n");
        exit(EXIT_FAILURE);
    }
    newNode->data = data;
    newNode->left = newNode->right = NULL;
    return newNode;
}

TreeNode* insert(TreeNode* root, int data) {
    if (root == NULL)
        return createNode(data);

    if (data < root->data)
        root->left = insert(root->left, data);
    else if (data > root->data)
        root->right = insert(root->right, data);

    return root;
}

//No da pilha
typedef struct StackNode {
    TreeNode* treeNode;
    struct StackNode* next;
} StackNode;

// pilha
typedef struct Stack {
    StackNode* top;
} Stack;

// Função da pilha vazia
Stack* createStack() {
    Stack* stack = (Stack*)malloc(sizeof(Stack));
    if (stack == NULL) {
        fprintf(stderr, "Erro ao alocar memória para a pilha\n");
        exit(EXIT_FAILURE);
    }
    stack->top = NULL;
    return stack;
}

int isEmpty(Stack* stack) {
    return stack->top == NULL;
}

// Empilhar árvore na pilha
void push(Stack* stack, TreeNode* treeNode) {
    StackNode* newNode = (StackNode*)malloc(sizeof(StackNode));
    if (newNode == NULL) {
        fprintf(stderr, "Erro ao alocar memória para o nó da pilha\n");
        exit(EXIT_FAILURE);
    }
    newNode->treeNode = treeNode;
    newNode->next = stack->top;
    stack->top = newNode;
}

// Desempilhar árvore da pilha
TreeNode* pop(Stack* stack) {
    if (isEmpty(stack))
        return NULL;
    StackNode* temp = stack->top;
    TreeNode* treeNode = temp->treeNode;
    stack->top = temp->next;
    free(temp);
    return treeNode;
}

int main(int argc, char *argv[], char *envp[]) {
    int pid;
    pid = fork();

    if (pid < 0) {
        perror("Erro ao criar processo: ");
        exit(-1);
    } else if (pid > 0) {
        // Processo pai
        wait(NULL); // Aguarda o processo filho
        printf("Tchau ! (Processo pai - PID: %d)\n", getpid());
    } else {
        // Processo filho
        printf("Este é o processo filho (PID: %d)\n", getpid());

        Stack* stack = createStack();

        // Empilhe os valores
        push(stack, createNode(5));
        push(stack, createNode(10));
        push(stack, createNode(3));

        TreeNode* treeRoot = NULL;

        // Desempilha e insira os valores na árvore
        while (!isEmpty(stack)) {
            TreeNode* node = pop(stack);
            treeRoot = insert(treeRoot, node->data);
            free(node);
        }

        printf("Valores inseridos na árvore pelo processo filho.\n");
        exit(0);
    }

    return 0;
}