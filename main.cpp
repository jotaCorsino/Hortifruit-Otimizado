#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <string.h>//usei para o comando strcmp e strlen
#include <ctype.h>// usar to lower <- colocar letras maiusculas em minuscula
#include <mysql.h>//precisa disso pra conectar com o banco de dados;
#include <conio.h>
//função para conectar ao banco de dados
void conectar_ao_banco(MYSQL* conn) {//mysql* conn, o parametro con é usado para conectar com o banco de dados;
    // Conectar ao banco de dados;; local host== rodando na minha maquina;; lucas== meu nome de usario no banco;; password== é a senha para eu logar;; sistec== nome do banco de dados
    if (mysql_real_connect(conn, "localhost", "root", "jaque12", "sistec_", 0, NULL, 0) == NULL) {//my sql_real_connect é a função que tenta se conectar ao banco usando os parametros
        fprintf(stderr, "Erro na conexão: %s\n", mysql_error(conn));//mysql error imprime uma msg de erro pro usuario 
        mysql_close(conn);//e o mysql_close fecha a conexão
        exit(1);
    }
}
//deixar as letras em padrão
void to_lowercase(char* str) {
    for (int i = 0; str[i]; i++) {
        str[i] = tolower(str[i]);
    }
}
//função para tirar os espaço antes de enviar para o banco e nao ter erros(se chama trim pq em sql é trim tbm KKKKKK)
void trim(char* str) {
    char* end;

    // Remove os espaços do início
    while (isspace((unsigned char)*str)) str++;

    // Se a string estiver vazia
    if (*str == 0)
        return;

    // Remove os espaços do fim
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;

    // Adiciona o terminador nulo no fim da string
    end[1] = '\0';
}

// verificar se a senha e o usuário digitados são os mesmos cadastrados
int verificar_usuario(MYSQL* conn, const char* nome_usuario, const char* senha) {
    MYSQL_STMT* stmt;
    MYSQL_BIND bind[2], result[1];
    char query[] = "SELECT cargo FROM usuarios WHERE nome_usuario = ? AND senha = ?";
    char cargo[20] = "";

    stmt = mysql_stmt_init(conn);
    if (stmt == NULL) {
        fprintf(stderr, "Erro ao inicializar a consulta: %s\n", mysql_error(conn));
        return -1;
    }

    if (mysql_stmt_prepare(stmt, query, (unsigned long)strlen(query)) != 0) {
        fprintf(stderr, "Erro ao preparar a consulta: %s\n", mysql_stmt_error(stmt));
        mysql_stmt_close(stmt);
        return -1;
    }

    memset(bind, 0, sizeof(bind));

    // Nome do usuário
    bind[0].buffer_type = MYSQL_TYPE_STRING;
    bind[0].buffer = (char*)nome_usuario;
    bind[0].buffer_length = (unsigned long)strlen(nome_usuario);

    // Senha
    bind[1].buffer_type = MYSQL_TYPE_STRING;
    bind[1].buffer = (char*)senha;
    bind[1].buffer_length = (unsigned long)strlen(senha);

    if (mysql_stmt_bind_param(stmt, bind) != 0) {
        fprintf(stderr, "Erro ao associar parametros: %s\n", mysql_stmt_error(stmt));
        mysql_stmt_close(stmt);
        return -1;
    }

    if (mysql_stmt_execute(stmt) != 0) {
        fprintf(stderr, "Erro ao executar a consulta: %s\n", mysql_stmt_error(stmt));
        mysql_stmt_close(stmt);
        return -1;
    }

    memset(result, 0, sizeof(result));

    result[0].buffer_type = MYSQL_TYPE_STRING;
    result[0].buffer = cargo;
    result[0].buffer_length = sizeof(cargo);

    if (mysql_stmt_bind_result(stmt, result) != 0) {
        fprintf(stderr, "Erro ao associar resultados: %s\n", mysql_stmt_error(stmt));
        mysql_stmt_close(stmt);
        return -1;
    }

    if (mysql_stmt_store_result(stmt) != 0) {
        fprintf(stderr, "Erro ao armazenar resultado: %s\n", mysql_stmt_error(stmt));
        mysql_stmt_close(stmt);
        return -1;
    }

    if (mysql_stmt_fetch(stmt) == MYSQL_NO_DATA) {
        printf("Usuario ou senha incorretos.\n");
        mysql_stmt_close(stmt);
        return 0;
    }

    if (strcmp(cargo, "admin") == 0) {
        printf("Login bem-sucedido! Bem-vindo, Administrador.\n");
        mysql_stmt_close(stmt);
        return 1;
    }
    else if (strcmp(cargo, "caixa") == 0) {
        printf("Login bem-sucedido! Bem-vindo, Caixa.\n");
        mysql_stmt_close(stmt);
        return 2;
    }
    else if (strcmp(cargo, "estoquista") == 0) {
        printf("Login bem-sucedido! Bem-vindo, Estoquista.\n");
        mysql_stmt_close(stmt);
        return 3;
    }
    else {
        printf("Cargo desconhecido. Acesso negado.\n");
        mysql_stmt_close(stmt);
        return -1;
    }
}
// função para cadastrar novos usuários
int cadastrar_usuario(MYSQL* conn, const char* nome_usuario, const char* senha, const char* cargo) {
    MYSQL_STMT* stmt;
    MYSQL_BIND bind[3];
    char query[] = "INSERT INTO usuarios (nome_usuario, senha, cargo) VALUES (?, ?, ?)";

    stmt = mysql_stmt_init(conn);
    if (stmt == NULL) {
        fprintf(stderr, "Erro ao inicializar a consulta: %s\n", mysql_error(conn));
        return -1;
    }

    if (mysql_stmt_prepare(stmt, query, strlen(query)) != 0) {
        fprintf(stderr, "Erro ao preparar a consulta: %s\n", mysql_stmt_error(stmt));
        mysql_stmt_close(stmt);
        return -1;
    }

    memset(bind, 0, sizeof(bind));

    bind[0].buffer_type = MYSQL_TYPE_STRING;
    bind[0].buffer = (char*)nome_usuario;
    bind[0].buffer_length = strlen(nome_usuario);

    bind[1].buffer_type = MYSQL_TYPE_STRING;
    bind[1].buffer = (char*)senha;
    bind[1].buffer_length = strlen(senha);

    bind[2].buffer_type = MYSQL_TYPE_STRING;
    bind[2].buffer = (char*)cargo;
    bind[2].buffer_length = strlen(cargo);

    if (mysql_stmt_bind_param(stmt, bind) != 0) {
        fprintf(stderr, "Erro ao associar parametros: %s\n", mysql_stmt_error(stmt));
        mysql_stmt_close(stmt);
        return -1;
    }

    if (mysql_stmt_execute(stmt) != 0) {
        fprintf(stderr, "Erro ao executar a consulta: %s\n", mysql_stmt_error(stmt));
        mysql_stmt_close(stmt);
        return -1;
    }

    printf("Usuário cadastrado com sucesso!\n");
    mysql_stmt_close(stmt);
    return 1;
}
//função para editar usuários ja cadastrados
int editar_usuario(MYSQL* conn) {
    char nome_usuario[50];
    char nova_senha[50];
    char novo_cargo[50];

    // Solicitando os dados ao usuário
    printf("Digite o nome do usuário a ser editado: ");
    scanf_s("%s", nome_usuario);

    printf("Digite a nova senha: ");
    scanf_s("%s", nova_senha);

    printf("Digite o novo cargo: ");
    scanf_s("%s", novo_cargo);

    MYSQL_STMT* stmt;
    MYSQL_BIND bind[3];
    char query[] = "UPDATE usuarios SET senha = ?, cargo = ? WHERE nome_usuario = ?";

    stmt = mysql_stmt_init(conn);
    if (stmt == NULL) {
        fprintf(stderr, "Erro ao inicializar a consulta: %s\n", mysql_error(conn));
        return -1;
    }

    if (mysql_stmt_prepare(stmt, query, strlen(query)) != 0) {
        fprintf(stderr, "Erro ao preparar a consulta: %s\n", mysql_stmt_error(stmt));
        mysql_stmt_close(stmt);
        return -1;
    }

    memset(bind, 0, sizeof(bind));

    // Associando os dados
    bind[0].buffer_type = MYSQL_TYPE_STRING;
    bind[0].buffer = (char*)nova_senha;
    bind[0].buffer_length = strlen(nova_senha);

    bind[1].buffer_type = MYSQL_TYPE_STRING;
    bind[1].buffer = (char*)novo_cargo;
    bind[1].buffer_length = strlen(novo_cargo);

    bind[2].buffer_type = MYSQL_TYPE_STRING;
    bind[2].buffer = (char*)nome_usuario;
    bind[2].buffer_length = strlen(nome_usuario);

    if (mysql_stmt_bind_param(stmt, bind) != 0) {
        fprintf(stderr, "Erro ao associar parametros: %s\n", mysql_stmt_error(stmt));
        mysql_stmt_close(stmt);
        return -1;
    }

    if (mysql_stmt_execute(stmt) != 0) {
        fprintf(stderr, "Erro ao executar a consulta: %s\n", mysql_stmt_error(stmt));
        mysql_stmt_close(stmt);
        return -1;
    }

    printf("Usuário '%s' editado com sucesso!\n", nome_usuario);
    mysql_stmt_close(stmt);
    return 1;
}


//função para ver todos os usuarios existentes
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
//função para excluir usuarios existentes
int excluir_usuario(MYSQL* conn, int usuario_id) {
    char query[256];
    sprintf_s(query, "DELETE FROM usuarios WHERE id = %d", usuario_id);

    if (mysql_query(conn, query)) {
        fprintf(stderr, "Erro ao excluir o usuario: %s\n", mysql_error(conn));
        return -1;
    }

    if (mysql_affected_rows(conn) == 0) {
        printf("Nenhum usuario encontrado com o ID %d\n", usuario_id);
        return -1;
    }

    printf("Usuario com ID %d excluido com sucesso.\n", usuario_id);
    return 0;
}
// Função para inserir produto no estoque
void inserir_produto(MYSQL* conn) {
    char nome_produto[100];
    int quantidade;
    char fornecedor[100];

    // Coletar o nome do produto
    printf("Digite o nome do produto: ");
    fgets(nome_produto, sizeof(nome_produto), stdin);
    nome_produto[strcspn(nome_produto, "\n")] = 0;  // Remover o '\n' capturado pelo fgets

    trim(nome_produto);  // Remover espaços antes e depois
    to_lowercase(nome_produto);  // Convertendo para minúsculo, se necessário

    // Coletar a quantidade
    printf("Digite a quantidade: ");
    scanf_s("%d", &quantidade);
    getchar();  // Limpar o buffer do '\n'

    // Coletar o nome do fornecedor
    printf("Digite o nome do fornecedor: ");
    fgets(fornecedor, sizeof(fornecedor), stdin);
    fornecedor[strcspn(fornecedor, "\n")] = 0;  // Remover o '\n' capturado pelo fgets

    trim(fornecedor);  // Remover espaços antes e depois
    to_lowercase(fornecedor);  // Convertendo para minúsculo, se necessário

    // Construir a query para inserir o produto
    char query[256];
    sprintf_s(query, "INSERT INTO estoque (nome_produto, quantidade, fornecedor, data_cadastrada) VALUES ('%s', %d, '%s', NOW())",
        nome_produto, quantidade, fornecedor);

    // Executar a query
    if (mysql_query(conn, query)) {
        fprintf(stderr, "Erro ao inserir produto: %s\n", mysql_error(conn));
        return;
    }

    printf("Produto inserido com sucesso.\n");
}
//editar produtos do estoque
void editar_estoque(MYSQL* conn) {
    int id;
    char nome_produto[100];
    int quantidade;
    char fornecedor[100];

    // Solicitar o ID do produto a ser editado
    printf("Digite o ID do produto a ser editado: ");
    scanf_s("%d", &id);
    getchar(); // Limpar o buffer do '\n'

    // Verificar se o ID do produto existe
    char query_check[256];
    sprintf_s(query_check, "SELECT id FROM estoque WHERE id=%d", id);
    if (mysql_query(conn, query_check)) {
        fprintf(stderr, "Erro ao verificar produto: %s\n", mysql_error(conn));
        return;
    }

    MYSQL_RES* resultado = mysql_store_result(conn);
    if (resultado == NULL || mysql_num_rows(resultado) == 0) {
        printf("Produto com ID %d não encontrado.\n", id);
        mysql_free_result(resultado);
        return;
    }
    mysql_free_result(resultado);

    // Coletar novos dados
    printf("Digite o novo nome do produto: ");
    fgets(nome_produto, sizeof(nome_produto), stdin);
    nome_produto[strcspn(nome_produto, "\n")] = 0;  // Remover o '\n'

    printf("Digite a nova quantidade: ");
    scanf_s("%d", &quantidade);
    getchar(); // Limpar o buffer do '\n'

    printf("Digite o novo nome do fornecedor: ");
    fgets(fornecedor, sizeof(fornecedor), stdin);
    fornecedor[strcspn(fornecedor, "\n")] = 0;  // Remover o '\n'

    // Construir e executar a query de atualização
    char query_update[256];
    sprintf_s(query_update, "UPDATE estoque SET nome_produto='%s', quantidade=%d, fornecedor='%s' WHERE id=%d",
        nome_produto, quantidade, fornecedor, id);

    if (mysql_query(conn, query_update)) {
        fprintf(stderr, "Erro ao editar o produto: %s\n", mysql_error(conn));
        return;
    }

    printf("Produto atualizado com sucesso.\n");
}
//mostrar os produtos do estoque
void mostrar_estoque(MYSQL* conn) {
    if (mysql_query(conn, "SELECT * FROM estoque")) {
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
//excluir produto no estoque
void excluir_produto(MYSQL* conn) {
    int id;

    // Solicitar o ID do produto a ser excluído
    printf("Digite o ID do produto que deseja excluir: ");
    scanf_s("%d", &id);

    // Construir e executar a query de exclusão
    char query[256];
    sprintf_s(query, "DELETE FROM estoque WHERE id = %d", id);

    if (mysql_query(conn, query)) {
        fprintf(stderr, "Erro ao excluir o produto: %s\n", mysql_error(conn));
        return;
    }

    if (mysql_affected_rows(conn) == 0) {
        printf("Nenhum produto encontrado com o ID %d\n", id);
    }
    else {
        printf("Produto com ID %d excluído com sucesso.\n", id);
    }
}
// Substituir vírgula por ponto em uma string
void replace_comma_with_dot(char* str) {
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] == ',') {
            str[i] = '.';
        }
    }
}
//função que realiza a venda e tira automaticamento o produto do estoque
void realizar_venda_abater_estoque(MYSQL* conn) {
    setlocale(LC_NUMERIC, "C");  // Definir locale para garantir o uso de ponto como separador decimal

    char nome_produto[100];
    int quantidade;
    char valor_str[20];  // Valor em string para capturar com vírgula
    float valor;  // Valor será convertido para float
    int quantidade_disponivel;

    // Coletar os dados da venda
    printf("Digite o nome do produto: ");
    fgets(nome_produto, sizeof(nome_produto), stdin);
    nome_produto[strcspn(nome_produto, "\n")] = 0;  // Remover o '\n' do fgets

    // Aplicar trim no nome do produto no código
    trim(nome_produto);  // Função que remove espaços extras antes e depois

    printf("Digite a quantidade a ser vendida: ");
    scanf_s("%d", &quantidade);
    getchar();  // Limpar o buffer do '\n'

    // Verificar se o produto existe no estoque usando TRIM no SQL
    char query_check[256];
    sprintf_s(query_check, "SELECT quantidade FROM estoque WHERE TRIM(nome_produto)='%s'", nome_produto);

    if (mysql_query(conn, query_check)) {
        fprintf(stderr, "Erro ao verificar produto no estoque: %s\n", mysql_error(conn));
        return;
    }

    MYSQL_RES* resultado = mysql_store_result(conn);
    if (resultado == NULL || mysql_num_rows(resultado) == 0) {
        printf("Produto '%s' não encontrado no estoque.\n", nome_produto);
        mysql_free_result(resultado);
        return;
    }

    // Obter a quantidade disponível
    MYSQL_ROW linha = mysql_fetch_row(resultado);
    quantidade_disponivel = atoi(linha[0]);  // Converter a string para int
    mysql_free_result(resultado);

    // Verificar se há quantidade suficiente no estoque
    if (quantidade_disponivel < quantidade) {
        printf("Quantidade insuficiente no estoque! Disponível: %d\n", quantidade_disponivel);
        return;
    }

    printf("Digite o valor unitário do produto (em reais): ");
    fgets(valor_str, sizeof(valor_str), stdin);
    replace_comma_with_dot(valor_str);  // Substituir vírgula por ponto
    valor = atof(valor_str);  // Converter string para float

    // Construir a query para inserir a venda no banco de dados
    char query_insert[512];
    sprintf_s(query_insert,
        "INSERT INTO vendas (nome_produto, quantidade, valor) VALUES ('%s', %d, %.2f)",
        nome_produto, quantidade, valor * quantidade);  // Valor = quantidade do produto vendido * valor por unidade;

    if (mysql_query(conn, query_insert)) {
        fprintf(stderr, "Erro ao realizar a venda: %s\n", mysql_error(conn));
        return;
    }

    // Atualizar o estoque, subtraindo a quantidade vendida do estoque
    char query_update[256];
    sprintf_s(query_update,
        "UPDATE estoque SET quantidade = quantidade - %d WHERE TRIM(nome_produto) = '%s'",
        quantidade, nome_produto);

    if (mysql_query(conn, query_update)) {
        fprintf(stderr, "Erro ao atualizar o estoque: %s\n", mysql_error(conn));
        return;
    }

    printf("Venda realizada com sucesso e estoque atualizado.\n");
}

//Função para mostrar todas as vendas feitas
void mostrar_vendas(MYSQL* conn) {
    // Query SQL para selecionar os dados da tabela de vendas
    const char* query = "SELECT nome_produto, quantidade, valor, data_venda FROM vendas";

    // Executar a consulta
    if (mysql_query(conn, query)) {
        fprintf(stderr, "Erro ao buscar dados das vendas: %s\n", mysql_error(conn));
        return;
    }

    // Armazenar o resultado da consulta
    MYSQL_RES* resultado = mysql_store_result(conn);
    if (resultado == NULL) {
        fprintf(stderr, "Erro ao armazenar resultado: %s\n", mysql_error(conn));
        return;
    }

    // Verificar o número de linhas retornadas
    int num_rows = mysql_num_rows(resultado);
    if (num_rows == 0) {
        printf("Não há registros de vendas.\n");
    }
    else {
        // Exibir os dados das vendas
        printf("+--------------------------------------------------------------------------------------+\n");
        printf("|  Nome do Produto  | Quantidade |   Valor Total   |         Data da Venda             |\n");
        printf("+--------------------------------------------------------------------------------------+\n");

        MYSQL_ROW row;
        while ((row = mysql_fetch_row(resultado))) {
            printf("| %-17s | %-10s | R$ %-12s | %-25s |\n", row[0], row[1], row[2], row[3]);
        }

        printf("+--------------------------------------------------------------------------------------+\n");
    }

    // Liberar o resultado
    mysql_free_result(resultado);
}

/* Início Gestão Financeira*/
// Função para registrar uma despesa
void registrar_despesa(MYSQL* conn) {
    setlocale(LC_NUMERIC, "C");  // Definir locale para garantir o uso de ponto como separador decimal
    char descricao[255];
    float valor;
    // Coletar os dados da despesa do usuário
    printf("Digite a descrição da despesa: \n");
    getchar(); // Limpar o '\n' residual no buffer após o scanf anterior
    fgets(descricao, sizeof(descricao), stdin);
    descricao[strcspn(descricao, "\n")] = '\0';  // Remover o '\n' no final da string

    printf("Digite o valor da despesa: ");
    fflush(stdin);
    scanf_s("%f", &valor);

    // Cria a query para inserir a despesa na tabela
    char query[512];
    sprintf_s(query, "INSERT INTO despesas (descricao, valor) VALUES ('%s', %.2f)", descricao, valor);

    // Executa a query
    if (mysql_query(conn, query)) {
        fprintf(stderr, "Erro ao registrar despesa: %s\n", mysql_error(conn));
    }
    else {
        printf("Despesa registrada com sucesso!\n");
    }
}
void mostrar_despesas(MYSQL* conn) {
    // Query SQL para selecionar os dados da tabela de despesas
    const char* query = "SELECT descricao, valor, data_despesa FROM despesas";

    // Executa a consulta
    if (mysql_query(conn, query)) {
        fprintf(stderr, "Erro ao buscar dados das despesas: %s\n", mysql_error(conn));
        return;
    }

    // Armazena o resultado da consulta
    MYSQL_RES* resultado = mysql_store_result(conn);
    if (resultado == NULL) {
        fprintf(stderr, "Erro ao armazenar resultado: %s\n", mysql_error(conn));
        return;
    }

    // Verifica o número de linhas retornadas
    int num_rows = mysql_num_rows(resultado);
    if (num_rows == 0) {
        printf("Não há registros de despesas.\n");
    }
    else {
        // Exibir os dados das despesas
        printf("+------------------------------------------------------+\n");
        printf("|  Descrição da Despesa  |   Valor   |    Data da Despesa   |\n");
        printf("+------------------------------------------------------+\n");

        MYSQL_ROW row;
        while ((row = mysql_fetch_row(resultado))) {
            printf("| %-20s | R$ %-8s | %-18s |\n", row[0], row[1], row[2]);
        }

        printf("+------------------------------------------------------+\n");
    }

    // Liberar o resultado
    mysql_free_result(resultado);
}
float somar_vendas(MYSQL* conn) {
    const char* query = "SELECT SUM(valor) FROM vendas";
    float tot_vendas = 0.0;

    if (mysql_query(conn, query)) {
        fprintf(stderr, "Erro ao buscar dados das vendas: %s\n", mysql_error(conn));
        return -1.0;  // Valor de retorno em caso de erro
    }

    MYSQL_RES* resultado = mysql_store_result(conn);
    if (resultado == NULL) {
        fprintf(stderr, "Erro ao armazenar resultado: %s\n", mysql_error(conn));
        return -1.0;  // Valor de retorno em caso de erro
    }

    MYSQL_ROW row = mysql_fetch_row(resultado);
    if (row == NULL || row[0] == NULL) {
        printf("Não há vendas registradas.\n");
        mysql_free_result(resultado);  // Liberar a memória antes de retornar
        return 0.0;
    }

    tot_vendas = atof(row[0]);
    mysql_free_result(resultado);  // Liberar a memória antes de retornar
    return tot_vendas;
}
float somar_despesas(MYSQL* conn) {
    const char* query = "SELECT SUM(valor) FROM despesas";
    float tot_despesas = 0.0;

    if (mysql_query(conn, query)) {
        fprintf(stderr, "Erro ao buscar dados das despesas: %s\n", mysql_error(conn));
        return -1.0;  // Valor de retorno em caso de erro
    }

    MYSQL_RES* resultado = mysql_store_result(conn);
    if (resultado == NULL) {
        fprintf(stderr, "Erro ao armazenar resultado: %s\n", mysql_error(conn));
        return -1.0;  // Valor de retorno em caso de erro
    }

    MYSQL_ROW row = mysql_fetch_row(resultado);
    if (row == NULL || row[0] == NULL) {
        printf("Não há despesas registradas.\n");
        mysql_free_result(resultado);  // Liberar a memória antes de retornar
        return 0.0;
    }

    tot_despesas = atof(row[0]);
    mysql_free_result(resultado);  // Liberar a memória antes de retornar
    return tot_despesas;
}
float calcular_lucro(MYSQL* conn) {
    float tot_vendas = somar_vendas(conn);
    if (tot_vendas < 0) {
        fprintf(stderr, "Erro ao calcular o total de vendas.\n");
        return -1.0;
    }

    float tot_despesas = somar_despesas(conn);
    if (tot_despesas < 0) {
        fprintf(stderr, "Erro ao calcular o total de despesas.\n");
        return -1.0;
    }

    float tot_lucro = tot_vendas - tot_despesas;
    printf("O valor total de vendas é: %.2f\n", tot_vendas);
    printf("O valor total de despesas é: %.2f\n", tot_despesas);
    printf("O valor total de lucro é: %.2f\n", tot_lucro);

    return tot_lucro;
}
/* Fim Gestão Financeira*/

typedef struct {
    char nome_cliente[100];
    char email[100];
    char telefone[20];
    char endereco[255];
} Cliente;
// Função para inserir um cliente no banco de dados
void inserir_cliente(MYSQL* conn) {
    Cliente cliente;

    // Coletar informações do cliente
    printf("Digite o nome do cliente: ");
    fgets(cliente.nome_cliente, sizeof(cliente.nome_cliente), stdin);
    cliente.nome_cliente[strcspn(cliente.nome_cliente, "\n")] = 0;  // Remover o '\n'
    fflush(stdin);
    printf("Digite o email do cliente: ");
    fgets(cliente.email, sizeof(cliente.email), stdin);
    cliente.email[strcspn(cliente.email, "\n")] = 0;
    fflush(stdin);
    printf("Digite o telefone do cliente: ");
    fgets(cliente.telefone, sizeof(cliente.telefone), stdin);
    cliente.telefone[strcspn(cliente.telefone, "\n")] = 0;
    fflush(stdin);
    printf("Digite o endereço do cliente: ");
    fgets(cliente.endereco, sizeof(cliente.endereco), stdin);
    cliente.endereco[strcspn(cliente.endereco, "\n")] = 0;

    // Query para inserir no banco de dados
    char query[512];  // Aumentando o buffer para incluir o campo endereço
    sprintf_s(query, "INSERT INTO clientes (nome_cliente, email, telefone, endereco) VALUES ('%s', '%s', '%s', '%s')",
        cliente.nome_cliente, cliente.email, cliente.telefone, cliente.endereco);

    // Executar a query
    if (mysql_query(conn, query)) {
        fprintf(stderr, "Erro ao inserir cliente: %s\n", mysql_error(conn));
        return;
    }

    printf("Cliente inserido com sucesso!\n");
}

// Função para editar um cliente existente no banco de dados
void editar_cliente(MYSQL* conn) {
    int id;
    Cliente cliente;

    // Solicitar o ID do cliente a ser editado
    printf("Digite o ID do cliente a ser editado: ");
    scanf_s("%d", &id);
    getchar();  // Limpar o buffer

    // Coletar novas informações do cliente
    printf("Digite o novo nome do cliente: ");
    fgets(cliente.nome_cliente, sizeof(cliente.nome_cliente), stdin);
    cliente.nome_cliente[strcspn(cliente.nome_cliente, "\n")] = 0;

    printf("Digite o novo email do cliente: ");
    fgets(cliente.email, sizeof(cliente.email), stdin);
    cliente.email[strcspn(cliente.email, "\n")] = 0;

    printf("Digite o novo telefone do cliente: ");
    fgets(cliente.telefone, sizeof(cliente.telefone), stdin);
    cliente.telefone[strcspn(cliente.telefone, "\n")] = 0;

    printf("Digite o novo endereço do cliente: ");
    fgets(cliente.endereco, sizeof(cliente.endereco), stdin);
    cliente.endereco[strcspn(cliente.endereco, "\n")] = 0;

    // Query para atualizar as informações do cliente
    char query[512];  // Aumentando o buffer para incluir o campo endereço
    sprintf_s(query, "UPDATE clientes SET nome_cliente='%s', email='%s', telefone='%s', endereco='%s' WHERE id=%d",
        cliente.nome_cliente, cliente.email, cliente.telefone, cliente.endereco, id);

    // Executar a query
    if (mysql_query(conn, query)) {
        fprintf(stderr, "Erro ao editar cliente: %s\n", mysql_error(conn));
        return;
    }

    printf("Cliente atualizado com sucesso!\n");
}

// Função para excluir um cliente do banco de dados
void excluir_cliente(MYSQL* conn) {
    int id;

    // Solicitar o ID do cliente a ser excluído
    printf("Digite o ID do cliente que deseja excluir: ");
    scanf_s("%d", &id);
    getchar();  // Limpar o buffer

    // Query para excluir o cliente
    char query[256];
    sprintf_s(query, "DELETE FROM clientes WHERE id = %d", id);

    // Executar a query
    if (mysql_query(conn, query)) {
        fprintf(stderr, "Erro ao excluir cliente: %s\n", mysql_error(conn));
        return;
    }

    if (mysql_affected_rows(conn) == 0) {
        printf("Nenhum cliente encontrado com o ID %d\n", id);
    }
    else {
        printf("Cliente com ID %d excluído com sucesso.\n", id);
    }
}

// Função para mostrar todos os clientes no banco de dados
void mostrar_clientes(MYSQL* conn) {
    // Query para buscar todos os clientes
    if (mysql_query(conn, "SELECT * FROM clientes")) {
        fprintf(stderr, "Erro ao buscar os clientes: %s\n", mysql_error(conn));
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
        printf("%-20s\t", campos[i].name);
    }
    printf("\n");

    // Imprimir linhas do resultado
    while ((linha = mysql_fetch_row(resultado))) {
        for (unsigned int i = 0; i < num_campos; i++) {
            printf("%-20s\t", linha[i] ? linha[i] : "NULL");
        }
        printf("\n");
    }

    mysql_free_result(resultado);
}

// Função principal para gerenciar clientes(menu de cliente XD pro menu normal nao ficar tao grande)
void clientes(MYSQL* conn) {
    int opcao;

    do {
        printf("\n--- Menu de Clientes ---\n");
        printf("1. Inserir novo cliente\n");
        printf("2. Editar cliente\n");
        printf("3. Excluir cliente\n");
        printf("4. Mostrar todos os clientes\n");
        printf("5. Sair\n");
        printf("Escolha uma opção: ");
        scanf_s("%d", &opcao);
        getchar();  // Limpar o buffer

        switch (opcao) {
        case 1:
            inserir_cliente(conn);
            _getch();
            printf("aperte qualquer tecla pra voltar");
            break;
        case 2:
            editar_cliente(conn);
            _getch();
            printf("aperte qualquer tecla pra voltar");
            break;
        case 3:
            excluir_cliente(conn);
            _getch();
            printf("aperte qualquer tecla pra voltar");
            break;
        case 4:
            mostrar_clientes(conn);
            _getch();
            printf("aperte qualquer tecla pra voltar");
            break;
        case 5:
            printf("Saindo do menu de clientes...\n");
            break;
        default:
            printf("Opção inválida!\n");
        }
    } while (opcao != 5);
}

//função de mostrar o menu ao usuario
void mostrar_menu(const char* cargo) {
    int opcao;
    MYSQL* conn = mysql_init(NULL);
    if (conn == NULL) {
        fprintf(stderr, "Erro ao inicializar MySQL: %s\n", mysql_error(conn));
        exit(1);
    }
    conectar_ao_banco(conn);

    do {
        system("cls"); // limpa a tela
        printf("\n--- MENU ---\n");
        printf("1. Acesso exclusivo ao Administrador\n");
        printf("2. Acesso ao Estoque\n");
        printf("3. Acesso ao Caixa\n");
        printf("0. Sair\n");
        printf("Escolha uma opcao: ");
        scanf_s("%d", &opcao);

        switch (opcao) {
        case 1:
            if (strcmp(cargo, "admin") == 0) {
                int tarefa;
                fflush(stdin);
                do {
                    system("cls");
                    printf("Bem-vindo ao Menu Exclusivo de Administradores!\n");
                    printf("1. Criar usuário\n");
                    printf("2. Exibir usuários\n");
                    printf("3. Excluir usuário\n");
                    printf("4. Editar usuarios\n");
                    printf("5. Criar despesa\n");
                    printf("6. Mostrar lucro\n");
                    printf("7. Mostrar despesas\n");
                    printf("8. Sair\n");
                    fflush(stdin);
                    if (!scanf_s("%d", &tarefa)) {
                        printf("Erro ao ler a opcao.\n");
                    }
                    switch (tarefa) {
                    case 1: {
                        char nome_usuario[50];
                        char senha[50];
                        char novo_cargo[20];
                        // Coletar informações do novo usuário
                        printf("Digite o nome do usuário a ser cadastrado: ");
                        fflush(stdin);
                        scanf_s("%49s", nome_usuario);  // Limitando a entrada a 49 caracteres para evitar overflow

                        printf("Digite a senha para o novo usuário: ");
                        fflush(stdin);
                        scanf_s("%49s", senha);

                        // Pedir o tipo de usuário
                        printf("Digite o tipo de usuário (admin, caixa, estoquista): ");
                        fflush(stdin);
                        scanf_s("%19s", novo_cargo);

                        // Chamar a função para cadastrar o usuário no banco de dados
                        if (cadastrar_usuario(conn, nome_usuario, senha, novo_cargo) == 1) {
                            printf("Usuário cadastrado com sucesso!\n");
                        }
                        else {
                            printf("Erro ao cadastrar o usuário.\n");
                        }
                        break;
                    }
                    case 2:
                        fflush(stdin);
                        buscar_usuarios(conn);
                        printf("aperte qualquer tecla pra voltar");
                        _getch();
                        break;
                    case 3: {
                        fflush(stdin);
                        int usuario_id;
                        printf_s("\nDigite o ID do usuário que deseja excluir: ");
                        scanf_s("%d", &usuario_id);

                        // Chamar a função para excluir o usuário
                        excluir_usuario(conn, usuario_id);
                        printf("aperte qualquer tecla para voltar");
                        _getch();
                        break;
                    }
                    case 4:
                        editar_usuario(conn);
                        printf("aperte qualquer tecla pra voltar");
                        _getch();
                        break;

                    case 5:
                        fflush(stdin);
                        registrar_despesa(conn);
                        printf("Aperte qualquer tecla para voltar ao menu!");
                        _getch();
                        break;
                    case 6:
                        calcular_lucro(conn);
                        printf("Aperte qualquer tecla para voltar ao menu!");
                        _getch();
                        break;
                    case 7:
                        mostrar_despesas(conn);
                        printf("Aperte qualquer tecla para voltar ao menu!");
                        _getch();
                        break;
                    default:
                        printf("opção inválida");
                    }
                } while (tarefa != 8);
            }
            else {
                printf("Acesso negado. Somente Administradores podem acessar esta opção.\n");
                _getch();
            }
            break;
        case 2:

            if (strcmp(cargo, "admin") == 0 || strcmp(cargo, "estoquista") == 0) {
                int tarefa;
                do {
                    system("cls");
                    printf("\nMenu de Opções:\n");
                    printf("1. Inserir produto ao estoque\n");
                    printf("2. Editar produto do estoque\n");
                    printf("3. Mostrar produtos do estoque\n");
                    printf("4. Excluir produto do estoque\n");
                    printf("5. Sair\n");
                    printf("Escolha uma opção: ");
                    scanf_s("%d", &tarefa);
                    getchar(); // Limpar o buffer

                    switch (tarefa) {
                    case 1:
                        inserir_produto(conn);
                        _getch();
                        printf("aperte qualquer tecla pra voltar");
                        break;
                    case 2:
                        editar_estoque(conn);
                        _getch();
                        printf("aperte qualquer tecla pra voltar");
                        break;
                    case 3:
                        mostrar_estoque(conn);
                        _getch();
                        printf("aperte qualquer tecla pra voltar");
                        break;
                    case 4:
                        excluir_produto(conn);
                        _getch();
                        printf("aperte qualquer tecla pra voltar");
                        break;
                    case 5:
                        printf("Saindo...\n");
                        break;
                    default:
                        printf("Opção inválida!\n");
                    }
                } while (tarefa != 5);
            }
            else {
                printf("Acesso negado. Somente Administradores ou Estoquistas podem acessar esta opção.\n");
                _getch();
            }
            break;
        case 3:

            if (strcmp(cargo, "admin") == 0 || strcmp(cargo, "caixa") == 0) {
                int tarefa;
                do {
                    system("cls");
                    printf("Bem-vindo ao Menu de Caixa!\n");
                    printf("\nMenu de Opções:\n");
                    printf("1. Realizar venda\n");
                    printf("2. mostrar relatório de vendas\n");
                    printf("3. Menu de clientes\n");
                    printf("4. sair \n");
                    printf("Escolha uma opção: ");
                    scanf_s("%d", &tarefa);
                    getchar();

                    switch (tarefa) {
                    case 1:
                        realizar_venda_abater_estoque(conn);
                        _getch();
                        printf("aperte qualquer tecla pra voltar");
                        break;
                    case 2:
                        mostrar_vendas(conn);
                        _getch();
                        printf("aperte qualquer tecla pra voltar");
                        break;
                    case 3:
                        clientes(conn);
                        break;
                    default:
                        printf("Opção inválida!\n");
                    }
                } while (tarefa != 4);
            }
            else {
                printf("Acesso negado. Somente Administradores ou Caixas podem acessar esta opção.\n");
                _getch();
            }
            break;
        case 0:
            printf("Saindo...\n");
            break;
        default:
            printf("Opcao invalida!\n");
        }
    } while (opcao != 0);

    // Fechar a conexão ao banco de dados ao sair
    mysql_close(conn);
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
    char cargo[20];
    int login_status;

    // Pedir informações ao usuário para login
    printf("Digite o nome do usuário: ");
    scanf_s("%49s", nome_usuario);  // Limitando a entrada a 49 caracteres para evitar overflow

    printf("Digite a senha: ");
    scanf_s("%49s", senha);

    // Verificar o usuário e obter o cargo (1 = admin, 2 = caixa, 3 = estoquista)
    login_status = verificar_usuario(conn, nome_usuario, senha);

    if (login_status == 1) {
        strcpy_s(cargo, "admin");
    }
    else if (login_status == 2) {
        strcpy_s(cargo, "caixa");
    }
    else if (login_status == 3) {
        strcpy_s(cargo, "estoquista");
    }
    else {
        printf("Nome de usuário ou senha incorretos.\n");
        mysql_close(conn);
        return -1;
    }

    // Mostrar menu de acordo com o cargo
    mostrar_menu(cargo);

    // Fechar a conexão com o banco de dados
    mysql_close(conn);

    return 0;
}
