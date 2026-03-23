#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#include <map>

using namespace std;

class HttpRequest {
private:
    string url;
    string method;
    map<string, string> headers;
    map<string,string> queryParams;
    string body;
    int timeout; // in seconds

    // Private constructor - can only be accessed by the Builder
    HttpRequest() { }

public:
    friend class HttpRequestBuilder;

    // Method to execute the HTTP request
    void execute() {
        cout << "Executing " << method << " request to " << url << endl;
        
        if (!queryParams.empty()) {
            cout << "Query Parameters:" << endl;
            for (const auto& param : queryParams) {
                cout << "  " << param.first << "=" << param.second << endl;
            }
        }

        cout << "Headers:" << endl;
        for (const auto& header : headers) {
            cout << "  " << header.first << ": " << header.second << endl;
        }
        
        if (!body.empty()) {
            cout << "Body: " << body << endl;
        }
        
        cout << "Timeout: " << timeout << " seconds" << endl;
        cout << "Request executed successfully!" << endl;
    }
};

class HttpRequestBuilder {
private:
    HttpRequest req;

public:    
    // Method chaining
    HttpRequestBuilder& withUrl(const string& u) {
        req.url = u; return *this;
    }

    HttpRequestBuilder& withMethod(string method) {
        req.method = method;
        return *this;
    }
    
    HttpRequestBuilder& withHeader(const string& key, const string& value) {
        req.headers[key] = value;
        return *this;
    }

    HttpRequestBuilder& withQueryParams(const string& key, const string& value) {
        req.queryParams[key] = value;
        return *this;
    }
    
    HttpRequestBuilder& withBody(const string& body) {
        req.body = body;
        return *this;
    }
    
    HttpRequestBuilder& withTimeout(int timeout) {
        req.timeout = timeout;
        return *this;
    }
    
    // Build method to create the immutable HttpRequest object
    HttpRequest build() {
        // Validation logic can be added here
        if (req.url.empty()) {
            throw runtime_error("URL cannot be empty");
        }
        return req;
    }
};

class HttpRequestDirector {
public:
    static HttpRequest createGetRequest(const string& url) {
        return HttpRequestBuilder()
                .withUrl(url)
                .withMethod("GET")
                .build();
    }

    // Creates a JSON POST request
    static HttpRequest createJsonPostRequest(const string& url, const string& jsonBody) {
        return HttpRequestBuilder()
            .withUrl(url)
            .withMethod("POST")
            .withHeader("Content-Type", "application/json")
            .withHeader("Accept", "application/json")
            .withBody(jsonBody)
            .build();
    }
};

int main() {

    // Normal Request from Builder Directly
    HttpRequest normalRequest = HttpRequestBuilder()
        .withUrl("https://api.example.com")
        .withMethod("POST")
        .withHeader("Content-Type", "application/json")
        .withHeader("Accept", "application/json")
        .withQueryParams("key", "12345")
        .withBody("{\"name\": \"Samarth\"}")
        .withTimeout(60)
        .build();
    
        normalRequest.execute(); // Guaranteed to be in a consistent state

    cout <<"\n----------------------------\n";

    HttpRequest getRequest = HttpRequestDirector::createGetRequest("https://api.example.com/users");
    getRequest.execute();

    cout <<"\n----------------------------\n";
    
    HttpRequest postRequest = HttpRequestDirector::createJsonPostRequest(
        "https://api.example.com/users", 
        "{\"name\": \"Samarth\", \"email\": \"samarth@example.com\"}");
    postRequest.execute();
}