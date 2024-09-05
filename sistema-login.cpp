#include <stdio.h>
#include <string.h>
#include <locale.h>

// Função para verificar login
int verificarLogin(char usuario[], char senha[], int *isAdmin) {
    // Credenciais predefinidas para usuário padrão e admin
    char usuarioPadrao[] = "usuario";
    char senhaPadrao[] = "senha123";
    char usuarioAdmin[] = "admin";
    char senhaAdmin[] = "admin123";

    // Verificando se o login é do usuário padrão
    if (strcmp(usuario, usuarioPadrao) == 0 && strcmp(senha, senhaPadrao) == 0) {
        *isAdmin = 0; // Usuário padrão
        return 1; // Login bem-sucedido
    }
    // Verificando se o login é do admin
    else if (strcmp(usuario, usuarioAdmin) == 0 && strcmp(senha, senhaAdmin) == 0) {
        *isAdmin = 1; // Admin
        return 1; // Login bem-sucedido
    } 
    else {
        return 0; // Login falhou
    }
}

int main() {
    // Define o locale para Português do Brasil com suporte a UTF-8
    setlocale(LC_ALL, "pt_BR.UTF-8");

    char usuario[50];
    char senha[50];
    int tentativas = 3;
    int isAdmin = 0; // Variável para verificar se é admin

    printf("=== Sistema de Login SISTEC ===\n");

    while (tentativas > 0) {
        // Solicita o nome de usuário
        printf("Usuario: ");
        fgets(usuario, sizeof(usuario), stdin); // lê até o final e armazena o valor
        usuario[strcspn(usuario, "\n")] = 0;  // Remove o caractere de nova linha

        // Solicita a senha
        printf("Senha: ");
        fgets(senha, sizeof(senha), stdin);
        senha[strcspn(senha, "\n")] = 0;  // Remove o caractere de nova linha

        // Verifica se o login é bem-sucedido
        if (verificarLogin(usuario, senha, &isAdmin)) {
            printf("Login bem-sucedido!\n");

            // Verifica o tipo de usuário logado
            if (isAdmin) {
                printf("Bem-vindo, Admin!\n");
                // Coloque as funcionalidades do admin aqui
            } else {
                printf("Bem-vindo, Usuario!\n");
                // Coloque as funcionalidades do usuário padrão aqui
            }
            return 0;
        } else {
            tentativas--;
            printf("Usuario ou senha incorretos. Você tem %d tentativa(s) restante(s).\n", tentativas);
        }
    }

    printf("Numero maximo de tentativas excedido. Saindo...\n");
    return 0;
}
