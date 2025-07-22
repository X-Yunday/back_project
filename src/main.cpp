#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <mysql.h>
#include <string>
#include <iostream>

#define PORT 8080
#define DB_HOST "localhost"
#define DB_USER "root"
#define DB_PASS "Ylh16888"  // 替换为你的密码
#define DB_NAME "l1_stock_db"         // 替换为数据库名

// 生成 JSON 格式的点数据
std::string getDotDataJson() {
    MYSQL* conn = mysql_init(nullptr);
    if (!mysql_real_connect(conn, DB_HOST, DB_USER, DB_PASS, DB_NAME, 3306, nullptr, 0)) {
        return "{\"error\":\"数据库连接失败\"}";
    }

    // 执行查询（假设表名 dot_data 含 id, x, y字段）
    if (mysql_query(conn, "SELECT * FROM dot_data")) {
        mysql_close(conn);
        return "{\"error\":\"查询失败\"}";
    }

    MYSQL_RES* res = mysql_store_result(conn);
    std::string json = "{\"dots\":[";
    MYSQL_ROW row;
    bool first = true;
    while ((row = mysql_fetch_row(res))) {
        if (!first) json += ",";
        json += "{\"x\":" + std::string(row[1]) + 
                ",\"y\":" + std::string(row[2]) + "}";
        first = false;
    }
    json += "]}";
    mysql_free_result(res);
    mysql_close(conn);
    return json;
}

int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    bind(server_fd, (struct sockaddr*)&address, sizeof(address));
    listen(server_fd, 5);  // 监听队列长度=5

    std::cout << "✅ 服务器已启动：http://localhost:" << PORT << "/getDotData\n";
    while (true) {
        int client_sock = accept(server_fd, nullptr, nullptr);
        char buffer[1024] = {0};
        read(client_sock, buffer, sizeof(buffer));  // 读取请求

        // 只处理 /getDotData 请求
        std::string response;
        if (std::string(buffer).find("GET /getDotData") != std::string::npos) {
            std::string json = getDotDataJson();
            response = "HTTP/1.1 200 OK\r\n"
                      "Content-Type: application/json\r\n"
                      "Content-Length: " + std::to_string(json.size()) + "\r\n\r\n" + json;
        } else {
            response = "HTTP/1.1 404 Not Found\r\n\r\n";
        }
        send(client_sock, response.c_str(), response.size(), 0);
        close(client_sock);  // 关闭连接（短连接）
    }
    close(server_fd);
    return 0;
}