#include <stdio.h>
#include <stdlib.h>
#include <string.h>//usei para o comando strcmp e strlen
#include <mysql.h>//precisa disso pra conectar com o banco de dados;
#include <conio.h>
//função para conectar ao banco de dados
void conectar_ao_banco(MYSQL* conn) {//mysql* conn, o parametro con é usado para conectar com o banco de dados;
    // Conectar ao banco de dados;; local host== rodando na minha maquina;; lucas== meu nome de usario no banco;; password== é a senha para eu logar;; sistec== nome do banco de dados
    if (mysql_real_connect(conn, "localhost", "lucas", "password", "sistec", 0, NULL, 0) == NULL) {//my sql_real_connect é a função que tenta se conectar ao banco usando os parametros
        fprintf(stderr, "Erro na conexão: %s\n", mysql_error(conn));//mysql error da imprime uma msg de erro pro usuario 
        mysql_close(conn);//e o mysql_close fecha a conexão
        exit(1);
    }
}

int verificar_usuario(MYSQL* conn, const char* nome_usuario, const char* senha) {
    MYSQL_STMT* stmt;//MYSQL_STMT* stmt: Inicializa uma estrutura para manipular a consulta preparada.
    MYSQL_BIND bind[2], result[1];//bind[2] será utilizado para associar os parâmetros da consulta (nome de usuário e senha).;; result[1] será utilizado para associar o resultado da consulta (campo role).
    char query[] = "SELECT role FROM usuarios WHERE nome_usuario = ? AND senha = ?";//char query[]: A consulta SQL que seleciona o campo role da tabela usuarios, onde nome_usuario e senha são iguais aos valores fornecidos.
    char role[20] = "";  // Buffer para armazenar o valor do papel (role), inicializado para evitar lixo de memória

    stmt = mysql_stmt_init(conn); //mysql_stmt_init: Inicializa a consulta preparada.Se falhar, imprime uma mensagem de erro e retorna - 1.
    if (stmt == NULL) {
        fprintf(stderr, "Erro ao inicializar a consulta: %s\n", mysql_error(conn));
        return -1;
    }

    if (mysql_stmt_prepare(stmt, query, (unsigned long)strlen(query)) != 0) {  // mysql_stmt_prepare: Prepara a consulta SQL armazenada na string query e O tamanho da string é passado com strlen(query).
        fprintf(stderr, "Erro ao preparar a consulta: %s\n", mysql_stmt_error(stmt));
        mysql_stmt_close(stmt);//Se houver erro, o código imprime a mensagem de erro e fecha o comando com mysql_stmt_close.
        return -1;
    }
    //memset: Inicializa o vetor bind com zeros para garantir que nenhum valor residual interfira.
    memset(bind, 0, sizeof(bind));

    // Nome do usuário
    bind[0].buffer_type = MYSQL_TYPE_STRING;
    bind[0].buffer = (char*)nome_usuario;
    bind[0].buffer_length = (unsigned long)strlen(nome_usuario);  // Conversão explícita

    // Senha
    bind[1].buffer_type = MYSQL_TYPE_STRING;
    bind[1].buffer = (char*)senha;
    bind[1].buffer_length = (unsigned long)strlen(senha);  // Conversão explícita

    // Associar parâmetros
    if (mysql_stmt_bind_param(stmt, bind) != 0) {//mysql_stmt_bind_param: Associa os parâmetros da consulta com os valores fornecidos (nome de usuário e senha).
        fprintf(stderr, "Erro ao associar parâmetros: %s\n", mysql_stmt_error(stmt));
        mysql_stmt_close(stmt);//Em caso de erro, imprime a mensagem e fecha o comando.
        return -1;
    }

    // Executar a consulta
    if (mysql_stmt_execute(stmt) != 0) {//mysql_stmt_execute: Executa a consulta preparada. Caso ocorra um erro, ele é tratado com uma mensagem de erro e a função retorna -1.
        fprintf(stderr, "Erro ao executar a consulta: %s\n", mysql_stmt_error(stmt));
        mysql_stmt_close(stmt);
        return -1;
    }

    // Obter o resultado da consulta
    memset(result, 0, sizeof(result));

    // Vincular o resultado da consulta
    result[0].buffer_type = MYSQL_TYPE_STRING;
    result[0].buffer = role;
    result[0].buffer_length = sizeof(role);

    if (mysql_stmt_bind_result(stmt, result) != 0) {
        fprintf(stderr, "Erro ao associar resultados: %s\n", mysql_stmt_error(stmt));
        mysql_stmt_close(stmt);
        return -1;
    }

    // Armazenar o resultado
    if (mysql_stmt_store_result(stmt) != 0) {
        fprintf(stderr, "Erro ao armazenar resultado: %s\n", mysql_stmt_error(stmt));
        mysql_stmt_close(stmt);
        return -1;
    }

    // Verificar se há linhas retornadas
    if (mysql_stmt_fetch(stmt) == MYSQL_NO_DATA) {
        printf("Usuário ou senha incorretos.\n");
        mysql_stmt_close(stmt);
        return 0;
    }

    // Verificar o papel do usuário
    if (strcmp(role, "admin") == 0) {
        printf("Login bem-sucedido! Bem-vindo, Administrador.\n");
    }
    else {
        printf("Login bem-sucedido! Bem-vindo, Usuário.\n");
    }

    mysql_stmt_close(stmt);
    return 1;
}

int main() {
    MYSQL* conn = mysql_init(NULL);
    if (conn == NULL) {
        fprintf(stderr, "Erro ao inicializar MySQL: %s\n", mysql_error(conn));
        exit(1);
    }

    conectar_ao_banco(conn);

    char nome_usuario[50];
    char senha[50];

    printf("Digite o nome de usuário: ");
    if (scanf_s("%49s", nome_usuario, (unsigned)_countof(nome_usuario)) != 1) {  // scanf_s exige o tamanho do buffer
        fprintf(stderr, "Erro ao ler nome de usuário.\n");
        return 1;
    }

    printf("Digite a senha: ");
    if (scanf_s("%49s", senha, (unsigned)_countof(senha)) != 1) {  // scanf_s exige o tamanho do buffer
        fprintf(stderr, "Erro ao ler senha.\n");
        return 1;
    }

    int resultado = verificar_usuario(conn, nome_usuario, senha);
    if (resultado == 1) {
        // Login bem-sucedido; continue para funcionalidades adicionais.

    }

    mysql_close(conn);
    return 0;
}
