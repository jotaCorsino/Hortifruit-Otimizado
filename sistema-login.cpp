#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
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
        fprintf(stderr, "Erro ao associar parametros: %s\n", mysql_stmt_error(stmt));
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
        printf("Usuario ou senha incorretos.\n");
        mysql_stmt_close(stmt);
        return 0;
    }

    // Verificar o papel do usuário
    if (strcmp(role, "admin") == 0) {
        printf("Login bem-sucedido! Bem-vindo, Administrador.\n");
        mysql_stmt_close(stmt);
        return 1; // Administrador
    }
    else {
        printf("Login bem-sucedido! Bem-vindo, Usuario.\n");
        mysql_stmt_close(stmt);
        return 2;//usuario comum
    }
}
    int cadastrar_usuario(MYSQL * conn, const char* nome_usuario, const char* senha, const char* role) {
        MYSQL_STMT* stmt;
        MYSQL_BIND bind[3];
        char query[] = "INSERT INTO usuarios (nome_usuario, senha, role) VALUES (?, ?, ?)";

        stmt = mysql_stmt_init(conn); // Inicializar o statement
        if (stmt == NULL) {
            fprintf(stderr, "Erro ao inicializar a consulta: %s\n", mysql_error(conn));
            return -1;
        }

        if (mysql_stmt_prepare(stmt, query, strlen(query)) != 0) {
            fprintf(stderr, "Erro ao preparar a consulta: %s\n", mysql_stmt_error(stmt));
            mysql_stmt_close(stmt);
            return -1;
        }

        // Inicializar o array de bind
        memset(bind, 0, sizeof(bind));

        // Nome do usuário
        bind[0].buffer_type = MYSQL_TYPE_STRING;
        bind[0].buffer = (char*)nome_usuario;
        bind[0].buffer_length = strlen(nome_usuario);

        // Senha
        bind[1].buffer_type = MYSQL_TYPE_STRING;
        bind[1].buffer = (char*)senha;
        bind[1].buffer_length = strlen(senha);

        // Role (admin ou usuario)
        bind[2].buffer_type = MYSQL_TYPE_STRING;
        bind[2].buffer = (char*)role;
        bind[2].buffer_length = strlen(role);

        // Associar os parâmetros
        if (mysql_stmt_bind_param(stmt, bind) != 0) {
            fprintf(stderr, "Erro ao associar parametros: %s\n", mysql_stmt_error(stmt));
            mysql_stmt_close(stmt);
            return -1;
        }

        // Executar a consulta
        if (mysql_stmt_execute(stmt) != 0) {
            fprintf(stderr, "Erro ao executar a consulta: %s\n", mysql_stmt_error(stmt));
            mysql_stmt_close(stmt);
            return -1;
        }

        printf("Usuário cadastrado com sucesso!\n");
        mysql_stmt_close(stmt);
        return 1; // Sucesso
    }

    
// Função para cadastrar produto
void cadastrar_produto(MYSQL* conn) {
    char nome[100] = "", descricao[255] = "";
    int quantidade = 0;
    double preco = 0.0;

    printf("Digite o nome do produto: ");
    if (scanf_s("%99s", nome, (unsigned)sizeof(nome)) != 1) {
        printf("Erro ao ler o nome do produto.\n");
        return;
    }

    printf("Digite a descricao do produto: ");
    if (scanf_s(" %[^\n]s", descricao, (unsigned)sizeof(descricao)) != 1) {
        printf("Erro ao ler a descricao do produto.\n");
        return;
    }

    printf("Digite a quantidade: ");
    if (scanf_s("%d", &quantidade) != 1) {
        printf("Erro ao ler a quantidade.\n");
        return;
    }

    printf("Digite o preco: ");
    if (scanf_s("%lf", &preco) != 1) {
        printf("Erro ao ler o preço.\n");
        return;
    }

    char query[512];
    if (sprintf_s(query, sizeof(query),
        "INSERT INTO controle_estoque (nome_produto, descricao, quantidade, preco) VALUES ('%s', '%s', %d, %.2f)",
        nome, descricao, quantidade, preco) < 0) {
        fprintf(stderr, "Erro ao formatar a consulta SQL.\n");
        return;
    }

    if (mysql_query(conn, query)) {
        fprintf(stderr, "Erro ao cadastrar produto: %s\n", mysql_error(conn));
    }
    else {
        printf("Produto cadastrado com sucesso!\n");
    }
}
void editar_produto(MYSQL* conn) {
    int id_produto;
    char novo_nome[100] = "", nova_descricao[255] = "";
    int nova_quantidade = 0;
    double novo_preco = 0.0;

    printf("Digite o ID do produto a ser editado: ");
    if (scanf_s("%d", &id_produto) != 1) {
        printf("Erro ao ler o ID do produto.\n");
        return;
    }

    // Verifica se o produto existe
    char verifica_query[100];
    if (sprintf_s(verifica_query, sizeof(verifica_query), "SELECT COUNT(*) FROM controle_estoque WHERE id_produto = %d", id_produto) < 0) {
        fprintf(stderr, "Erro ao formatar a consulta SQL.\n");
        return;
    }

    if (mysql_query(conn, verifica_query)) {
        fprintf(stderr, "Erro ao verificar o produto: %s\n", mysql_error(conn));
        return;
    }

    MYSQL_RES* verifica_result = mysql_store_result(conn);
    if (verifica_result == NULL) {
        fprintf(stderr, "Erro ao armazenar resultado: %s\n", mysql_error(conn));
        return;
    }

    MYSQL_ROW verifica_linha = mysql_fetch_row(verifica_result);
    if (atoi(verifica_linha[0]) == 0) {
        printf("Produto com ID %d nao encontrado.\n", id_produto);
        mysql_free_result(verifica_result);
        return;
    }

    mysql_free_result(verifica_result);

    // Solicitar novos dados do produto
    printf("Digite o novo nome do produto (deixe vazio para nao alterar): ");
    scanf_s(" %[^\n]s", novo_nome, (unsigned)sizeof(novo_nome));//[^\n] é um conjunto de caracteres que diz "leia tudo que não é um \n". Isso permite a leitura de entradas que incluem espaços.
    printf("Digite a nova descricao do produto (deixe vazio para nao alterar): ");
    scanf_s(" %[^\n]s", nova_descricao, (unsigned)sizeof(nova_descricao));
    printf("Digite a nova quantidade (ou 0 para não alterar): ");
    if (scanf_s("%d", &nova_quantidade) != 1) {
        printf("Erro ao ler a nova quantidade.\n");
        return;
    }

    printf("Digite o novo preco (ou 0 para nao alterar): ");
    if (scanf_s("%lf", &novo_preco) != 1) {
        printf("Erro ao ler o novo preço.\n");
        return;
    }

    // Formatar a query de atualização
    char update_query[512];
    sprintf_s(update_query, sizeof(update_query),
        "UPDATE controle_estoque SET "
        "nome_produto = IF('%s' != '', '%s', nome_produto), "
        "descricao = IF('%s' != '', '%s', descricao), "
        "quantidade = IF(%d != 0, %d, quantidade), "
        "preco = IF(%.2f != 0, %.2f, preco) "
        "WHERE id_produto = %d",
        novo_nome, novo_nome, nova_descricao, nova_descricao, nova_quantidade, nova_quantidade, novo_preco, novo_preco, id_produto);

    // Executar a query de atualização
    if (mysql_query(conn, update_query)) {
        fprintf(stderr, "Erro ao atualizar produto: %s\n", mysql_error(conn));
    }
    else {
        printf("Produto atualizado com sucesso!\n");
    }
}

// Função para mostrar o estoque
void mostrar_estoque(MYSQL* conn) {
    if (mysql_query(conn, "SELECT * FROM controle_estoque")) {
        fprintf(stderr, "Erro ao buscar o estoque: %s\n", mysql_error(conn));
        return;
    }

    MYSQL_RES* resultado = mysql_store_result(conn);
    if (resultado == NULL) {
        fprintf(stderr, "Erro ao armazenar resultado: %s\n", mysql_error(conn));
        return;
    }

    MYSQL_ROW linha;
    MYSQL_FIELD* campos = mysql_fetch_fields(resultado);
    unsigned int num_campos = mysql_num_fields(resultado);

    // Imprimir cabeçalho
    for (unsigned int i = 0; i < num_campos; i++) {
        printf("%-20s", campos[i].name);
    }
    printf("\n");

    // Imprimir linhas do resultado
    while ((linha = mysql_fetch_row(resultado))) {
        for (unsigned int i = 0; i < num_campos; i++) {
            printf("%-20s", linha[i] ? linha[i] : "NULL");
        }
        printf("\n");
    }

    mysql_free_result(resultado);
}
// Função para inserir uma transação no banco de dados
void transacao(MYSQL* conn) {
    char descricao[255];
    char tipo[10];
    double valor;
    int ano, mes, dia;

    // Solicita dados ao usuário
    printf("Digite a data da transação (dia mes ano): ");
    if (scanf_s("%d %d %d", &dia, &mes, &ano) != 3) {
        printf("Erro ao ler a data da transação.\n");
        return;
    }

    printf("Digite a descrição da transação: ");
    scanf_s(" %[^\n]s", descricao, (unsigned)sizeof(descricao));

    printf("Digite o valor da transação: ");
    if (scanf_s("%lf", &valor) != 1) {
        printf("Erro ao ler o valor da transação.\n");
        return;
    }

    printf("Digite o tipo da transação (Venda ou Despesa): ");
    scanf_s(" %9s", tipo, (unsigned)sizeof(tipo));

    // Verifica se o tipo é válido
    if (strcmp(tipo, "Venda") == 0 || strcmp(tipo, "venda") == 0 || strcmp(tipo, "Despesa") == 0 || strcmp(tipo, "despesa") == 0) {
        // Se o tipo for válido, prossegue
    }
    else {
        printf("Tipo de transação inválido. Deve ser 'Venda' ou 'Despesa'.\n");
        return;
    }

    // Cria a query SQL
    char query[512];
    snprintf(query, sizeof(query),
        "INSERT INTO transacoes (data, descricao, valor, tipo) VALUES ('%02d-%02d-%d', '%s', %.2f, '%s')",
        ano, mes, dia, descricao, valor, tipo);

    // Executa a query
    if (mysql_query(conn, query)) {
        fprintf(stderr, "Erro ao inserir a transação: %s\n", mysql_error(conn));
    }
    else {
        printf("Transação inserida com sucesso!\n");
    }
}
//busca usuario, senha e cargo no banco de dados
int buscar_usuarios(MYSQL* conn) {
    if (mysql_query(conn, "SELECT * FROM usuarios")) {
        fprintf(stderr, "Erro ao buscar o usuario: %s\n", mysql_error(conn));
        return -1;
    }
    MYSQL_RES* resultado = mysql_store_result(conn);
    if (resultado == NULL) {
        fprintf(stderr, "Erro ao armazenar resultado: %s\n", mysql_error(conn));
        return -1;
    }

    MYSQL_ROW linha;
    MYSQL_FIELD* campos = mysql_fetch_fields(resultado);
    unsigned int num_campos = mysql_num_fields(resultado);

    // Imprimir cabeçalho
    for (unsigned int i = 0; i < num_campos; i++) {
        printf("%-20s", campos[i].name);
    }
    printf("\n");

    // Imprimir linhas do resultado
    while ((linha = mysql_fetch_row(resultado))) {
        for (unsigned int i = 0; i < num_campos; i++) {
            printf("%-20s", linha[i] ? linha[i] : "NULL");
        }
        printf("\n");
    }

    mysql_free_result(resultado);
}
void editar_usuario(MYSQL* conn) {
    char nome_usuario[50] = "";
    char nova_senha[50] = "", novo_role[20] = "";

    // Solicitar o nome do usuário a ser editado
    printf("Digite o nome de usuário a ser editado: ");
    scanf_s(" %[^\n]s", nome_usuario, (unsigned)sizeof(nome_usuario));

    // Verifica se o usuário existe
    char verifica_query[200];
    if (sprintf_s(verifica_query, sizeof(verifica_query), "SELECT COUNT(*) FROM usuarios WHERE nome_usuario = '%s'", nome_usuario) < 0) {
        fprintf(stderr, "Erro ao formatar a consulta SQL.\n");
        return;
    }

    if (mysql_query(conn, verifica_query)) {
        fprintf(stderr, "Erro ao verificar o usuário: %s\n", mysql_error(conn));
        return;
    }

    MYSQL_RES* verifica_result = mysql_store_result(conn);
    if (verifica_result == NULL) {
        fprintf(stderr, "Erro ao armazenar resultado: %s\n", mysql_error(conn));
        return;
    }

    MYSQL_ROW verifica_linha = mysql_fetch_row(verifica_result);
    if (atoi(verifica_linha[0]) == 0) {
        printf("Usuário '%s' não encontrado.\n", nome_usuario);
        mysql_free_result(verifica_result);
        return;
    }

    mysql_free_result(verifica_result);

    // Solicitar novos dados do usuário
    printf("Digite a nova senha (deixe vazio para não alterar): ");
    scanf_s(" %[^\n]s", nova_senha, (unsigned)sizeof(nova_senha));

    printf("Digite o novo role (admin ou usuario, deixe vazio para não alterar): ");
    scanf_s(" %[^\n]s", novo_role, (unsigned)sizeof(novo_role));

    // Formatar a query de atualização
    char update_query[512];
    sprintf_s(update_query, sizeof(update_query),
        "UPDATE usuarios SET "
        "senha = IF('%s' != '', '%s', senha), "
        "role = IF('%s' != '', '%s', role) "
        "WHERE nome_usuario = '%s'",
        nova_senha, nova_senha, novo_role, novo_role, nome_usuario);

    // Executar a query de atualização
    if (mysql_query(conn, update_query)) {
        fprintf(stderr, "Erro ao atualizar usuário: %s\n", mysql_error(conn));
    }
    else {
        printf("Usuário atualizado com sucesso!\n");
    }
}
int main() {
    setlocale(LC_ALL, "Portuguese");
    MYSQL* conn = mysql_init(NULL);
    if (conn == NULL) {
        fprintf(stderr, "Erro ao inicializar MySQL: %s\n", mysql_error(conn));
        exit(1);
    }

    conectar_ao_banco(conn);

    char nome_usuario[50];
    char senha[50];

    printf("Digite o nome de usuário: ");
    if (scanf_s("%49s", nome_usuario) != 1) {  
        fprintf(stderr, "Erro ao ler nome de usuário.\n");
        return 1;
    }

    printf("Digite a senha: ");
    if (scanf_s("%49s", senha) != 1) {  
        fprintf(stderr, "Erro ao ler senha.\n");
        return 1;
    }

    int status_login = verificar_usuario(conn, nome_usuario, senha);
    if (status_login == 1 || status_login == 2) {
        // Login bem-sucedido; continue para funcionalidades adicionais.
        int opcao, tarefa;
        do {
            system("cls");
            printf("\n1. Financeiro\n");
            printf("2. Estoque\n");
            printf("3. Transacoes\n");
            printf("4. Sair\n");
            printf("Escolha uma opcao: ");
            if (scanf_s("%d", &opcao) != 1) {
                printf("Erro ao ler a opcao.\n");
                opcao = 4; // Sair em caso de erro de entrada
            }

            switch (opcao) {
            case 1:
                if (status_login == 1) {
                    system("cls");
                    printf("********FINANCEIRO***********");
                    printf("\n1. Relatorio do estoque\n");
                    printf("2. Criar transacao\n");
                    printf("3. Criar novo usuario\n");
                    printf("4. verificar usuarios\n");
                    printf("5. editar usuarios\n");
                    printf("6. Sair\n");
                    if (scanf_s("%d", &tarefa) != 1) {
                        printf("Erro ao ler a opcao.\n");
                    }
                    switch (tarefa) {
                    case 1:
                        mostrar_estoque(conn);
                        break;
                    case 2:
                        transacao(conn);
                        break;
                    case 3:
                        // Variáveis para armazenar as entradas do usuário
                        char nome_novo_usuario[50];
                        char senha_novo_usuario[50];
                        char role[10];

                        // Pedir ao usuário que insira os dados do novo usuário
                        printf("Digite o nome do novo usuario: ");
                        scanf_s("%49s", nome_novo_usuario, (unsigned)_countof(nome_novo_usuario));  // Ler nome do usuário

                        printf("Digite a senha do novo usuario: ");
                        scanf_s("%49s", senha_novo_usuario, (unsigned)_countof(senha_novo_usuario));  // Ler a senha

                        // verifica valor de role é "admin" ou "usuario"
                        do {
                            printf("Digite o cargo (admin ou usuario): ");
                            scanf_s("%9s", role, (unsigned)_countof(role));  // Ler o role

                            if (strcmp(role, "admin") != 0 && strcmp(role, "usuario") != 0) {
                                printf("cargo inválido. Digite 'admin' ou 'usuario'.\n");
                            }
                        } while (strcmp(role, "admin") != 0 && strcmp(role, "usuario") != 0);

                        // Cadastrar o usuário
                        if (cadastrar_usuario(conn, nome_novo_usuario, senha_novo_usuario, role) == 1) {
                            printf("Usuário cadastrado com sucesso!\n");
                        }
                        else {
                            printf("Erro ao cadastrar o usuário.\n");
                        }
                        break;
                    case 4:
                        buscar_usuarios(conn);
                        _getch();
                        break;
                    case 5:
                        editar_usuario(conn);
                    break;
                    case 6:
                    printf("Saindo...\n");
                    break;
                    default:
                    printf("Opcao invalida.\n");
                    }
                    }
            
        
                else {
                    printf("Você não tem permissão para acessar o financeiro! Pressione qualquer tecla para voltar.");
                    _getch();
                }
                break;

            case 2:
                if (status_login == 1 || status_login == 2) { // Se admin ou usuário comum
                    system("cls");
                    printf("********ESTOQUE************");
                    printf("\n1. Relatorio do estoque\n");
                    printf("2. Editar produto\n");
                    printf("3. Cadastrar produto\n");
                    printf("4. Sair\n");
                    if (scanf_s("%d", &tarefa) != 1) {
                        printf("Erro ao ler a opcao.\n");
                    }

                    switch (tarefa) {
                    case 1:
                        mostrar_estoque(conn);
                        break;
                    case 2:
                        editar_produto(conn);
                        break;
                    case 3:
                        cadastrar_produto(conn);
                        break;
                    case 4:
                        printf("Saindo...\n");
                        break;
                    default:
                        printf("Opcao invalida.\n");
                    }
                }
                break;

            case 3:
                transacao(conn);
                break;

            case 4:
                printf("Saindo...\n");
                break;
                }
                
            
        } while (opcao != 4);  

        mysql_close(conn); 
        return 0;
    }
    else {
        printf("Falha no login.\n");
        mysql_close(conn); 
        return 1;
    }
}
